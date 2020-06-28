var base = '';

const FETCH_CONFIG = 1;
const FETCH_ZONE_NAMES = 2;
const FETCH_ZONE_DATA = 4;
const FETCH_VERSION = 8;
const FETCH_BOOT_COUNT = 16;
const FETCH_ALIASES_START_PAIRING = 32;
const FETCH_ALIASES_START_UNPAIRING = 64;
const FETCH_SHUTTER_PREFS = 128;
const FETCH_GMU = 256;

let config_fetched = false;
const ZONE_COUNT = 14;

const FW_UPD_STATE_NONE = 0;
const FW_UPD_STATE_IN_PROGRESS = 1;
const FW_UPD_STATE_DONE = 2;
const FW_UPD_STATE_ERROR = -1;

function dbLog(msg) {  console.log(msg); }

function add_kv(root,cmd,key,val) {
  if (!(cmd in root))
    root[cmd] = {};

  root[cmd][key] = val;
}

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
    this.tfmcu_config = {};
    this.load_fetch = 0;


  }
  load() {
    this.tabIdx = this.mTabIdx;
    this.http_fetchByMask(this.load_fetch);
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

  http_handleResponses(obj) {
    console.log("reply-json: "+JSON.stringify(obj));

    if ("config" in obj) {
      let config = obj.config;
      this.tfmcu_config = Object.assign(this.tfmcu_config, config);

      if (!document.getElementById("cfg_table_id")) {
        if ("verbose" in config) {
          document.getElementById("config-div").innerHTML = mcuConfigTable_genHtml(obj.config);
        }
      }
      
      if (document.getElementById("cfg_table_id")) {
        mcuConfig_updHtml(obj.config);
      }
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
      if ("chip" in mcu) {
        document.getElementById("id_chip").innerHTML = mcu.chip;
      }
      if ("firmware" in mcu) {
        document.getElementById("id_firmware").innerHTML = mcu.firmware;
      }
      if ("build-time" in mcu) {
        document.getElementById("id_buildTime").innerHTML = mcu["build-time"];
      }
      if ("status" in mcu) {
	if (this.mStm32FwUpdState == FW_UPD_STATE_IN_PROGRESS) {
	  this.setRvFwUpdState((mcu.status == "ok") ? FW_UPD_STATE_DONE : FW_UPD_STATE_ERROR);
	}
      }
      if ("boot-count" in mcu) {
	this.mEsp32BootCount = mcu["boot-count"];
	this.updateHtml_bootCount();
      }
      if ("ota-state" in mcu) {
        let e = document.getElementById("netota_progress_div");
        switch(mcu["ota-state"]) {
        case 0: // none

          break;
        case 1: // run
          document.getElementById("netota_progress_bar").value = (++netota_progressCounter).toString();
          break;
        case 2: // fail
          e.innerHTML += "<br><strong>Update failed<br><br></strong>";
          break;
        case 3: // done
          document.getElementById("netota_progress_bar").value = document.getElementById("netota_progress_bar").max;
          e.innerHTML += '<br><strong>Update succeeded <button type="button" onClick="req_mcuRestart()">Reboot ESP32 MCU</button><br><br></strong>';
          break;
        }
        if (netota_isInProgress && mcu["ota-state"] != 1) {
          clearInterval(netota_intervalID);
          netota_progressCounter = 0;
          netota_isInProgress = false;
          document.getElementById("netota_controls").style.display = "";
        }

      }

      if ("stm32ota-state" in mcu) {
        let e = document.getElementById("stm32ota_progress_div");
        switch(mcu["stm32ota-state"]) {
        case 0: // none

          break;
        case 1: // run
          document.getElementById("stm32ota_progress_bar").value = (++stm32ota_progressCounter).toString();
          break;
        case 2: // fail
          e.innerHTML += "<br><strong>Update failed<br><br></strong>";
          break;
        case 3: // done
          document.getElementById("stm32ota_progress_bar").value = document.getElementById("stm32ota_progress_bar").max;
          e.innerHTML += '<br><strong>Update succeeded <button type="button" onClick="postRvMcuRestart()">Reboot STM32 MCU</button><br><br></strong>';
          break;
        }
        if (stm32ota_isInProgress && mcu["stm32ota-state"] != 1) {
          clearInterval(stm32ota_intervalID);
          stm32ota_progressCounter = 0;
          stm32ota_isInProgress = false;
          document.getElementById("stm32ota_controls").style.display = "";
        }

      }
    }
  }



  http_fetchByMask(mask) {
    let tfmcu = {to:"tfmcu"};

    if (mask & FETCH_CONFIG)
      tfmcu.config = { all: "?" };

    if (mask & FETCH_GMU)
      tfmcu.config = { 'gm-used': "?" };

    if (mask & FETCH_VERSION) {
      tfmcu.mcu = { version:"?" };
      add_kv(tfmcu,"cmd","rv-version","?");
    }


    if (mask & FETCH_ZONE_NAMES)
      tfmcu.kvs = { zn:"?" };

    if (mask & FETCH_ZONE_DATA) {
      add_kv(tfmcu,"cmd","dur","?");
      add_kv(tfmcu,"cmd","rem","?");
      add_kv(tfmcu,"cmd","status","?");
    }


    if (mask & FETCH_BOOT_COUNT)
      tfmcu.mcu = {
        "boot-count":"?"
      };

    if (mask & FETCH_ALIASES_START_PAIRING)
      tfmcu.pair = {
        a: "?",
        g: this.g,
        m: this.m,
        c: "pair"
      };
    if (mask & FETCH_ALIASES_START_UNPAIRING)
      tfmcu.pair = {
        a: "?",
        g: this.g,
        m: this.m,
        c: "unpair"
      };

    if (mask & FETCH_SHUTTER_PREFS)
      tfmcu.shpref = {
        g: this.g,
        m: this.m,
        c: "read",
      };

    let url = '/cmd.json';
    http_postRequest(url, tfmcu);
  }

}

let ast;


function configTr_genHtml(name,value) {
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


function mcuConfigTable_genHtml(cfg) {
  var html ='<table id="cfg_table_id" class="conf-table">';
  Object.keys(cfg).forEach (function (key, idx) {
    html += '<tr id="cfg_'+key+'_tr">'+configTr_genHtml(key, cfg[key])+'</tr>'+"\n";
  });
  html +='</table>';
  return html;
}

function mcuConfig_updHtml(cfg) {
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

function http_postRequest(url = '', data = {}) {
  dbLog("post-json: "+JSON.stringify(data));

  const fetch_data = {
    method: "POST",
    cache: "no-cache",
    headers: {
      "Content-Type": "application/json",
    },
    referrer: "no-referrer",
    body: JSON.stringify(data),
  };



  return fetch(url, fetch_data)

    .then(response => {
      if(!response.ok) {
        console.log("error");
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => ast.http_handleResponses(json))

    .catch((error) => {
      console.log("error: http_postRequest(): ", error);
    });

}
// -------------- restart/reload -----------------
const reload_Progress = {
    ivId: 0,
    ms: 1000,
    count: 12,
    counter: 0,
    divs: ["stm32ota_restart_div","netota_restart_div", "config_restart_div" ],
  };
  function req_reloadTick() {
    const rpr = reload_Progress;

    if (++rpr.counter > rpr.count) {
      location.reload();
      clearInterval(rpr.ivId); // may be useless after reload...
    } else {
      document.getElementById("reload_progress_bar").value = rpr.counter;
    }
  }
  function req_reloadStart() {
    const rpr = reload_Progress;
    let e = null;
    for (let div of rpr.divs) {
      let e = document.getElementById(div);
      if (e.offsetParent === null)
        continue;

      let html = '<strong>Wait for MCU to restart...</strong><br>';
      html += '<progress id="reload_progress_bar" value="0" max="'+rpr.count.toString()+'">0%</progress>';
      e.innerHTML = html;
      rpr.ivId = setInterval(req_reloadTick, rpr.ms);
      break;
    }

  }

function req_mcuRestart() {
  var json = { to:"tfmcu", config: { restart:"1" } };
  var url = '/cmd.json';
  http_postRequest(url, json);
  req_reloadStart();
  //setTimeout(function(){ location.reload(); }, 10000);
}

function postRvMcuRestart() {
  var json = { to:"tfmcu", mcu: { rfw:"1" } };
  var url = base+'/cmd.json';
  http_postRequest(url, json);
}

function mcuConfig_fromHtml_toMcu() {
  const cfg = ast.tfmcu_config;

  var new_cfg = {};
  var has_changed = false;

  Object.keys(cfg).forEach (function (key, idx) {
    let new_val = 0;
    let el = document.getElementById('cfg_'+key);
    dbLog("key: "+key);

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
      dbLog(key);
    }
  });

  if (has_changed) {
    new_cfg.all = "?";
    var url = '/cmd.json';
    http_postRequest(url, { config: new_cfg });
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
  http_postRequest(url, netmcu);
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
  http_postRequest(url, tfmcu);
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
  http_postRequest(url, netmcu);
}

function rvFirmwareFlash() {
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    flrv: 1
  };
  let url = base+'/cmd.json';
  console.log("url: "+url);
  http_postRequest(url, netmcu);
}

function rvFirmwareOTA() {
  let fwUrl = document.getElementById("id-stm32FirmwareURL").value;
  // TODO: validate URL here
  var netmcu = {to:"netmcu"};
  netmcu.mcu = {
    stm32ota: fwUrl
  };
  let url = base+'/cmd.json';
  console.log("url: "+url);
  http_postRequest(url, netmcu);
  ast.setRvFwUpdState(FW_UPD_STATE_IN_PROGRESS);
}

//----------------- firmware div ---------------
var stm32ota_intervalID = 0;
var stm32ota_isInProgress = false;
var stm32ota_progressCounter = 0;
function stm32ota_FetchFeedback() {
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    "stm32ota":"?"
  };
  let url = '/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, netmcu);
}

const stm32otaName_master = 'github-master';
const stm32otaName_beta = 'github-beta';
function stm32FirmwareOTA(ota_name) {
  if (stm32ota_isInProgress)
    return;
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    stm32ota: ota_name
  };
  let url = '/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, netmcu);
  document.getElementById("stm32ota_progress_div").innerHTML = "<strong>Firmware is updating...<br></strong>" + '<progress id="stm32ota_progress_bar" value="0" max="30">70 %</progress>';
  stm32ota_intervalID = setInterval(stm32ota_FetchFeedback, 1000);
  stm32ota_isInProgress = true;
  document.getElementById("stm32ota_controls").style.display = "none";
}
//----------------- firmware div ---------------
var netota_intervalID = 0;
var netota_isInProgress = false;
var netota_progressCounter = 0;
function netota_FetchFeedback() {
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    "ota":"?"
  };
  let url = '/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, netmcu);
}

const otaName_master = 'github-master';
const otaName_beta = 'github-beta';
function netFirmwareOTA(ota_name) {
  if (netota_isInProgress)
    return;
  var netmcu = {to:"tfmcu"};
  netmcu.mcu = {
    ota: ota_name
  };
  let url = '/cmd.json';
  dbLog("url: "+url);
  http_postRequest(url, netmcu);
  document.getElementById("netota_progress_div").innerHTML = "<strong>Firmware is updating...<br></strong>" + '<progress id="netota_progress_bar" value="0" max="30">70 %</progress>';
  netota_intervalID = setInterval(netota_FetchFeedback, 1000);
  netota_isInProgress = true;
  document.getElementById("netota_controls").style.display = "none";
}
//----------------------------
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

//--------------- nav tabs ------------------
let tabs = [
  { 'text':'RV', 'div_id':['id-rvDiv'], fetch:(FETCH_ZONE_DATA), fetch_init:(FETCH_ZONE_NAMES|FETCH_VERSION) },
  { 'text':'Config', 'div_id':['configdiv'], fetch:(FETCH_CONFIG) },
  { 'text':'Firmware', 'div_id':['id-fwDiv'], fetch_init:FETCH_VERSION, fetch:FETCH_BOOT_COUNT},

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
    nt.fetch_init = 0;
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

  writeHtml_timerTableDiv();

  document.getElementById("zrlb").onclick = function() { ast.http_fetchByMask(FETCH_ZONE_DATA|FETCH_ZONE_NAMES); };
  document.getElementById("znsb").onclick = () => postZoneNames();

  document.getElementById("rvrstb").onclick = () => postRvMcuRestart();

  document.getElementById("csvb").onclick = () => mcuConfig_fromHtml_toMcu();
  document.getElementById("crlb").onclick = () => ast.http_fetchByMask(FETCH_CONFIG);

  document.getElementById("mrtb").onclick = () => req_mcuRestart();

  document.getElementById("stm32ota").onclick = () => stm32FirmwareOTA(document.getElementById("id-stm32FirmwareURL").value);//dev-distro-delete-line//
  document.getElementById("stm32ota_master").onclick = () => stm32FirmwareOTA(otaName_master);
  document.getElementById("stm32ota_beta").onclick = () => stm32FirmwareOTA(otaName_beta);

  document.getElementById("netota").onclick = () => netFirmwareOTA(document.getElementById("id-esp32FirmwareURL").value);//dev-distro-delete-line//
  document.getElementById("netota_master").onclick = () => netFirmwareOTA(otaName_master);
  document.getElementById("netota_beta").onclick = () => netFirmwareOTA(otaName_beta);
}

/*
  Local Variables:
  compile-command: "jshint tfmcu_dev.js"
  js-indent-level: 2
  indent-tabs-mode: nil
  End:
*/
