#version 330 core
out vec3 new_pixel_pos;

in vec3 world_pos;

uniform mat4 view_2;
uniform mat4 projection_2;
uniform vec4 viewport;

void main()
{
    vec4 position = projection_2 * view_2 * vec4(world_pos, 1.0);   // 已经乘过model了
    position=position/position.w;
    vec3 pixel_pos;
    pixel_pos.x=((position.x+1.0)*viewport.z/2.0+viewport.x)/viewport.z;
    pixel_pos.y=((position.y+1.0)*viewport.w/2.0+viewport.y)/viewport.w;
    pixel_pos.z=(position.z+1)/2;
    if(pixel_pos.x<0.0||pixel_pos.x>1.0||pixel_pos.y<0.0||pixel_pos.y>1.0)
    {
        discard;
    }
    new_pixel_pos=pixel_pos;
}