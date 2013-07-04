#version 330

uniform bool drawSkyBox;
uniform vec3 worldCameraPosition;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;
uniform mat4 mvpNoTranslate;

struct MaterialInfo {
    float eta;              // Ratio of indices of refraction
    float reflectionFactor; // Fraction of light reflected
};
uniform MaterialInfo material;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out vec3 reflectDir;
out vec3 refractDir;

void main()
{
    if ( drawSkyBox )
    {
        reflectDir = vertexPosition;
        gl_Position = vec4( mvp * vec4( vertexPosition, 1.0 ) ).xyww;
    }
    else
    {
        vec3 worldPos = vec3( modelMatrix * vec4( vertexPosition, 1.0 ) );
        vec3 worldNorm = normalize( vec3( modelMatrix * vec4( vertexNormal, 0.0 ) ) );
        vec3 worldView = normalize( worldCameraPosition - worldPos );

        reflectDir = reflect( -worldView, worldNorm );
        refractDir = refract( -worldView, worldNorm, material.eta );

        gl_Position = mvp * vec4( vertexPosition, 1.0 );
    }
}
