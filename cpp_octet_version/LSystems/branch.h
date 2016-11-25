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

  class Branch {

    int depth_from_trunk;

    vec2 half_size_ = (1, 1);

    vec4 root_colour_ = { 0.66f, 0.63f, 0.51f, 1 };
    vec4 tip_colour_ = { 0.7f, 0.67f, 0.59f, 1 };


  public:
    
    mat4t model_to_world_;

    std::array<float, 12> vertices_;

    static bool& IsAfterSplit() { static bool is_after_split; return is_after_split; }
    
    // Loads a new branch into the tree branches array,
    // initialises the new branche's size and vertex positions,
    // returns a reference to the new branch.
    static Branch& NewBranch(std::vector<Branch>& branches, const vec2& parent_half_size, const float& thinning_ratio) {
      // Add a new branch to the branches vector
      branches.push_back(Branch());
      Branch& new_branch = branches.at(branches.size() - 1);
      
      new_branch.model_to_world_.loadIdentity();  // Reset its transforms
      new_branch.half_size_ = vec2(parent_half_size.x() * thinning_ratio, parent_half_size.y());  // Adapt the size

      // Setup the vertex positions
      new_branch.vertices_ = {
        - parent_half_size.x()     , - new_branch.half_size_.y() * 2, 0,  // Lower left
          parent_half_size.x()     , - new_branch.half_size_.y() * 2, 0,  // Lower right
          new_branch.half_size_.x(), 0                              , 0,  // Upper right
        - new_branch.half_size_.x(), 0                              , 0   // Upper left
      };

      //printf("Parent: %f, %f    New: %f, %f\n", parent_half_size.x(), parent_half_size.y(), new_branch.half_size_.x(), new_branch.half_size_.y());
      return new_branch;
    }

    const vec2& HalfSize() {
      return half_size_;
    }

    // Function from Octet Invaiderers example
    void SetRelative(Branch& rhs, float x, float y)
    {
      model_to_world_ = rhs.model_to_world_;
      model_to_world_.translate(x, y, 0);
    }

    // Function adapted from Octet Invaiderers example
    void Render(color_shader& shader, mat4t& camera_to_world)    // From octet
    {
      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t model_to_projection = mat4t::build_projection_matrix(model_to_world_, camera_to_world);

      // use "old skool" rendering
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shader.render(model_to_projection, root_colour_);

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices_.data());
      glEnableVertexAttribArray(attribute_pos);

      // finally, draw the box (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

  };

}
