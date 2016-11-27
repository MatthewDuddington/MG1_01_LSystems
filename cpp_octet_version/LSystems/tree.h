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
      std::vector<Branch> matrix_version_position_stack;
      
      vec3 position = ( 0,0,0 );
      std::vector<vec3> polar_version_position_stack;
      
      bool is_after_split = true;  // Tells draw forward whether it should thin the next branch
      int current_seed_step = 0;  // Enables memory of what the turtle should draw next in 'step by step' mode

      float rotation = 0;  // Keep track of rotation total for remove rotation option
      std::vector<float> rotation_stack;

      std::vector<Branch> turtle_sprite;  // Misusing the branch class to render a square
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



    // Const var getter functions
    const float HeightOfTree() {
      return height_of_tree_;
    }



    // Mutable var getter functions
    Recipe& GetRecipe() {
      return recipe_;
    }

    std::vector<Branch>& GetBranches() {
      return branches_;
    }

    static bool& InPolarMode() {
      static bool should_use_polar_mode;
      return should_use_polar_mode;
    }



    // Public functions
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

        // Add extra axiom trunk
        previous_branch_ = &Branch::NewBranch(branches_, recipe_.AxiomHalfSize(), recipe_.ThinningRatio());
        previous_branch_->ModelToWorld().translate(0, recipe_.AxiomHalfSize().y() * 2, 0);

        // Start on step 1 (otherwise we go out of range!)
        turtle_.current_seed_step = 1;
        // Reset turtle position
        //turtle_.turtle_to_world = previous_branch_->model_to_world_;
        turtle_.turtle_to_world.loadIdentity();
        if (InPolarMode()) { turtle_.position = vec3(0, 0, 0); }
        turtle_.rotation = 0;
        
        // If on 'step by step' mode add the visual marker to the turtle
        if (is_step_by_step) {
          Branch::NewBranch(turtle_.turtle_sprite, vec2(0.1f, 0.1f), 1.0f, vec3(1, 0, 0), vec3(0, 0, 1));
          turtle_.turtle_sprite.at(0).ModelToWorld().loadIdentity();
        }
      }

      int loop_iterator;

      if (is_step_by_step) {
        loop_iterator = turtle_.current_seed_step + 1;  // In 'step by step' we only want to do one run through the loop starting at the next seed character
      }
      else { loop_iterator = seed.size() + 1; }  // Otherwise we want to run through the whole seed

      // Loop through recipe and apply rules to 'turtle'
      for (; turtle_.current_seed_step < loop_iterator; turtle_.current_seed_step++) {
        switch (seed.at(turtle_.current_seed_step - 1))
        {
          //*/
        case 'F':  // Draw forwards
        {
          if (seed_evolution_order < 3) { printf("F   \nAbout to go forwards. Angle is: %f\n", turtle_.rotation); }  // DEBUG
          vec2 half_size = previous_branch_->HalfSize();  // TODO Workaround to stop undefined vec2 being passed (work out why)

          // Check if thinning is needed
          float thinning_ratio = 1.0f;
          if (turtle_.is_after_split) {
            if (turtle_.rotation + 90 > 95.0f && turtle_.rotation + 90 < 85.0f) { thinning_ratio = recipe_.ThinningRatio(); }
            else { thinning_ratio = recipe_.ThinningRatio() + ((1 - recipe_.ThinningRatio()) * 0.5); }
            turtle_.is_after_split = false;
          }

          // Move turtle to end of next brach position
          if (InPolarMode()) {
            // Polar version
            float turtle_polar_rotation_radians = (turtle_.rotation + 90) * atan(1) / 45;
            float new_x_offset = half_size.length() * 2 * cos(turtle_polar_rotation_radians);
            float new_y_offset = half_size.length() * 2 * sin(turtle_polar_rotation_radians);
            if (seed_evolution_order < 3) { printf("dx: %f,   dy: %f\n", new_x_offset, new_y_offset); }  // DEBUG
            turtle_.turtle_to_world.translate(new_x_offset, new_y_offset, 0);
            turtle_.position += vec3(new_x_offset, new_y_offset, 0);
          }
          else {
            // Matrix version
            turtle_.turtle_to_world.translate(0, previous_branch_->HalfSize().y() * 2.0f, 0);
            if (is_step_by_step) { turtle_.turtle_sprite.at(0).ModelToWorld() = turtle_.turtle_to_world; }
          }

          // DEBUG Print matrix
          if (seed_evolution_order < 3) {
            for (int x = 0; x < 4; x++) {
              for (int y = 0; y < 4; y++) {
                printf("%f,  ", turtle_.turtle_to_world[x][y]);
              }
              printf("\n");
            }
            printf("Turtle Pos: %f, %f, %f", turtle_.position.x(), turtle_.position.y(), turtle_.position.z());
            printf("\n");
          }
          
          // Instatiate new branch behind turtle
          Branch& new_branch = Branch::NewBranch(branches_, half_size, thinning_ratio);
          if (InPolarMode()) {
            // Polar version
            new_branch.ModelToWorld().translate(turtle_.position);
            new_branch.ModelToWorld().rotateZ(turtle_.rotation);
          }
          else {
            // Matrix version
            new_branch.ModelToWorld() = turtle_.turtle_to_world;
          }
          previous_branch_ = &new_branch;

          // Check if camera move needed
          if (height_of_tree_ < (turtle_.turtle_to_world[3][1]) * 1.02f) { height_of_tree_ = (turtle_.turtle_to_world[3][1]) * 1.02f; }
          break;
        }

        case '-':  // Turn Right
          // Polar Version
          turtle_.rotation += recipe_.RightRotation();

          if (!InPolarMode()) {
            // Matrix version
            turtle_.turtle_to_world.rotateZ(recipe_.RightRotation());

            if (is_step_by_step) { turtle_.turtle_sprite.at(0).ModelToWorld() = turtle_.turtle_to_world; }
          }

          if (seed_evolution_order < 3) { printf("-   \nJust turned Right, angle is now: %f\n\n", turtle_.rotation); }  // DEBUG
          break;

        case '+':  // Turn Left
          // Polar version
          turtle_.rotation += recipe_.LeftRotation();
          
          if (!InPolarMode()) {
            // Matrix version
            turtle_.turtle_to_world.rotateZ(recipe_.LeftRotation()); 

            if (is_step_by_step) { turtle_.turtle_sprite.at(0).ModelToWorld() = turtle_.turtle_to_world; }
          }
          
          if (seed_evolution_order < 3) { printf("+   \nJust turned Left, angle is now: %f\n\n", turtle_.rotation); }  // DEBUG
          break;

        case '[':  // Save position
          turtle_.matrix_version_position_stack.push_back(*previous_branch_);  // Need this in both modes for previous branch size
          turtle_.rotation_stack.push_back(turtle_.rotation);  // Need this in both modes 
            
          if (InPolarMode()) {
            // Polar version
            turtle_.polar_version_position_stack.push_back(turtle_.position);
          }

          turtle_.is_after_split = true;

          if (seed_evolution_order < 3) { printf("[   \nSaved Position %d on stack, angle is now: %f\n\n", turtle_.matrix_version_position_stack.size(), turtle_.rotation); }  // DEBUG
          break;

        case ']':  // Load position
          previous_branch_ = &turtle_.matrix_version_position_stack.at(turtle_.matrix_version_position_stack.size() - 1);  // Need this in both cases for previous branch size
          
          if (InPolarMode()) {
            // Polar version
            float dx = turtle_.polar_version_position_stack.at(turtle_.polar_version_position_stack.size() - 1).x() - turtle_.position.x();
            float dy = turtle_.polar_version_position_stack.at(turtle_.polar_version_position_stack.size() - 1).y() - turtle_.position.y();
            turtle_.turtle_to_world.translate(dx, dy, 0);
            turtle_.position = turtle_.polar_version_position_stack.at(turtle_.polar_version_position_stack.size() - 1);
            turtle_.polar_version_position_stack.pop_back();

            if (rotation_load_type_ == LOAD_FROM_SAVE) {
              turtle_.rotation = turtle_.rotation_stack.at(turtle_.rotation_stack.size() - 1);
              turtle_.rotation_stack.pop_back();
            }
            else if (rotation_load_type_ == ZERO_OUT) {
            }
            else if (rotation_load_type_ == PRESERVE_CURRENT) {
            }

            turtle_.matrix_version_position_stack.pop_back();
          }
          else {
            // Matrix version

            turtle_.turtle_to_world = turtle_.matrix_version_position_stack.at(turtle_.matrix_version_position_stack.size() - 1).ModelToWorld();
            turtle_.matrix_version_position_stack.pop_back();

            // If rotation_load_type_ == LOAD_FROM_SAVE -> This should be inherently done by copying the matrix
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

            if (is_step_by_step) { turtle_.turtle_sprite.at(0).ModelToWorld() = turtle_.turtle_to_world; }
          }

          turtle_.is_after_split = true;

          if (seed_evolution_order < 3) { printf("]   \nLoaded Position %d from stack, angle is now: %f\n\n", turtle_.matrix_version_position_stack.size() + 1, turtle_.rotation); }  // DEBUG
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
        new_branch.ModelToWorld().translate(turtle.position_and_z_rotation.x(), turtle.position_and_z_rotation.y(), 0);
        new_branch.ModelToWorld().rotateZ(turtle.position_and_z_rotation.z());

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
        turtle.polar_version_position_stack.push_back(turtle.position_and_z_rotation);
        break;

        case ']':
        turtle.position_and_z_rotation = turtle.polar_version_position_stack.at(turtle.polar_version_position_stack.size() - 1);
        turtle.polar_version_position_stack.pop_back();
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
