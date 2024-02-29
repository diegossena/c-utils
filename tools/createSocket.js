const net = require('net')

const socket = net.createConnection({
  host: 'localhost',
  port: 123,
  readable: false,
  writable: false
})

socket.readable