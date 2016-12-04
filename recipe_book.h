#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

namespace octet {

  class RecipeBook {

    public:

    // Struct to represent variable and constant rules 
    struct Rule
    {
      char variable;
      std::string replacement_symbols;
    };

    // Struct to represent a tree design
    struct TreeDesign {
      // Imported paramters
      std::string axiom;        // Starting seed
      int order;                // Number of iterations
      float angle_Left;         // Amount to rotate for left direction branches
      float angle_right;        // Amount to rotate for right direction branches
      std::vector<Rule> rules;  // Collection of rules for seed generation and drawing processes
      // Default value parameters
      vec2 axiom_half_size;     // Initial branch cercumference and length
      float thinning_ratio;     // Proportion to reduce branch size by compared to parent
      float randomise_length;   // Amount to vary length by
      float randomise_angle;    // Amount to vary branch roatation
    };

    // All the imported designs are stored here as well as the 'current design' at index 0
    static std::vector<TreeDesign>& Designs(TreeDesign* new_design = NULL) {
      static std::vector<TreeDesign> designs_;
      if (new_design != NULL) { designs_.push_back(*new_design); }
      return designs_;
    }

    static void ImportDesigns() {
      Designs().push_back(TreeDesign());
      // Get design file
      for (int design_index = 1; design_index <= 9; design_index++) {
        Designs().push_back(TreeDesign());
        std::string design_file_location = "assets\tree_design_" + std::to_string(design_index) + ".txt";  // Construct file path as string.
        std::ifstream input_file(design_file_location);  // Open file.
        if (input_file.bad()) { printf("File load failed"); }

        int number_of_rules = 4;
        for (int record = 0; input_file.eof() > 0; record++)
        {
          std::string current_line;
          std::getline(input_file, current_line, '\n');
          std::stringstream string_stream(current_line);
          switch (record) {
          case 0:  // Axiom
            string_stream >> Designs().at(design_index).axiom;
            break;
          case 1:  // Order
            string_stream >> Designs().at(design_index).order;
            break;
          case 2:  // Angles
            string_stream >> Designs().at(design_index).angle_Left;
            break;
          case 3:
            string_stream >> Designs().at(design_index).angle_right;
            break;
          default:  // Every other pair of lines should be a variable and some replacement symbols
            if (record % 2 == 0) { string_stream >> Designs().at(design_index).rules.at(number_of_rules).variable; }
            else { string_stream >> Designs().at(design_index).rules.at(number_of_rules++).replacement_symbols; }  // ++ AFTER indexing
            break;
          }  // End of switch
          Designs().at(design_index).axiom_half_size = vec2(0.3f, 1.0f);
          Designs().at(design_index).thinning_ratio = 0.95f;
          Designs().at(design_index).randomise_length = 0.0f;
          Designs().at(design_index).randomise_angle = 0.0f;
        }  // End of current design file
      }  // End of all design files
    }

    // TODO Get user input of / read from file the variables
    static void ImportDesignsManually() {

      TreeDesign design_A = { 
          "F"    //Axiom
        , 5      // Order
        , 25.7f  // Angle left turn
        , 25.7f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'F', "F[+F]F[-F]F" }
          }

        , vec2(0.3f, 1.0f)   // Half size x
        , 0.95f  // Thinning ratio

        , 0.02f      // Randomise length
        , 0.6f      // Randomise angle
      };

      TreeDesign design_B = {
          "F"    //Axiom
        , 5      // Order
        , 20.0f  // Angle left turn
        , 20.0f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'F', "F[+F]F[-F][F]" }
          }

        , vec2(0.2f, 1.0f)   // Half size x
        , 0.7f  // Thinning ratio

        , 0.06f      // Randomise length
        , 0.6f      // Randomise angle
      };

      TreeDesign design_C = { 
          "F"    //Axiom
        , 4      // Order
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'F', "FF-[-F+F+F]+[+F-F-F]" }
          }

        , vec2(0.2f, 1.0f)   // Half size x
        , 1.0f  // Thinning ratio

        , 0.06f      // Randomise length
        , 0.2f      // Randomise angle
      };

      TreeDesign design_D = {
          "X"    //Axiom
        , 7      // Order
        , 20.0f  // Angle left turn
        , 20.0f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'X', "F[+X]F[-X]+X" },
          { 'F', "FF" }
          }

        , vec2(0.5f, 1.0f)   // Half size x
        , 0.95f  // Thinning ratio

        , 0.02f      // Randomise length
        , 0.6f      // Randomise angle
      };

      TreeDesign design_E = {
          "X"    //Axiom
        , 7      // Order
        , 25.7f  // Angle left turn
        , 25.7f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'X', "F[+X][-X]FX" },
          { 'F', "FF" }
          }

        , vec2(0.5f, 1.0f)   // Half size x
        , 0.95f  // Thinning ratio

        , 0.02f      // Randomise length
        , 0.6f      // Randomise angle
      };

      TreeDesign design_F = { 
          "X"    //Axiom
        , 5      // Order
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'X', "F-[[X]+X]+F[+FX]-X" },
          { 'F', "FF" }
          }

        , vec2(0.2f, 1.0f)   // Half size x
        , 0.75f  // Thinning ratio

        , 0.02f      // Randomise length
        , 0.6f      // Randomise angle
      };

      //TreeDesign design_G = { 
      //    "F"    //Axiom
      //  , 4      // Order
      //  , 90.0f  // Angle left turn
      //  , 90.0f  // Angle right turn
      //  , std::vector<Rule>{
      //      // Constants
      //    { '-' , "-" },
      //    { '+' , "+" },
      //    { '[' , "[" },
      //    { ']' , "]" },
      //      // Variables
      //    { 'F', "F+F-F-F+F" }
      //    }

      //  , vec2(0.3f, 1.0f)   // Half size x
      //  , 0.75f  // Thinning ratio

      //  , 0      // Randomise length
      //  , 0      // Randomise angle
      //};


      TreeDesign design_G = {
        "F"    //Axiom
        , 5      // Order
        , 10.0f  // Angle left turn
        , 10.0f  // Angle right turn
        , std::vector<Rule>{
        // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'F', "-F[-F]+" }
      }

      , vec2(0.3f, 1.0f)   // Half size x
        , 0.75f  // Thinning ratio

        , 0      // Randomise length
        , 0      // Randomise angle
      };

      TreeDesign design_H = { 
          "F-G-G"    //Axiom
        , 7      // Order
        , 120.0f  // Angle left turn
        , 120.0f  // Angle right turn
        , std::vector<Rule>{
            // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'F', "F-G+F+G-F" },
          { 'G', "GG" }
          }

        , vec2(0.3f, 1.0f)   // Half size x
        , 0.75f  // Thinning ratio

        , 0      // Randomise length
        , 0      // Randomise angle
      };

      TreeDesign design_I = { 
          "FX"    //Axiom
        , 10      // Order
        , 90.0f  // Angle left turn
        , 90.0f  // Angle right turn
        , std::vector<Rule>{
        // Constants
          { '-' , "-" },
          { '+' , "+" },
          { '[' , "[" },
          { ']' , "]" },
            // Variables
          { 'X', "X+YF+" },
          { 'Y', "-FX-Y" }
          }

        , vec2(0.3f, 1.0f)   // Half size x
        , 0.75f  // Thinning ratio

        , 0      // Randomise length
        , 0      // Randomise angle
      };

      // Do these still exist after this block terminates???
      Designs(&TreeDesign()); // Location for user customised tree;
      Designs(&design_A);
      Designs(&design_B);
      Designs(&design_C);
      Designs(&design_D);
      Designs(&design_E);
      Designs(&design_F);
      Designs(&design_G);
      Designs(&design_H);
      Designs(&design_I);
      Designs().at(0) = Designs().at(1);
    }

  };

}
