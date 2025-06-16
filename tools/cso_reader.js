const fs = require('fs')

const vs_cso_path = `share/vs.cso`
const ps_cso_path = `share/ps.cso`

const vs_cso = '{ '
  + fs.readFileSync(vs_cso_path, 'hex')
    .match(/.{2}/g)
    .map(hex => `0x${hex}`)
    .join(', ')
  + ' };'
const ps_cso = '{ '
  + fs.readFileSync(ps_cso_path, 'hex')
    .match(/.{2}/g)
    .map(hex => `0x${hex}`)
    .join(', ')
  + ' };'

console.log('VS', vs_cso)
console.log('PS', ps_cso)