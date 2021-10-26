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
        type: String,
        require: true
    },
    pressure: {
        type: Number,
        require: true
    },
    auto: {
        type: Boolean,
        require: true
    },
    error: {
        type: Boolean,
        require: true
    }
}, {timestamps:true});

DataSchema.set('collection', 'sensorData')
DataSchema.set('toObject', { getters: true });

const Data = mongoose.model('sensorData', DataSchema);

module.exports = Data;