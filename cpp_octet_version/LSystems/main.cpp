////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//
//  Using Octet framework (c) Andy Thomason 2012 - 2014
//
//  Using some code with modifications from:
//  Octet Invaiderers example (c) Andy Thomason 2012 - 2014
//

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
#include "branch.h"
#include "tree.h"
#include "l_system_app.h"

int main(int argc, char **argv)
{
    // set up the platform.
    octet::app::init_all(argc, argv);

    // our application.
    octet::LSystemApp app(argc, argv);
    app.init();

    // open windows
    octet::app::run_all_apps();

  return 0;
}
