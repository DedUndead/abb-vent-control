const socket = io("localhost:3000");

/**
 * @function get_random_color
 * @description generate random colors
 * @return {String} returns a random color string
 **/
function get_random_color() {
    var letters = '0123456789ABCDEF';
    var color = '#';
    for (var i = 0; i < 6; i++) {
        color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
}

// Retrieves the user sessions data from the database
socket.emit("get_sessions_data");
socket.on("send_sessions_data", (user_sessions) => {
    line_chart("pressure_line_chart_container","myPressureLineChart",0, 0, "Pressure Line Chart");
    line_chart("speed_line_chart_container","mySpeedLineChart",0, 0, "Speed Line Chart");

    const ctx = document.getElementById("myBarChart").getContext("2d");
    let x_axis_members = [];
    let y_axis_sessions = [];
    let colors = []
    let datasetConfig = []

    user_sessions.forEach(session_data => {
        x_axis_members.push(session_data._id.replace(',', ' '));
        y_axis_sessions.push(session_data.sessions)
        colors.push(get_random_color());
    })

    for (let i = 0; i < x_axis_members.length; i++) {
        datasetConfig.push({
            label: x_axis_members[i],
            data: [y_axis_sessions[i]],
            backgroundColor: colors[i],
            borderColor: colors[i],
            borderWidth: 1
        })

    }

    const data = {
        labels: ['Sessions'],
        datasets: datasetConfig
    };
    const config = {
        type: 'bar',
        data: data,
        options: {
            plugins: {
                legend: {
                    display: true,
                    position: 'bottom',
                },
                title: { display: true, text: 'Sessions Per User' },
                responsive: true,
            },
            scales: { y: { beginAtZero: true } }
        }
    };
    const myChart = new Chart(ctx, config);
})  

// Retrieves the user events data from the database
socket.emit("get_events_data");
socket.on("send_events_data", (user_events) => {

    const ctx = document.getElementById("myPieChart").getContext("2d");
    let legend = [];
    let event_count = [];
    let pie_colors = []
    let datasetConfig = []

    console.log(user_events);

    user_events.forEach(event_data => {
        legend.push(event_data._id.replace(',', ' '));
        event_count.push(event_data.events)
        pie_colors.push(get_random_color());
    })

    const data = {
        labels: legend,
        datasets: [{
            data: event_count,
            backgroundColor: pie_colors,
            hoverOffset: 4
        }]
    };
    const config = {
        type: 'doughnut',
        data: data,
        options: {
            plugins: {
                legend: {
                    display: true,
                    position: 'bottom',
                },
                title: { display: true, text: 'Events Per User' },
                responsive: true,
            }
        }
    };
    const myChart = new Chart(ctx, config);
});

// Setting starting time form
const start_form = document.getElementById('form_start');
let start_user_date = new Date(), LocalTimeOffset = start_user_date.getTimezoneOffset();
start_user_date.setMinutes(start_user_date.getMinutes() - LocalTimeOffset);
start_user_date.setSeconds(0);
start_user_date.setMilliseconds(0);
start_form.start_current_date.valueAsDate = start_form.start_current_time.valueAsDate = start_user_date;

// Setting target time form
const form_target = document.getElementById('form_target');
let target_user_date = new Date(), LocalTimeOffset_target = target_user_date.getTimezoneOffset();
target_user_date.setMinutes((target_user_date.getMinutes() - LocalTimeOffset_target) + 1);
target_user_date.setSeconds(0);
target_user_date.setMilliseconds(0);
form_target.target_date.valueAsDate = form_target.target_time.valueAsDate = target_user_date;

// On click sends user input to the server
const submit_date = document.getElementById('submit');
submit_date.addEventListener("click", (event) => {
    // Set input date
    let user_date_input = {
        start_date: start_form.start_current_date.value,
        start_time: start_form.start_current_time.value,
        target_date: form_target.target_date.value,
        target_time: form_target.target_time.value
    }
    socket.emit('get_sensor_data', user_date_input)
});

// Retrives sensor data from the server
socket.on('send_sensor_data', (sensor_data) => {

    let x_axis = [];
    let y_pressure_axis = [];
    let y_speed_axis = [];

    sensor_data.forEach(data => {
        x_axis.push(new Date(data.createdAt).toISOString());
        y_pressure_axis.push(data.pressure);
        y_speed_axis.push(data.speed);
    });
    line_chart("pressure_line_chart_container","myPressureLineChart",x_axis, y_pressure_axis, "Pressure Line Chart");
    line_chart("speed_line_chart_container","mySpeedLineChart",x_axis, y_speed_axis, "Speed Line Chart");
})


/**
 * @function line_chart
 * @description Creates line chart
 * @param {String} id_name_div gets the div element in the html page
 * @param {String} id_name_canva gets the canvas element in the html page
 * @param {Array} x_axis sets the x axis of the line chart
 * @param {Array} y_axis sets the y axis of the line chart
 * @param {String} title_name sets the title of the line chart
 * @return no return
 **/
function line_chart(id_name_div, id_name_canva, x_axis, y_axis, title_name) {
    reset_canva(id_name_div, id_name_canva);
    const ctx = document.getElementById(id_name_canva).getContext("2d");

    const data = {
        labels: x_axis,
        datasets: [{
            data: y_axis,
            fill: false,
            borderColor: 'rgb(75, 192, 192)',
            tension: 0.1
        }]
    };
    const config = {
        type: 'line',
        data: data,
        options: {
            plugins: {
                title: { display: true, text: title_name },
                legend: { display: false }
            },
            scales: {
                x: {
                    ticks: {
                        type: 'time',
                        // For a category axis, the val is the index so the lookup via getLabelForValue is needed
                        callback: function (val, index) {
                            // Hide the label of every 2nd dataset
                            if (index > 0 && index < x_axis.length -1) {
                                console.log("inside if" + val + "\n")
                                return '';
                            }
                            else {
                                console.log("inside else" + val + "\n")
                                return this.getLabelForValue(val);
                            }
                        },
                        autoSkip: false, 
                        maxTicksLimit: 2,
                        font: {
                            size: 9
                        },
                    }
                },
                y: {
                    beginAtZero: true,
                    ticks: {
                        stepSize: 5
                    }
                }
            },
            elements: {
                point: {
                    radius: 0
                }
            }
        }
    };

    let lineChart = new Chart(ctx, config);
}


/**
 * @function reset_canva
 * @description resets the canvas element to enable pushing new data to line charts
 * @param {String} id_name_div gets the div element in the html page
 * @param {String} id_name_canva gets the canvas element in the html page
 * @return no return
 **/
function reset_canva(id_name_div,id_name_canva) {
    let div = document.getElementById(id_name_div);
    let canvas = document.createElement('canvas');

    while (div.firstChild) {
        div.removeChild(div.firstChild);
    }
    canvas.setAttribute('id', id_name_canva);
    canvas.setAttribute('class', 'h-full w-full' );
    div.appendChild(canvas);
}