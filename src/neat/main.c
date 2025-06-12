#include <sdk/types.h>
#include <sdk/math.h>
#include <sdk/console.h>
#include <sdk/unity.h>

typedef enum node_type_t {
  NODE_TYPE_INPUT,
  NODE_TYPE_HIDDEN,
  NODE_TYPE_OUTPUT
} node_type_t;

typedef enum activation_t {
  ACTIVATION_SIGMOID,
  ACTIVATION_TANH,
  ACTIVATION_RELU
} activation_t;

// execution

typedef struct node_t {
  u8 type; // node_type_t
  u8 activation_fn; // activation_t
  f32 value;
  f32 raw_sum;
} node_t;

typedef struct connection_t {
  node_t* node_in;
  node_t* node_out;
  f32 weight;
} connection_t;

typedef struct network_t {

} network_t;

// training

typedef struct genome_t {
  node_t* nodes;
  u64 nodes_length;

  u64 in_length;
  u64 out_length;

  connection_t* connections;
  u64 connections_length;
} genome_t;

//

void network_create(network_t* this, const genome_t* genome) {
  // required
  connection_t** required = memory_alloc0(sizeof(void*) * genome->connections_length);
  u64 required_length = genome->out_length;
  connection_t* it = genome->nodes + genome->in_length;
  u64 count = genome->out_length;
  while (count--) {
    *required = it;
  }
}

void network_activate(
  const network_t* this, f32* inputs, f32* outputs
) {
  assert(this != null);
  assert(this->in_length > 0);
  assert(this->out_length > 0);
  assert(this->nodes_length > 0);
  // input
  node_t* it = this->nodes;
  u64 count = this->in_length;
  do {
    it->value = *inputs;
    if (--count == 0)
      break;
    ++it;
  } while (true);
  // clear_raw_sum
  it = this->nodes + this->in_length;
  count = this->nodes_length - this->in_length;
  while (count--) {
    it->raw_sum = 0;
  }
}

i32 main() {
  if (true) {

  } else {
    math_random_inicialize();
    const u64 population_size = 150;
    const f32 fitness_threshold = 3.5f;
    const u64 max_epoch = 450;
    // structure
    const u64 input_nodes = 2;
    const u64 output_nodes = 1;
    const u8 activation_fn = ACTIVATION_RELU;
    // mutation_rate
    const f32 weight_range = 5.f; // Intervalo dos pesos [-5.0, 5.0]
    const f32 mutation_weight_strength = .5f; // Força da mutação de peso
    const f32 mutation_add_connection = .02f;
    const f32 mutation_remove_connection = .01f;
    const f32 mutation_add_node = .01f;
    const f32 mutation_remove_node = .005f;
    const f32 mutation_change_weight = .01f;
    // distance_constants
    const f32 neat_c1 = 2.f; // Coeficiente para genes 
    const f32 neat_c2 = .5f; // Coeficiente para genes disjuntos
    const f32 neat_c3 = 1.f; // Coeficiente para diferença de peso médio
    const f32 compatibility_threshold = 1.5f; // Limite para formar novas espécies
    // create_population

    // run
    for (u64 epoch = 1; epoch <= max_epoch; epoch++) {
      console_log("randon=%d", math_random());
    }
  }
  return 0;
}