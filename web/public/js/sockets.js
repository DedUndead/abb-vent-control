// Initiate websockets
const socket = io("localhost:3000");

// Handle updates from MCU
socket.on("stats", (data) => {
    
    // Handle mqtt timeout
    if(mqtt_oneshot == true){
        console.log("[*] calling success popup")
        mqtt_oneshot = false
        success_popup()
    }

    // Update dashboard if status is correct
    if(data.status == 1 || data.status == 0){
        // Save elements to variables
        let current_pressure = document.getElementById("pressure")
        let mode = document.getElementById('toggleBtn')

        let pressure_slider = document.getElementById('sliderPressure');
        let display_pressure = document.getElementById('displayPressure');
        let pressure_container = document.getElementById('sliderPressureContainer');

        let speed_slider = document.getElementById('sliderSpeed');
        let display_speed = document.getElementById('displaySpeed');
        let speed_container = document.getElementById('sliderSpeedContainer');

        // Update current pressure
        current_pressure.innerHTML = "Current pressure: " + data.pressure + "Pa";

        // Ypdate slider values
        pressure_slider.value = data.setpoint;
        speed_slider.value = data.speed;
        display_pressure.innerHTML = data.setpoint + "Pa";
        display_speed.innerHTML = data.speed + "%";

        // Update correct slider
        if(data.mode == 1) {
            mode.checked = true;

            // Show pressure
            pressure_container.hidden = false;
            speed_container.hidden = true;
        }
        else {
            mode.checked = false;

            // Show speed
            pressure_container.hidden = true;
            speed_container.hidden = false;
        }
    }

    // Show popups
    else if (data.status == -1 || data.status == -2) {
        error_popup(data.status)
    }

    // Refresh mqtt timeout
    mqtt_timeout()
});

/**
 * @function update_mqtt
 * @description Get data from the dashboard and publish to mqtt
 * @return no return
 **/
function update_mqtt(){
	//	Get necessary elements
	let speed_slider = document.getElementById('sliderSpeed');
	let pressure_slider = document.getElementById('sliderPressure');
	let mode = document.getElementById('toggleBtn')

	let mqtt_data = {
		speed : + speed_slider.value,
		setpoint : + pressure_slider.value,
		mode : + mode.checked
	}
	socket.emit("update_mqtt", mqtt_data)
}

/**
 * @function update_userdb
 * @description Emits user action data to be updated in the db to the server
 * @param {String} type_ String specifying action type
 * @param {String} desc_ String specifying action description
 * @param {Integer} val_ New value of the updated element
 * @return no return
 **/
function update_userdb(type_, desc_, val_){
	let user_data = {
		type: type_,
		description: desc_,
		value: val_
	};
	socket.emit("update_userdb", user_data);
}