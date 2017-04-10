var util  = require('util')
var exec  = require('child_process').exec;
var fs    = require('fs');
var sleep = require('sleep');
var yaml  = require('yamljs');

CONF = yaml.load('config.yaml');
var RADIO_PACKET       = CONF["DIR"]["RADIO_PACKET"];
var PI_PACKET          = CONF["DIR"]["PI_PACKET"];
var SCRIPT_RADIO_COMMS = CONF["DIR"]["RADIO_COMMS"];
var F1_SENSORS         = CONF["F1_SENSORS"];
var F1_PINS            = CONF["F1_PINS"];

// RPI ---
// GET
exports.getGpio = function(req, res){
  // res.json({message: 'Hello world!' });
  var packet = JSON.parse(fs.readFileSync(PI_PACKET, "utf8"));
  console.log(packet);
  res.status(200).send(packet[String(req.params.id)]);
};

exports.getGpios = function(req, res){
  console.log('all inputs');
  //res.status(200).send(inputs);
  res.status(200);
};

exports.getSensor = function(req,res){
// api_req, id, cmd, from
  if(req.body.from == null) res.status(500).json({"status":"undefined"});
  else if(F1_SENSORS.indexOf(req.params.sens) > -1){
      execute_script(SCRIPT_RADIO_COMMS + " get_sensor " 
                            + req.body.id + " " + req.body.cmd + " " + req.body.from);
      sleep.sleep(10);
      var packet = JSON.parse(fs.readFileSync(RADIO_PACKET, "utf8"));
      res.status(200).send(packet);
  }
  else res.status(500).json({"status":"undefined"});
}

// Arduino ---
exports.getF1Pin = function(req, res){
  if(F1_PINS.indexOf(req.params.pin) > -1){
    execute_script(SCRIPT_RADIO_COMMS + " getPin f1 " + req.params.pin);
    sleep.sleep(2);
    var packet = JSON.parse(fs.readFileSync(RADIO_PACKET, "utf8"));
    res.status(200).send(packet);
  }
  else res.status(500).json({"status":"undefined"});
}

exports.getF1Sensor = function(req, res){
  if(F1_SENSORS.indexOf(req.params.sens) > -1){
    execute_script(SCRIPT_RADIO_COMMS + " getSensor f1 " + req.params.sens + " client");
    sleep.sleep(2);
    var packet = JSON.parse(fs.readFileSync(RADIO_PACKET, "utf8"));
    res.status(200).send(packet);
  }
  else res.status(500).json({"status":"undefined"});
}

// PUT
exports.putF1Pin = function(req,res){
  if(F1_PINS.indexOf(req.params.pin) > -1){
    execute_script(SCRIPT_RADIO_COMMS + " putPin f1 " + req.params.pin + " " + req.body.value);
    sleep.sleep(2);
    var packet = JSON.parse(fs.readFileSync(RADIO_PACKET, "utf8"));
    res.status(200).send(packet);
  }
  else res.status(500).json({"status":"undefined"});
}

// UNKOWN
exports.unknown = function(req, res){
  res.status(404).json({"status":"undefined"});
};

function execute_script(script){
  console.log("Executing: " + script);
  exec(script, puts);
  sleep.sleep(2);
}

function puts(error, stdout, stderr){
  console.log(stdout);
  console.warn("Executing done..");
}

// https://github.com/denschu/rcswitch-rest/blob/master/routes/api.js
// http://www.robert-drummond.com/2015/06/01/rest-api-on-a-pi-part-2-control-your-gpio-io-ports-over-the-internet/
