#pragma once

namespace octet {

  class Branch {

  public:

    mat4t model_to_world;

    float vertices[12]{}

    void Init(float node_pos_x, float node_pos_y, float shape_half_width, float shape_half_height) {
      vertices = {
        (node_pos_x + shape_half_width), node_pos_y, 0,  // Lower right
        (node_pos_x - shape_half_width), node_pos_y, 0,  // Lower left
        (node_pos_x + shape_half_width), (node_pos_y + (2 * shape_half_height)), 0,  // Upper right
        (node_pos_x - shape_half_width), (node_pos_y + (2 * shape_half_height)), 0   // Upper left
      }
    }

    void render(texture_shader &shader, mat4t &cameraToWorld)
    {
      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      // set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      // use "old skool" rendering
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shader.render(modelToProjection, 0, tint_colour, blend_mode);

      // this is an array of the positions of the corners of the sprite in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -halfWidth, -halfHeight, 0,
        halfWidth, -halfHeight, 0,
        halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
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
