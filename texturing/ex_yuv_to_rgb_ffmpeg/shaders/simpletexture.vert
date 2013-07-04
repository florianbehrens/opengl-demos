#version 330

//uniform mat4 projectionMatrix;
//uniform mat4 modelViewMatrix;

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec2 textureCoords;

out vec2 fragmentTextureCoords;

void main( void )
{
    // Pass through the texture coordinates
    fragmentTextureCoords = textureCoords;

    // Calculate the clip-space coordinates
    gl_Position =
            //projectionMatrix * modelViewMatrix *
            vertex;
}
