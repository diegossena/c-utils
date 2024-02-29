const http = require('http')
http
  .createServer((_request, response) => {
    response.end('text')
  })
  .listen(8080)