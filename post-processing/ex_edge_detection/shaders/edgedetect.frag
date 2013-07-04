#version 330

in vec3 position;
in vec2 texCoord;

uniform float edgeThreshold;
uniform float width;
uniform float height;
uniform sampler2D texture0;

layout( location = 0 ) out vec4 fragColor;

float luminance( vec3 color )
{
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main()
{
    float dx = 1.0 / width;
    float dy = 1.0 / height;

    float s00 = luminance( texture( texture0, texCoord + vec2( -dx,  dy ) ).rgb );
    float s10 = luminance( texture( texture0, texCoord + vec2( -dx, 0.0 ) ).rgb );
    float s20 = luminance( texture( texture0, texCoord + vec2( -dx, -dy ) ).rgb );
    float s01 = luminance( texture( texture0, texCoord + vec2( 0.0,  dy ) ).rgb );
    float s21 = luminance( texture( texture0, texCoord + vec2( 0.0, -dy ) ).rgb );
    float s02 = luminance( texture( texture0, texCoord + vec2(  dx,  dy ) ).rgb );
    float s12 = luminance( texture( texture0, texCoord + vec2(  dx, 0.0 ) ).rgb );
    float s22 = luminance( texture( texture0, texCoord + vec2(  dx, -dy ) ).rgb );

    float sx = s00 + 2.0 * s10 + s20 - ( s02 + 2.0 * s12 + s22 );
    float sy = s00 + 2.0 * s01 + s02 - ( s20 + 2.0 * s21 + s22 );

    float dist = sx * sx + sy * sy;

    if ( dist > edgeThreshold )
        fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    else
        fragColor = vec4( 1.0 );
}
