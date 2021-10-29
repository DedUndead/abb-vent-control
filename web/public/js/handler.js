//  Create svg animation
var fan_speed = anime({
    targets: ".fanSvg",
    easing: "linear",
    rotate: "1turn",
    loop: true,
    duration: 1000,
});

var blocking = false
var changed_during_blocking = false
var blocking_duration = 1000
function block_updates() {
    blocking = true;
    changed_during_blocking = false
    setTimeout(function () {
        if (changed_during_blocking == true){
            console.log("updated with new data and blocking again...")
            update_mqtt()
            block_updates()
        }
        else{
            console.log("unblocked...")
            blocking = false;
        }
    }, blocking_duration);
}

function handle_mqtt_update(){
    if (blocking == false) {
        console.log("updating and blocking...")
        update_mqtt();
        block_updates();
    }
    else{
        console.log("will update after unblock...")
        changed_during_blocking = true
    }
}

function refresh_switch(){
    let pressure_container = document.getElementById("sliderPressureContainer");
    let speed_container = document.getElementById("sliderSpeedContainer");
    let mode_switch = document.getElementById("toggleBtn");

    // mode_switch.checked = true
    pressure_container.hidden = false;
    speed_container.hidden = true;
}

document.getElementById("sliderPressure").addEventListener("change", (event) => {
    let new_pressure = event.target.value

    document.getElementById("displayPressure").innerHTML = new_pressure + "Pa";
    update_userdb(
        "PRESSURE_SLIDER",
        "Pressure slider changed value",
        new_pressure
    );
    handle_mqtt_update()
});

document.getElementById("sliderSpeed").addEventListener("change", (event) => {
    let new_speed = event.target.value

    document.getElementById("displaySpeed").innerHTML = new_speed + "%";
    update_userdb(
        "SPEED_SLIDER",
        "Speed slider changed value",
        new_speed
    );
    handle_mqtt_update()
});

document.getElementById("toggleBtn").addEventListener("change", (event) => {

    let mode = document.getElementById('toggleBtn')
    let pressure_container = document.getElementById("sliderPressureContainer");
    let speed_container = document.getElementById("sliderSpeedContainer");

    if (mode.checked) {
        pressure_container.hidden = false;
        speed_container.hidden = true;
    }
    else {
        pressure_container.hidden = true;
        speed_container.hidden = false;
    }

    update_userdb(
        "TOGGLE_BUTTON",
        "Toggle button has been pressed",
        event.target.checked
    );
    handle_mqtt_update()
});

// refresh_switch()