var base = '';

var tfmcu_config = {};
let config_fetched = false;
const ZONE_COUNT = 14;

const FW_UPD_STATE_NONE = 0;
const FW_UPD_STATE_IN_PROGRESS = 1;
const FW_UPD_STATE_DONE = 2;
const FW_UPD_STATE_ERROR = -1;

class AppState {

  constructor() {
    this.mTabVisibility = Number.parseInt((localStorage.getItem("tab_vis") || "1"), 10);

    this.mZoneRemainingTimes = [0,0,0,0,0,0,0,0,0,0,0,0,0,0];
    this.mZoneDescriptions = ["<enter names...>",0, 0,0,0,0,0,0,0,0,0,0,0,0];
    this.mZoneTimerDurations = [0,0,0,0,0,0,0,0,0,0,0,0,0,0];
    this.mZoneTimers = {};
    this.mPressControlStatus = false;
    this.mWaterPumpStatus = false;
    this.mRainSensorStatus = false;
    this.mStm32Time = "";

    this.mStm32FwUpdState = FW_UPD_STATE_NONE;
    this.mEsp32FwUpdState = FW_UPD_STATE_NONE;
    this.mEsp32BootCount = 0;
    this.mStm32Version = "";


  }
  load() {
    this.tabIdx = this.mTabIdx;
    this.tabVisibility = this.mTabVisibility;
  }

  set tabVisibility(value) {
    this.mTabVisibility = value;
    localStorage.setItem("tab_vis", value.toString());
    navTabs_updHtml(parseInt(value));
  }

  getZoneRemainingTime(n)  { return this.mZoneRemainingTimes[n]; }
  getZoneDescription(n) { return this.mZoneDescriptions[n]; }
  getZoneTimerDuration(n) { return this.mZoneTimerDurations[n]; }

  setRvFwUpdState(state) {
    this.mStm32FwUpdState = state;
    this.updateHtml_fwUpd();
  }

  updateHtml_bootCount() {
    document.getElementById("id-bootCount").innerHTML = this.mEsp32BootCount.toString();
  }

  updateHtml_stm32Version() {
    document.getElementById("id-stm32Version").innerHTML = this.mStm32Version;
  }

  updateHtml_fwUpd() {
    console.log("fwupd: ", this.mStm32FwUpdState);
    let el = document.getElementById("id-rvFwUpdState");
    switch (this.mStm32FwUpdState) {
    case FW_UPD_STATE_NONE:
      el.innerHTML = "";
      break;
    case FW_UPD_STATE_IN_PROGRESS:
      el.innerHTML = " ...firmware update in progress...";
      break;
    case FW_UPD_STATE_DONE:
      el.innerHTML = " ...firmware update successfully done.";
      break;
    case FW_UPD_STATE_ERROR:
      el.innerHTML = " ...firmware update FAILED.";
      break;
    }
  }

  formatTimeToMS(time) {
    return ~~(time/60)+':' + ~~(time%60);
  }

  updateHtml_zoneTable() {
    console.log("test");
    for (let i=0; i < ZONE_COUNT; ++i) {
      let sfx = i.toString();
      let dur = 'id-zoneTimerDuration-'+sfx;
      let rem = 'id-zoneRemainingTime-'+sfx;
      let name = 'id-zoneName-'+sfx;
      let timer = 'timer'+sfx+".0";

      console.log(dur);
      document.getElementById(dur).value = (this.mZoneTimerDurations[i] / 60.0).toFixed(2);
      document.getElementById(rem).value = (this.mZoneRemainingTimes[i] / 60.0).toFixed(2);
      document.getElementById(name).value = this.mZoneDescriptions[i];
    }
  }

  updateHtml_rvStatus() {
    document.getElementById("id-pressControlStatus").checked = this.mPressControlStatus;
    document.getElementById("id-waterPumpStatus").checked = this.mWaterPumpStatus;
    document.getElementById("id-rainSensorStatus").checked = this.mRainSensorStatus;
    document.getElementById("id-stm32Time").value = this.mStm32Time;
  }

  updateAutomaticHtml() {
    let auto = this.auto;
    document.getElementById('tfti').value = ("f" in auto) ? auto.f : "";

    document.getElementById('tati').value = ("astro" in auto) ? auto.astro : "";
    document.getElementById('twti').value = ("weekly" in auto) ? auto.weekly : "";

    let f = ("f" in auto) ? auto.f : "";
    document.getElementById('tdci').checked = f.indexOf("D") >= 0;
    document.getElementById('twci').checked = f.indexOf("W") >= 0;
    document.getElementById('taci').checked = f.indexOf("A") >= 0;
    document.getElementById('trci').checked = f.indexOf("R") >= 0;
    document.getElementById('tsci').checked = f.indexOf("S") >= 0;
    document.getElementById('tmci').checked = f.indexOf("M") >= 0;

    let up_elem = document.getElementById('tduti');
    let down_elem = document.getElementById('tddti');
    up_elem.value = "";
    down_elem.value = "";

    if ("daily" in auto) {
      let d = auto.daily;
      let l = auto.daily.length;
      up_elem.value = d.startsWith("-") ? "" : d.substring(0,2)+":"+d.substring(2,4);
      down_elem.value = d.endsWith("-") ? "" : d.substring(l-4,l-2)+":"+d.substring(l-2);
    }
  }

  handleFetchedData(obj) {
    console.log("reply-json: "+JSON.stringify(obj));

    if ("config" in obj) {
      if (config_fetched) {
        updateHtmlByConfigData(obj.config);
      } else {
        config_fetched = true;
        document.getElementById("config-div").innerHTML = buildHtmlByConfigData(obj.config);
      }
      tfmcu_config = obj;
    }

    if ("position" in obj) {
      document.getElementById('spi').value = obj.position.p;
    }

    if ("data" in obj) {
      let data = obj.data;
      if ("version" in data) {
	this.mStm32Version = data.version;
	this.updateHtml_stm32Version();
      } else {
	this.mZoneTimers = {};
	for (let i=0; i < ZONE_COUNT; ++i) {
          let sfx = i.toString()+".0";
          let dur = 'dur'+sfx;
          let rem = 'rem'+sfx;
          this.mZoneTimerDurations[i] = (dur in data) ? data[dur] : 0;
          this.mZoneRemainingTimes[i] = (rem in data) ? data[rem] : 0;

	  for (let k=0; k < 10; ++k) {
	    let sfx = i.toString()+'.'+k;
	    let timer = 'timer'+sfx;
	    if (timer in data) {
	      this.mZoneTimers[timer] = data[timer];
	    }
	  }
	}
	this.updateHtml_zoneTable();

	this.mPressControlStatus = ("pc" in data && data.pc);
	this.mWaterPumpStatus = ("pump" in data && data.pump);
	this.mRainSensorStatus = ("rain" in data && data.rain);
	this.mStm32Time = "time" in data ? data.time : "";
	this.updateHtml_rvStatus();
      }

    }

    if ("kvs" in obj) {
      let kvs = obj.kvs;
      for (let i=0; i < ZONE_COUNT; ++i) {
	let key = "zn"+i.toString();
	this.mZoneDescriptions[i] = (key in kvs) ? kvs[key] : "";
      }
      this.updateHtml_zoneTable();
    }

    if ("mcu" in obj) {
      let mcu = obj.mcu;
      if ("status" in mcu) {
	if (this.mStm32FwUpdState == FW_UPD_STATE_IN_PROGRESS) {
	  this.setRvFwUpdState((mcu.status == "ok") ? FW_UPD_STATE_DONE : FW_UPD_STATE_ERROR);
	}
      }
      if ("boot-count" in mcu) {
	this.mEsp32BootCount = mcu["boot-count"];
	this.updateHtml_bootCount();
      }
    }
  }


  fetchConfig() {
    var json = { to:"tfmcu", config: { all:"?" } };
    var url = base+'/cmd.json';
    postData(url, json);

  }





  fetchZoneData() {
    var json = { to:"rv", cmd: { dur:"?", rem:"?", status:"?" } };
    var url = base+'/cmd.json';
    postData(url, json);
  }

  fetchZoneNames() {
    var json = { to:"netmcu", kvs: { zn:"?" } };
    var url = base+'/cmd.json';
    postData(url, json);
  }

}

let ast;


function buildConfigTableRowHtml(name,value) {
  if (name.endsWith("-enable")) {
    console.log("value: "+value);
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input cb" type="checkbox" id="cfg_'+name+
      '" name="'+name +'"' + (value ? " checked" : "") +'></td>';
  } else if (name === 'verbose') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input" type="number" min="0" max="5" id="cfg_'+name+
      '" name="'+name+'" value="'+value+'"></td>';
  } else if (name === 'network') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><select  class="config-input" id="cfg_'+name+'">'+
      '<option value="wlan">Connect to existing WLAN</option>'+
      '<option value="ap">AP (192.168.4.1, ssid/pw=tronferno)</option>'+
      '<option value="lan">Connect to Ethernet</option>'+ // dev-no-lan-delete-line
      '</select></td>';
  } else if (name === 'lan-phy') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><select  class="config-input" id="cfg_'+name+'">'+
      '<option value="lan8270">LAN8270</option>'+
      '<option value="rtl8201">RTL8201</option>'+
      '<option value="ip101">IP101</option>'+
      '</select></td>';
  } else if (name === 'lan-pwr-gpio') {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input" type="number" min="-1" max="36" id="cfg_'+name+
      '" name="'+name+'" value="'+value+'"></td>';
  } else {
    return '<td><label class="config-label">'+name+
      '</label></td><td><input class="config-input text" type="text" id="cfg_'+name+
      '" name="'+name+'" value="'+value+'"></td>';
  }

}


function buildHtmlByConfigData(cfg) {
  var html ="<table class=\"conf-table\";\">";
  Object.keys(cfg).forEach (function (key, idx) {
    html += '<tr id="cfg_'+key+'_tr">'+buildConfigTableRowHtml(key, cfg[key])+'</tr>'+"\n";
  });
  html +='</table>';
  return html;
}

function updateHtmlByConfigData(cfg) {
  Object.keys(cfg).forEach (function (key, idx) {
    let el = document.getElementById('cfg_'+key);

    switch(el.type) {
    case 'checkbox':
      el.checked = cfg[key] != 0;
      break;
    default:
      el.value = cfg[key];
    }
  });
}


function postData(url = '', data = {}) {
  // Default options are marked with *
  console.log("post-json: "+JSON.stringify(data));
  return fetch(url, {
    method: "POST", // *GET, POST, PUT, DELETE, etc.
    //mode: "cors", // no-cors, cors, *same-origin
    cache: "no-cache", // *default, no-cache, reload, force-cache, only-if-cached
    // credentials: "same-origin", // include, *same-origin, omit
    headers: {
      "Content-Type": "application/json",
    },
    redirect: "follow", // manual, *follow, error
    referrer: "no-referrer", // no-referrer, *client
    body: JSON.stringify(data), // body data type must match "Content-Type" header
  })
    .then(response => {
      if(response.ok) {
        response.json().then(json => {
          ast.handleFetchedData(json);
        });
      }
    });
}

function postMcuRestart() {
  var json = { to:"tfmcu", config: { restart:"1" } };
  var url = base+'/cmd.json';
  postData(url, json);
}

function postRvMcuRestart() {
  var json = { to:"tfmcu", mcu: { rfw:"1" } };
  var url = base+'/cmd.json';
  postData(url, json);
}

function postConfig() {
  let tfmcu = Object.assign({},tfmcu_config);
  var cfg = tfmcu.config;
  var new_cfg = {};
  var has_changed = false;
  Object.keys(cfg).forEach (function (key, idx) {
    let new_val = 0;
    let el = document.getElementById('cfg_'+key);
    console.log("key: "+key);

    switch(el.type) {
    case 'checkbox':
      new_val = el.checked ? 1 : 0;
      break;
    default:
      new_val = el.value;
    }
    let old_val = cfg[key];
    if (new_val != old_val) {
      new_cfg[key] = new_val;
      has_changed = true;
      console.log(key);
    }
  });

  if (has_changed) {
    new_cfg.all = "?";
    tfmcu.config = new_cfg;

    console.log(JSON.stringify(tfmcu));
    var url = base+'/cmd.json';
    console.log("url: "+url);
    postData(url, tfmcu);
  }
}

function postZoneNames() {
  let netmcu = {to:"netmcu"};
  let kvs = {};
  netmcu.kvs = kvs;

  for (let i=0; i < ZONE_COUNT; ++i) {
    let sfx = i.toString();
    let key = 'zn'+sfx;
    let id = 'id-zoneName-'+sfx;
    let new_name =  document.getElementById(id).value;
    let old_name = ast.mZoneDescriptions[i];
    if (old_name != new_name) {
      kvs[key] = new_name;
    }
  }

  console.log(JSON.stringify(netmcu));
  var url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
}


function postSendCommand(c=document.getElementById('send-c').value) {
  var tfmcu = {to:"tfmcu"};
  let g = ast.g.toString();
  let m = ast.m.toString();

  var send = {
    g: g,
    m: m,
    c: c,
  };
  tfmcu.send = send;
  console.log(JSON.stringify(tfmcu));
  var url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, tfmcu);
}

function rvFirmwareDownload() {
  let fwUrl = document.getElementById("id-stm32FirmwareURL").value;
  // TODO: validate URL here
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    dlrvbin: fwUrl
  };
  let url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
}

function rvFirmwareFlash() {
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    flrv: 1
  };
  let url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
}

function rvFirmwareOTA() {
  let fwUrl = document.getElementById("id-stm32FirmwareURL").value;
  // TODO: validate URL here
  var netmcu = {to:"netmcu"};
  netmcu.mcu = {
    rvota: fwUrl
  };
  let url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
  ast.setRvFwUpdState(FW_UPD_STATE_IN_PROGRESS);
}

function netFirmwareOTA() {
  let fwUrl = document.getElementById("id-esp32FirmwareURL").value;
  // TODO: validate URL here
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    ota: fwUrl
  };
  let url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
}

function genHtml_timerTableRow(nmb, name) {
  return '<tr>'+
    '<td>'+nmb+'</td>'+
    '<td><input type="text" id="id-zoneName-'+nmb+'" value="'+name+'"></td>'+
    '<td><input type="number" min="0" max="60" id="id-zoneTimerDuration-'+nmb+'" value="'+ast.getZoneTimerDuration(nmb)+'"</td>'+
    '<td><input type="number" min="0" max="60" value="0" id="id-zoneRemainingTime-'+nmb+'"></td>'+
    '</tr>';
}

function genHtml_timerTable(n) {
  let html='<table><tr><th>Zone</th><th>Name</th></th><th>Dauer</th><th>Rest</th></tr>';
  for(let i=0; i < n; ++i) {
    html+= genHtml_timerTableRow(i, ast.getZoneDescription(i));
  }
  html+='</table>';
  return html;
}

function writeHtml_timerTableDiv() {
  document.getElementById("id-timer-table-div").innerHTML = genHtml_timerTable(ZONE_COUNT);

}

function fetchVersions() {
  var netmcu = {to:"netmcu"};
  netmcu.cmd = {
    "rv-version":"?"
  };

  let url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
}

function fetchBootCount() {
  var netmcu = {to:"netmcu"};
  netmcu.mcu = {
    "boot-count":"?"
  };

  let url = base+'/cmd.json';
  console.log("url: "+url);
  postData(url, netmcu);
}

const VIS_NET = 0x01;
const VIS_RV  = 0x02;
const VIS_FIRMWARE = 0x04;


function tabSwitchVisibility(mask) {
  const NONE = "none";
  const SHOW = "";
  document.getElementById("id-rvDiv").style.display = (mask & VIS_RV) ? SHOW : NONE;
  document.getElementById("configdiv").style.display = (mask & VIS_NET) ? SHOW : NONE;
  document.getElementById("id-fwDiv").style.display = (mask & VIS_FIRMWARE) ? SHOW : NONE;

  const BGC1 = "hsl(220, 60%, 60%)";
  const BGC0 = "#eee";
  document.getElementById("atb").style.backgroundColor =  (mask & VIS_RV) ? BGC1 : BGC0;
  document.getElementById("ctb").style.backgroundColor =  (mask & VIS_NET) ? BGC1 : BGC0;
  document.getElementById("stb").style.backgroundColor =  (mask & VIS_FIRMWARE) ? BGC1 : BGC0;

}

//--------------- nav tabs ------------------
const tabs = [
  { 'text':'RV', 'div_id':['id-rvDiv'], fetch_gm:0},
  { 'text':'Config', 'div_id':['configdiv'], fetch:0 },
  { 'text':'Firmware', 'div_id':['id-fwDiv'], fetch_init:0},

];
let div_ids = [];

function navTabs_updHtml(idx) {
  const NONE = "none";
  const SHOW = "";
  const BGC1 = "hsl(220, 60%, 60%)";
  const BGC0 = "#eee";

  if (!(0 <= idx && idx < tabs.length))
    idx = 0;

  for (let i=0; i < tabs.length; ++i) {
    document.getElementById('tabbt'+i.toString()).style.backgroundColor =  (i == idx) ? BGC1 : BGC0;
  }

  for (let div_id of div_ids) {
    document.getElementById(div_id).style.display = tabs[idx].div_id.includes(div_id) ? SHOW : NONE;
  }

  const nt = tabs[idx];

  let fetch = 0;

  if ('fetch_init' in nt) {
    fetch |= nt.fetch_init;
  }
  if ('fetch' in nt) {
    fetch |= nt.fetch;
  }

  if (fetch) {
    ast.http_fetchByMask(fetch);
  }
}

function onNavTab(idx) {
  ast.tabVisibility = idx;
}

function navTabs_genHtml() {
  let html = '';
  for (let i=0; i < tabs.length; ++i) {
    const tab = tabs[i];
    html += '<button class="tablinks" id="tabbt'+i.toString()+'" onClick="onNavTab('+i.toString()+')">'+tab.text+'</button>\n';

    for (let k=0; k < tabs[i].div_id.length; ++k) {
      const div_id = tabs[i].div_id[k];
      if (!div_ids.includes(div_id)) {
        div_ids.push(div_id);
      }
    }
  }
  document.getElementById('tabBar').innerHTML = html;
}


function onContentLoaded() {
  console.log("onContentLoaded()");
  navTabs_genHtml();

  ast = new AppState();
  ast.load();
  ast.fetchConfig();
  ast.fetchZoneNames();
  ast.fetchZoneData();

  fetchVersions();
  fetchBootCount();

  writeHtml_timerTableDiv();

  document.getElementById("zrlb").onclick = function() { ast.fetchZoneNames(); ast.fetchZoneData();};
  document.getElementById("znsb").onclick = () => postZoneNames();

  document.getElementById("rvrstb").onclick = () => postRvMcuRestart();

  document.getElementById("rvota").onclick = () => rvFirmwareOTA();

  document.getElementById("netota").onclick = () => netFirmwareOTA();

  document.getElementById("csvb").onclick = () => postConfig();
  document.getElementById("crlb").onclick = () => ast.fetchConfig();

  document.getElementById("mrtb").onclick = () => postMcuRestart();

}

/*
  Local Variables:
  compile-command: "jshint tfmcu_dev.js"
  js-indent-level: 2
  indent-tabs-mode: nil
  End:
*/
