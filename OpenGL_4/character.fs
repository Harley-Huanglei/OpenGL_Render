#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{   
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoord).r);
    FragColor = vec4(textColor, 0.5) * sampled;
}