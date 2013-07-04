#version 120

attribute vec3 vertexPosition;
attribute vec3 vertexColor;

varying vec3 color;

void main()
{
    // Pass through the color to the fragment shader
    color = vertexColor;

    // Calculate the vertex position
    gl_Position = vec4( vertexPosition, 1.0 );
}
