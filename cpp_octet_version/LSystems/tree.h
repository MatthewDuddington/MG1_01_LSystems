////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//
//  Using Octet framework (c) Andy Thomason 2012 - 2014
//

#pragma once

namespace octet {

  class Tree {
    // Conceptual 'turtle' for when processing recipe
    struct Turtle {
      mat4t turtle_to_world;
      std::vector<Branch> position_stack;
      bool is_after_split = true;

      //vec3 position_and_z_rotation;
      //std::vector<vec3> alt_position_stack;
      float rotation = 0;  // Keep track of rotation total for remove rotation option
      std::vector<float> rotation_stack;

      int current_seed_step = 0;  // Enables memory of what the turtle should draw next in 'step by step' mode
      std::vector<Branch> turtle_sprite;       // Misusing the branch class to render a square
    };

    Turtle turtle_;

    // Holds the recipe that defines the tree
    Recipe recipe_;

    // Stores the trees branches to be rendered later
    std::vector<Branch> branches_;

    Branch* previous_branch_;

    float height_of_tree_ = 0;  // Store the approx highest y value (for camera distance)


  public:
    enum RotationLoadType {
      LOAD_FROM_SAVE,
      ZERO_OUT,
      PRESERVE_CURRENT
    };

    RotationLoadType rotation_load_type_ = LOAD_FROM_SAVE;

    Tree() {

    }

    // Avoid circular referencing in order to draw step by step
    static mat4t& CameraReference(mat4t* camera_to_world = NULL) {
      static mat4t* camera_to_world_;
      if (camera_to_world != NULL) { camera_to_world_ = camera_to_world; }
      return *camera_to_world_;
    }

    static color_shader& ColourShaderReference(color_shader* colour_shader = NULL) {
      static color_shader* colour_shader_;
      if (colour_shader != NULL) { colour_shader_ = colour_shader; }
      return *colour_shader_;
    }



    Recipe& GetRecipe() {
      return recipe_;
    }

    std::vector<Branch>& GetBranches() {
      return branches_;
    }

    float HeightOfTree() {
      return height_of_tree_;
    }



    void ClearTree() {
      // Reset branches
      branches_.clear();

      // Remove visual turtle
      turtle_.turtle_sprite.clear();
    }

    void ResetCameraHeight() {
      height_of_tree_ = 0;
    }

    int GrowTree(int seed_evolution_order, bool is_step_by_step) {
      // Get a reference to the seed 
      std::string& seed = recipe_.GetSeedEvolution(seed_evolution_order);

      // For new drawings...
      if (turtle_.current_seed_step == 0) {
        // Reset
        ClearTree();

        // Add axiom trunk
        previous_branch_ = &Branch::NewBranch(branches_, recipe_.AxiomHalfSize(), recipe_.ThinningRatio());
        previous_branch_->model_to_world_.translate(0, recipe_.AxiomHalfSize().y() * 2, 0);

        // Start on step 1 (otherwise we go out of range!)
        turtle_.current_seed_step = 1;
        // Reset turtle position
        turtle_.turtle_to_world.loadIdentity();
        turtle_.turtle_to_world = previous_branch_->model_to_world_;
        //turtle.position_and_z_rotation = vec3(0, 0, 0);
        
        // If on 'step by step' mode add the visual marker to the turtle
        if (is_step_by_step) {
          Branch::NewBranch(turtle_.turtle_sprite, vec2(0.1f, 0.1f), 1.0f, vec3(1, 0, 0), vec3(0, 0, 1));
          turtle_.turtle_sprite.at(0).model_to_world_.loadIdentity();
        }
      }

      int loop_iterator;

      if (is_step_by_step) {
        loop_iterator = turtle_.current_seed_step + 1;  // In 'step by step' we only want to do one run through the loop starting at the next seed character
      }
      else { loop_iterator = seed.size(); }  // Otherwise we want to run through the whole seed

      // Loop through recipe and apply rules to 'turtle'
      for (; turtle_.current_seed_step < loop_iterator; turtle_.current_seed_step++) {
        switch (seed.at(turtle_.current_seed_step - 1))
        {
          //*/
        case 'F':  // Draw forwards
        {
          printf("F   \nAbout to go forwards. Angle is: %f\n", turtle_.rotation);
          vec2 half_size = previous_branch_->HalfSize();  // TODO Workaround to stop undefined vec2 being passed (work out why)

          // Check if thinning is needed
          float thinning_ratio = 1.0f;
          if (turtle_.is_after_split) {
            thinning_ratio = recipe_.ThinningRatio(); 
            turtle_.is_after_split = false;
          }

          // Move turtle to end of nex brach position
          turtle_.turtle_to_world.translate(0, previous_branch_->HalfSize().y() * 2.0f, 0);
          if (is_step_by_step) { turtle_.turtle_sprite.at(0).model_to_world_ = turtle_.turtle_to_world; }

          // Instatiate new branch behind turtle
          Branch& new_branch = Branch::NewBranch(branches_, half_size, thinning_ratio);
          new_branch.model_to_world_ = turtle_.turtle_to_world;
          previous_branch_ = &new_branch;

          // Check if camera move needed
          if (height_of_tree_ < (turtle_.turtle_to_world[3][1]) * 1.02f) { height_of_tree_ = (turtle_.turtle_to_world[3][1]) * 1.02f; }
          break;
        }

        case '-':  // Turn Right
          printf("-   \nJust about to Turn Right. Angle is: %f   ", turtle_.rotation);
          turtle_.rotation += recipe_.RightRotation();
          turtle_.turtle_to_world.rotateZ(recipe_.RightRotation());
          if (is_step_by_step) { turtle_.turtle_sprite.at(0).model_to_world_ = turtle_.turtle_to_world; }
          turtle_.is_after_split = true;
          printf("Just turned Right. Angle is: %f\n", turtle_.rotation);
          break;

        case '+':  // Turn Left
          printf("+   \nJust about to Turn Left. Angle is: %f   ", turtle_.rotation);
          turtle_.rotation += recipe_.LeftRotation();
          turtle_.turtle_to_world.rotateZ(recipe_.LeftRotation());
          if (is_step_by_step) { turtle_.turtle_sprite.at(0).model_to_world_ = turtle_.turtle_to_world; }
          turtle_.is_after_split = true;
          printf("Just turned Left. Angle is: %f\n", turtle_.rotation);
          break;

        case '[':  // Save position
          turtle_.position_stack.push_back(*previous_branch_);
          turtle_.rotation_stack.push_back(turtle_.rotation);
          printf("[   \nSaved Position %d on stack. And angle is %f\n", turtle_.position_stack.size(), turtle_.rotation);
          break;

        case ']':  // Load position
          previous_branch_ = &turtle_.position_stack.at(turtle_.position_stack.size() - 1);
          
          turtle_.turtle_to_world = turtle_.position_stack.at(turtle_.position_stack.size() - 1).model_to_world_;
          turtle_.position_stack.pop_back();
          
          /*if (rotation_load_type_ == LOAD_FROM_SAVE) {
            vec3 temp_pos = vec3(turtle_.turtle_to_world[3][0], turtle_.turtle_to_world[3][1], turtle_.turtle_to_world[3][2]);
            turtle_.turtle_to_world.loadIdentity();
            turtle_.turtle_to_world.translate(temp_pos);
            turtle_.turtle_to_world.rotateZ(turtle_.rotation_stack.at(turtle_.rotation_stack.size() - 1));
            turtle_.rotation_stack.pop_back();
          }*/
          if (rotation_load_type_ == ZERO_OUT) {
            turtle_.turtle_to_world.rotateZ(-turtle_.rotation);
            turtle_.rotation_stack.clear();
            turtle_.rotation = 0;
          }
          else if (rotation_load_type_ == PRESERVE_CURRENT) {
            turtle_.turtle_to_world.rotateZ(-turtle_.rotation_stack.at(turtle_.rotation_stack.size() - 1));
            turtle_.rotation_stack.pop_back();
            turtle_.turtle_to_world.rotateZ(turtle_.rotation);
          }
          // If it's LOAD_FROM_SAVE this has inherently already been done through the matrix copy - Or not as the case may be???

          turtle_.rotation = turtle_.position_stack.size() + 1, turtle_.rotation;
          printf("]   \nLoaded Position %d from stack. And angle is %f\n", turtle_.position_stack.size() + 1, turtle_.rotation);
          turtle_.position_stack.pop_back();

          if (is_step_by_step) { turtle_.turtle_sprite.at(0).model_to_world_ = turtle_.turtle_to_world; }
          //turtle_.is_after_split = true;
          break;
        }
        /*/
        case 'F':
        {
        vec2 half_size = previous_branch->HalfSize();

        float thinning_ratio = 1;
        if (Branch::IsAfterSplit()) { thinning_ratio = recipe_.ThinningRatio(); Branch::IsAfterSplit() = false; }

        turtle.position_and_z_rotation.x() = half_size.y() * 2 * cos(turtle.position_and_z_rotation.z());
        turtle.position_and_z_rotation.y() = half_size.y() * 2 * sin(turtle.position_and_z_rotation.z());

        Branch& new_branch = Branch::NewBranch(branches_, half_size, thinning_ratio);
        new_branch.model_to_world_.translate(turtle.position_and_z_rotation.x(), turtle.position_and_z_rotation.y(), 0);
        new_branch.model_to_world_.rotateZ(turtle.position_and_z_rotation.z());

        previous_branch = &new_branch;

        // Check if camera move needed
        if (highest_point < turtle.position_and_z_rotation.y() * 1.02f) { highest_point = turtle.position_and_z_rotation.y() * 1.02f; }
        break;
        }

        case '-':
        turtle.position_and_z_rotation.z() += recipe_.LeftRotation();
        Branch::IsAfterSplit() = true;
        break;

        case '+':
        turtle.position_and_z_rotation.z() += recipe_.RightRotation();
        Branch::IsAfterSplit() = true;
        break;

        case '[':
        turtle.alt_position_stack.push_back(turtle.position_and_z_rotation);
        break;

        case ']':
        turtle.position_and_z_rotation = turtle.alt_position_stack.at(turtle.alt_position_stack.size() - 1);
        turtle.alt_position_stack.pop_back();
        break;
        }
        //*/

      }
      if (is_step_by_step && turtle_.current_seed_step != recipe_.CurrentDesign().order) {
        return 1; // Tells app to expect more iterations
      }

      // Reset turtle step count for next time we draw
      turtle_.current_seed_step = 0;

      return 0; // Tell app that we have finished drawing current evolution order
    }

    void RenderTurtle(color_shader colour_shader, mat4t& camera_to_world) {
      if (turtle_.turtle_sprite.size() != 0) {
        turtle_.turtle_sprite.at(0).Render(colour_shader, camera_to_world);
      }
    }

  };

}
