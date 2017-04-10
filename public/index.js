var express = require('express');
        api = require('./js/api');
body_parser = require('body-parser');
var app     = express();
var router = express.Router();

// middleware to use for all requests
router.use(function(req, res, next) {
    console.log('\nAccessing API...');
    next();
});

// configure Express to serve index.html
app.use(express.static(__dirname));
app.use(body_parser.urlencoded({ extended: true}));
app.use(body_parser.json());
app.use('/', router);

// Express route to handle errors
app.use(function (err, req, res, next) {
  if (req.xhr) {
    res.status(500).send('Oops, Something went wrong!');
  } else {
    next(err);
  }
});

// JSON API
// GET
router.get('/rpi/gpio/:id', api.get_gpio);
router.get('/rpi/gpio/', api.get_gpios);
router.get('/radio/sensor/', api.get_sensor); // Server call. Sends JSON packet

// Arduino F1
router.get('/radio/f1/sensor/:sens', api.get_f1_sensor);
router.get('/radio/f1/pin/:pin', api.get_f1_pin);

// PUT (router.put did not work?)
router.post('/radio/f1/pin/:pin', api.put_f1_pin);  // JSON: value


router.get('*', api.unknown);
router.post('*', api.unknown);

// Start server
app.listen(3000);
console.log('App Server is listening on port 3000');