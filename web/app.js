const express = require('express');
const expressLayouts = require('express-ejs-layouts');
const mongoose = require('mongoose');
const flash = require('connect-flash')
const session = require('express-session');
const passport = require('passport');
const mqtt = require('async-mqtt');
const { Server } = require("socket.io");
const { createServer } = require("http");

const app = express();
const httpServer = createServer(app);
const io = new Server(httpServer);

// MQTT conf
const mqtt_client = mqtt.connect('mqtt://127.0.0.1:1883')

// connect to broker and subscribe
mqtt_client.on('connect', function () {
    mqtt_client.subscribe('sim/sensors/tmp', function (err) {
        if (err) {
            console.log("error encountered")
            mqtt_client.reconnect()
        }
        else {
            console.log("Well hello there 8)")
        }
    })
})

// Handle new mqtt message
mqtt_client.on('message', async function (topic, message) {
    let input = JSON.parse(message)
    console.log(input)
    io.emit("stats", input);
});

// WebSocket conf
io.on("connection", (socket) => {
    console.log("New connection: "+socket.id); 
});

// Passport config
require('./config/passport')(passport);

// db conf
const db = require('./config/keys').MongoURI;

// Connect to Mongo
mongoose.connect(db, {useNewUrlParser: true})
    .then(() => console.log("MongoDB connected"))
    .catch(err => console.log(err));


// EJS
app.use(express.static('./public/'))
app.use(expressLayouts);
app.set('view engine', 'ejs');

// Bodyparser
app.use(express.urlencoded({extended:false}));

// Session
app.use(session({
    secret: 'secret',
    resave: true,
    saveUninitialized: true
}));

// Passport middleware
app.use(passport.initialize());
app.use(passport.session());

// Connect flash
app.use(flash());

// Global vars
app.use((req,res,next) => {
    res.locals.success_msg = req.flash('success_msg');
    res.locals.error_msg = req.flash('error_msg');
    res.locals.error = req.flash('error');
    next();
})

// Routes
app.use('/', require('./routes/users'))

const PORT = 3000;
httpServer.listen(PORT, () => {
    console.log(`server running on port ${PORT}`)
})
