#version 330

uniform sampler2D ySampler;
uniform sampler2D uvSampler;

uniform float filterPosition;

in vec2 fragmentTextureCoords;

layout(location = 0) out vec4 fragColor;

void main( void )
{
    float y = texture( ySampler, fragmentTextureCoords ).r;
    float u = texture( uvSampler, fragmentTextureCoords ).r;
    float v = texture( uvSampler, fragmentTextureCoords ).g;

    float r = y + 1.402 * ( v - 0.5 );
    float g = y - 0.34414 * ( u - 0.5 ) - 0.71414 * ( v - 0.5 );
    float b = y + 1.772 * ( u - 0.5 );

    fragColor = vec4( r, g, b, 1.0 );
}
