const fs = require('fs')

const name = 'texture'

const vs_cso = '{ '
  + fs.readFileSync(`assets/${name}.vs.cso`, 'hex')
    .match(/.{2}/g)
    .map(hex => `0x${hex}`)
    .join(', ')
  + ' };'
const ps_cso = '{ '
  + fs.readFileSync(`assets/${name}.ps.cso`, 'hex')
    .match(/.{2}/g)
    .map(hex => `0x${hex}`)
    .join(', ')
  + ' };'

console.log('VS', vs_cso)
console.log('PS', ps_cso)