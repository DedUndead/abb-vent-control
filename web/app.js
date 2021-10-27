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

// db conf
const db = require('./config/keys').MongoURI;

// Connect to Mongo
const Data = require('./models/Data');
const UserActivity = require('./models/UserActivity');

function retrieveUserActivityData() {
    return UserActivity.find({})
        .sort({createdAt: -1})
        .limit(10)
        .exec();
}

mongoose.connect(db, {useNewUrlParser: true})
    .then(() => console.log("MongoDB connected"))
    .catch(err => console.log(err));


// MQTT conf
const mqtt_client = mqtt.connect('mqtt://127.0.0.1:1883')

// connect to broker and subscribe
mqtt_client.on('connect', function () {
    mqtt_client.subscribe('controller/status', function (err) {
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
    //console.log(input)
    io.emit("stats", input);
    let sensorData = new Data({
                            nr: input.nr, 
                            speed: input.speed, 
                            setpoint: input.setpoint,
                            pressure: input.pressure,
                            auto: input.auto,
                            error: input.error
                        })
    sensorData.save((err,info) => {
        if(err) console.log(err)
    })
});

// WebSocket conf
io.on("connection", (socket) => {
    console.log("New connection: "+socket.id);

    // Handle updates from client
    socket.on('update', function (data) {
        //console.log("Got Data from client: " + JSON.stringify(data));
        // todo:
        // * parse and transform data
        // * publish serialized data on MQTT

        mqtt_client.publish('client_update', "test_data");
    });
    socket.on('user_activity', async function (userData) { 
        let member = await retrieveUserActivityData();
        member[0].events.push(userData)
        //console.log(member[0].events);
        //console.log(member[0].session)
        let update = await UserActivity.findOneAndUpdate({member:member[0].member ,session: member[0].session,}, {events: member[0].events}, {new:true});
        console.log(update);
        update.save((err,info) => {
            if(err) console.log(err)
        })
    });
});

// Passport config
require('./config/passport')(passport);

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
