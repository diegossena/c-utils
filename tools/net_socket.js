const { createSecureServer } = require('http2')
const net = require('net')

const socket = new net.Socket({
  host: 'localhost',
  port: 123,
  readable: true,
  writable: true,
})
socket.write()
socket.read()
socket.end()

const server = new net.Server()
server.listen(9000)
server.close()