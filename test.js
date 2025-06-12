const NODE_TYPE_INPUT = 0
const NODE_TYPE_HIDDEN = 1
const NODE_TYPE_OUTPUT = 2

/**
 * @param {number[]} x 
 * @returns {number}
 */
function sum_aggregation(x) {
  return x.reduce((acc, cur) => acc + cur, 0)
}
/**
 * @param {number} x 
 * @returns {number}
 */
function activation_relu(x) {
  return Math.max(0, x)
}

const net_inputs = [
  {
    id: 0,
    type: NODE_TYPE_INPUT,
    bias: 0,
    activation: activation_relu,
    aggregation: sum_aggregation,
    value: Math.round(Math.random() * 100)
  },
  {
    id: 1,
    type: NODE_TYPE_INPUT,
    bias: 0,
    activation: activation_relu,
    aggregation: sum_aggregation,
    value: Math.round(Math.random() * 100)
  },
]
const net_outputs = [
  {
    id: 2,
    type: NODE_TYPE_OUTPUT,
    bias: 0,
    activation: activation_relu,
    aggregation: sum_aggregation,
    value: 0
  },
]

const net_nodes = [
  ...net_inputs,
  ...net_outputs,
  {
    id: 3,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    aggregation: sum_aggregation,
    bias: 0,
    value: 0
  },
  {
    id: 4,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    aggregation: sum_aggregation,
    bias: 0,
    value: 0
  },
  {
    id: 5,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    aggregation: sum_aggregation,
    bias: 0,
    value: 0
  },
  {
    id: 6,
    type: NODE_TYPE_HIDDEN,
    activation: activation_relu,
    aggregation: sum_aggregation,
    bias: 0,
    value: 0
  },
]
const net_connections = [
  {
    node_in: net_nodes[0],
    node_out: net_nodes[3],
    weight: .5,
  },
  {
    node_in: net_nodes[1],
    node_out: net_nodes[3],
    weight: .5,
  },
  {
    node_in: net_nodes[3],
    node_out: net_nodes[4],
    weight: .5,
  },
  {
    node_in: net_nodes[4],
    node_out: net_nodes[5],
    weight: .5,
  },
  {
    node_in: net_nodes[3],
    node_out: net_nodes[5],
    weight: .5,
  },
  {
    node_in: net_nodes[5],
    node_out: net_nodes[2],
    weight: .5,
  },
  {
    node_in: net_nodes[5],
    node_out: net_nodes[6],
    weight: .5,
  },
  {
    node_in: net_nodes[5],
    node_out: net_nodes[3],
    weight: .5,
  },
  {
    node_in: net_nodes[6],
    node_out: net_nodes[2],
    weight: .5,
  },
]

// required_for_output
const required_nodes = [...net_outputs]
if (true) {
  while (true) {
    const layer_nodes = net_connections
      .filter(connection => (
        required_nodes.includes(connection.node_out)
        && !required_nodes.includes(connection.node_in)
        && !net_inputs.includes(connection.node_in)
      ))
      .map(connection => connection.node_in)
    if (!layer_nodes.length) {
      break
    }
    required_nodes.push(...layer_nodes)
  }
} else {
  let processed_connections = [...net_outputs]
  while (true) {
    const t = net_connections
      .filter(connection => (
        processed_connections.includes(connection.node_out)
        && !processed_connections.includes(connection.node_in)
      ))
      .map(connection => connection.node_in)
    if (!t.length) {
      break
    }
    const layer_nodes = t.filter(node => !net_inputs.includes(node))
    if (!layer_nodes.length) {
      break
    }
    required_nodes.push(...layer_nodes)
    processed_connections.push(...t)
  }
}
required_nodes.reverse()
console.log('required_nodes', required_nodes)
if (true) {
  const required_connections = net_connections.filter(connection => (
    required_nodes.includes(connection.node_in)
    || required_nodes.includes(connection.node_out)
  ))
  for (const connection of required_connections) {

  }
  console.log('required_connections', required_connections)
} else {
  // node_evals
  const node_inputs = new Map([])
  for (const connection of net_connections) {
    if (!required_nodes.includes(connection.node_in) || !required_nodes.includes(connection.node_out))
      continue
    if (node_inputs.has(connection.node_out)) {
      node_inputs.get(connection.node_out).links.push({ in: connection.node_in, w: connection.weight })
    } else {
      node_inputs.set(connection.node_out, {
        node: connection.node_out,
        links: [{ in: connection.node_in, w: connection.weight }]
      })
    }
  }
  const node_evals = [...node_inputs.values()]
  console.log('node_evals', node_evals)
  // activate
  for (const node_eval of node_evals) {
    const { node, links } = node_eval
    const node_inputs = links.map(link => link.in.value * link.w)
    const s = node.aggregation(node_inputs)
    node.value = node.activation(node.bias + node.value + s)
  }
  console.log('net_outputs', net_outputs)
}