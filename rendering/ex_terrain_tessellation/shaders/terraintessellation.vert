#version 400

layout (location = 0) in vec2 vertexPosition;

out vec2 v_position;

void main()
{
    v_position = vertexPosition;
}
