#version 330

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

in vec3 position;
in vec3 normal;

layout (location = 0) out vec4 fragColor;


vec3 adsModel( const in vec3 pos, const in vec3 norm )
{
    // Calculate the vector from the light to the fragment
    // ... TODO
    
    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    // .... TODO

    // Reflect the light beam using the normal at this fragment
    // .... TODO

    // Calculate the diffuse component
    float diffuse = ... TODO
    
    // Calculate the specular component
    float specular = ... TODO

    // Combine the ambient, diffuse and specular contributions
    return light.intensity * ( material.ka + material.kd * diffuse + material.ks * specular );
}

void main()
{
    // Use adsModel() function to calculate the fragment color
    fragColor = TODO
}
