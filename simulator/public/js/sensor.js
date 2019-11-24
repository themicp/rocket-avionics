const socket = new WebSocket('ws://localhost:8080');

socket.addEventListener('open', (event) => {
  console.log('Connected to WebSocket server')
});

socket.addEventListener('message', (event) => {
  data = JSON.parse(event.data)

  setPitch(-data.pitch)
  setYaw(data.yaw)
  setRoll(data.roll)
});
