////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//
//  Using Octet framework (c) Andy Thomason 2012 - 2014
//
//  Using draw_world() with modifications from:
//  Octet Invaiderers example (c) Andy Thomason 2012 - 2014
//

#pragma once

namespace octet {

  class LSystemApp : public octet::app {

    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t camera_to_world_;
    
    // shader to draw a textured triangle
    texture_shader texture_shader_;

    // Colour shader for branches etc.
    color_shader colour_shader_;

    // Persistant settings toggles
    bool is_add_to_parameter_ = true;
    bool turtle_step_mode_ = false;
    bool should_reset_camera_zoom_ = true;
    bool need_to_refresh_tree_ = true;

    // For now just work with a single tree
    Tree tree_;
    
    // Keeps note of the current order being requested from the tree (TODO possibly better within the tree itself?)
    int current_order_ = 5;

    // Increment / decrement amounts for hotkey paramters
    float angle_increment_ = 0.5f;  // Rotation Left and Right
    float size_increment_ = 0.05f;  // Branch Circumference and Length



    // Clear the branches, leaves and turtle to prevent them redisplaying the old tree when next draw state is entered
    void ResetProgram() {
      tree_.ClearSprites();

      // Reset various defaults
      is_add_to_parameter_ = true;
      turtle_step_mode_ = false;
      tree_.ResetStepByStep();
      should_reset_camera_zoom_ = true;
      need_to_refresh_tree_ = true;

      // Feedback to user
      PrintInstructions();
    }


    // TODO Placeholder function to manage the drawing of information to the screen
    void UpdateUI() {
      // TODO draw text to screen
      PrintInstructions();
    }


    // Provide console UI instructions with updating parameter value display
    void PrintInstructions() {
      printf(
        "\n"
        "********************************************************************************"
        "\n"
        "INSTRUCTIONS: \n"
        "  Hold 'L' and press number from 1 to %d to load the prarameters of a \n"
        "  predesigned LSystem \n"
        "\n"
        "  Hold 'P' (increment) or 'O' (decrement) and press the numbers '1' - '5' to \n"
        "  change the following recipe parameters: \n"
        "      1 -> Order      +/- 1                          Currently: %d \n"
        "      2 -> Rotation Left  +/- %.1f degrees            Currently: %.1f deg \n"
        "      3 -> Rotation Right +/- %.1f degrees            Currently: %.1f deg \n"
        "      4 -> Branch Circumference  +/- %.2f meters     Currently: %.2f m \n"
        "      5 -> Branch Thinning Ratio +/- %.2f percent    Currently: %.2f \% \n"
        "\n"
        "  Hold 'I' and press the corosponding parameter number to make it stochastic: \n"
        "      3 -> Branch Rotation \n"
        "      5 -> Branch Length \n"
        "      7 -> Leaves (experimental!)\n"
        "\n"
        "  Press 'T' to toggle 'turtle step mode', which draws the branches one at a time \n"
        "  Press 'M' to toggle matrix mode (experimental!) and polar mode (stable) \n"
        "\n"
        "  Hold 'R' when in matrix mode and press 'A', 'S' or 'D' to switch how rotation \n"
        "  is handled during 'load' drawing steps: \n"
        "      A -> Rotation from saved position will be loaded upon load position \n"
        "      S -> Rotation will be zeroed out upon load position \n"
        "      D -> Current rotation will be preserved upon load position \n"
        "\n"
        "  Press 'H' to print these instructions to the console again \n"
        "\n"
        "********************************************************************************"
        "\n"
        , 
        tree_.GetRecipe().NumberOfPremadeDesigns(),
        current_order_,
        angle_increment_,
        tree_.GetRecipe().CurrentDesign().angle_Left,
        angle_increment_,
        tree_.GetRecipe().CurrentDesign().angle_right,
        size_increment_,
        tree_.GetRecipe().CurrentDesign().axiom_half_size.x(),
        size_increment_,
        tree_.GetRecipe().CurrentDesign().thinning_ratio
      );
    }


    // Refers to the prepared tree to determin where to position the camera in order to frame the whole design
    void UpdateCameraValue() {
      float dx = tree_.MaxViewOfTree().x() - tree_.MinViewOfTree().x();  // Total width of the tree
      float dy = tree_.MaxViewOfTree().y() - tree_.MinViewOfTree().y();  // Total height of the tree
      // Take the largest of the width and height to be used  as the zoom (position in Z) of the camera
      float move_z = dx * 0.51f;
      if (dy > dx) { move_z = dy * 0.51f; }
      
      // Find the centre of the design
      float move_x = (dx * 0.5f) + tree_.MinViewOfTree().x();
      float move_y = (dy * 0.5f) + tree_.MinViewOfTree().y();

      // Reset the camera and then position accordingly
      camera_to_world_.loadIdentity();
      camera_to_world_.translate( move_x, move_y, move_z);
    }


    // Loop through the branches of the tree and render them
    void DrawBranches() {
      std::vector<Branch>& branches = tree_.GetBranches();
      for (int i = 1; i < branches.size(); i++) {  // Ignore branch (0) as this is simply to validate the previous branch initial reference
        branches.at(i).Render(colour_shader_, camera_to_world_);
      }
    }



  public:
    // This is called when we construct the class
    LSystemApp(int argc, char **argv) : app(argc, argv) {}


    // This is called once OpenGL is initialized
    void app_init() {
      // Set up the shaders
      texture_shader_.init();
      colour_shader_.init();

      // Set up the matrices with a camera
      camera_to_world_.loadIdentity();

      // Load the predefined recipes and setup initial parameters for modification
      tree_.GetRecipe().Init();

      // Begin in Polar mode as this works for all the trees correctly
      tree_.InPolarMode() = true;

      // Refresh the program state to begin
      ResetProgram();
    }


    // Main controller function (mostly handling input)
    void MainLoop() {
      // Reset
      if (is_key_going_down(key_esc) || is_key_going_down(key_Q)) {
        ResetProgram();
        return;
      }

      // Display instructions
      else if (is_key_going_down(key_I)) {
        PrintInstructions();
      }

      // Hold 'L' and press 1 - 8 to load premade recipies
      else if (is_key_down(key_L)) {  // Hold L
        for (int i = 0; i < tree_.GetRecipe().NumberOfPremadeDesigns(); i++) {  // Prevent array out of bounds
          if (is_key_going_down(key_1 + i)) {  // Number corisponding to design
            tree_.GetRecipe().LoadDesign(i + 1);
            current_order_ = tree_.GetRecipe().CurrentDesign().order;
            need_to_refresh_tree_ = true;
          }
        }
      }
      
      //
      //  RECIPE PARAMTERS //
      // 

      // Paramter Increment or decrement keys
      else if (is_add_to_parameter_ && is_key_down(key_O)) {
        is_add_to_parameter_ = false;
        //printf("Decrementing parameters \n");  // DEBUG
      }
      else if (is_key_going_up(key_O)) {
        is_add_to_parameter_ = true;
        //printf("Incrementing parameters \n");  // DEBUG
      }
      // Hold 'P' for parameters
      else if (is_key_down(key_P) || is_key_down(key_O)) {
        // Order
        if (is_key_going_down(key_1)) {
          // Increment value
          if (is_add_to_parameter_) { current_order_++; }
          else { current_order_--; }
          // Check bounds
          if (current_order_ < 1) { current_order_ = 1; }
          else if (current_order_ > tree_.GetRecipe().CurrentDesign().order) { current_order_ = tree_.GetRecipe().CurrentDesign().order; }
          // Feedback to user
          //printf("Order: %d\n", current_order_);  // DEBUG
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
        // Angle Left
        else if (is_key_down(key_2)) {
          // Reference value for readability
          float& angle_left = tree_.GetRecipe().CurrentDesign().angle_Left;
          // Increment value
          if (is_add_to_parameter_) { angle_left += angle_increment_; }
          else { angle_left -= angle_increment_; }
          // Check bounds
          if (angle_left < 0.1f) { angle_left = 0.01f; }
          else if (angle_left > 165.0f) { angle_left = 165.0f; }
          // Feedback to user
          //printf("Angle Left: %.1f\n", angle_left);  // DEBUG
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
        // Angle Right
        else if (is_key_down(key_3)) {
          // Reference value for readability
          float& angle_right = tree_.GetRecipe().CurrentDesign().angle_right;
          // Increment value
          if (is_add_to_parameter_) { angle_right += angle_increment_; }
          else { angle_right -= angle_increment_; }
          // Check bounds
          if (angle_right < 0.1f) { angle_right = 0.01f; }
          else if (angle_right > 165.0f) { angle_right = 165.0f; }
          // Feeback to user
          //printf("Angle Right: %.1f\n", angle_right);  // DEBUG
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
        // Branch Circumference
        else if (is_key_down(key_4)) {
          // Reference value for readability
          float& circumference = tree_.GetRecipe().CurrentDesign().axiom_half_size.x();
          // Increment value
          if (is_add_to_parameter_) { circumference += size_increment_; }
          else { circumference -= size_increment_; }
          // Check bounds
          if (circumference < 0.01f) { circumference = 0.01f; }
          else if (circumference > 1.0f) { circumference = 1.0f; }
          // Feeback to user
          //printf("Branch Circumference: %.2f\n", circumference);  // DEBUG
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
        // Branch Thinning Ratio
        else if (is_key_down(key_5)) {
          // Reference value for readability
          float& thinning_ratio = tree_.GetRecipe().CurrentDesign().thinning_ratio;
          // Increment value
          if (is_add_to_parameter_) { thinning_ratio += size_increment_; }
          else { thinning_ratio -= size_increment_; }
          // Check bounds
          if (thinning_ratio < 0.6f) { thinning_ratio = 0.6f; }
          else if (thinning_ratio > 1.0f) { thinning_ratio = 1.0f; }
          // Feedback to user
          //printf("Branch Thinning Ratio: %.2f\n", thinning_ratio);  // DEBUG
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
      }

      //
      // STOCHATIC PARAMETERS //
      //

      // Hold 'I' for stochatic options
      else if (is_key_down(key_I)) {
        // Angle 
        if (is_key_going_down(key_2) || is_key_going_down(key_3)) {
          tree_.StochasticBranchAngle() = !tree_.StochasticBranchAngle();
          //Feedback to user
          if (tree_.StochasticBranchAngle()) { printf("Stochastic Branch Angles: Active \n"); }
          else { printf("Stochastic Branch Angles: Off \n"); }
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
        else if (is_key_going_down(key_5)) {
          tree_.StochasticBranchLength() = !tree_.StochasticBranchLength();
          // Feedback to user
          if (tree_.StochasticBranchLength()) { printf("Stochastic Branch Length: Active \n"); }
          else { printf("Stochastic Branch Length: Off \n"); }
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
        else if (is_key_going_down(key_7)) {
          tree_.StochasticLeaves() = !tree_.StochasticLeaves();
          // Feedback to user
          if (tree_.StochasticLeaves()) { printf("Stochastic Leaves: Active \n"); }
          else { printf("Stochastic Leaves: Off \n"); }
          UpdateUI();
          need_to_refresh_tree_ = true;
        }
      }

      //
      // ADDITIONAL MODES //
      //

      // Press 'T' to toggle 'turtle step mode'
      else if (is_key_going_down(key_T)) {
        turtle_step_mode_ = !turtle_step_mode_;
        // Feedback to user
        if (turtle_step_mode_) { printf("turtle step mode: Active\n"); }
        else { printf("turtle step mode: Off\n"); }
        UpdateUI();
        need_to_refresh_tree_ = true;
      }
      // Press 'M' to toggle between Polar and Matrix modes (Polar works, Matrix has issues with trees C and F)
      else if (is_key_going_down(key_M)) {
        Tree::InPolarMode() = !Tree::InPolarMode();
        // Feedback to user
        if (Tree::InPolarMode()) { printf("Polar mode: Active\n"); }
        else { printf("Matrix mode: Active\n"); }
        UpdateUI();
        need_to_refresh_tree_ = true;
      }
      // Press 'C' to toggle camera mode
      else if (is_key_going_down(key_C)) {
        should_reset_camera_zoom_ = !should_reset_camera_zoom_;
        if (should_reset_camera_zoom_) { printf("Camera will reset zoom for each iteration\n"); }
        else { printf("Camera will remain at highest point\n"); }
      }
      // Hold 'R' and press 'A', 'S' or 'D' to change rotation mode
      else if (is_key_down(key_R) && !Tree::InPolarMode()) {
        if (is_key_going_down(key_A)) { 
          tree_.rotation_load_type_ = Tree::LOAD_FROM_SAVE;
          printf("Rotation from saved position will be loaded upon load position\n"); 
          need_to_refresh_tree_ = true;
        }
        else if (is_key_going_down(key_S)) { 
          tree_.rotation_load_type_ = Tree::ZERO_OUT;
          printf("Rotation will be zeroed out upon load position\n"); 
          need_to_refresh_tree_ = true;
        }
        else if (is_key_going_down(key_D)) { 
          tree_.rotation_load_type_ = Tree::PRESERVE_CURRENT; 
          printf("Current rotation will be preserved upon load position\n"); 
          need_to_refresh_tree_ = true;
        }
      }
    }


    // Function adapted from Octet Invaiderers example
    // Octet calls this (do not refactor name)
    void draw_world(int x, int y, int w, int h) {
      MainLoop();  // Check for input and changes

      if (need_to_refresh_tree_) {  // If a paramter has been changed (or force called)
                                    // Process the seed to generate the branch/es
        if (tree_.GrowTree(current_order_, turtle_step_mode_) == 0) {  
          UpdateCameraValue();  // Move the camera to fit the whole design on screen
          need_to_refresh_tree_ = false;  // Prevent recalculations until somthing changes
          if (!turtle_step_mode_ && should_reset_camera_zoom_) { tree_.ResetCameraHeight(); }
        }
        // GorwTree() returns 1 if in turtle step mode and not yet completed whole seed
        else { 
          UpdateCameraValue();  // Update camera position for 'turtle step mode'
          need_to_refresh_tree_ = true;  // Keep looping untill turttle is finished
        }  
      }

      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glDisable(GL_DEPTH_TEST);

      // allow alpha blend (transparency when alpha channel is 0)
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      DrawBranches();
      if (turtle_step_mode_) { tree_.RenderTurtle(colour_shader_, camera_to_world_); }

      /*char some_text[1024];
      sprintf(some_text, "This is text drawn");
      draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, some_text);*/

      // move the listener with the camera
      vec4 &cpos = camera_to_world_.w();
      alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
    }

  };
}
