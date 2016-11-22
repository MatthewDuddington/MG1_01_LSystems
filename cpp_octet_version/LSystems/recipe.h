////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//

#pragma once

class Recipe {

  std::string axiom_;       // Starting seed
  int order_;               // Number of iterations
  float axiom_length_;      // Starting branch size
  float randomise_length_;  // Amount to vary size by
  float axiom_width_;       // Starting branch cercumference
  float angle_Left_;        // Amount to rotate for left direction branches
  float angle_right_;       // Amount to rotate for right direction branches
  float randomise_angle_;   // Amount to vary branch roatation
  float gravity_;           // Proportion to droop based on distance from trunk

  // Struct to represent rules 
  struct Rule
  {
    char variable;
    std::string replacement_symbols;
  };

  std::vector<Rule> rules_;  // Collection of rules for seed generation and drawing processes

  std::string seed_ = "";  // The string of variables and constants that define the tree at each step

  void process_rules(std::vector<Rule> rules, std::string &seed) {
    std::string new_seed_buffer;  // Store new seed here as it is being built

    for (int j = 0; j < (int)seed.length(); j++) {   // For each character in the seed...
      for (int i = 0; i < (int)rules.size(); i++) {  // ...and each rule in the collection
        if (seed.at(j) == rules.at(i).variable) {    // If the character matches one of our rule variables...
          new_seed_buffer.append(rules.at(i).replacement_symbols);  // ...add the evolution symbols to the new seed
          break;  // Once rule is identified, skip to the next character
        }
      }
    }

    seed = new_seed_buffer;  // Update seed to the new seed
    printf(seed.data());  // DEBUG
  }


public:
  void define_recipe() {
    // TODO Get user input of / read from file the variables
    axiom_ = "AB";
    order_ = 5;
    axiom_length_ = 20;
    randomise_length_ = 0;
    axiom_width_ = 6;
    angle_Left_ = 27.5f;
    angle_right_ = 27.5f;
    randomise_angle_ = 0;
    gravity_ = 1.8f;

    // TODO Get user input of / read from file the rules
    rules_ = { 
      // Variables
      { 'A', "ABA" },
      { 'B', "AA" },
      // Constants
      { 'F' , "F" },
      { '-' , "-" },
      { '+' , "+" },
      { '[' , "[" },
      { ']' , "]" },
    };

    seed_ = axiom_;  // Set the starting seed

    process_rules(rules_, seed_);  // Generate the instruction set for building the tree
  }

  // Return the resultant seed for drawing the tree,
  // after a certain number of iterations are processed on the current seed
  std::string GetSeed(int number_of_steps) {
    if (!number_of_steps > order_) {
      for (int i = 0; i < number_of_steps; i++) {
        process_rules(rules_, seed_);
      }
    }
    return seed_;
  }

  float AxiomHalfHeight() {
    return 3.0f;
    //return axiom_length_ * 0.5f;
  }

  float AxiomHalfWidth() {
    return 0.5f;
    //return axiom_width_ * 0.5f;
  }
};

