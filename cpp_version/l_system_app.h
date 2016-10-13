#pragma once

class LSystemApp {

  TreeRecipe recipe;
  LSystemTree tree;

  void DrawStuff() {
    // vertex shader copies pos to glPosition
    const char *vs = "attribute vec2 pos; void main() { gl_Position = vec4(pos, 0, 1); }";
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vs, nullptr);
    glCompileShader(vertex_shader);

    // fragment shader draws in white
    const char *fs = "void main() { gl_FragColor = vec4(1, 1, 1, 1); }";
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fs, nullptr);
    glCompileShader(fragment_shader);

    // combine fragment and vertex shader
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program);

    glutReshapeFunc(L_system_tree::reshape);
    glutDisplayFunc(L_system_tree::display);
    glutMainLoop();
  }

public:

  void MainLoop() {
    if ()
      // Run algorithm to create tree string.
      recipe.process_recipe();

    // Create the vertex map for the tree.
    tree.prepare_tree();

    DrawStuff();
  }

};
