#version 150 core
in vec3 Color;
out vec4 outColor;
uniform float time;
uniform vec3 flatColor;

vec3 colorA = vec3(0.700,0.545,0.428);
vec3 colorB = vec3(0.516,0.461,0.550);
void main()
{
    //vec3 color = Color;
    //float pct = abs(sin(time));
    // Mix uses pct (a value from 0-1) to
    // mix the two colors
    //color = mix(colorA, colorB, pct);

    outColor = vec4(flatColor,1.0);
}
