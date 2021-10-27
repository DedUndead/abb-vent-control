//	init socket
const socket = io("localhost:3000");

//	important elements
const pressureSlider = document.getElementById('sliderPressure');
const displayPressure = document.getElementById('displayPressure');
const pressureContainer = document.getElementById('sliderPressureContainer');

const speedSlider = document.getElementById('sliderSpeed');
const displaySpeed = document.getElementById('displaySpeed');
const speedContainer = document.getElementById('sliderSpeedContainer');

let mode = document.getElementById('toggleBtn')
const currentPressure = document.getElementById("pressure")

socket.on("connect", () => {  
	console.log(socket.id); 
});

// update stats realtime
socket.on("stats", (data) => {
	// set current pressure
	currentPressure.innerHTML = "Current pressure: " + data.pressure + "Pa";

	// set slider values
	pressureSlider.value = data.setpoint;
	displayPressure.innerHTML = data.setpoint + "Pa";

	speedSlider.value = data.speed;
	displaySpeed.innerHTML = data.speed + "%";

	// show correct slider
	if(data.mode === true) {
		mode.checked = true;

		// show pressure
		pressureContainer.hidden = false;
		speedContainer.hidden = true;
	}
	else {
		mode.checked = false;

		// show speed
		pressureContainer.hidden = true;
		speedContainer.hidden = false;
	}
});

function update_mqtt(){
	let data = {
		speed : speedSlider.value,
		setpoint : pressureSlider.value,
		mode : mode.value
	}
	socket.emit("update", data)
	console.log(data.mode)
}


document.getElementById('sliderPressure').addEventListener('change', (event) => {
	userData = 
		{
			type:"PRESSURE_SLIDER",
			description:"Pressure slider changed value",
			value: event.target.value
		};

	socket.emit("user_activity", userData);
	update_mqtt();
});

document.getElementById('sliderSpeed').addEventListener('change', (event) => {
	userData = 
		{
			type:"SPEED_SLIDER",
			description:"Speed slider changed value",
			value: event.target.value
		};

	socket.emit("user_activity", userData);
	update_mqtt();
});

document.getElementById('toggleBtn').addEventListener('change', (event) => {
	userData = 
		{
			type:"TOGGLE_BUTTON",
			description:"Toggle button has been pressed",
			value: event.target.checked
		};

	socket.emit("user_activity", userData);
	update_mqtt();
});