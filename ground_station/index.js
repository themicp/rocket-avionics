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
      battery = data[2] ? parseFloat(data[2]) : null
      state = data[3].replace(/\n|\r/g, '')
      agl = data[4] ? parseFloat(data[4]) : null
      accX = data[5] ? parseFloat(data[5]) : null
      accY = data[6] ? parseFloat(data[6]) : null
      accZ = data[7] ? parseFloat(data[7]) : null
      gyroX = data[8] ? parseFloat(data[8]) : null
      gyroY = data[9] ? parseFloat(data[9]) : null
      gyroZ = data[10] ? parseFloat(data[10]) : null

      verticalVelocity = null
      if (agl != null && previousData && previousData[4]) {
        dt = met - previousData[0]
        scale = 1000 / dt
        diff = agl - parseFloat(previousData[4])
        verticalVelocity = diff * scale // meters per second
      }

      connection.query('INSERT INTO raw SET ?', {
        met, free_memory: freeMemory, battery, state,
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
