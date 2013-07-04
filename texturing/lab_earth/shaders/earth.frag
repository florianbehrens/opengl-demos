#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D dayColor;
uniform sampler2D specularMap;
uniform sampler2D nightColor;

struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity; // A,D,S intensity
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 ka;            // Ambient reflectivity
    vec3 kd;            // Diffuse reflectivity
    vec3 ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

layout( location = 0 ) out vec4 fragColor;

void phongModel( vec3 pos, vec3 norm,
                 out vec3 ambient, out vec3 diffuse, out vec3 spec,
                 out float sDotN )
{
    // Some useful vectors
    vec3 s = normalize( vec3( light.position ) );
    vec3 v = normalize( -pos.xyz );
    vec3 r = reflect( -s, norm );

    // Calculate the ambient contribution
    ambient = light.intensity * material.ka;

    // Calculate the diffuse contribution
    sDotN = max( dot( s, norm ), 0.0 );
    diffuse = light.intensity * material.kd * sDotN;

    // Calculate the specular highlight contribution
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
    {
        spec = light.intensity * material.ks * pow( max( dot( r, v ), 0.0 ), material.shininess );
    }
}

vec4 gammaCorrect( const in vec4 color )
{
    float gamma = 1.0 / 1.8;
    return pow( color, vec4( gamma ) );
}

void main()
{
    // TODO 1: Fix direction of texture coordinates. The s component varies in the
    // wrong direction leading to all countries being flipped left-right


    // TODO 2: Sample the textures at the interpolated texCoords for daylight color
    // and the specular map. Also apply gamma correction to the daylight color


    // TODO 3: Calculate the lighting model, keeping the ambient, diffuse
    // and specular components separate


    // TODO 4: Calculate the daylight fragment color by multiplying the ambient plus diffuse color
    // by the daylight texture color and then adding on the specular highlight contribution


    // TODO 5: Blend the daylight and night time colors based upon the diffuse, sDotN factor

    // Remove this default:
    fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}
