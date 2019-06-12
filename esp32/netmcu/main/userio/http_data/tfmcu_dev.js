var base = '';
var base = 'http://10.0.0.27'; //dev-delete-line//


var tfmcu_config = {};
let config_fetched = false;
const ZONE_COUNT = 14;

class AppState {

    constructor() {
	this.mZoneRemainingTimes = [0,0,0,0,0,0,0,0,0,0,0,0,0,0];
	this.mZoneDescriptions = ["<enter names...>",0, 0,0,0,0,0,0,0,0,0,0,0,0];
	this.mZoneTimerIntervals = [0,0,0,0,0,0,0,0,0,0,0,0,0,0];
	this.mZoneTimerDurations = [0,0,0,0,0,0,0,0,0,0,0,0,0,0];
	this.mZoneTimers = {};
	this.mPressControlStatus = false;
	this.mWaterPumpStatus = false;
	this.mRainSensorStatus = false;
	this.mStm32Time = "";
    }


    getZoneRemainingTime(n)  { return this.mZoneRemainingTimes[n]; }
    getZoneDescription(n) { return this.mZoneDescriptions[n]; }
    getZoneTimerInterval(n) { return this.mZoneTimerIntervals[n]; }
    getZoneTimerDuration(n) { return this.mZoneTimerDurations[n]; }

    updateHtml_zoneTable() {
        for (let i=0; i < ZONE_COUNT; ++i) {
            let sfx = i.toString();
            let dur = 'id-zoneTimerDuration-'+sfx;
            let rem = 'id-zoneRemainingTime-'+sfx;
	    let tim = 'id-zoneTimerInterval-'+sfx;
	    let name = 'id-zoneName-'+sfx;
	    
	    let timer = 'timer'+sfx+".0";
            document.getElementById(dur).value = this.mZoneTimerDurations[i];
            document.getElementById(rem).value = this.mZoneRemainingTimes[i];
            document.getElementById(tim).value = (timer in this.mZoneTimers) ? JSON.stringify(this.mZoneTimers[timer]) : "-";
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

	if ("kvs" in obj) {
	    let kvs = obj.kvs;
	    for (let i=0; i < ZONE_COUNT; ++i) {
		let key = "zn"+i.toString();
		this.mZoneDescriptions[i] = (key in kvs) ? kvs[key] : "";
	    }
	    this.updateHtml_zoneTable();
	}
    }


    fetchConfig() {
	console.log("fetchConfig()");
        var url = base+'/config.json';
        fetch (url, {
            method: 'get',
            headers: {
                Accept: 'application/json'
            },
        }).then(response => {
	    	console.log("Response: fetchConfig()");
            if(response.ok) {
                response.json().then(obj => this.handleFetchedData(obj));
            }
        });
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

    load() {

    }

}

let app_state;


function buildConfigTableRowHtml(name,value) {
    if (name.endsWith("-enable")) {
        console.log("value: "+value);
        return '<td><label class="config-label">'+name+
            '</label></td><td><input class="config-input cb" type="checkbox" id="cfg_'+name+
            '" name="'+name +'"' + (value ? " checked" : "") +'></td>';
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
                    app_state.handleFetchedData(json);
                });
            }
        });
}

function postMcuRestart() {
    var json = { to:"tfmcu", config: { restart:"1" } };
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
	let old_name = app_state.mZoneDescriptions[i];
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
    let g = app_state.g.toString();
    let m = app_state.m.toString();

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
	'<td>'+nmb+'</td><td><input type="text" id="id-zoneName-'+nmb+'" value="'+name+'"></td><td><input type="number" min="0" max="60" value="0" id="id-zoneRemainingTime-'+nmb+'"></td>'+
	'<td><input type="text" id="id-zoneTimerInterval-'+nmb+'" value="'+app_state.getZoneTimerInterval(nmb)+'"</td>'+
	'<td><input type="number" min="0" max="60" id="id-zoneTimerDuration-'+nmb+'" value="'+app_state.getZoneTimerDuration(nmb)+'"</td>'+
	'</tr>';
}

function genHtml_timerTable(n) {
    let html='<table><tr><th>Zone</th><th>Ort</th><th>Restlaufzeit</th><th>Timer Intervall</th><th>Timer Dauer</th></tr>';
    for(let i=0; i < n; ++i) {
	html+= genHtml_timerTableRow(i, app_state.getZoneDescription(i));
    }
    html+='</table>';
    return html;
}

function writeHtml_timerTableDiv() {
    console.log("test");
    document.getElementById("id-timer-table-div").innerHTML = genHtml_timerTable(14);

}

function onContentLoaded() {
    app_state = new AppState();
    app_state.load();
    app_state.fetchConfig();
    app_state.fetchZoneNames();
    app_state.fetchZoneData();

    writeHtml_timerTableDiv();

    document.getElementById("zrlb").onclick = function() { app_state.fetchZoneNames(); app_state.fetchZoneData();}
    document.getElementById("znsb").onclick = () => postZoneNames();

    document.getElementById("rvdl").onclick = () => rvFirmwareDownload();
    document.getElementById("rvfl").onclick = () => rvFirmwareFlash();

    document.getElementById("netota").onclick = () => netFirmwareOTA();
    
    document.getElementById("csvb").onclick = () => postConfig();
    document.getElementById("crlb").onclick = () => app_state.fetchConfig();

    document.getElementById("mrtb").onclick = () => postMcuRestart();

    document.getElementById("stb").onclick = () => app_state.tabIdx = 0;
    document.getElementById("atb").onclick = () => app_state.tabIdx = 1;
    document.getElementById("ctb").onclick = () => app_state.tabIdx = 2;
}

/*
Local Variables:
compile-command: "jshint tfmcu_dev.js"
End:
*/
