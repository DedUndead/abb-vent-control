const mongoose = require('mongoose');

const UserSchema = new mongoose.Schema({
    member: {
        type: String,
        require: true
    },
    email: {
        type: String,
        require: true
    },
    password: {
        type: String,
        require: true
    },
    rights: {
        type: String,
        require: true
    }

});

UserSchema.set('collection', 'metropoliaStudents')

const User = mongoose.model('metropoliaStudents', UserSchema);

module.exports = User;