const fs = require('fs')

const name = 'texture'

const vs_cso_path = `assets/${name}.vs.cso`
const ps_cso_path = `assets/${name}.ps.cso`

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

fs.rmSync(vs_cso_path)
fs.rmSync(ps_cso_path)