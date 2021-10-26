const express = require('express');
const router = express.Router();
const bcrypt = require('bcrypt');
const mongoose = require('mongoose');
const passport = require('passport');
const {ensureAuthenticated} = require('../config/auth');

// User model
const User = require('../models/User');
const Data = require('../models/Data');

// Retrieve Data
function retrieveData() {
    return Data.find({})
        .sort({createdAt: -1})
        .limit(10)
        .exec();
}

// Login Page
router.get('/', (req, res) => { 
    res.render('login');
});

// Login handle
router.post('/login', (req,res,next) => {
  passport.authenticate('local', {
      successRedirect: '/dashboard',
      failureRedirect: '/',
      failureFlash: true
  })(req, res, next);
});

router.get('/dashboard', ensureAuthenticated, async (req,res) => { 
    const fanPressure = await retrieveData();
    res.render('dashboard', {"pressure": fanPressure[0]});
});

router.get('/stats', ensureAuthenticated, async (req,res) =>{
    res.render('stats');
} )

router.get('/logout', (req,res) => {
    req.logout();
    req.flash('success_msg', 'You are logged out');
    res.redirect('/');
});

module.exports = router;