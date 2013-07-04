#version 120

varying vec3 color;

void main()
{
    // The color variable is interpolated to this fragment's position
    // for us by OpenGL.
    gl_FragColor = vec4( color, 1.0 );
}
