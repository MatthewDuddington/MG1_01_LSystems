#pragma once

class TreeRecipe {

  // Possible drawing actions
  // Define here to avoid circular dependency with recipe
public: enum Action {
    DRAW_FORWARD,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    SAVE_POSITION,
    LOAD_POSITION
  };

private:
  std::string axiom_;       // Starting seed
  int order_;               // Number of iterations
  float axiom_length_;      // Starting branch size
  float randomise_length_;  // Amount to vary size by
  float axiom_width_;       // Starting branch cercumference
  float angle_Left_;        // Amount to rotate for left direction branches
  float angle_right_;       // Amount to rotate for right direction branches
  float randomise_angle_;   // Amount to vary branch roatation
  float gravity_;           // Proportion to droop based on distance from trunk

  struct SeedRule
  {
    char variable;
    std::string replacement_symbols;
  };

  struct DrawRule
  {
    char constant;
    Action action;
  };

  std::vector<SeedRule> seed_rules;
  // TODO User input for rules
  SeedRule rule1 = { 'A', "ABA" };
  SeedRule rule2 = { 'B', "AA" };

  std::string seed_;

  void process_rules(std::vector<SeedRule> seed_rules, std::string &seed) {
    // Store updated seed here
    std::string new_seed_buffer;

    for (int j = 0; j < (int)seed.length(); j++) {   // For each character in the seed...
      for (int i = 0; i < (int)seed_rules.size(); i++) {  // ...and each rule in the collection.
        if (seed.at(j) == seed_rules.at(i).variable) {    // If the character matches one of our rule variables
          new_seed_buffer.append(seed_rules.at(i).replacement_symbols);
          break;
        }
      }
    }

    seed = new_seed_buffer;
    printf(seed.data());  // Debug
  }

public:
  // Constructor
  TreeRecipe() {

  }

  void process_recipe() {
    // TODO Get user defined input of variables.
    axiom_ = "AB";
    order_ = 5;
    axiom_length_ = 20;
    randomise_length_ = 0;
    axiom_width_ = 6;
    angle_Left_ = 27.5f;
    angle_right_ = 27.5f;
    randomise_angle_ = 0;
    gravity_ = 1.8f;

    seed_rules = { rule1, rule2 };

    // Set the starting seed.
    seed_ = axiom_;

    // Generate the instruction set for building the tree
    process_rules(seed_rules, seed_);
  }

  std::string seed() {
    return seed_;
  }

  float shape_half_height() {
    return axiom_length_ * 0.5f;
  }

  float shape_half_width() {
    return axiom_width_ * 0.5f;
  }
};

