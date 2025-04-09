const fs = require('fs')

const vs_texture_cso = '{ '
  + fs.readFileSync('assets/texture.vs.cso', 'hex')
    .match(/.{2}/g)
    .map(hex => `0x${hex}`)
    .join(', ')
  + ' };'
const ps_texture_cso = '{ '
  + fs.readFileSync('assets/texture.ps.cso', 'hex')
    .match(/.{2}/g)
    .map(hex => `0x${hex}`)
    .join(', ')
  + ' };'
console.log('VS', vs_texture_cso)
console.log('PS', ps_texture_cso)