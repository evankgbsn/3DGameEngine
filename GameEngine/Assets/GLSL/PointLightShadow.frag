#version 460 core

in vec4 FragPos;

layout(std140, binding = 2) uniform PointLightData
{
    vec4 lightPos;
    float far_plane;
} lightData;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightData.lightPos.xyz);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / lightData.far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  