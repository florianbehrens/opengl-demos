#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

out vec3 v_position;
out vec3 v_normal;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;

void main()
{
    v_normal = normalize( normalMatrix * vertexNormal );
    v_position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
