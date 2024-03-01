const http = require('http')
const http_server = new http.Server()
http_server.
  http_server.listen(3000)

http.request({

})

const request = new http.ClientRequest({
  port: 80,
  path: '/',
  timeout: 0,
  method: 'GET',
})
request.setHeader("key", "value")
request.getHeader("key")
request.appendHeader
request.write()
request.end()