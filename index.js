const nbind = require('nbind');
const path = require('path');
const esptk = nbind.init(path.join(__dirname, 'esptk')).lib;

module.exports.default = esptk.ESPFile;
