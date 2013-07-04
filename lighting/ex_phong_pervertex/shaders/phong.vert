#version 330

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

out vec3 color;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

struct MaterialInfo
{
    vec3 ka;            // Ambient reflectivity
    vec3 kd;            // Diffuse reflectivity
    vec3 ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

void main()
{
    // Transform vertex normal and position to eye coordinates
    vec3 normal = normalize( normalMatrix * vertexNormal );
    vec3 position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( light.position ) - position );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize( -position );

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect( -s, normal );

    // Calculate the diffuse component
    float diffuse = max( dot( s, normal ), 0.0 );

    // Calculate the specular component
    float specular = 0.0;
    if ( dot( s, normal ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), material.shininess );

    // Combine the ambient, diffuse and specular contributions
    color = light.intensity * ( material.ka + material.kd * diffuse + material.ks * specular );

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
