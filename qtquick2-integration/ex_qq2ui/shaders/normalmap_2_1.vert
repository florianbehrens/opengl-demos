#version 120

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;
attribute vec3 vertexTangent;

varying vec3 lightDir;
varying vec3 viewDir;
varying vec2 texCoord;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;

struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity; // A,D,S intensity
};
uniform LightInfo light;

void main()
{
    // Pass through texture coordinates
    texCoord = vertexTexCoord;

    // Transform position, normal, and tangent to eye coords
    vec3 normal = normalize( normalMatrix * vertexNormal );
    vec3 tangent = normalize( normalMatrix * vertexTangent );
    vec3 position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    // Calculate binormal vector
    vec3 binormal = normalize( cross( normal, tangent ) );

    // Construct matrix to transform from eye coords to tangent space
    mat3 tangentMatrix = mat3 (
        tangent.x, binormal.x, normal.x,
        tangent.y, binormal.y, normal.y,
        tangent.z, binormal.z, normal.z );

    // Transform light direction and view direction to tangent space
    vec3 s = vec3( light.position ) - position;
    lightDir = normalize( tangentMatrix * s );

    vec3 v = -position;
    viewDir = normalize( tangentMatrix * v );

    // Calculate vertex position in clip coordinates
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
