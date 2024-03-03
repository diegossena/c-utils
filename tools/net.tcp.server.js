const http = require('http')

http.request({
  host: '127.0.0.1',
  port: 8080
}, response => {
  console.log(response)
})