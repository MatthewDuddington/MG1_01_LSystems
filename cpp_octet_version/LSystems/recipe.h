////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//

#include <io.h>

#pragma once

namespace octet {

  class Recipe {

    std::string axiom_;       // Starting seed
    int order_;               // Number of iterations
    vec2 axiom_half_size_;    // Starting branch cercumference and length
    float randomise_length_;  // Amount to vary length by
    float thinning_ratio_;    // Proportion to reduce branch size by compared to parent
    float angle_Left_;        // Amount to rotate for left direction branches
    float angle_right_;       // Amount to rotate for right direction branches
    float randomise_angle_;   // Amount to vary branch roatation
    float gravity_;           // Proportion to droop based on distance from trunk

    int steps_processed_ = 0;  // Keeps track of how many iterations have been carried out on the seed (should not go above order_)

    // Struct to represent rules 
    struct Rule
    {
      char variable;
      std::string replacement_symbols;
    };

    std::vector<Rule> rules_;  // Collection of rules for seed generation and drawing processes

    std::string seed_ = "";  // The string of variables and constants that define the tree at each step

    void ProcessRules(std::vector<Rule> rules, std::string &seed) {
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
      //printf(seed.data());  // DEBUG
      printf("ProcessRules\n");
    }


  public:
    void DefineRecipe() {
      LoadDesign(2);

      seed_ = axiom_;  // Set the starting seed
      printf("DefineRecipe\n");
    }

    // Return the resultant seed for drawing the tree,
    // after a certain number of iterations are processed on the current seed
    std::string Seed(int number_of_steps) {
      if (steps_processed_ == order_) {
        printf("Max number of iterations reached, as specified by the recipe Order");
        return seed_;
      }
      else if ((number_of_steps + steps_processed_) > order_) {
        printf("Number of steps requested is greater than the recipe Order. Processing max order of tree.");
        number_of_steps = order_;
      }
      for (int i = 0; i < number_of_steps; i++) {
        ProcessRules(rules_, seed_);  // Generate the instruction set for building the tree
        steps_processed_++;
        printf("After iteration %d, seed string is: %", steps_processed_);
        //std::cout << seed_ << "\n";
      }
      return seed_;
    }

    const vec2& AxiomHalfSize() {
      return axiom_half_size_;
    }

    const float& LeftRotation() {
      return -angle_Left_;  // Reversed because the branch root is at its top
    }

    const float& RightRotation() {
      return angle_right_;  // Reversed because the branch root is at its top
    }

    const float& ThinningRatio() {
      return thinning_ratio_;
    }

    void LoadDesign(int index) {
      switch (index)
      {
      case 1:
        // TODO Get user input of / read from file the variables
        axiom_ = "F";
        order_ = 5;
        axiom_half_size_.x() = 1;
        axiom_half_size_.y() = 3.5;
        randomise_length_ = 0;
        thinning_ratio_ = 0.6f;
        angle_Left_ = 25.7f;
        angle_right_ = 25.7f;
        randomise_angle_ = 0;
        gravity_ = 1.0f;

        // TODO Get user input of / read from file the rules
        rules_ = {
          // Variables
          { 'F', "F[+F]F[-F]F" },
          // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
        };
        break;

      case 2:
        // TODO Get user input of / read from file the variables
        axiom_ = "F";
        order_ = 5;
        axiom_half_size_.x() = 1;
        axiom_half_size_.y() = 3.5;
        randomise_length_ = 0;
        thinning_ratio_ = 0.6f;
        angle_Left_ = 20.0f;
        angle_right_ = 20.0f;
        randomise_angle_ = 0;
        gravity_ = 1.0f;

        // TODO Get user input of / read from file the rules
        rules_ = {
          // Variables
          { 'F', "F[+F]F[-F][F]" },
          // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
        };
        break;

      case 3:
        // TODO Get user input of / read from file the variables
        axiom_ = "F";
        order_ = 4;
        axiom_half_size_.x() = 1;
        axiom_half_size_.y() = 3.5;
        randomise_length_ = 0;
        thinning_ratio_ = 0.6f;
        angle_Left_ = 22.5f;
        angle_right_ = 22.5f;
        randomise_angle_ = 0;
        gravity_ = 1.0f;

        // TODO Get user input of / read from file the rules
        rules_ = {
          // Variables
          { 'F', "FF-[-F+F+F]+[+F-F-F]" },
          // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
        };
        break;

      case 4:
        // TODO Get user input of / read from file the variables
        axiom_ = "X";
        order_ = 7;
        axiom_half_size_.x() = 1;
        axiom_half_size_.y() = 3.5;
        randomise_length_ = 0;
        thinning_ratio_ = 0.95f;
        angle_Left_ = 20.0f;
        angle_right_ = 20.0f;
        randomise_angle_ = 0;
        gravity_ = 1.0f;

        // TODO Get user input of / read from file the rules
        rules_ = {
          // Variables
          { 'X', "F[+X]F[-X]+X" },
          { 'F', "FF" },
          // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
        };
        break;
      }
    }

  };

}
