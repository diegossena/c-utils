const net = require('net')

const PORT = 8080

if (false) {
  const socket = new net.Socket()

  socket.on('connect', () => {
    socket.write('buffer', callback => {

    })
    socket.read('read', callback => {

    })
  })
  socket.on('data', () => {

  })
  socket.on('timeout', () => {

  })
  socket.on('close', () => {

  })

  socket.connect({ host: '127.0.0.1', port: 8080 })
}


if (true) {
  const server = new net.Server()
  server.listen(PORT)
  server.on('listening', () => {
    console.log(`listening ${PORT}`)
  })
  server.on('connection', socket => {
    console.log('connection')
    socket.on('close', () => {
      console.log('close')
    })
    socket.on('connect', () => {
      console.log('connect')
    })
    socket.on('data', chunk => {
      console.log('data', chunk.toString('utf-8'))
      socket.write(Buffer.from('Hello Client').toString('utf-8'))
    })
    socket.on('error', console.error)
    socket.on('timeout', () => {
      console.log('timeout')
    })
  })
}