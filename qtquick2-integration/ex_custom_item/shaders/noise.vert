#version 330

layout (location = 0) in vec3 vertexPosition;

out vec3 texCoord;

uniform mat4 mvp;
uniform float time;

void main()
{
    // Generate texture coords
    texCoord = vertexPosition.xyz * 2.0 + 0.05 * vec3( 0.0, -time, 0.0 );

    // Transform vertex to clip space
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}