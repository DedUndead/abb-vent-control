const express = require('express');
const mongoose = require('mongoose');
const flash = require('connect-flash')
const session = require('express-session');
const passport = require('passport');
const mqtt = require('async-mqtt');
const { Server } = require("socket.io");
const { createServer } = require("http");
const mongoStore = require('connect-mongo');


const app = express();
const httpServer = createServer(app);
const io = new Server(httpServer);

// db conf
const db = require('./config/keys').MongoURI;

// Connect to Mongo
const Data = require('./models/Data');
const UserActivity = require('./models/UserActivity');

mongoose.connect(db, { useNewUrlParser: true })
    .then(() => console.log("MongoDB connected"))
    .catch(err => console.log(err));

function retrieveUserActivityData() {
    return UserActivity.find({})
        .sort({ createdAt: -1 })
        .limit(10)
        .exec();
}

function retrieveUserSessionsData() {
    return UserActivity.aggregate([
        { $group: { _id: { member: "$member", session: "$session" }, number: { $sum: 1 } } },
        { $group: { _id: "$_id.member", sessions: { $sum: 1 } } }
    ])
}

function retrieveUserEventsData() {
    return UserActivity.aggregate([
        { $unwind: "$events" },
        { $group: { _id: { member: "$member", events: "$events" }, number: { $sum: 1 } } },
        { $group: { _id: "$_id.member", events: { $sum: 1 } } }
    ])
}

function sensorDataDates(date_range) {
    let start = date_range.start_date + 'T' + date_range.start_time + ":00.000Z";
    console.log(start);
    let end = date_range.target_date + 'T' + date_range.target_time + ":00.000Z";
    console.log(end);
    return Data.find({
        createdAt: {
            $gte: start,
            $lt: end
        }
    })
}

// MQTT conf
const mqtt_client = mqtt.connect('mqtt://18.198.188.151:21883')

// connect to broker and subscribe
mqtt_client.on('connect', function () {
    mqtt_client.subscribe('/iot/grp1/mcu', function (err) {
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
    console.log("Received: " + JSON.stringify(input))
    io.emit("stats", input);
    let sensorData = new Data({
        nr: input.nr,
        speed: input.speed,
        setpoint: input.setpoint,
        pressure: input.pressure,
        mode: input.mode,
        status: input.status
    })
    /*sensorData.save((err,info) => {
        if(err) console.log(err)
    })*/
});

// WebSocket conf
io.on("connection", (socket) => {
    console.log("New connection: " + socket.id);

    // Handle updates from client
    socket.on('update_mqtt', function (data) {
        console.log("mqtt publish: " + JSON.stringify(data))
        //mqtt_client.publish('/iot/grp1/web', JSON.stringify(data));
    });

    // Handle user activity update
    socket.on('update_userdb', async function (userData) {
        let member = await retrieveUserActivityData();
        member[0].events.push(userData)
        let update = await UserActivity.findOneAndUpdate({ member: member[0].member, session: member[0].session, }, { events: member[0].events }, { new: true });
        update.save((err,info) => {
            if(err) console.log(err)
        })
    });

    socket.on("get_sessions_data", async () => {
        socket.emit("send_sessions_data", await retrieveUserSessionsData());
    });

    socket.on("get_events_data", async () => {
        socket.emit("send_events_data", await retrieveUserEventsData());
    });

    socket.on('get_sensor_data', async (date_range) => {
        socket.emit('send_sensor_data',await sensorDataDates(date_range));
    })

});

// Passport config
require('./config/passport')(passport);

// EJS
app.use(express.static('./public/'))
app.set('view engine', 'ejs');

// Bodyparser
app.use(express.urlencoded({ extended: false }));

// Session
app.use(session({
    secret: 'secret',
    resave: true,
    saveUninitialized: true,
    store: mongoStore.create({ mongoUrl: db })
}));

// Passport middleware
app.use(passport.initialize());
app.use(passport.session());

// Connect flash
app.use(flash());

// Global vars
app.use((req, res, next) => {
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
