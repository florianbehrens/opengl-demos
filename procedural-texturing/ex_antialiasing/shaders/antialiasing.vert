#version 330

layout (location = 0) in vec3 vertexPosition;

out vec2 texCoord;

uniform mat4 mvp;

void main()
{
    // Generate texture coords
    texCoord = vertexPosition.xz + vec2(0.5, 0.5);

    // Transform vertex to clip space
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}