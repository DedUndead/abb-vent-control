const socket = io("localhost:3000");

socket.on("connect", () => {  
	console.log(socket.id); 
});

socket.on("stats", (data) => {
    // HANDLE INCOMING DATA
	console.log(data)
});