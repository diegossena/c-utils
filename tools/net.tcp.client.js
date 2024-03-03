const net = require('net')

const PORT = 8080

net
  .createServer((socket) => {
    console.log('socket')
    socket.end()
  })
  .listen(PORT, "127.0.0.1", () => {
    console.log(`listening on port ${PORT}`)
  })