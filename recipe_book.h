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
      std::vector<Rule> rules = {  // Collection of rules for seed generation and drawing processes
        // Initial constants
        { '-' , "-" },
        { '+' , "+" },
        { '[' , "[" },
        { ']' , "]" },
      };

      // Common starting paramters
      vec2 axiom_half_size = (0.3f, 1.0f);  // Initial branch cercumference and length
      float thinning_ratio = 0.95f;         // Proportion to reduce branch size by compared to parent
      
      // Fixed paramters
      float randomise_length = 0.5f;        // Amount to vary length by
      float randomise_angle = 20.0f;        // Amount to vary branch roatation
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
          std::getline(input_file, current_line);
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
        }  // End of current design file
      }  // End of all design files
    }

    /*
    // TODO Get user input of / read from file the variables
    static void ImportDesignsManually() {

      TreeDesign design_A{ 
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
          { 'F', "F[+F]F[-F]F" },
          }

        , vec2(0.1f, 1.0f)   // Half size x
        , 0.99f  // Thinning ratio

        , 0      // Randomise length
        , 0      // Randomise angle
      };

      TreeDesign design_B = { "F"    //Axiom
        , 5      // Order
        , vec2(0.1f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 0.7f  // Thinning ratio
        , 20.0f  // Angle left turn
        , 20.0f  // Angle right turn
        , 0      // Randomise angle
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
        , vec2(0.1f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 1.0f  // Thinning ratio
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , 0      // Randomise angle
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
        , vec2(0.1f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 0.95f  // Thinning ratio
        , 20.0f  // Angle left turn
        , 20.0f  // Angle right turn
        , 0      // Randomise angle
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
        , vec2(0.1f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 0.95f  // Thinning ratio
        , 25.7f  // Angle left turn
        , 25.7f  // Angle right turn
        , 0      // Randomise angle
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
        , vec2(0.2f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 0.75f  // Thinning ratio
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , 0      // Randomise angle
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

      TreeDesign design_G = { "X"    //Axiom
        , 5      // Order
        , vec2(0.2f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 0.75f  // Thinning ratio
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , 0      // Randomise angle
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

      TreeDesign design_H = { "X"    //Axiom
        , 5      // Order
        , vec2(0.2f, 1.0f)   // Half size x
        , 0      // Randomise length
        , 0.75f  // Thinning ratio
        , 22.5f  // Angle left turn
        , 22.5f  // Angle right turn
        , 0      // Randomise angle
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
      Designs(); // Location for user customised tree;
      Designs(&design_A);
      Designs(&design_B);
      Designs(&design_C);
      Designs(&design_D);
      Designs(&design_E);
      Designs(&design_F);
      Designs(&design_G);
      Designs(&design_H);

    }
    */

  };

}
