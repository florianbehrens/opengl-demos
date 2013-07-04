#version 330

uniform vec3 worldCameraPosition;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;
uniform mat4 mvpNoTranslate;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out vec3 reflectDir;

void main()
{
    vec3 worldPos = vec3( modelMatrix * vec4( vertexPosition, 1.0 ) );
    vec3 worldNorm = normalize( vec3( modelMatrix * vec4( vertexNormal, 0.0 ) ) );
    vec3 worldView = normalize( worldCameraPosition - worldPos );

    reflectDir = reflect( -worldView, worldNorm );
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
