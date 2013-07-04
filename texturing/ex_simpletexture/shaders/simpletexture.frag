#version 330

uniform sampler2D textureSampler;
uniform float filterPosition;

in vec2 fragmentTextureCoords;

layout(location = 0, index = 0) out vec4 fragmentColor;

void main( void )
{
    // Get the color for this fragment by using the texture sampler and
    // the interpolated texture coordinates
    vec4 color = texture( textureSampler, fragmentTextureCoords );

    // Does this fragment lie inside or outside the filtered region?
    if ( fragmentTextureCoords.s >= filterPosition )
    {
        // Let's filter the right-hand side of the image so that it is in greyscale
        float grey = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;
        fragmentColor = vec4( grey, grey, grey, 1.0f );
    }
    else
    {
        // Leave the left-hand side of the image alone
        fragmentColor = color;
    }
}
