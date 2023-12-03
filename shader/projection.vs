#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out vec3 world_pos;

uniform mat4 model;
uniform mat4 projection_1;
uniform mat4 view_1;

void main()
{
    world_pos = vec3(model * vec4(position, 1.0));
    gl_Position = projection_1 * view_1 * model * vec4(position, 1.0);
}