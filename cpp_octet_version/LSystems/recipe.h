////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//

#include <io.h>
#include <istream>

#pragma once

namespace octet {

  class Recipe {

    // Struct to represent rules 
    struct Rule
    {
      char variable;
      std::string replacement_symbols;
    };

    struct TreeDesign {
      std::string axiom;       // Starting seed
      int order;               // Number of iterations
      vec2 axiom_half_size;    // Starting branch cercumference and length
      float randomise_length;  // Amount to vary length by
      float thinning_ratio;    // Proportion to reduce branch size by compared to parent
      float angle_Left;        // Amount to rotate for left direction branches
      float angle_right;       // Amount to rotate for right direction branches
      float randomise_angle;   // Amount to vary branch roatation
      float gravity;           // Proportion to droop based on distance from trunk

      std::vector<Rule> rules;  // Collection of rules for seed generation and drawing processes
    };

    static std::vector<TreeDesign>& Designs(TreeDesign* new_design = NULL) {
      static std::vector<TreeDesign> designs_;
      if (new_design != NULL) { designs_.push_back(*new_design); }
      return designs_;
    }

    TreeDesign* tree_design_;

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
      printf("ProcessRules\n");
    }

    /*
    void ImportDesigns(int number_of_designs_to_import) {
      // Get design file
      std::string file_location;
      for (int i = 0; i < number_of_designs_to_import; i++) {
        file_location = "Resources/tree_design_" + std::to_string(i + 1) + ".txt";  // Construct file path as string.

        std::ifstream input_file(file_location);  // Open file.

        if (input_file.bad()) {  // Check if file loaded.
          printf("ERROR: File was not loaded successfully. \n");
        }
        else {
          std::string line_buffer;  // Store current line here

          while (!input_file.eof()) {              // Stop at end of file
            // Extract the current line to the buffer
            std::getline(input_file, line_buffer);

            // Add each char from the temporary buffer to the permanent vector
            for (int i = 0; i < line_buffer.length(); i++) {
              designs.push_back(line_buffer.at(i));
            }
          }
        }
      }
    }
    */

    // TODO Get user input of / read from file the variables
    static void ImportDesigns(int number_of_designs_to_import = 6) {

      TreeDesign user_custom_design{ "F"    //Axiom
        , 5      // Order
        , vec2(0.1f, 0.35f)   // Half size x
        , 0      // Randomise length
        , 0.95f  // Thinning ratio
        , 25.7f  // Angle left turn
        , 25.7f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'F', "F" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      TreeDesign design_A{ "F"    //Axiom
        , 5      // Order
        , vec2(0.1f, 0.35f)   // Half size x
        , 0      // Randomise length
        , 0.95f  // Thinning ratio
        , 25.7f  // Angle left turn
        , 25.7f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'F', "F[+F]F[-F]F" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      TreeDesign design_B = { "F"    //Axiom
        , 5      // Order
        , vec2(0.1f, 0.35f)   // Half size x
        , 0      // Randomise length
        , 0.7f  // Thinning ratio
        , 20.0f  // Angle left turn
        , 20.0f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'F', "F[+F]F[-F][F]" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      TreeDesign design_C = { "F"    //Axiom
        , 4      // Order
        , vec2(0.1f, 0.35f)   // Half size x
        , 0      // Randomise length
        , 1.0f  // Thinning ratio
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'F', "FF-[-F+F+F]+[+F-F-F]" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      TreeDesign design_D = { "X"    //Axiom
        , 7      // Order
        , vec2(0.1f, 0.25f)   // Half size x
        , 0      // Randomise length
        , 0.95f  // Thinning ratio
        , 20.0f  // Angle left turn
        , 20.0f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'X', "F[+X]F[-X]+X" },
                             { 'F', "FF" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      TreeDesign design_E = { "X"    //Axiom
        , 7      // Order
        , vec2(0.1f, 0.35f)   // Half size x
        , 0      // Randomise length
        , 0.95f  // Thinning ratio
        , 25.7f  // Angle left turn
        , 25.7f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'X', "F[+X][-X]FX" },
                             { 'F', "FF" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      TreeDesign design_F = { "X"    //Axiom
        , 5      // Order
        , vec2(0.2f, 0.35f)   // Half size x
        , 0      // Randomise length
        , 0.75f  // Thinning ratio
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , 0      // Randomise angle
        , 1.0f   // Gravity
        , std::vector<Rule>{ // Variables
                             { 'X', "F-[[X]+X]+F[+FX]-X" },
                             { 'F', "FF" },
                             // Constants
                             { '-' , "-" },
                             { '+' , "+" },
                             { '[' , "[" },
                             { ']' , "]" },
          }
      };

      // Do these still exist after this block terminates???
      Designs(&user_custom_design); // Location for user customised tree;
      Designs(&design_A);
      Designs(&design_B);
      Designs(&design_C);
      Designs(&design_D);
      Designs(&design_E);
      Designs(&design_F);

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
      return Designs().size() - 1;
    }

    // Mutable var getter functions
    TreeDesign& CurrentDesign() {
      return *tree_design_;
    }



    // Public functions
    void Init() {
      ImportDesigns();
      tree_design_ = &Designs().at(0);  // Set default values for custom design
    }

    // Overwrite customised values with a predefined design
    void LoadDesign(int design_index = 0) {
      Designs().at(0) = Designs().at(design_index);
      printf("Predesigned tree #%d selected\n", design_index);
    }

    // Return the resultant seed for drawing the tree at a certain iteration step
    std::string GetSeedEvolution(int requested_order) {
      if (requested_order > CurrentDesign().order) {
        printf("Order requested is greater than the recipe Order. Reinterpreting request as max order of tree.\n");
        requested_order = CurrentDesign().order;
      }
      if (requested_order != current_stored_seed_order) {  // If we already have the right seed (usually because of 'step by step' mode) skip calculations
        seed_ = CurrentDesign().axiom;  // Reset the starting seed to the axiom
        for (int i = 0; i < requested_order; i++) {
          ProcessRules();  // Generate the instruction set for building the tree
        }
        current_stored_seed_order = requested_order;
        printf("At order %d, seed string is: ", requested_order);
        std::cout << seed_ << "\n";
      }
      return seed_;
    }

  };

}
