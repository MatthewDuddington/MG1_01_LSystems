#pragma once

namespace octet {

  class LSystemApp : public octet::app {

    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t camera_to_world;
    // shader to draw a textured triangle
    texture_shader texture_shader_;
    // Texture for our text
    GLuint font_texture;
    // Information for our text
    bitmap_font font;

    // For now just work with a single tree
    Tree tree;

    //void DrawStuff() {
    //  // vertex shader copies pos to glPosition
    //  const char *vs = "attribute vec2 pos; void main() { gl_Position = vec4(pos, 0, 1); }";
    //  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //  glShaderSource(vertex_shader, 1, &vs, nullptr);
    //  glCompileShader(vertex_shader);

    //  // fragment shader draws in white
    //  const char *fs = "void main() { gl_FragColor = vec4(1, 1, 1, 1); }";
    //  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //  glShaderSource(fragment_shader, 1, &fs, nullptr);
    //  glCompileShader(fragment_shader);

    //  // combine fragment and vertex shader
    //  GLuint program = glCreateProgram();
    //  glAttachShader(program, vertex_shader);
    //  glAttachShader(program, fragment_shader);
    //  glLinkProgram(program);
    //  glUseProgram(program);

    //  glutReshapeFunc(Tree::reshape);
    //  glutDisplayFunc(Tree::display);
    //  glutMainLoop();
    //}

    // draw_text function from Octet example Invaiderers
    void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, camera_to_world);

      /*mat4t tmp;
      glLoadIdentity();
      glTranslatef(x, y, 0);
      glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);
      glScalef(scale, scale, 1);
      glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);*/

      enum { max_quads = 32 };
      bitmap_font::vertex vertices[max_quads * 4];
      uint32_t indices[max_quads * 6];
      aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

      unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, font_texture);

      shader.render(modelToProjection, 0, vec4(1,1,1,1));

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x);
      glEnableVertexAttribArray(attribute_pos);
      glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u);
      glEnableVertexAttribArray(attribute_uv);

      glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
    }

  public:

    // this is called when we construct the class
    LSystemApp(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {}

    // this is called once OpenGL is initialized
    void app_init() {
      // set up the shader
      texture_shader_.init();

      // set up the matrices with a camera 5 units from the origin
      camera_to_world.loadIdentity();
      camera_to_world.translate(0, 0, 5);

      font_texture = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");
    }

    void MainLoop() {
      if (is_key_down(key_enter)) {
        // TODO Hide input controlls 
        // Run algorithm to create tree string.
        tree.GetRecipe().define_recipe();
      }

      // Create the vertex map for the tree.
      tree.prepare_tree();
    }

    // Octet calls this
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

      //DrawStuff();

      char some_text[32];
      sprintf(some_text, "This is text drawn");
      draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, some_text);

      // move the listener with the camera
      vec4 &cpos = camera_to_world.w();
      alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
    }

  };

}
