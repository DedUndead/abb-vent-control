//	init socket
const socket = io("localhost:3000");

socket.on("connect", () => {  
    console.log("Socket id: " + socket.id); 
});

//	Update stats realtime
socket.on("stats", (data) => {

	let current_pressure = document.getElementById("pressure")
	let mode = document.getElementById('toggleBtn')

	let pressure_slider = document.getElementById('sliderPressure');
	let display_pressure = document.getElementById('displayPressure');
	let pressure_container = document.getElementById('sliderPressureContainer');

	let speed_slider = document.getElementById('sliderSpeed');
	let display_speed = document.getElementById('displaySpeed');
	let speed_container = document.getElementById('sliderSpeedContainer');

	// update current pressure
	current_pressure.innerHTML = "Current pressure: " + data.pressure + "Pa";

	// update fan svg
	// updte_fan_svg()

	// update slider values
	pressure_slider.value = data.setpoint;
	speed_slider.value = data.speed;
	display_pressure.innerHTML = data.setpoint + "Pa";
	display_speed.innerHTML = data.speed + "%";

	//	update correct slider
	if(data.mode == 1) {
		mode.checked = true;

		// show pressure
		pressure_container.hidden = false;
		speed_container.hidden = true;
	}
	else {
		mode.checked = false;

		// show speed
		pressure_container.hidden = true;
		speed_container.hidden = false;
	}
});

//	Emit data needed to update the MCU
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

//	Emit data needed tu update the user activity db
function update_userdb(type_, desc_, val_){
	let user_data = {
		type: type_,
		description: desc_,
		value: val_
	};
	socket.emit("update_userdb", user_data);
}
