#version 120

attribute vec3 vertexPosition;
attribute vec3 vertexColor;

varying vec3 color;

uniform mat4 modelMatrix;

void main()
{
    // Pass through the color to the fragment shader
    color = vertexColor;

    // Calculate the vertex position
    gl_Position = modelMatrix * vec4( vertexPosition, 1.0 );
}
