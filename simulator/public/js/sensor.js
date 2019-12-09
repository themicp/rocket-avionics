const delay = async (ms) => {
  return new Promise((resolve) => {
    setTimeout(resolve, ms)
  })
}

const socket = new WebSocket('ws://localhost:8080');

socket.addEventListener('open', (event) => {
  console.log('Connected to WebSocket server')
}); 

socket.addEventListener('message', async (eventStr) => {
  event = JSON.parse(eventStr.data)
  if (event.error) {
    console.error(event.error)
  }

  if (event.type === 'stream') {
    setPitch(event.data.pitch)
    // setYaw(event.data.yaw)
    setRoll(event.data.roll)
  } else if (event.type === 'replay') {
    lines = event.data
    previousTimePoint = 0
    for (const line of lines) {
      values = line.split(',')
      time = values[0]
      pitch = values[1]
      yaw = values[2]
      roll = values[3]

      if (previousTimePoint === 0) {
        previousTimePoint = time
      }

      await delay(time - previousTimePoint)
      previousTimePoint = time

      try {
        setPitch(pitch)
        // setYaw(yaw)
        setRoll(roll)
      } catch (e) {
        console.error(e)
      }
    }
  }
});
