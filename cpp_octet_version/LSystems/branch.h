#pragma once

namespace octet {

  class Branch {

  public:

    mat4t model_to_world_;

    float vertices_[12];

    void Init(float node_pos_x, float node_pos_y, float shape_half_width, float shape_half_height) {
      float init_vertices[12] = {
        (node_pos_x + shape_half_width), node_pos_y, 0,  // Lower right
        (node_pos_x - shape_half_width), node_pos_y, 0,  // Lower left
        (node_pos_x + shape_half_width), (node_pos_y + (2 * shape_half_height)), 0,  // Upper right
        (node_pos_x - shape_half_width), (node_pos_y + (2 * shape_half_height)), 0   // Upper left
      }
      vertices_ = init_vertices;
    }

    vec2& Size() {
      return vec2(1, 1);
    }

    void SetRelative(Branch &rhs, float x, float y)
    {
      modelToWorld = rhs.modelToWorld;
      modelToWorld.translate(x, y, 0);
    }

    void Render(texture_shader &shader, mat4t &camera_to_world)
    {
      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t model_to_projection = mat4t::build_projection_matrix(model_to_world_, camera_to_world);

      // use "old skool" rendering
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shader.render(model_to_projection, 0);

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices_);
      glEnableVertexAttribArray(attribute_pos);

      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)uvs);
      glEnableVertexAttribArray(attribute_uv);

      // finally, draw the sprite (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

  };

}
