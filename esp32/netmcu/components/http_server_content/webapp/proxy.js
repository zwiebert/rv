#!/usr/bin/env node

'use strict';

const path = require('path');
const express = require('express');
const httpProxy = require('http-proxy');

let proj_dir=path.dirname(path.dirname(path.dirname(path.dirname(__dirname))));
let tff_dir=path.dirname(proj_dir)+"/tronferno-fhem";
let build_dir=proj_dir+"/src/esp32/build";
let cont_dir=__dirname
let mcu = 'http://192.168.1.69:80';

let server = express();

// forward any requests to MCU
let proxy = httpProxy.createProxyServer();

server.all("/*.json", (req, res) => {
    proxy.web(req, res, { target: mcu });
});
server.all("/doc", (req, res) => {
    proxy.web(req, res, { target: mcu });
});


// serve some files



// static files of MCU HTTP server
server.get("/index.html", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.html');
});
server.get("/tfmcu_dev.html", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.html');
});
server.get("/tfmcu_dev.js", (req, res) => {
    res.sendFile(cont_dir + '/tfmcu_dev.js');
});
server.get("/tfmcu.html", (req, res) => {
  res.sendFile(cont_dir + '/tfmcu.html');
});
server.get("/tfmcu.js", (req, res) => {
  res.sendFile(cont_dir + '/tfmcu.js');
});


// ESP32 firware image for OTA update
server.get("/tronferno-mcu.bin", (req, res) => {
    res.sendFile(build_dir+'/tronferno-mcu.bin');
});


server.listen(3000);


/*
Local Variables:
compile-command: "jshint proxy.js"
End:
*/
