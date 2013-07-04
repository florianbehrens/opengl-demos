#version 330

uniform mat4 mvp;

in vec3 vertexPosition;

out vec3 direction;

void main()
{
    direction = vertexPosition;
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
