#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

 // 原样输出
float asDepth()
{
  return gl_FragCoord.z;
}
void main()
{
    float depth = asDepth();
    FragColor = vec4(vec3(depth), 1.0f);
}