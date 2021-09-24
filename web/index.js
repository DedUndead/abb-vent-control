'use strict'

/* IMPORTS */
const path = require('path');
const mongoUtils = require('./utils/mongo_utils.js');
const express = require('express');
const { appendFile } = require('fs');


/* CONSTANTS */


/* UART/WIFI SET UP */


/* EXPRESS SET UP */
const app = express();
app.use(express.static(path.resolve(__dirname, 'public')));

/* EXPRESS PATHS */


/* LISTENERS */
app.listen(3000);
