////////////////////////////////////////////////////////////////////////////////
//
//  (c) Matthew Duddington 2016
//
//  Using Octet framework (c) Andy Thomason 2012 - 2014
//

#pragma once

namespace octet {

  class Tree {
    // Holds the recipe that defines the tree
    Recipe recipe_;

    // Conceptual 'turtle' for when processing recipe
    struct Turtle {
      vec3 position;
      float rotation;
      mat4t turtle_to_world;
    };

    vec3 axiom_node_ = (0, 0, 0);

    std::vector<Branch> branches_;

    /*
    struct Branch {
      vec3 position;
      float roatation;
      mat4t model_to_world;  // Matrix for holding position and rotation of node
      vec4 colour_tip;   // Colour for gradient at edge nearest tip of branch
      vec4 colour_root;  // Colour for gradient at edge neaerest root of branch
    };
    */


  public:
    Recipe& GetRecipe() {
      return recipe_;
    }

    std::vector<Branch>& GetBranches() {
      return branches_;
    }
    
    void PrepareTree(int number_of_steps = 1) {
      // Get a reference to the recipe 
      std::string& recipe = recipe_.GetSeed(number_of_steps);

      // Setup 'turtle' at start
      Turtle turtle;
      turtle.turtle_to_world.translate(axiom_node_.x(), axiom_node_.y(), axiom_node_.z());

      // Reset branches and set axiom trunk
      branches_.clear();
      branches_.push_back(Branch());
      branches_.at(0).Init(axiom_node_.x(), axiom_node_.y(), recipe_.AxiomHalfWidth(), recipe_.AxiomHalfHeight());
      Branch* previous_branch = &branches_.at(0);

      // Loop through recipe and apply rules to 'turtle'
      for (int i = 0; i < recipe.size(); i++) {
        switch (recipe.at(i))
        {
        case 'F':  // Draw forwards
          branches_.at(i).Init(turtle.turtle_to_world[3][0], turtle.turtle_to_world[3][1], previous_branch->Size().x(), previous_branch->Size().y());
          break;
        case '-':  // Turn left

          break;
        case '+':  // Turn right

          break;
        case '[':  // Save position

          break;
        case ']':  // Load position

          break;
        }
      }

      // read recipe string and setup branches to be drawn
    }

    //static void display()
    //{
    //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  // set the attributes (assume pos=0)
    //  glVertexAttribPointer(
    //    0, 2, GL_FLOAT, GL_FALSE,
    //    2 * sizeof(float), (GLvoid*)vertices_().data()
    //  );
    //  glEnableVertexAttribArray(0);

    //  // draw the triangle
    //  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    //  glutSwapBuffers();
    //}

    //static void reshape(int w, int h)
    //{
    //  glViewport(0, 0, w, h);
    //}
  };

}
