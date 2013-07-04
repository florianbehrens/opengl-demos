#version 330

uniform sampler2D textureSampler;

in vec2 fragmentTextureCoords;

layout(location = 0, index = 0) out vec4 fragmentColor;

void main( void )
{
    // Get the color for this fragment by using the texture sampler and
    // the interpolated texture coordinates
    vec4 color = texture( textureSampler, fragmentTextureCoords );

    fragmentColor = color;

}
