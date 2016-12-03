////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//

#include <io.h>

#pragma once

namespace octet {

  class Recipe {

    RecipeBook::TreeDesign* tree_design_;

    // Keeps track of which version of the seed evolution is currently stored
    int current_stored_seed_order = 0;

    // The string of variables and constants that define the tree at each step
    std::string seed_ = "";



    // Iterate through the current seed evolution and apply the rules to evolve the seed by another step
    void ProcessRules() {
      std::string new_seed_buffer;  // Store new seed here as it is being built

      for (int j = 0; j < seed_.length(); j++) {   // For each character in the seed...
        for (int i = 0; i < CurrentDesign().rules.size(); i++) {  // ...and each rule in the collection
          if (seed_.at(j) == CurrentDesign().rules.at(i).variable) {    // If the character matches one of our rule variables...
            new_seed_buffer.append(CurrentDesign().rules.at(i).replacement_symbols);  // ...add the evolution symbols to the new seed
            break;  // Once rule is identified, skip to the next character
          }
        }
      }

      seed_ = new_seed_buffer;  // Update seed to the new seed
      //printf("ProcessRules\n");  // DEBUG
    }



  public:
    Recipe() {

    }

    // Const var getter functions
    const vec2& AxiomHalfSize() {
      return tree_design_->axiom_half_size;
    }

    const float& LeftRotation() {
      return tree_design_->angle_Left;
    }

    const float& RightRotation() {
      return - tree_design_->angle_right;
    }

    const float& ThinningRatio() {
      return tree_design_->thinning_ratio;
    }

    const int NumberOfPremadeDesigns() {
      return RecipeBook::Designs().size() - 1;
    }



    // Mutable var getter functions
    RecipeBook::TreeDesign& CurrentDesign() {
      return *tree_design_;
    }



    // Public functions
    void Init() {
      RecipeBook::ImportDesignsManually();
      tree_design_ = &RecipeBook::Designs().at(0);  // Set default values for custom design
    }

    // Overwrite customised values with a predefined design
    void LoadDesign(int design_index = 0) {
      RecipeBook::Designs().at(0) = RecipeBook::Designs().at(design_index);
      current_stored_seed_order = 0;  // Ensure tree is redrawn even if default order matches
      printf("Predesigned tree #%d selected\n", design_index);
    }

    // Return the resultant seed for drawing the tree at a certain iteration step
    std::string GetSeedEvolution(int requested_order) {
      if (requested_order > CurrentDesign().order) {
        printf("Order requested is greater than the recipe Order. Reinterpreting request as max order of tree.\n");
        requested_order = CurrentDesign().order;
      }
      if (requested_order != current_stored_seed_order) {  // If we already have the right seed (usually because of 'turtle step mode') skip calculations
        seed_ = CurrentDesign().axiom;  // Reset the starting seed to the axiom
        for (int i = 0; i < requested_order; i++) {
          ProcessRules();  // Generate the instruction set for building the tree
        }
        current_stored_seed_order = requested_order;
        
        if (requested_order < 3) {
          printf("At order %d, seed string is: ", requested_order);
          std::cout << seed_ << "\n";
        }
      }
      return seed_;
    }

  };

}
