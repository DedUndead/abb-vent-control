const socket = io("localhost:3000");

socket.on("connect", () => {  
	console.log(socket.id); 
});

socket.on("stats", (data) => {
	let pressureSlider = document.getElementById('sliderPressure');
	let displayPressure = document.getElementById('displayPressure');
	let pressureContainer = document.getElementById('sliderPressureContainer');
	
	let sliderSpeed = document.getElementById('sliderSpeed');
	let displaySpeed = document.getElementById('displaySpeed');
	let speedContainer = document.getElementById('sliderSpeedContainer');

	let mode = document.getElementById('toggleBtn')


	console.log(data.pressure)
	document.getElementById("pressure").innerHTML = "Current pressure: " + data.pressure + "Pa";

	pressureSlider.value = data.setpoint;
	displayPressure.innerHTML = data.setpoint + "Pa";

	sliderSpeed.value = data.speed;
	displaySpeed.innerHTML = data.speed + "%";
	if(data.auto === true) {
		mode.checked = true;
		pressureContainer.hidden = false;
		speedContainer.hidden = true;
	}
	else {
		mode.checked = false;
		pressureContainer.hidden = true;
		speedContainer.hidden = false;
	}


});

// Template function which emits to server
/*const update = async () => {
	socket.emit("update", data)
}*/

const userActivity = async (userData) => {
	socket.emit("user_activity", userData)
}


document.getElementById('sliderPressure').addEventListener('change', (event) => {
	userData = 
		{
			type:"PRESSURE_SLIDER",
			description:"Pressure slider changed value",
			value: event.target.value
		};

		userActivity(userData);
});

document.getElementById('sliderSpeed').addEventListener('change', (event) => {
	userData = 
		{
			type:"SPEED_SLIDER",
			description:"Speed slider changed value",
			value: event.target.value
		};

		userActivity(userData);
});

document.getElementById('toggleBtn').addEventListener('change', (event) => {
	userData = 
		{
			type:"TOGGLE_BUTTON",
			description:"Toggle button has been pressed",
			value: event.target.checked
		};

		userActivity(userData);
});
//timer = setInterval(update, 5000);*/