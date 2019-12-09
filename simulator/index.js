const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const express = require('express')
const WebSocket = require('ws');
const fs = require('fs')
const app = express()

SERVER_PORT = 3000
WS_PORT = 8080

if (!process.env.PORT_PATH && !process.env.IMU_LOGS) {
  throw new Error('Provide PORT_PATH or IMU_LOGS in env.')
}

let parser, serialPort
if (process.env.PORT_PATH) {
  parser = new Readline()
  serialPort = new SerialPort(process.env.PORT_PATH, { baudRate: 9600 })
  serialPort.pipe(parser)
}

app.use(express.static('public'))
app.listen(SERVER_PORT, () => {
  console.log(`HTTP Server listening on port ${SERVER_PORT}.`)
  console.log(`Setting WS Server on port ${WS_PORT}`)

  const wss = new WebSocket.Server({port: WS_PORT});
  wss.on('connection', (ws) => {
    if (process.env.PORT_PATH) {
      parser.on('data', line => {
        [pitch, yaw, roll] = line.split(',').map(value => parseFloat(value))
        ws.send(JSON.stringify({type: 'stream', data: {pitch, yaw, roll}}));
      })
    } else {
      fs.readFile(process.env.IMU_LOGS, {encoding: 'utf-8'}, async (err, data) => {
        if (!err) {
          ws.send(JSON.stringify({type: 'replay', data: data.split('\n')}))
        } else {
          ws.send(JSON.stringify({error: 'Error reading IMU logs'}))
          ws.close()
        }
      })
    }
  })
})
