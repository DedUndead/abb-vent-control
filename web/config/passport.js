const LocalStrategy = require('passport-local').Strategy;
const mongoose = require('mongoose');
const bcrypt = require('bcrypt');

const User = require('../models/User');

function validateEmail(email) {
    let re = new RegExp("[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?");
    return re.test((String(email).toLowerCase()));
}

function compatibleEmail(email) { 
    const arr = email.split(".");

    for (var i = 0; i < (arr.length-1); i++) {
        arr[i] = arr[i].charAt(0).toUpperCase() + arr[i].slice(1);

    }
    const str2 = arr.join(".");

    return str2;
}

module.exports = async function(passport) {
    await passport.use( 
        new LocalStrategy ({ usernameField: 'email'}, async (email, password, done) => {
            //Match User
            try {
                // 2nd check for correct email input
                if(validateEmail(email) == true) {
                    // Fromat email to match db email
                    email = compatibleEmail(email);
                }

                const user = await User.findOne({email: email});
                if(!user) {
                    return done(null, false, {message: 'That email is not registered'})
                }
                bcrypt.compare(password, user.password, (err, isMatch) => {
                    if(err) throw err;
    
                    if(isMatch) {
                        console.log(user);
                        return done(null, user);
                    }
                    else {
                        return done(null, false, { message: 'Password incorrect'});
                    }
                }); 
            }
            catch(err) {
                console.log(err);
            }
        }));

    passport.serializeUser((user,done) => {
        done(null, user.id);
    });
    passport.deserializeUser((id, done) => {
        User.findById(id, (err,user) =>{
            done(err, user);
        });
    });
};
