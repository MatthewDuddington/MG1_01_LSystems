////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//
//  Using Octet framework (c) Andy Thomason 2012 - 2014
//
//  Using some code with modifications from:
//  Octet Invaiderers example (c) Andy Thomason 2012 - 2014
//

#pragma once

namespace octet {

  class LSystemApp : public octet::app {

    enum ProgramState {
      RECIPE_SETTINGS,
      DRAWING_TREE,
      VIEWING_TREE
    };

    ProgramState program_state_ = RECIPE_SETTINGS;  // Default state on start

    bool is_add_to_parameter_ = true;
    bool turtle_step_mode_ = false;
    bool should_reset_camera_zoom_ = true;

    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t camera_to_world_;
    // shader to draw a textured triangle
    texture_shader texture_shader_;
    // Texture for our text
    GLuint font_texture_;
    // Information for our text
    bitmap_font font_;

    // Colour shader for branches etc.
    color_shader colour_shader_;

    // For now just work with a single tree
    Tree tree_;

    float camera_distance_ = 25;

    // Increment / decrement amounts for hotkey paramters
    float angle_increment_ = 0.1f;  // Rotation Left and Right
    float size_increment_ = 0.01f;  // Branch Circumference and Length

    /*
    // Function from Octet Invaiderers example
    void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, camera_to_world_);

      enum { max_quads = 32 };
      bitmap_font::vertex vertices[max_quads * 4];
      uint32_t indices[max_quads * 6];
      aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

      unsigned num_quads = font_.build_mesh(bb, vertices, indices, max_quads, text, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, font_texture_);

      shader.render(modelToProjection, 0);

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x);
      glEnableVertexAttribArray(attribute_pos);
      glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u);
      glEnableVertexAttribArray(attribute_uv);

      glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
    }
    */

    void DrawBranches() {
      std::vector<Branch>& branches = tree_.GetBranches();
      for (int i = 1; i < branches.size(); i++) {  // Ignore branch (0) as this is simply to validate the previous branch initial reference
        branches.at(i).Render(colour_shader_, camera_to_world_);
      }
    }

    void UpdateUI() {
      // TODO draw text to screen
    }

    void ResetProgram() {
      // Clear the branches, leaves and turtle to prevent them redisplaying the old tree when next draw state is entered
      tree_.ClearSprites();

      // Reset various defaults
      is_add_to_parameter_ = true;
      turtle_step_mode_ = false;
      tree_.ResetStepByStep();
      should_reset_camera_zoom_ = true;
      program_state_ = RECIPE_SETTINGS;
      tree_.rotation_load_type_ = Tree::LOAD_FROM_SAVE;
      Tree::InPolarMode() = true;

      // Feedback to user
      printf("Resetting Program...\nProgram state now: Setting Recipe\n");
      PrintRecipeSettingInfo();
    }

    void PrintRecipeSettingInfo() {
      printf(
        "\n"
        "********************************************************************************"
        "\n"
        "INSTRUCTIONS: \n"
        "  Hold P and press number from 1 to %d to load the prarameters of a \n"
        "  predesigned LSystem \n"
        "\n"
        "  Press Enter to enter drawing view \n"
        "\n"
        "  Press T to toggle 'turtle step mode' (experimental!) \n"
        "\n"
        "  Press Space Bar to switch to incrementing or decrementing the following \n"
        "  recipe parameters: \n"
        "      1 -> Max Order      +/- 1  (a.k.a. maximum number of iteration steps) \n"
        "      2 -> Rotation Left  +/- %.1f degrees \n"
        "      3 -> Rotation Right +/- %.1f degrees \n"
        "      4 -> Branch Circumference  +/- %.2f meters \n"
        "      5 -> Branch Length         +/- %.2f meters \n"
        "      6 -> Branch Thinning Ratio +/- %.2f \% \n"
        "\n"
        "  Hold S and press the corosponding parameter number to make it stochastic: \n"
        "  (experimental!)\n"
        "      2 or 3 -> Rotation \n"
        "      5 -> Branch Length \n"
        "      7 -> Leaves \n"
        "\n"
        "  At any time press Esc to return to this recipe settings mode \n"
        "\n"
        "  Press I to print these instructions to the console again \n"
        "\n"
        "********************************************************************************"
        "\n"
        , 
        tree_.GetRecipe().NumberOfPremadeDesigns(),
        angle_increment_,
        angle_increment_,
        size_increment_,
        size_increment_
      );
    }

    void PrintDrawingInstructions() {
      printf(
        "\n"
        "********************************************************************************"
        "\n"
        "INSTRUCTIONS: \n"
        "  Press keys 1 - 9 to load that iteration step of the tree \n"
        "  (Up to the value of the maximum order in the recipe)"
        "\n"
        "  If in 'Turtle Step Mode' repeatedly press the above key to step through the \n"
        ""
        "\n"
        "********************************************************************************"
        "\n"
      );
    }

    void UpdateCameraValue(float approx_tree_height) {
      camera_to_world_.loadIdentity();
      camera_to_world_.translate(0, approx_tree_height * 0.5f, approx_tree_height * 0.5f);
    }



  public:
    // this is called when we construct the class
    LSystemApp(int argc, char **argv) : app(argc, argv) {} // , font_(512, 256, "assets/big.fnt") {}

    // this is called once OpenGL is initialized
    void app_init() {
      // Set up the shaders
      texture_shader_.init();
      colour_shader_.init();

      tree_.GetRecipe().Init();

      // set up the matrices with a camera 5 units from the origin
      camera_to_world_.loadIdentity();
      camera_to_world_.translate(0, camera_distance_, camera_distance_);

      //font_texture_ = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");

      ResetProgram();
    }

    void MainLoop() {
      if (is_key_going_down(key_esc)) {
        ResetProgram();
        return;
      }

      switch (program_state_)
      {
      case octet::LSystemApp::RECIPE_SETTINGS:
        // Display menu
        if (is_key_going_down(key_I)) {
          PrintRecipeSettingInfo();
        }
        // Premade recipe loaders
        else if (is_key_down(key_P)) {  // Hold P
          for (int i = 0; i < tree_.GetRecipe().NumberOfPremadeDesigns(); i++) {  // Prevent array out of bounds
            if (is_key_going_down(key_1 + i)) {  // Number corisponding to design
              tree_.GetRecipe().LoadDesign(i + 1);
            }
          }
        }
        // Paramter change keys
        else if (is_key_going_down(key_space)) {
          is_add_to_parameter_ = !is_add_to_parameter_;
          // Feedback to user
          if (is_add_to_parameter_) { printf("Incrementing parameters \n"); }
          else { printf("Decrementing parameters \n"); }
          UpdateUI();
        }
        // Stochatic paramters
        else if (is_key_down(key_S)) {
          // Angle 
          if (is_key_going_down(key_2) || is_key_going_down(key_3)) {
            tree_.StochasticBranchAngle() = !tree_.StochasticBranchAngle();
            //Feedback to user
            if (tree_.StochasticBranchAngle()) { printf("Stochastic Branch Angles: Active \n"); }
            else { printf("Stochastic Branch Angles: Off \n"); }
            UpdateUI();

            /*
            // Reference value for readability
            float& angle_random_offset = tree_.GetRecipe().CurrentDesign().randomise_angle;
            // Increment value
            if (is_add_to_parameter_) { angle_random_offset += angle_increment_; }
            else { angle_random_offset -= angle_increment_; }
            // Check bounds
            if (angle_random_offset < 0.0f) { angle_random_offset = 0.0f; }
            else if (angle_random_offset > 100.0f) { angle_random_offset = 100.0f; }
            // Feedback to user
            printf("Max Order: %d\n", angle_random_offset);
            */
          }
          else if (is_key_going_down(key_5)) {
            tree_.StochasticBranchLength() = !tree_.StochasticBranchLength();
            // Feedback to user
            if (tree_.StochasticBranchLength()) { printf("Stochastic Branch Length: Active \n"); }
            else { printf("Stochastic Branch Length: Off \n"); }
            UpdateUI();

            /*
            // Reference value for readability
            float& branch_length_random_offset = tree_.GetRecipe().CurrentDesign().randomise_length;
            // Increment value
            if (is_add_to_parameter_) { branch_length_random_offset += size_increment_; }
            else { branch_length_random_offset -= size_increment_; }
            // Check bounds
            if (branch_length_random_offset < 0.0f) { branch_length_random_offset = 0.0f; }
            else if (branch_length_random_offset > 100.0f) { branch_length_random_offset = 100.0f; }
            // Feedback to user
            printf("Max Order: %d\n", branch_length_random_offset);
            */
          }
          else if (is_key_going_down(key_7)) {
            tree_.StochasticLeaves() = !tree_.StochasticLeaves();
            // Feedback to user
            if (tree_.StochasticLeaves()) { printf("Stochastic Leaves: Active \n"); }
            else { printf("Stochastic Leaves: Off \n"); }
            UpdateUI();
          }
        }
        // Order
        else if (is_key_going_down(key_1)) {
          // Reference value for readability
          int& order = tree_.GetRecipe().CurrentDesign().order;
          // Increment value
          if (is_add_to_parameter_) { order++; }
          else { order--; }
          // Check bounds
          if (order < 1) { order = 1; }
          else if (order > 9) { order = 9; }
          // Feedback to user
          printf("Max Order: %d\n", order);
          UpdateUI();
        }
        // Angle Left
        else if (is_key_going_down(key_2)) {
          // Reference value for readability
          float& angle_left = tree_.GetRecipe().CurrentDesign().angle_Left;
          // Increment value
          if (is_add_to_parameter_) { angle_left += angle_increment_; }
          else { angle_left -= angle_increment_; }
          // Check bounds
          if (angle_left < 0.1f) { angle_left = 0.01f; }
          else if (angle_left > 165.0f) { angle_left = 165.0f; }
          // Feedback to user
          printf("Angle Left: %.1f\n", angle_left);
          UpdateUI();
        }
        // Angle Right
        else if (is_key_going_down(key_3)) {
          // Reference value for readability
          float& angle_right = tree_.GetRecipe().CurrentDesign().angle_right;
          // Increment value
          if (is_add_to_parameter_) { angle_right += angle_increment_; }
          else { angle_right -= angle_increment_; }
          // Check bounds
          if (angle_right < 0.1f) { angle_right = 0.01f; }
          else if (angle_right > 165.0f) { angle_right = 165.0f; }
          // Feeback to user
          printf("Angle Right: %.1f\n", angle_right);
          UpdateUI();
        }
        // Branch Circumference
        else if (is_key_going_down(key_4)) {
          // Reference value for readability
          float& circumference = tree_.GetRecipe().CurrentDesign().axiom_half_size.x();
          // Increment value
          if (is_add_to_parameter_) { circumference += size_increment_; }
          else { circumference -= size_increment_; }
          // Check bounds
          if (circumference < 0.01f) { circumference = 0.01f; }
          else if (circumference > 1.0f) { circumference = 1.0f; }
          // Feeback to user
          printf("Branch Circumference: %.2f\n", circumference);
          UpdateUI();
        }
        // Branch Length
        else if (is_key_going_down(key_5)) {
          // Reference value for readability
          float& length = tree_.GetRecipe().CurrentDesign().axiom_half_size.y();
          // Increment value
          if (is_add_to_parameter_) { length += size_increment_; }
          else { length -= size_increment_; }
          // Check bounds
          if (length < 0.01f) { length = 0.01f; }
          else if (length > 1.0f) { length = 1.0f; }
          // Feedback to user
          printf("Branch Length: %.2f\n", length);
          UpdateUI();
        }
        // Turn 'turtle step mode' on / off
        else if (is_key_going_down(key_T)) {
          turtle_step_mode_ = !turtle_step_mode_;
          // Feedback to user
          if (turtle_step_mode_) { printf("turtle step mode: Active\n"); }
          else { printf("turtle step mode: Off\n"); }
          UpdateUI();
        }
        // Toggle between Polar and Matrix modes (Polar works, Matrix has issues with trees C and F)
        else if (is_key_going_down(key_M)) {
          Tree::InPolarMode() = !Tree::InPolarMode();
          // Feedback to user
          if (Tree::InPolarMode()) { printf("Polar mode: Active\n"); }
          else { printf("Matrix mode: Active\n"); }
          UpdateUI();
        }
        // Enter tree drawing mode
        else if (is_key_going_down(key_enter)) {
          // TODO Hide input controlls 
          program_state_ = DRAWING_TREE;
          printf("Program state now: Drawing Tree\n");
          PrintDrawingInstructions();
        }
        break;

      case octet::LSystemApp::DRAWING_TREE:
        for (int i = 0; i < 9; i++) {
          if (is_key_going_down(key_1 + i)) {
            if (tree_.GrowTree(i + 1, turtle_step_mode_) == 0) {
              UpdateCameraValue(tree_.HeightOfTree());
              if (!turtle_step_mode_ && should_reset_camera_zoom_) { tree_.ResetCameraHeight(); }
              program_state_ = VIEWING_TREE;
              printf("Program state now: Viewing Tree. Press Esc to return to recipe settings.\n");
              return;
            }
            else { UpdateCameraValue(tree_.HeightOfTree()); }  // Set camera height for 'turtle step mode'
            return;
          }
        }
        // Press C to change camera mode
        if (is_key_going_down(key_C)) {
          should_reset_camera_zoom_ = !should_reset_camera_zoom_;
          if (should_reset_camera_zoom_) { printf("Camera will reset zoom for each iteration\n"); }
          else { printf("Camera will remain at highest point\n"); }
        }
        // Hold R to change rotation mode
        else if (is_key_down(key_R)) { 
          if      (is_key_going_down(key_A)) { tree_.rotation_load_type_ = Tree::LOAD_FROM_SAVE;   printf("Rotation from saved position will be loaded upon load position\n"); }
          else if (is_key_going_down(key_S)) { tree_.rotation_load_type_ = Tree::ZERO_OUT;         printf("Rotation will be zeroed out upon load position\n"); }
          else if (is_key_going_down(key_D)) { tree_.rotation_load_type_ = Tree::PRESERVE_CURRENT; printf("Current rotation will be preserved upon load position\n"); }
        }
        // 
        else if (is_key_going_down(key_I)) { PrintDrawingInstructions(); }  
        break;
      }
    }

    // Function adapted from Octet Invaiderers example
    // Octet calls this (do not refactor name)
    void draw_world(int x, int y, int w, int h) {
      MainLoop();

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

      if (program_state_ == DRAWING_TREE) {
        DrawBranches();
        if (turtle_step_mode_) { tree_.RenderTurtle(colour_shader_, camera_to_world_); }
      }

      /*
      char some_text[32];
      sprintf(some_text, "This is text drawn");
      draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, some_text);

      // move the listener with the camera
      vec4 &cpos = camera_to_world_.w();
      alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
      */
    }

  };

}
