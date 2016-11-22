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

    vec4 root_colour_ = { 1, 0.7f, 1, 1 };
    vec4 tip_colour_ = { 0.7f, 1, 0, 1 };

  public:
    mat4t model_to_world_;

    std::array<float, 12> vertices_;

    void Init(float node_pos_x, float node_pos_y, float branch_half_width, float branch_half_height) {
      vertices_ = {
        (node_pos_x + branch_half_width), (node_pos_y),                            0,  // Lower right
        (node_pos_x - branch_half_width), (node_pos_y),                            0,  // Lower left
        (node_pos_x + branch_half_width), (node_pos_y + (2 * branch_half_height)), 0,  // Upper right
        (node_pos_x - branch_half_width), (node_pos_y + (2 * branch_half_height)), 0   // Upper left
      };
    }

    vec2& Size() {
      return vec2(1, 1);
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
