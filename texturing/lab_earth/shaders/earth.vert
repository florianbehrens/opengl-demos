#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoord;

out vec3 position;
out vec3 normal;
out vec2 texCoord;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;

void main()
{
    texCoord = vertexTexCoord;
    normal = normalize( normalMatrix * vertexNormal );
    position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}