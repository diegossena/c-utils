const NODE_TYPE_INPUT = 0
const NODE_TYPE_HIDDEN = 1
const NODE_TYPE_OUTPUT = 2


/** 
 * @typedef {object} NeatNode
 * @property {number} id
 * @property {number} type
 * @property {number} bias
 * @property {(x: number) => number} activation
 * @property {(x: number[]) => number} aggregation
 * @property {number} value
 * @property {number} ivalue
 */
/**
 * @typedef {object} Connection
 * @property {NeatNode} in
 * @property {NeatNode} out
 * @property {number} weight
 */

/**
 * @typedef {object} NodeLink
 * @property {NeatNode} in
 * @property {number} weight
 */

/**
 * @param {number[]} x 
 * @returns {number}
 */
function aggregation_sum(x) {
  return x.reduce((acc, cur) => acc + cur, 0)
}
/**
 * @param {number} x 
 * @returns {number}
 */
function activation_relu(x) {
  return Math.max(0, x)
}
/**
 * @param {number} x 
 * @returns {number}
 */
function activation_identity(x) {
  return x
}
/**
 * @param {number} z 
 * @returns {number}
 */
function activation_sigmoid(z) {
  z = Math.max(-60, Math.min(60, 5 * z))
  return 1.0 / (1.0 + Math.exp(-z))
}
/** @type {NeatNode[]} */
const net_inputs = [
  {
    type: NODE_TYPE_INPUT,
    bias: 0,
    activation: activation_identity,
    aggregation: aggregation_sum,
  },
]
/** @type {NeatNode[]} */
const net_outputs = [
  {
    type: NODE_TYPE_OUTPUT,
    bias: 0,
    activation: activation_relu,
    aggregation: aggregation_sum,
  },
]
/** @type {NeatNode[]} */
const net_nodes = [
  ...net_inputs,
  ...net_outputs,
  {
    type: NODE_TYPE_HIDDEN,
    bias: 0,
    activation: activation_relu,
    aggregation: aggregation_sum,
  },
].map((node, i) => {
  node.id = i
  node.value = 0
  node.ivalue = 0
  return node
})
/** @type {Connection[]} */
const net_connections = [
  {
    in: net_nodes[0],
    out: net_nodes[2],
    weight: 1,
  },
  {
    in: net_nodes[2],
    out: net_nodes[2],
    weight: 1,
  },
  {
    in: net_nodes[2],
    out: net_nodes[1],
    weight: 1,
  },
]

// required_for_output
const required_nodes = [...net_outputs]
if (true) {
  while (true) {
    const layer_nodes = net_connections
      .filter(connection => (
        required_nodes.includes(connection.out)
        && !required_nodes.includes(connection.in)
        && !net_inputs.includes(connection.in)
      ))
      .map(connection => connection.in)
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
        processed_connections.includes(connection.out)
        && !processed_connections.includes(connection.in)
      ))
      .map(connection => connection.in)
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

// node_evals

/** @type {Map<NeatNode, NodeLink[]>} */
const node_links = new Map([])
for (const connection of net_connections) {
  if (!required_nodes.includes(connection.in) && !required_nodes.includes(connection.out))
    continue
  if (!node_links.has(connection.out)) {
    node_links.set(connection.out, [])
  }
  node_links.get(connection.out).push({ in: connection.in, weight: connection.weight })
}
const input_sequence = [
  0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1
]
for (const input of input_sequence) {
  // apply
  net_inputs[0].value = input
  // save_state
  for (const node of net_nodes) {
    node.ivalue = node.value
  }
  // activate
  for (const [node, links] of node_links) {
    const node_inputs = links.map(link => link.in.ivalue * link.weight)
    const aggregation = node.aggregation(node_inputs)
    node.value = node.activation(node.bias + aggregation)
  }
  console.log('output', net_nodes)
}
