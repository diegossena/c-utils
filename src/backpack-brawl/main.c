#include <sdk/types.h>
#include <sdk/console.h>
#include <sdk/math.h>
#include <sdk/unity.h>

#define BACKPACK_SIZE 54

typedef struct item_t {
  const char* name;
  f32 damage_min;
  f32 damage_max;
  f32 cooldown;
  u8 slots;
  u16 cooldown_ticks;
  f32 accurancy;
  f32 life_steal;
  f32 stamina_cost;
  f32 critical_chance;
  f32 critical_damage;
  u8 shield_damage;
  f32 acceleration;
  bool onstart_attack;
  f32 onhit_attack_chance;
  f32 onhit_poison;
  f32 onhit_fire;
  f32 onhit_bleeding;
  f32 ondamaged_bleeding;
  f32 ondamaged_stars_bleeding;
  u8 stars;
  u8 stars_poison;
  f32 stars_bleeding;
  u8 onhit_damage_increment;
  f32 onhit_accurancy;
  f32 onhit_critical_chance;
  u8 oncritical_acceleration;
  u8 oncritical_fire;
  u8 oncritical_poison;
  bool oncritical_fire_activate;
  u8 oncritical_bleeding;
  u8 oncriticalhit_damage_increment;
  // simulation
  u64 tick;
} item_t;

i32 main(i32 argc, char** argv) {
  // states
  f32 fire_stack = 0;
  f32 poison_stack = 0;
  f32 bleeding_stack = 0;
  f32 acceleration_stack = 0;
  bool fire_persist = false;
  // ITEMS
  const item_t brown_rat = {
    .name = "Brown Rat",
    .damage_min = 3,
    .damage_max = 6,
    .cooldown = 3.5f,
    .accurancy = 0.8f,
    .critical_chance = .05f,
    .critical_damage = 1.1f,
  };
  const item_t plague_rat = {
    .name = "Plage Rat",
    .damage_min = 5,
    .damage_max = 7,
    .cooldown = 3.2f,
    .accurancy = 0.9f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .stars_poison = 1
  };
  const item_t excalibur = {
    .name = "Excalibur",
    .damage_min = 32,
    .damage_max = 45,
    .cooldown = 3.5f,
    .accurancy = 0.90f,
    .critical_chance = .35f,
    .critical_damage = 1.4f
  };
  const item_t stone_golem = {
    .name = "Stone Golem",
    .damage_min = 12,
    .damage_max = 20,
    .cooldown = 3.0f,
    .accurancy = 0.85f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    // .shield_damage = 30
  };
  const item_t lava_dagger = {
    .name = "Lava Dagger",
    .damage_min = 5,
    .damage_max = 10,
    .cooldown = 2.7f,
    .accurancy = 0.85f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .onhit_fire = 3,
    .onstart_attack = true,
    .onhit_attack_chance = 0.05f,
    .slots = 2
  };
  const item_t knights_razor = {
    .name = "Knight's Razor",
    .damage_min = 5,
    .damage_max = 8,
    .cooldown = 2.2f,
    .accurancy = 0.85f,
    .critical_chance = .15f,
    .critical_damage = 1.1f,
    .oncritical_bleeding = 2,
  };
  const item_t poison_dagger = {
    .name = "Poison Dagger",
    .damage_min = 3,
    .damage_max = 7,
    .cooldown = 3.0f,
    .accurancy = 0.85f,
    .critical_chance = .05f,
    .critical_damage = 1.0f,
    .onhit_poison = 2
  };
  const item_t flaming_sword = {
    .name = "Flaming Sword",
    .damage_min = 10,
    .damage_max = 14,
    .cooldown = 2.2f,
    .accurancy = 0.90f,
    .stamina_cost = 1.4f,
    .critical_chance = .10f,
    .critical_damage = 1.0f,
    .onhit_fire = 3 * .5f,
    .acceleration = 3 * .5f,
    .oncritical_acceleration = 3,
    .oncritical_fire = 3,
    .oncritical_fire_activate = true,
    .slots = 3
  };
  const item_t vampire_bat = {
    .name = "Vampire Bat",
    .damage_min = 11,
    .damage_max = 14,
    .cooldown = 2.8f,
    .accurancy = 0.90f,
    .life_steal = 12 * 0.5f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .slots = 2
  };
  const item_t red_window = {
    .name = "Red Window",
    .damage_min = 3,
    .damage_max = 5,
    .cooldown = 2.7f,
    .accurancy = 0.40f,
    .critical_chance = .05f,
    .critical_damage = 1.0f,
  };
  const item_t lava_rock = {
    .name = "Lava Rock",
    .damage_min = 7,
    .damage_max = 12,
    .cooldown = 4.0f,
    .accurancy = 0.80f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .onhit_fire = 1
  };
  const item_t lava_boulder = {
    .name = "Lava Boulder",
    .damage_min = 18,
    .damage_max = 36,
    .cooldown = 4.0f,
    .accurancy = 0.90f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .onhit_fire = 6
  };
  const item_t lava_golem = {
    .name = "Lava Golem",
    .damage_min = 21,
    .damage_max = 34,
    .cooldown = 3.0f,
    .accurancy = 0.90f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .onhit_fire = 8,
    .slots = 4
  };
  const item_t rat_king = {
    .name = "Rat King",
    .damage_min = 9,
    .damage_max = 14,
    .cooldown = 3.2f,
    .accurancy = 0.80f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
  };
  const item_t unholy_sword = {
    .name = "Unholy Sword",
    .damage_min = 9,
    .damage_max = 15,
    .cooldown = 2.6f,
    .accurancy = 0.90f,
    .critical_chance = .10f,
    .critical_damage = 1.0f,
    .onhit_damage_increment = 2,
    .oncriticalhit_damage_increment = 4
  };
  const item_t sundancer = {
    .name = "Sundancer",
    .damage_min = 2,
    .damage_max = 3,
    .cooldown = 2.9f,
    .accurancy = 0.50f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .onhit_accurancy = 0.05f
  };
  const item_t spicy_sausage = {
    .name = "Spicy Sausage",
    .cooldown = 5.f,
    .accurancy = 1.00f,
    .onhit_fire = 2 + 5 * 0.12f
  };
  const item_t burning_water = {
    .name = "Burning Water",
    .cooldown = 4.3f,
    .accurancy = 1.00f,
    .onhit_fire = 2 * 0.5f
  };
  const item_t hooked_dagger = {
    .name = "Hooked Dagger",
    .cooldown = 2.9f,
    .accurancy = 0.85f,
    .onhit_bleeding = 2
  };
  const item_t dragons_tail_jelly = {
    .name = "Dragon's Tail Jelly",
    .damage_min = 4,
    .damage_max = 7,
    .cooldown = 3.3f,
    .accurancy = .80f,
    .critical_chance = .00f,
    .critical_damage = 1.0f,
    .onhit_fire = 2,
    .onhit_poison = 1,
    .onhit_critical_chance = .01f,
    .oncritical_fire = 1,
    .oncritical_poison = 2
  };
  const item_t ancient_vessel = {
    .name = "Ancient Vessel",
    .cooldown = 4.2f,
    .accurancy = .80f,
    .stars_bleeding = 1
  };
  const item_t mars_flytrap = {
    .name = "Mars Flytrap",
    .cooldown = 9.0f,
    .accurancy = .80f,
    .ondamaged_stars_bleeding = 1 * 0.2f
  };
  const item_t champions_ripper = {
    .name = "Champion's Ripper",
    .damage_min = 9,
    .damage_max = 14,
    .cooldown = 2.4f,
    .accurancy = 1.0f,
    .critical_chance = .25f,
    .critical_damage = 1.0f,
    .onhit_bleeding = 2
  };
  // BACKPACK
#if 0
  // TOTAL DAMAGE 3007
  item_t backpack [] = {
    flaming_sword, flaming_sword,
    flaming_sword, flaming_sword,
    flaming_sword, flaming_sword,
    lava_golem, lava_golem, lava_golem,
    lava_golem, lava_golem, lava_golem,
    lava_golem
  };
#else
  // TOTAL DAMAGE ?
  item_t backpack [] = {
    rat_king
  };
  backpack[0].stars = 1;
  backpack[1].stars = 1;
#endif
  const u64 backpack_size = sizeof(backpack) / sizeof(item_t);
  // SIMULATION
  const f32 duration = 11.2f;
  const u8 enemy_cooldown_ticks = .2f / .1f;
  const u8 fire_cooldown_ticks = 1.f / .1f;
  const u8 poison_cooldown_ticks = 2.f / .1f;
  const u8 bleeding_cooldown_ticks = 3.f / .1f;
  // states
  u64 total_damage = 0;
  u8 used_slots = 0;
  // stamina
  f32 stamina_total = 0;
  f32 stamina_pool = 0;
  // timers
  u8 fire_ticks = 0;
  u8 poison_ticks = 0;
  u8 bleeding_ticks = 0;
  u8 enemy_ticks = 0;
  // item_precalc
  for (u64 i = 0; i < backpack_size; i++) {
    item_t* item = &backpack[i];
    used_slots += item->slots;
    stamina_pool += item->stamina_cost;
    item->onhit_poison += item->stars_poison * item->stars;
    item->onhit_bleeding += item->stars_bleeding * item->stars;
    item->ondamaged_bleeding += item->ondamaged_stars_bleeding * item->stars;
    item->cooldown_ticks = (item->cooldown / .1f) / (1 + ((u64)acceleration_stack / 100.f));
  }
  // loop
  for (f32 timer = 0; timer <= duration; timer += .1f) {
    console_color(ANSI_FORE_LIGHTBLUE);
    console_log("%.1f", timer);
    console_color(ANSI_RESET);
    // debuffs
    if (++fire_ticks >= fire_cooldown_ticks) {
      if (fire_stack > 1.f) {
        total_damage += fire_stack;
        console_color(ANSI_FORE_LIGHTRED);
        console_log("Fire %.0f", fire_stack);
        console_color(ANSI_RESET);
        if (fire_persist == false) {
          --fire_stack;
        }
      }
      fire_ticks = 0;
    }
    if (++poison_ticks >= poison_cooldown_ticks) {
      if (poison_stack > 1.f) {
        total_damage += poison_stack;
        console_color(ANSI_FORE_LIGHTGREEN);
        console_log("Poison %.0f", poison_stack);
        console_color(ANSI_RESET);
      }
      poison_ticks = 0;
    }
    if (++bleeding_ticks >= bleeding_cooldown_ticks) {
      if (bleeding_stack > 1.f) {
        total_damage += bleeding_stack;
        console_color(ANSI_FORE_RED);
        console_log("Bleeding %.0f", bleeding_stack);
        console_color(ANSI_RESET);
      }
      bleeding_ticks = 0;
    }
    if (++enemy_ticks >= enemy_cooldown_ticks) {
      for (u64 i = 0; i < backpack_size; i++) {
        item_t* item = &backpack[i];
        bleeding_stack += item->ondamaged_bleeding;
      }
      enemy_ticks = 0;
    }
    for (u64 i = 0; i < backpack_size; i++) {
      item_t* item = &backpack[i];
      if (item->tick >= item->cooldown_ticks) {
        // stamina
        stamina_total += item->stamina_cost;
        // damage
        const f32 damage_average = (item->damage_min + item->damage_max) / 2.f;
        u64 damage = (
          damage_average * (1.f - item->critical_chance)
          + damage_average * (1 + item->critical_damage) * item->critical_chance
          + item->shield_damage
          + item->life_steal
          ) * item->accurancy;
        if (item->onstart_attack) {
          damage *= 2;
          item->onstart_attack = false;
        }
        damage += damage * item->onhit_attack_chance;
        if (damage > 0) {
          console_log("%s %llu", item->name, damage);
        }
        fire_stack += item->onhit_fire + item->oncritical_fire * item->critical_chance;
        poison_stack += item->onhit_poison + item->oncritical_poison * item->critical_chance;
        bleeding_stack += item->onhit_bleeding + item->oncritical_bleeding * item->critical_chance;
        total_damage += damage;
        // onhit_damage_increment
        item->damage_min += item->onhit_damage_increment;
        item->damage_max += item->onhit_damage_increment;
        // onhit_accurancy
        item->accurancy += item->onhit_accurancy;
        item->accurancy = math_min(item->accurancy, 1.f);
        // onhit_critical_chance
        item->critical_chance += item->onhit_critical_chance;
        item->critical_chance = math_min(item->critical_chance, 1.f);
        // oncriticalhit_damage_increment
        item->damage_min += item->oncriticalhit_damage_increment * item->critical_damage;
        item->damage_max += item->oncriticalhit_damage_increment * item->critical_damage;
        // oncritical_fire_activate
        if (item->oncritical_fire_activate) {
          f32 fire_damage = fire_stack * item->critical_chance;
          if (fire_damage > 1.f) {
            total_damage += fire_damage;
            console_color(ANSI_FORE_LIGHTRED);
            console_log("%s (Fire) %.0f", item->name, fire_damage);
            console_color(ANSI_RESET);
          }
        }
        // speed
        acceleration_stack += item->acceleration + item->oncritical_acceleration * item->critical_chance;
        // cooldown
        item->tick = 0;
      } else {
        ++item->tick;
      }
    }
    for (u64 i = 0; i < backpack_size; i++) {
      item_t* item = &backpack[i];
      item->cooldown_ticks = (item->cooldown / .1f) / (1 + ((u64)acceleration_stack / 100.f));
    }
  }
  console_color(ANSI_FORE_LIGHTYELLOW);
  console_log("\ntotal_damage %llu META 981", total_damage);
  console_log("stamina_pool %.1f", stamina_pool);
  console_log("stamina_total %.1f", stamina_total);
  console_log("stamina/s %.1f", stamina_total / duration);
  // potions
  f32 potions = math_ceil(stamina_total / duration - 1);
  potions = math_max(0, potions);
  console_log("potions %.0f", potions);
  // potions_rounds
  u8 potions_rounds = 1;
  const f32 potion_gain = 2;
  const f32 potion_regen = 1;
  const f32 potion_duration = 7;
  const f32 player_regen = 1.15f;
  const f32 player_regen_total = player_regen * duration;
  while (true) {
    // total regen test
    const f32 potion_regen_total = potions * potions_rounds * potion_gain + potions * potion_duration;
    f32 stamina_deficit = stamina_pool - stamina_total
      + player_regen_total
      + potion_regen_total;
    if (stamina_deficit >= 0)
      break;
    ++potions_rounds;
  }
  console_log("potions_rounds %u", potions_rounds);
  // Slots
  used_slots += potions * 2 + (potions_rounds - 1) * 2;
  console_log("used_slots %u from %u", used_slots, BACKPACK_SIZE);
  console_color(ANSI_RESET);
}