"use strict";

import { McuConfig } from "./store/mcu_config.js";
import {
  McuBootCount,
  McuGitTagNames,
  McuFirmwareBuildDate,
  McuChipId,
  McuFirmwareVersion,
  Stm32McuFirmwareVersion,
  McuFirmwareUpdProgress,
  McuFirmwareUpdState,
} from "./store/mcu_firmware.js";
import { McuDocs } from "./store/mcu_docs.js";
import {
  ZoneCountMax,
  ZoneNames,
  ZoneRemainingSeconds,
  ZoneDurations,
  ZoneTimers,
  PressControlStatus,
  WaterPumpStatus,
  RainSensorStatus,
  Stm32Time,
} from "./store/zones.js";

export function http_handleResponses(obj) {
  console.log("reply-json: " + JSON.stringify(obj));

  if ("config" in obj) {
    let config = obj.config;
    McuConfig.update(config);
  }

  if ("position" in obj) {
    document.getElementById("spi").value = obj.position.p;
  }

  if ("data" in obj) {
    let data = obj.data;
    if ("version" in data) {
      Stm32McuFirmwareVersion.set(data.version);
    } else {
      let zoneTimers = {};
      let zoneDurations = [];
      let zoneRemainingSeconds = [];
      for (let i = 0; i < ZoneCountMax; ++i) {
        let sfx = i.toString() + ".0";
        let dur = "dur" + sfx;
        let rem = "rem" + sfx;
        zoneDurations[i] = dur in data ? data[dur] : 0;
        zoneRemainingSeconds[i] = rem in data ? data[rem] : 0;

        for (let k = 0; k < 10; ++k) {
          let sfx = i.toString() + "." + k;
          let timer = "timer" + sfx;
          if (timer in data) {
            zoneTimers[timer] = data[timer];
          }
        }
      }

      ZoneTimers.set(zoneTimers);
      ZoneDurations.set(zoneDurations);
      ZoneRemainingSeconds.set(zoneRemainingSeconds);
      PressControlStatus.set("pc" in data && data.pc);
      WaterPumpStatus.set("pump" in data && data.pump);
      RainSensorStatus.set("rain" in data && data.rain);
      Stm32Time.set("time" in data ? data.time : "");
    }
  }

  if ("rve" in obj) {
    let rve = obj.rve;
    if ("pump" in rve) {
      WaterPumpStatus.set(rve.pump);
    }
    if ("rain" in rve) {
      RainSensorStatus.set(rve.rain);
    }
    if ("pc" in rve) {
      PressControlStatus.set(rve.pc);
    }
  }

  if ("kvs" in obj) {
    let kvs = obj.kvs;
    let zoneNames = [];
    for (let i = 0; i < ZoneCountMax; ++i) {
      let key = "zn" + i.toString();
      zoneNames[i] = key in kvs ? kvs[key] : "";
    }
    ZoneNames.set(zoneNames);
  }

  if ("mcu" in obj) {
    let mcu = obj.mcu;
    if ("chip" in mcu) {
      McuChipId.set(mcu.chip);
    }
    if ("firmware" in mcu) {
      McuFirmwareVersion.set(mcu.firmware);
    }
    if ("build-time" in mcu) {
      McuFirmwareBuildDate.set(mcu["build-time"]);
    }
    if ("boot-count" in mcu) {
      McuBootCount.set(mcu["boot-count"]);
    }
    if ("ota-state" in mcu) {
      let ota_state = mcu["ota-state"];

      McuFirmwareUpdState.set(Number.parseInt(ota_state));
      handleOtaState(ota_state);
    }
    if ("stm32ota-state" in mcu) {
      let ota_state = mcu["stm32ota-state"];

      McuFirmwareUpdState.set(Number.parseInt(ota_state));
      handleOtaState(ota_state);
    }
  }
}

export function http_handleDocResponses(name, text) {
  let obj = {};
  obj[name] = { text: text };
  McuDocs.update(obj);
}

export function gitTags_handleResponse(json) {
  let names = [];
  json.forEach((item) => {
    names.push(item.name);
  });
  McuGitTagNames.set(names);
}


function handleOtaState(state) {
  if (state === 1)
    McuFirmwareUpdProgress.update((old) => old+1);
  else
    McuFirmwareUpdProgress.set(0);
}