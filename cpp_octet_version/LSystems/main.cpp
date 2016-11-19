// Using Octet library (c) Andy Thomason 
// Using framework code from drawing a single red triangle in open GL example at: https://github.com/andy-thomason/gles2-triangle

//#ifdef WIN32
//// This is the "GL extension wrangler"
//// Note: you will need to use the right version of glew32s.lib (32 bit vs 64 bit)
//#define GLEW_STATIC
//#include "GL/glew.h"
//#pragma comment(lib, "glew32s.lib")
//
//// This is the "GL utilities" framework for drawing with OpenGL
//#include "freeglut/include/GL/glut.h"
//#else // (osx?)
//#include "GLUT/glut.h"
//#endif

#include "octet\src\octet.h"

#include <string>
#include <vector>

#include "recipe.h"
#include "tree.h"
#include "l_system_app.h"

int main(int argc, char **argv)
{
  /*
  glutInit(&argc, argv);
  glutCreateWindow("L-System Trees");

  #ifdef WIN32
    // On windows, we need to do this to get modern OpenGL. Thanks, Microsoft.
    glewInit();
    if (!glewIsSupported("GL_VERSION_2_0"))
    {
      return 1;
    }
  #endif
  */

    // set up the platform.
    octet::app::init_all(argc, argv);

    // our application.
    octet::LSystemApp app(argc, argv);
    app.Init();

    // open windows
    octet::app::run_all_apps();

  return 0;
}
