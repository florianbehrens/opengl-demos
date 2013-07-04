#version 330

uniform mat4 mvp;

in vec3 vertexPosition;

out vec3 reflectDir;

void main()
{

    reflectDir = vertexPosition;
    gl_Position = vec4( mvp * vec4( vertexPosition, 1.0 ) ).xyww;
}
