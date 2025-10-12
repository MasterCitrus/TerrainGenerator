#version 460 core

out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    //FragColor = texture(texture1, TexCoord);
    FragColor = vec4(0.8, 0.2, 0.6, 1.0);
}