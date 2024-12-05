const dgram = require('node:dgram')
const udp = dgram.createSocket('udp4')

udp.send('test', 9100, 'localhost', () => udp.close())
