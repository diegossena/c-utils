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
 * @typedef {object} GenomeConfig
 * @property {number} inputs_count
 * @property {number} hidden_count
 * @property {number} outputs_count
 */
/**
 * @typedef {object} Genome
 * @property {NeatNode[]} inputs
 * @property {NeatNode[]} outputs
 * @property {NeatNode[]} nodes
 * @property {Connection[]} connections
 */
/**
 * @typedef {object} Network
 * @property {NeatNode[]} inputs
 * @property {NeatNode[]} outputs
 * @property {NeatNode[]} nodes
 * @property {NodeLink[]} node_links
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

/**
 * @param {Network} network 
 * @param {number[]} inputs 
 */
function network_activate(network, inputs) {
  for (let i = 0; i < network.inputs.length; i++) {
    network.nodes[i].value = inputs[i]
  }
  // save_state
  for (const node of network.nodes) {
    node.ivalue = node.value
  }
  // activate
  for (const [node, links] of network.node_links) {
    const node_inputs = links.map(link => link.in.ivalue * link.weight)
    const aggregation = node.aggregation(node_inputs)
    node.value = node.activation(node.bias + aggregation)
  }
  console.log('output', net_nodes)
}
/**
 * @param {Genome} genome
 * @returns {Network}
 */
function network_create(genome) {
  const inputs = genome.inputs.map(node => ({ ...node }))
  const outputs = [...genome.outputs].map(node => ({ ...node }))
  // required_for_output
  const required_nodes = outputs
  while (true) {
    const layer_nodes = genome.connections
      .filter(connection => (
        required_nodes.includes(connection.out)
        && !required_nodes.includes(connection.in)
        && !genome.inputs.includes(connection.in)
      ))
      .map(connection => connection.in)
      .map(node => ({ ...node }))
    if (!layer_nodes.length) {
      break
    }
    required_nodes.push(...layer_nodes)
  }
  required_nodes.reverse()
  // node_links
  /** @type {Map<NeatNode, NodeLink[]>} */
  const node_links = new Map([])
  for (const connection of genome.connections) {
    if (!required_nodes.includes(connection.in) && !required_nodes.includes(connection.out))
      continue
    if (!node_links.has(connection.out)) {
      node_links.set(connection.out, [])
    }
    node_links.get(connection.out).push({ in: connection.in, weight: connection.weight })
  }
  return {
    inputs,
    outputs: outputs,
    nodes: required_nodes,
    node_links: [...node_links.values()]
  }
}
/**
 * @param {GenomeConfig} config
 * @returns {Genome}
 */
function genome_create(config) {
  const input_nodes = Array.from(Array(config.inputs_count), (_, _i) => ({
    type: NODE_TYPE_INPUT,
    bias: 0,
    activation: activation_identity,
    aggregation: aggregation_sum,
  }))
  const output_nodes = Array.from(Array(config.outputs_count), (_, _i) => ({
    type: NODE_TYPE_OUTPUT,
    bias: 0,
    activation: activation_identity,
    aggregation: aggregation_sum,
  }))
  const hidden_nodes = Array.from(Array(config.hidden_count), (_, _i) => ({
    type: NODE_TYPE_HIDDEN,
    bias: 0,
    activation: activation_relu,
    aggregation: aggregation_sum,
  }))
  return {
    inputs: input_nodes,
    outputs: output_nodes,
    nodes: [...input_nodes, ...output_nodes, ...hidden_nodes],
    connections: [],
  }
}

const genome = genome_create({
  inputs_count: 1,
  hidden_count: 1,
  outputs_count: 1
})
const network = network_create(genome)
const input_sequence = [
  0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1
]
for (const input of input_sequence) {
  const output = network_activate(network, [input])
  console.log('output', output)
}
