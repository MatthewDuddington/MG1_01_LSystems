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
    
    float PrepareTree(int number_of_steps = 1) {
      // Get a reference to the recipe 
      std::string& recipe = recipe_.Seed(number_of_steps);

      // Setup 'turtle' at start
      Turtle turtle;
      turtle.turtle_to_world.loadIdentity();  // Position is 0,0,0

      // Reset branches and set axiom trunk
      Branch::IsAfterSplit() = false;
      branches_.clear();
      Branch* previous_branch = &Branch::NewBranch(branches_, recipe_.AxiomHalfSize(), recipe_.ThinningRatio());
       
      float  highest_point = 0;  // Store the approx highest y value here to return

      // Loop through recipe and apply rules to 'turtle'
      for (int i = 0; i < recipe.size(); i++) {
        switch (recipe.at(i))
        {
        case 'F':  // Draw forwards
        {
          turtle.turtle_to_world.translate(0, previous_branch->HalfSize().y() * 2, 0);  // TODO Does this move in World or Model space?
          if (highest_point < (turtle.turtle_to_world[3][1] + previous_branch->HalfSize().y()) * 1.1) { highest_point = (turtle.turtle_to_world[3][1] + previous_branch->HalfSize().y()) * 1.1; }

          vec2 half_size = previous_branch->HalfSize();  // TODO Workaround to stop undefined vec2 being passed (work out why)
          float thinning_ratio = 1;
          if (Branch::IsAfterSplit()) { thinning_ratio = recipe_.ThinningRatio(); Branch::IsAfterSplit() = false; }
          Branch& new_branch = Branch::NewBranch(branches_, half_size, thinning_ratio);
          new_branch.model_to_world_ = turtle.turtle_to_world;
          previous_branch = &new_branch;
          break;
        }
        case '-':  // Turn left
          turtle.turtle_to_world.rotateZ(recipe_.LeftRotation());
          Branch::IsAfterSplit() = true;
          //turtle.rotation += recipe_.LeftRotation();
          break;
        case '+':  // Turn right
          turtle.turtle_to_world.rotateZ(recipe_.RightRotation());
          Branch::IsAfterSplit() = true;
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

      return highest_point;
    }

  };

}
