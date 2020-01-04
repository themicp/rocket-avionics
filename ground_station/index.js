const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const express = require('express')
const WebSocket = require('ws');
const { EventEmitter } = require('events')
const app = express()
const mysql = require('mysql')

SERVER_PORT = 3210
WS_PORT = 8080

if (!process.env.PORT_PATH) {
  throw new Error('Provide PORT_PATH in env.')
}

const connection = mysql.createConnection({
  host     : 'localhost',
  user     : '',
  password : '',
  database : ''
});

connection.connect();

const dataEmitter = new EventEmitter();
const parser = new Readline()
const serialPort = new SerialPort(process.env.PORT_PATH, { baudRate: 9600 })
serialPort.pipe(parser)

parser.on('data', (line) => {
  dataEmitter.emit('data', line)
})

const COMMANDS = {
  init_calibration: 1,
  launched: 3,
  trigger_fts: 6
}

previousData = null
dataEmitter.on('data', data => {
  if (data.indexOf('RAW:') == 0) {
    data = data.replace('RAW:', '')
    try {
      data = data.split(',')
      met = +data[0]
      freeMemory = parseInt(data[1], 10)
      state = data[2].replace(/\n|\r/g, '')
      agl = data[3] ? parseFloat(data[3]) : null
      accX = data[4] ? parseFloat(data[4]) : null
      accY = data[5] ? parseFloat(data[5]) : null
      accZ = data[6] ? parseFloat(data[6]) : null
      gyroX = data[7] ? parseFloat(data[7]) : null
      gyroY = data[8] ? parseFloat(data[8]) : null
      gyroZ = data[9] ? parseFloat(data[9]) : null

      verticalVelocity = null
      if (agl != null && previousData) {
        dt = met - previousData[0]
        scale = 1000 / dt
        diff = agl - parseFloat(previousData[3])
        verticalVelocity = diff * scale // meters per second
      }

      connection.query('INSERT INTO raw SET ?', {
        met, free_memory: freeMemory, state,
        ground_level: null, agl, altitude: null, pressure: null,
        acc_x: accX, acc_y: accY, acc_z: accZ,
        gyro_x: gyroX, gyro_y: gyroY, gyro_z: gyroZ,
        vertical_velocity: verticalVelocity,
        created_at: new Date()
      }, (err) => {
        if (err) console.log(err)
      })
    } catch (e) {
      console.log(e)
    }

    previousData = data
  }
})

app.use(express.static('public'))
app.listen(SERVER_PORT, () => {
  console.log(`HTTP Server listening on port ${SERVER_PORT}.`)
  console.log(`Setting WS Server on port ${WS_PORT}`)

  const wss = new WebSocket.Server({port: WS_PORT});
  wss.on('connection', (ws) => {
    dataEmitter.on('data', line => ws.send(line))

    ws.on('message', message => {
      if (message.indexOf('COMMAND') === 0) {
        command = message.split('COMMAND:')[1].toLowerCase()
        if (typeof COMMANDS[command] !== 'undefined') {
          serialPort.write("CMD:" + COMMANDS[command], function(err) {
            if (err) {
              return console.log('Error on write: ', err.message)
            }
          })
        }
      }
    })
  })
})
