#version 330

in vec3 position;
in vec2 texCoord;

uniform int pass = 1;
uniform float width;
uniform float height;
uniform sampler2D texture0;

/*
// Use non-filtered texture fetches
uniform float pixelOffset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270,
                                   0.1945945946,
                                   0.1216216216,
                                   0.0540540541,
                                   0.0162162162 );
*/

// Use bilinear filtered texture fetches
uniform float pixelOffset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270,
                                   0.3162162162,
                                   0.0702702703 );

layout( location = 0 ) out vec4 fragColor;

float luminance( vec3 color )
{
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main()
{
    vec4 sum;

    if ( pass == 1 )
    {
        float dy = 1.0 / height;
        sum = texture( texture0, texCoord ) * weight[0];
        for ( int i = 1; i < 3; i++ )
        {
             sum += texture( texture0, texCoord + vec2( 0.0, pixelOffset[i] ) * dy ) * weight[i];
             sum += texture( texture0, texCoord - vec2( 0.0, pixelOffset[i] ) * dy ) * weight[i];
        }
    }
    else
    {
        float dx = 1.0 / width;
        sum = texture( texture0, texCoord ) * weight[0];
        for ( int i = 1; i < 3; i++ )
        {
           sum += texture( texture0, texCoord + vec2( pixelOffset[i], 0.0 ) * dx ) * weight[i];
           sum += texture( texture0, texCoord - vec2( pixelOffset[i], 0.0 ) * dx ) * weight[i];
        }
    }

    fragColor = sum;
}
