const mongoose = require('mongoose');

const DataSchema = new mongoose.Schema({
    nr: {
        type: Number,
        require: true
    },
    speed: {
        type: Number,
        require: true
    },
    setpoint: {
        type: Number,
        require: true
    },
    pressure: {
        type: Number,
        require: true
    },
    mode: {
        type: Number,
        require: true
    },
    status: {
        type: Number,
        require: true
    }
}, {timestamps:true});

DataSchema.set('collection', 'sensorData')
DataSchema.set('toObject', { getters: true });

const Data = mongoose.model('sensorData', DataSchema);

module.exports = Data;