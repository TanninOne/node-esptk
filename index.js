Object.defineProperty(exports, "__esModule", { value: true });

const path = require('path');
const esptk = require(path.join(__dirname, 'build', 'Release', 'esptk'));
// const esptk = nbind.init().lib;

module.exports.default = esptk.ESPFile;
