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
      SETTING_RECIPE,
      DRAWING_TREE,
      VIEWING_SCENE
    };

    ProgramState program_state_ = SETTING_RECIPE;  // Default state on start

    bool is_add_to_parameter_ = true;
    bool is_step_by_step_ = false;
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
      for (int i = 0; i < branches.size(); i++) {
        branches.at(i).Render(colour_shader_, camera_to_world_);
      }
    }

    void UpdateUI() {
      // TODO draw text to screen
    }

    void ResetProgram() {
      tree_.ClearTree();
      is_add_to_parameter_ = true;
      is_step_by_step_ = false;
      program_state_ = SETTING_RECIPE;
      tree_.rotation_load_type_ = Tree::LOAD_FROM_SAVE;
      printf("Resetting Program...\nProgram state now: Setting Recipe\n");
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

      // Avoid circular referencing in order to draw live
      Tree::CameraReference(&camera_to_world_);
      Tree::ColourShaderReference(&colour_shader_);

      // set up the matrices with a camera 5 units from the origin
      camera_to_world_.loadIdentity();
      camera_to_world_.translate(0, camera_distance_, camera_distance_);

      //font_texture_ = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");
    }

    void MainLoop() {
      if (is_key_going_down(key_esc)) {
        ResetProgram();
        return;
      }

      switch (program_state_)
      {
      case octet::LSystemApp::SETTING_RECIPE:
        // Premade recipe loaders
        if (is_key_down(key_P)) {
          for (int i = 0; i < tree_.GetRecipe().NumberOfPremadeDesigns(); i++) {
            if (is_key_going_down(key_1 + i)) {
              tree_.GetRecipe().LoadDesign(i + 1);
            }
          }
        }
        // Paramter change keys
        else if (is_key_going_down(key_plus)) {
          is_add_to_parameter_ = true;
        }
        else if (is_key_going_down(key_minus)) {
          is_add_to_parameter_ = false;
        }
        // Order
        else if (is_key_going_down(key_1)) {
          if (is_add_to_parameter_) { tree_.GetRecipe().CurrentDesign().order++; }
          else { tree_.GetRecipe().CurrentDesign().order--; }
          printf("Order: %d\n", tree_.GetRecipe().CurrentDesign().order);
          UpdateUI();
        }
        else if (is_key_going_down(key_0)) {
          is_step_by_step_ = !is_step_by_step_;
          if (is_step_by_step_) { printf("Step by step: Active\n"); }
          else { printf("Step by step: Off\n"); }
        }
        else if (is_key_going_down(key_enter)) {
          // TODO Hide input controlls 
          program_state_ = DRAWING_TREE;
          printf("Program state now: Drawing Tree\n");
        }
        break;

      case octet::LSystemApp::DRAWING_TREE:
        for (int i = 0; i < 9; i++) {
          if (is_key_going_down(key_1 + i)) {
            if (tree_.GrowTree(i + 1, is_step_by_step_) == 0) {
              UpdateCameraValue(tree_.HeightOfTree());
              if (!is_step_by_step_ && should_reset_camera_zoom_) { tree_.ResetCameraHeight(); }
              return;
            }
            else {
              program_state_ == VIEWING_SCENE;
              printf("Program state now: Viewing Scene\n");
              return;
            }
          }
        }
        if (is_key_going_down(key_C)) {
          should_reset_camera_zoom_ = !should_reset_camera_zoom_;
          if (should_reset_camera_zoom_) { printf("Camera will reset zoom for each iteration\n"); }
          else { printf("Camera will remain at highest point\n"); }
        }
        else if (is_key_down(key_R)) {
          if      (is_key_going_down(key_A)) { tree_.rotation_load_type_ = Tree::LOAD_FROM_SAVE;   printf("Rotation from saved position will be loaded upon load position\n"); }
          else if (is_key_going_down(key_S)) { tree_.rotation_load_type_ = Tree::ZERO_OUT;         printf("Rotation will be zeroed out upon load position\n"); }
          else if (is_key_going_down(key_D)) { tree_.rotation_load_type_ = Tree::PRESERVE_CURRENT; printf("Current rotation will be preserved upon load position\n"); }
        }
        break;

      case octet::LSystemApp::VIEWING_SCENE:
        if (is_key_going_down(key_enter)) {
          ResetProgram();
        }
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
        if (is_step_by_step_) { tree_.RenderTurtle(colour_shader_, camera_to_world_); }
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
