#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;

out vec3 color;

uniform mat4 rotationMatrix;

void main()
{
    // Pass through the color to the fragment shader
    color = vertexColor;

    // Calculate the vertex position
    gl_Position = rotationMatrix * vec4( vertexPosition, 1.0 );
}
