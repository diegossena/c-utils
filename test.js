const NODE_TYPE_INPUT = 0
const NODE_TYPE_HIDDEN = 1
const NODE_TYPE_OUTPUT = 2

function activation_relu() {

}

const inputs = [
  {
    id: 0,
    type: NODE_TYPE_INPUT,
    activation: activation_relu,
    value: 0
  },
  {
    id: 1,
    type: NODE_TYPE_INPUT,
    activation: activation_relu,
    value: 0
  },
]
const outputs = [
  {
    id: 2,
    type: NODE_TYPE_OUTPUT,
    activation: activation_relu,
    value: 0
  },
]

const nodes = [
  ...inputs,
  ...outputs,
  {
    id: 3,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    value: 0
  },
  {
    id: 4,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    value: 0
  },
  {
    id: 5,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    value: 0
  },
  {
    id: 6,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    value: 0
  },
]
const connections = [
  {
    node_in: nodes[0],
    node_out: nodes[3],
    weight: .5,
  },
  {
    node_in: nodes[1],
    node_out: nodes[3],
    weight: .5,
  },
  {
    node_in: nodes[3],
    node_out: nodes[4],
    weight: .5,
  },
  {
    node_in: nodes[3],
    node_out: nodes[5],
    weight: .5,
  },
  {
    node_in: nodes[5],
    node_out: nodes[2],
    weight: .5,
  },
  {
    node_in: nodes[5],
    node_out: nodes[6],
    weight: .5,
  },
  {
    node_in: nodes[6],
    node_out: nodes[2],
    weight: .5,
  },
]

// required_for_output
const required_connections = []
let layer = [nodes[2]]
while (true) {
  const layer_connections = connections
    .filter(connection => (
      layer.includes(connection.node_out)
      && !layer.includes(connection.node_in)
    ))
  console.log({
    layers: layer.map(connection => connection.id),
    layer_connections: layer_connections.map(connection => ({
      node_in: connection.node_in.id,
      node_out: connection.node_out.id,
    })),
    connections_in: layer_connections
      .filter(connection => !inputs.includes(connection.node_in))
      .map(connection => connection.node_in.id)
  })
  if (!layer_connections.length)
    break
  required_connections.push(...layer_connections)
  layer = layer_connections
    .filter(connection => !inputs.includes(connection.node_in))
    .map(connection => connection.node_in)
  if (!layer.length)
    break

}
required_connections.reverse()
console.log('required', required_connections)
// node_inputs
const node_inputs = new Map([])
for (const connection of required_connections) {
  if (node_inputs.has(connection.node_out.id)) {
    node_inputs.get(connection.node_out.id).push({ in: connection.node_in, w: connection.weight })
  } else {
    node_inputs.set(connection.node_out.id, [{ in: connection.node_in, w: connection.weight }])
  }
}
for (const [key, connections] of node_inputs.entries()) {
  const ids = connections.map(connection => connection.in.id)
  console.log('node_input', key, ids)
}
// node_evals
