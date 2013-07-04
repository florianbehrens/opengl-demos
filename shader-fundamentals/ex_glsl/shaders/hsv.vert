#version 330

in vec3 vertexPosition;
in vec3 vertexColor;

out vec3 color;

vec3 hsvToRgb(vec3 hsv)
{
    // luminance = saturation * value
    float luminance = hsv.y * hsv.z; 
        
    float h2 = hsv.x * 6;
    float x = luminance * (1 - abs(mod(h2, 2) - 1));
    
    if (h2 < 1) return vec3(luminance, x, 0);
    if (h2 < 2) return vec3(x, luminance, 0);
    if (h2 < 3) return vec3(0, luminance, x);
    if (h2 < 4) return vec3(0, x, luminance);
    if (h2 < 5) return vec3(x, 0, luminance);
    return vec3(luminance, 0, x);
}

void main()
{
    // Pass through the color to the fragment shader
    color = hsvToRgb(vertexColor);

    // Calculate the vertex position
    gl_Position = vec4( vertexPosition, 1.0 );
}
