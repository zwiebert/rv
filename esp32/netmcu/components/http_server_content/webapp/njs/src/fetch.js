'use strict';
import * as appDebug from './app_debug.js';
import * as httpResp from './http_resp.js';

export const FETCH_CONFIG = 1;
export const FETCH_ZONE_NAMES = 2;
export const FETCH_ZONE_DATA = 4;
export const FETCH_VERSION = 8;
export const FETCH_BOOT_COUNT = 16;
export const FETCH_ALIASES_START_PAIRING = 32;
export const FETCH_ALIASES_START_UNPAIRING = 64;
export const FETCH_SHUTTER_PREFS = 128;

export const FETCH_GIT_TAGS = 0; //XXX


const MAX_RETRY_COUNT = 3;

export function http_postRequest(url = '', data = {}, state = { retry_count:0 }) {
  appDebug.dbLog("post-json: " + JSON.stringify(data));

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
      if (!response.ok) {
        console.log("error");
        if (state.retry_count++ < MAX_RETRY_COUNT) {
          return http_postRequest(url, data, state);
        }
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => httpResp.http_handleResponses(json))

    .catch((error) => {
      console.log("error: http_postRequest(): ", error);
    });

}

export function http_postDocRequest(name) {
  let url = '/doc';
  // Default options are marked with *
  return fetch(url, {
    method: "POST",
    cache: "no-cache",
    headers: {
      "Content-Type": "text/plain",
    },
    referrer: "no-referrer",
    body: name,
  })
    .then(response => {
      if (response.ok) {
        response.text().then(text => {
          httpResp.http_handleDocResponses(name, text);
        });
      }
    });
}

export function http_postShutterCommand(c = document.getElementById('send-c').value) {
  let tfmcu = { to: "tfmcu" };

  let send = {
    g: g.toString(),
    m: m.toString(),
    c: c,
  };
  tfmcu.send = send;
  appDebug.dbLog(JSON.stringify(tfmcu));
  let url = '/cmd.json';
  appDebug.dbLog("url: " + url);
  http_postRequest(url, tfmcu);
}

  export function http_fetchByMask(mask) {
    let tfmcu = {to:"tfmcu"};

    if (mask & FETCH_CONFIG)
      tfmcu.config = { all: "?" };

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



export function fetchWithTimeout(url, data, timeout) {
  return new Promise((resolve, reject) => {
    // Set timeout timer
    let timer = setTimeout(
      () => reject(new Error('Request timed out')),
      timeout
    );

    fetch(url, data).then(
      response => resolve(response),
      err => reject(err)
    ).finally(() => clearTimeout(timer));
  });
}

function gitTags_fetch() {
  const tag_url =
    "https://api.github.com/repos/zwiebert/tronferno-mcu-bin/tags";
  const fetch_data = {
    method: "GET",
    cache: "no-cache",
    headers: {},
    referrer: "no-referrer",
    //body: JSON.stringify(data),
  };
  return fetch(tag_url, fetch_data)
    .then((response) => {
      if (!response.ok) {
        console.log("error");
        throw new Error("network repsonse failed");
      }
      return response.json();
    })

    .then((json) => httpResp.gitTags_handleResponse(json))

    .catch((error) => {
      console.log("error: httpFetch.http_postRequest(): ", error);
    });
}


function add_kv(root,cmd,key,val) {
  if (!(cmd in root))
    root[cmd] = {};

  root[cmd][key] = val;
}
