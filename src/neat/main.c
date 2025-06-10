#include <sdk/types.h>
#include <sdk/math.h>
#include <sdk/console.h>
#include <sdk/unity.h>

typedef enum node_type_t {
  NODE_INPUT,
  NODE_HIDDEN,
  NODE_OUTPUT
} node_type_t;

typedef enum activation_t {
  ACTIVATION_SIGMOID,
  ACTIVATION_TANH,
  ACTIVATION_RELU
} activation_t;

typedef struct node_t {
  u64 id;
  u8 type; // node_type_t
  u64 value;
  u64 innovation;
  u8 activation_fn; // activation_t
} node_t;
typedef struct connection_t {

} connection_t;

typedef struct genome_t {
  u64 id;

  u64 in;
  u64 hidden;
  u64 out;

  u64 fitness;
  u8 activation_fn; // activation_t
} genome_t;

typedef struct specie_t {
  u64 id;
  genome_t* genomes;
} specie_t;

f32 fitness_function(u64 value) {
  f32 fitness = 4;
  // fitness -= math_abs(a.activate([1, 1])[0]);
  // fitness -= math_abs(a.activate([1, 0])[0] - 1);
  // fitness -= math_abs(a.activate([0, 1])[0] - 1);
  // fitness -= math_abs(a.activate([0, 0])[0]);
  // if (a.connections.length < 2) fitness *= .001f;
  return math_max(fitness, .001f);
}

i32 main() {
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
  return 0;
}