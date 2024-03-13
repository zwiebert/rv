#!/usr/bin/env node

'use strict';

const express = require('express');
const httpProxy = require('http-proxy');
var expressWs = require('express-ws')

let cont_dir=__dirname
let mcu = 'http://192.168.1.71:80';
let mcu_ws = 'ws://192.168.1.71:80';

let app = express();
 expressWs(app);
let server = require('http').createServer(app);



// forward any requests to MCU
let proxy = httpProxy.createProxyServer({ ws: true });

app.all("/*.json", (req, res) => {
    proxy.web(req, res, { target: mcu });
});
app.all("/f/cli/*", (req, res) => {
    proxy.web(req, res, { target: mcu });
});

app.ws("/ws", (req, res) => {
    proxy.web(req, res, { target: mcu_ws });
});
server.on('upgrade', function (req, socket, head) { 
    console.log('head', JSON.stringify(head));
     proxy.ws(req, socket, head, { target: mcu_ws });
     });

// serve some files



// static files of MCU HTTP server
app.get("/index.html", (req, res) => {
    res.sendFile(cont_dir + '/wapp_dev.html');
});
app.get("/wapp_dev.html", (req, res) => {
    res.sendFile(cont_dir + '/wapp_dev.html');
});
app.get("/wapp.html", (req, res) => {
    res.sendFile(cont_dir + '/build/wapp.html');
});
app.get("/f/js/wapp.js", (req, res) => {
    //res.sendFile(cont_dir + '/wapp_dev.js');
    res.sendFile(cont_dir + '/njs/build_dev/wapp.js');
});
app.get("/f/js/wapp.js.map", (req, res) => {
    //res.sendFile(cont_dir + '/wapp_dev.js');
    res.sendFile(cont_dir + '/njs/build_dev/wapp.js.map');
});
app.get("/f/css/global.css", (req, res) => {
    //res.sendFile(cont_dir + '/wapp_dev.js');
    res.sendFile(cont_dir + '/njs/src/app.css');
});
app.get("/f/css/wapp.css", (req, res) => {
    //res.sendFile(cont_dir + '/wapp_dev.js');
    res.sendFile(cont_dir + '/njs/build_dev/wapp.css');
});
app.get("/f/css/wapp.css.map", (req, res) => {
    //res.sendFile(cont_dir + '/wapp_dev.js');
    res.sendFile(cont_dir + '/njs/build_dev/wapp.css.map');
});
app.get("/src/", (req, res) => {
    //res.sendFile(cont_dir + '/wapp_dev.js');
    res.sendFile(cont_dir + '/njs/src/');
});



server.listen(3002);


/*
Local Variables:
compile-command: "jshint proxy.js"
End:
*/
