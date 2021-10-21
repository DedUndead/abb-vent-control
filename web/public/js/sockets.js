const socket = io("localhost:3000");

socket.on("connect", () => {  
	console.log(socket.id); 
});

socket.on("stats", (data) => {
    // HANDLE INCOMING DATA
	console.log(data)
});

// Template function which emits to server
const update = async () => {
	data = {
		one: "one",
		two: "two",
		three: "three"
	}
	console.log(data)
	socket.emit("update", data)
}

timer = setInterval(update, 5000);