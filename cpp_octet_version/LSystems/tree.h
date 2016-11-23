////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//
//  Using Octet framework (c) Andy Thomason 2012 - 2014
//

#pragma once

namespace octet {

  class Tree {
    // Holds the recipe that defines the tree
    Recipe recipe_;

    // Conceptual 'turtle' for when processing recipe
    struct Turtle {
      //float rotation;
      mat4t turtle_to_world;
      std::vector<mat4t> position_stack;
    };

    vec3 axiom_node_ = (0, 0, 0);

    std::vector<Branch> branches_;


  public:
    Recipe& GetRecipe() {
      return recipe_;
    }

    std::vector<Branch>& GetBranches() {
      return branches_;
    }
    
    void PrepareTree(int number_of_steps = 1) {
      // Get a reference to the recipe 
      std::string& recipe = recipe_.GetSeed(number_of_steps);

      // Setup 'turtle' at start
      Turtle turtle;
      turtle.turtle_to_world.translate(axiom_node_.x(), axiom_node_.y(), axiom_node_.z());

      // Reset branches and set axiom trunk
      branches_.clear();
      branches_.push_back(Branch());
      branches_.at(0).Init(axiom_node_.x(), axiom_node_.y(), recipe_.AxiomHalfWidth(), recipe_.AxiomHalfHeight());
      Branch* previous_branch = &branches_.at(0);
       
      // Loop through recipe and apply rules to 'turtle'
      for (int i = 0; i < recipe.size(); i++) {
        vec2 new_half_size = (previous_branch->HalfSize().x(), previous_branch->HalfSize().y());
        switch (recipe.at(i))
        {
        case 'F':  // Draw forwards
          turtle.turtle_to_world.translate(0, new_half_size.y(), 0);  // TODO Does this move in World or Model space?
          branches_.push_back(Branch());
          branches_.at(branches_.size() - 1).Init(turtle.turtle_to_world[3][0], turtle.turtle_to_world[3][1], 1, 3);
          break;
        case '-':  // Turn left
          turtle.turtle_to_world.rotateZ(recipe_.LeftRotation());
          //turtle.rotation += recipe_.LeftRotation();
          break;
        case '+':  // Turn right
          turtle.turtle_to_world.rotateZ(recipe_.RightRotation());
          //turtle.rotation += recipe_.RightRotation();
          break;
        case '[':  // Save position
          turtle.position_stack.push_back(turtle.turtle_to_world);
          //mat4t& turtle_matrix = turtle.turtle_to_world;
          //turtle.position_stack.push_back(vec4(turtle_matrix[3][0], turtle_matrix[3][1], turtle_matrix[3][2], turtle.rotation));
          break;
        case ']':  // Load position
          turtle.turtle_to_world = turtle.position_stack.at(turtle.position_stack.size() - 1);
          turtle.position_stack.pop_back();
          break;
        }
      }
    }

  };

}
