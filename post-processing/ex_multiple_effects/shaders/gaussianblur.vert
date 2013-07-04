#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 vertexTexCoord;

out vec3 position;
out vec2 texCoord;

void main()
{
    // Pass through the texture coords and vertex position
    texCoord = vertexTexCoord;
    gl_Position = vec4( vertexPosition, 1.0 );
}