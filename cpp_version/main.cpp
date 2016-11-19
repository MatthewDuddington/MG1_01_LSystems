#pragma once

#include "../octet/src/octet.h"

#include <string>
#include <vector>

#include "tree_recipe.h"
#include "l_system_tree.h"
#include "l_system_app.h"

int main(int argc, char **argv)
{
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

  LSystemApp app;
  app.Init();
  app.MainLoop();

  return 0;
}
