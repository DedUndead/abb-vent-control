const mongoose = require('mongoose');

const UserActivitySchema = new mongoose.Schema({
    member: {
        type: String,
        require: true
    },
    session: {
        type: Number,
        default: 0
    },
    events: [],
}, {timestamps:true});

UserActivitySchema.set('collection', 'userDataActivity')

const UserActivity = mongoose.model('userDataActivity', UserActivitySchema);

module.exports = UserActivity;