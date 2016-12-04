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

    bool need_to_refresh_tree_ = true;

    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t camera_to_world_;
    // shader to draw a textured triangle
    texture_shader texture_shader_;
    
    // Texture for our text
    //GLuint font_texture_;
    // Information for our text
    //bitmap_font font_;

    // Colour shader for branches etc.
    color_shader colour_shader_;

    // For now just work with a single tree
    Tree tree_;

    float camera_distance_ = 25;
    
    int current_order_ = 5;

    // Increment / decrement amounts for hotkey paramters
    float angle_increment_ = 0.5f;  // Rotation Left and Right
    float size_increment_ = 0.05f;  // Branch Circumference and Length



    /*
    // draw_text function from Octet Invaiderers example
    void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, camera_to_world_);

      enum { max_quads = 512 };
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
      PrintInstructions();
    }

    void ResetProgram() {
      // Clear the branches, leaves and turtle to prevent them redisplaying the old tree when next draw state is entered
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

    void UpdateCameraValue() {
      camera_to_world_.loadIdentity();
      float dx = tree_.MaxViewOfTree().x() - tree_.MinViewOfTree().x();
      float dy = tree_.MaxViewOfTree().y() - tree_.MinViewOfTree().y();
      float move_z = dx * 0.51f;
      if (dy > dx) { move_z = dy * 0.51f; }
      
      float move_x = (dx * 0.5f) + tree_.MinViewOfTree().x();
      float move_y = (dy * 0.5f) + tree_.MinViewOfTree().y();

      camera_to_world_.translate( move_x, move_y, move_z);
    }



  public:
    // this is called when we construct the class
    LSystemApp(int argc, char **argv) : app(argc, argv) {} //, font_(512, 256, "big.fnt") {}

    // this is called once OpenGL is initialized
    void app_init() {
      // Set up the shaders
      texture_shader_.init();
      colour_shader_.init();

      tree_.GetRecipe().Init();
      tree_.InPolarMode() = true;

      // set up the matrices with a camera 5 units from the origin
      camera_to_world_.loadIdentity();
      camera_to_world_.translate(0, camera_distance_, camera_distance_);

      //font_texture_ = resource_dict::get_texture_handle(GL_RGBA, "big_0.gif");

      ResetProgram();
    }

    void MainLoop() {
      // Reset
      if (is_key_going_down(key_esc)) {
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
      
      
      
      // RECIPE PARAMTERS //

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
      else if (is_key_down(key_P)) {
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



      // STOCHATIC PARAMETERS //

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



      // ADDITIONAL MODES //

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
      MainLoop();

      if (need_to_refresh_tree_) {
        if (tree_.GrowTree(current_order_, turtle_step_mode_) == 0) {
          UpdateCameraValue();
          need_to_refresh_tree_ = false;
          if (!turtle_step_mode_ && should_reset_camera_zoom_) { tree_.ResetCameraHeight(); }
        }
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
