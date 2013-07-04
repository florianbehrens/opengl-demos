#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

out vec3 color;

// Matrices for transforming between coord systems
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvp;

uniform vec4 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 kd; // Diffuse reflectivity

void main()
{
    // Transform vertex normal and position to eye coordinates
    vec3 normal = normalize( normalMatrix * vertexNormal );
    vec3 position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( lightPosition ) - position );

    // Calculate the diffuse lighting factor
    float diffuse = max( dot( s, normal ), 0.0 );

    // Multiply by incoming light intensity
    color = lightIntensity * kd * diffuse;

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
