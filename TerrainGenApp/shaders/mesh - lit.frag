#version 460 core

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 FragNorm;
in vec3 FragPos;

uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}