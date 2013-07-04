#version 330

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

layout (location = 0) out vec4 fragColor;

uniform sampler2D ambientOcclusionTexture;
uniform sampler2D diffuseTexture;

void phongModel( const in vec3 pos, const in vec3 norm,
                 out vec3 ambient, out vec3 diffuse )
{
    // Some useful vectors
    vec3 s = normalize( vec3( light.position ) - pos );
    vec3 v = normalize( -pos );
    vec3 r = reflect( -s, norm );

    // Calculate the ambient contribution
    ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( s, norm ), 0.0 );
    diffuse = light.intensity * material.Kd * sDotN;
}

void main()
{
    vec3 ambient, diffuse;
    phongModel( position, normalize( normal ), ambient, diffuse );

    vec4 diffuseTextureColor = texture( diffuseTexture, texCoord );
    vec4 diffuseColor = vec4( diffuse, 1.0 ) * diffuseTextureColor;

    float aoFactor = texture( ambientOcclusionTexture, texCoord ).r;
    vec4 ambientColor = vec4( ambient, 1.0 ) * aoFactor;

    fragColor = ambientColor + diffuseColor;
}
