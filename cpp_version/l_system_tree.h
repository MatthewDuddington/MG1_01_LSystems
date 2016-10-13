// Using framework code from drawing a single red triangle in open GL example at: https://github.com/andy-thomason/gles2-triangle

#ifdef WIN32
  // This is the "GL extension wrangler"
  // Note: you will need to use the right version of glew32s.lib (32 bit vs 64 bit)
  #define GLEW_STATIC
  #include "GL/glew.h"
  #pragma comment(lib, "glew32s.lib")

  // This is the "GL utilities" framework for drawing with OpenGL
  #include "freeglut/include/GL/glut.h"
#else // (osx?)
  #include "GLUT/glut.h"
#endif

namespace LSys {

  class LSystemTree
  {
    float axiom_node_pos_x = 0;
    float axiom_node_pos_y = 0;

    static std::vector<float>& vertices_() {
      static std::vector<float> vertices;
      if (vertices.size() == 0) {
        vertices.push_back(0);
      }
      return vertices;
    }

    void add_new_vertices(float node_pos_x, float node_pos_y, float shape_half_height, float shape_half_width) {
      // Set up verts as per a quad made of a triangle strip.
      float new_vertices[8] = {
        (node_pos_x + shape_half_width), node_pos_y, // Lower right
        (node_pos_x - shape_half_width), node_pos_y, // Lower left
        (node_pos_x + shape_half_width), (node_pos_y + (2 * shape_half_height)), // Upper right
        (node_pos_x - shape_half_width), (node_pos_y + (2 * shape_half_height)) // Upper left
      };

      for (int i = 0; i < sizeof(new_vertices); i++) {
        vertices_().push_back(new_vertices[i]);
      }
    }

  public:
    void prepare_tree() {

    }

    static void display()
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // set the attributes (assume pos=0)
      glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        2 * sizeof(float), (GLvoid*)vertices_().data()
      );
      glEnableVertexAttribArray(0);

      // draw the triangle
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

      glutSwapBuffers();
    }

    static void reshape(int w, int h)
    {
      glViewport(0, 0, w, h);
    }
  };

}
