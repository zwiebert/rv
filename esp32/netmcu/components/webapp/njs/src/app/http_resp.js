"use strict";

import * as mcuComPb from "main/pbuf/mcu_com_pb.js";
import * as b64 from "js-base64";

import { McuConfig } from "stores/mcu_config.js";
import {
  McuBootCount,
  McuGitTagNames,
  McuFirmwareBuildDate,
  McuChipId,
  McuFirmwareVersion,
  Stm32McuFirmwareVersion,
  McuFirmwareUpdProgress,
  McuFirmwareUpdState,
} from "stores/mcu_firmware.js";
import { McuDocs } from "stores/mcu_docs.js";
import {
  ZoneLPHs,
  ZoneDataMsg,
  ZoneCountMax,
  ZoneNames,
  ZoneRemainingSeconds,
  ZoneDurations,
  ZoneTimers,
  PressControlStatus,
  WaterPumpStatus,
  RainSensorStatus,
  Stm32Time,
} from "stores/zones.js";

export function http_handleResponses(obj) {
  console.log("reply-json: " + JSON.stringify(obj));

  if ("pbuf" in obj) {
    let pbuf = obj.pbuf;
    if ("zd" in obj.pbuf) {
      let buf = b64.toUint8Array(pbuf.zd);
      let msg = mcuComPb.ZoneData.deserializeBinary(buf);
      ZoneDataMsg.set(msg);
      ZoneLPHs.set(msg.getLphList());
    }
  }

  if ("config" in obj) {
    let config = obj.config;
    McuConfig.update(config);
  }

  if ("position" in obj) {
    document.getElementById("spi").value = obj.position.p;
  }

  if ("data" in obj || "update" in obj) {
    let update = "update" in obj;
    let data = obj.data || obj.update;
    if ("version" in data) {
      Stm32McuFirmwareVersion.set(data.version);
    } else if ("timer" in data) {
      const timer = data.timer;
      const update_timer = function (t) {
        const key = "timer" + (t.vn || 0) + "." + (t.tn || 0);
        ZoneTimers.update((o) => {
          o[key] = data.timer;
          return o;
        });
      };
      if (!update) ZoneTimers.set({});

      if (Array.isArray(timer)) {
        timer.forEach(update_timer);
      } else {
        update_timer(timer);
      }
    } else {
      let zoneDurations = [];
      let zoneRemainingSeconds = [];
      for (let i = 0; i < ZoneCountMax; ++i) {
        let sfx = i.toString() + ".0";
        let dur = "dur" + sfx;
        let rem = "rem" + sfx;
        zoneDurations[i] = dur in data ? data[dur] : 0;
        zoneRemainingSeconds[i] = rem in data ? data[rem] : 0;
      }

      //ZoneTimers.set(zoneTimers);
      ZoneDurations.set(zoneDurations);
      ZoneRemainingSeconds.set(zoneRemainingSeconds);
      if ("pc" in data) PressControlStatus.set(data.pc);
      if ("pump" in data) WaterPumpStatus.set(data.pump);
      if ("rain" in data) RainSensorStatus.set(data.rain);
      if ("time" in data) Stm32Time.set(data.time);
    }
  }

  if ("rve" in obj) {
    let rve = obj.rve;
    if ("pump" in rve) WaterPumpStatus.set(rve.pump);
    if ("rain" in rve) RainSensorStatus.set(rve.rain);
    if ("pc" in rve) PressControlStatus.set(rve.pc);
  }

  if ("kvs" in obj) {
    let kvs = obj.kvs;

    for (let i = 0; i < ZoneCountMax; ++i) {
      let key = "zn" + i.toString();
      if (key in kvs) {
        ZoneNames.update(i, kvs[key]);
      }
      key = "lph" + i.toString();
      if (key in kvs) {
        ZoneLPHs.update(i, kvs[key]);
      }
    }
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
  if (state === 1) McuFirmwareUpdProgress.update((old) => old + 1);
  else McuFirmwareUpdProgress.set(0);
}
