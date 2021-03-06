#pragma once

#include "vcl/external_lib/headers/glad.hpp"
#include "vcl/core/math/vec/vec3/vec3.hpp"
#include <vector>

namespace vcl
{
struct segments_gpu
{
    segments_gpu();
    segments_gpu(const std::vector<vec3>& position);

    GLuint vao;
    GLuint vbo_position;
    unsigned int number_elements;
};

void draw(const segments_gpu& segments);
}
