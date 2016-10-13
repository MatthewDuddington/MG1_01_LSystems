#pragma once

class TreeRecipe {

  std::string axiom_; // Starting seed
  int order_; // Number of iterations
  float axiom_length_;
  float randomise_length_;
  float axiom_width_;
  float angle_Left_;
  float angle_right_;
  float randomise_angle_;
  float gravity_;

  struct L_rule
  {
    char variable;
    std::string replacement_symbols;
  };

  L_rule rule1 = { 'A', "ABA" };
  L_rule rule2 = { 'B', "AA" };
  std::vector<L_rule> rules;

  std::string seed_;
  std::string new_seed_buffer;

  void process_rules(std::vector<L_rule> rules, std::string &seed) {
    for (int j = 0; j < (int)seed.length(); j++) {
      for (int i = 0; i < (int)rules.size(); i++) {
        if (seed.at(j) == rules.at(i).variable) {
          new_seed_buffer.append(rules.at(i).replacement_symbols);
          break;
        }
      }
    }
    seed = new_seed_buffer;
    printf(seed.data());
  }

public:
  // Constructor
  tree_recipe() {

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

    rules = { rule1, rule2 };

    // Set the starting seed.
    seed_ = axiom_;

    // Generate the instruction set for building the tree
    process_rules(rules, seed_);
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

