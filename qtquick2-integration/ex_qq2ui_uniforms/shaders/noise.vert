#version 330

layout (location = 0) in vec3 vertexPosition;

out vec3 texCoord;

uniform mat4 mvp;
uniform float time;

uniform float texCoordScale = 2.0;
uniform float texCoordTimeScale = 0.05;

void main()
{
    // Generate texture coords
    texCoord = texCoordScale * vertexPosition.xyz + texCoordTimeScale * vec3( 0.0, -time, 0.0 );

    // Transform vertex to clip space
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}