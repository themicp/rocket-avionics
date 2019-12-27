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

dataEmitter.on('data', data => {
  if (data.indexOf('RAW:') == 0) {
    data = data.replace('RAW:', '')
    try {
      data = data.split(',')
      met = +data[0]
      freeMemory = parseInt(data[1], 10)
      state = data[2].replace(/\n|\r/g, '')
      groundLevel = data[3] ? parseFloat(data[3]) : null
      agl = data[4] ? parseFloat(data[4]) : null
      altitude = data[5] ? parseFloat(data[5]) : null
      pressure = data[6] ? parseFloat(data[6]) : null
      accX = data[7] ? parseFloat(data[7]) : null
      accY = data[8] ? parseFloat(data[8]) : null
      accZ = data[9] ? parseFloat(data[9]) : null
      gyroX = data[10] ? parseFloat(data[10]) : null
      gyroY = data[11] ? parseFloat(data[11]) : null
      gyroZ = data[12] ? parseFloat(data[12]) : null

      connection.query('INSERT INTO raw SET ?', {
        met, free_memory: freeMemory, state,
        ground_level: groundLevel, agl, altitude, pressure,
        acc_x: accX, acc_y: accY, acc_z: accZ,
        gyro_x: gyroX, gyro_y: gyroY, gyro_z: gyroZ,
        created_at: new Date()
      }, (err) => {
        if (err) console.log(err)
      })
    } catch (e) {
      console.log(e)
    }
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
