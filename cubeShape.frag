#version 330 core
out vec4 outColor;

uniform float time;
uniform vec3 flatColor;

void main()
{
    outColor = vec4(flatColor,1.0);
}
