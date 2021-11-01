// Global variables needed for the mqtt publish blocking
var blocking = false
var changed_during_blocking = false
var blocking_duration = 1000

// Global variables needed to control the popup timers
var popup_timer
var popup_duration = 3000

//Global variables needed to control the mqtt timeout timer
var mqtt_timer
var mqtt_duration = 10000
var mqtt_oneshot = true

/**
 * @function block_updates
 * @description control function which allows mqtt publishing only once per second. 
 *      If changes are made during the blocking period, it publishes the new changes after
 *      the blocking timer times out and goes into a blocking state again until no
 *      changes are made
 * @return no return
 **/
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

/**
 * @function handle_mqtt_update
 * @description Encapsulates block_updates() and update_mqtt()
 * @return no return
 **/
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

/**
 * @function error_popup
 * @description Creates a red popup when displaying an error message or mqtt inactivity
 * @param {Integer} code specifies error message
 * @return no return
 **/
function error_popup(code){
    let popup_container
    let popup

    // Clear timer for popups
    clearTimeout(popup_timer)

    // Create elements if they don't exist
    if(document.getElementById("popup") == null) {
        popup_container = document.createElement("div")
        popup_container.id = "popup_container"

        popup = document.createElement("p")
        popup.id = "popup"

        document.getElementById("main_content").appendChild(popup_container)
        popup_container.appendChild(popup)
    }

    // If they do, save them to var
    else {
        popup = document.getElementById("popup")
        popup_container = document.getElementById("popup_container")
        
    }

    // Set general class styling
    popup_container.className = "flex flex-row justify-center bg-red-100 absolute inset-x-0 top-0"

    // Handle error codes
    if (code == -1){
        popup.innerHTML = "Hardware failure :("

        // Start popup timer
        popup_timer = setTimeout(function () {
            document.getElementById("popup").remove()
            document.getElementById("popup_container").remove()
        }, popup_duration);
    }
    else if (code == -2){
        popup.innerHTML = "Timeout error :("

        // Start popup timer
        popup_timer = setTimeout(function () {
            document.getElementById("popup").remove()
            document.getElementById("popup_container").remove()
        }, popup_duration);
    }
    else if (code == -3){
        popup.innerHTML = "No mqtt data for a while now :("
    }
}

/**
 * @function success_popup
 * @description Creates a green popup when receiving mqtt data after a pause
 * @return no return
 **/
function success_popup(){
    let popup_container
    let popup

    // Clear timer for popups
    clearTimeout(popup_timer)

    // Create elements if they don't exist
    if(document.getElementById("popup") == null) {
        popup_container = document.createElement("div")
        popup_container.id = "popup_container"

        popup = document.createElement("p")
        popup.id = "popup"

        document.getElementById("main_content").appendChild(popup_container)
        popup_container.appendChild(popup)
    }

    // If they do, save them to var
    else {
        popup = document.getElementById("popup")
        popup_container = document.getElementById("popup_container")
    }

    // Set content and styling
    document.getElementById("popup").innerHTML = "Mqtt data received ! :)"
    document.getElementById("popup_container").className = "flex flex-row justify-center bg-green-100 absolute inset-x-0 top-0"

    // Start popup timer
    popup_timer = setTimeout(function () {
        document.getElementById("popup").remove()
        document.getElementById("popup_container").remove()
    }, popup_duration);
}

/**
 * @function mqtt_timeout
 * @description Starts a timer which creates a red popup notifying user of no mqtt data.
 * @return no return
 **/
function mqtt_timeout(){
    clearTimeout(mqtt_timer)

    mqtt_timer = setTimeout(function () {
        mqtt_oneshot = true
        error_popup(-3)
    }, mqtt_duration)
}

// Add onchange listener to the main dashboard inputs
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
    let current_info = document.getElementById("current_info")

    // Auto mode
    if (mode.checked) {
        current_info.innerHTML =  "Current pressure: " + document.getElementById("sliderPressure").value;
        pressure_container.hidden = false;
        speed_container.hidden = true;
    }
    
    // Manual mode
    else {
        current_info.innerHTML = "Current fan speed: " + document.getElementById("sliderSpeed").value;
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

// Create svg animation
anime({
    targets: ".fanSvg",
    easing: "linear",
    rotate: "1turn",
    loop: true,
    duration: 1000,
});

// Start first mqtt timeout
mqtt_timeout()