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
      std::vector<Branch> position_stack;
    };

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
      std::string& recipe = recipe_.Seed(number_of_steps);

      // Setup 'turtle' at start
      Turtle turtle;
      turtle.turtle_to_world.loadIdentity();  // Position is 0,0,0

      // Reset branches and set axiom trunk
      branches_.clear();
      Branch* previous_branch = &Branch::NewBranch(branches_, recipe_.AxiomHalfSize(), recipe_.ThinningRatio());
       
      // Loop through recipe and apply rules to 'turtle'
      for (int i = 0; i < recipe.size(); i++) {
        switch (recipe.at(i))
        {
        case 'F':  // Draw forwards
        {
          turtle.turtle_to_world.translate(0, previous_branch->HalfSize().y() * 2, 0);  // TODO Does this move in World or Model space?
          vec2 temp = previous_branch->HalfSize();
          Branch& new_branch = Branch::NewBranch(branches_, temp, recipe_.ThinningRatio());
          new_branch.model_to_world_ = turtle.turtle_to_world;
          previous_branch = &new_branch;
          break;
        }
        case '-':  // Turn left
          turtle.turtle_to_world.rotateZ(recipe_.LeftRotation());
          //turtle.rotation += recipe_.LeftRotation();
          break;
        case '+':  // Turn right
          turtle.turtle_to_world.rotateZ(recipe_.RightRotation());
          //turtle.rotation += recipe_.RightRotation();
          break;
        case '[':  // Save position
          turtle.position_stack.push_back(*previous_branch);
          break;
        case ']':  // Load position
          previous_branch = &turtle.position_stack.at(turtle.position_stack.size() - 1);
          turtle.turtle_to_world = previous_branch->model_to_world_;
          turtle.position_stack.pop_back();
          break;
        }
      }
    }

  };

}
