#include "texturescene.h"

#include <camera.h>
#include <cube.h>

#include <QGLWidget>
#include <QImage>

TextureScene::TextureScene( QObject* parent )
    : CameraScene( parent ),
      m_cube( 0 ),
      m_theta( 0.0f ),
      m_rotateCube( true )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.8f, 4.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void TextureScene::initialise()
{
    // Create a material that performs multi-texturing
#if !defined(Q_OS_MAC)
    MaterialPtr material = createTextureMaterial(
                               "../../../opengl/assets/textures/pattern_09/diffuse.png" );
#else
    MaterialPtr material = createTextureMaterial(
                               "../../../../../../opengl/assets/textures/pattern_09/diffuse.png" );
#endif

    // Create a cube and set the material on it
    m_cube = new Cube( this );
    m_cube->setMaterial( material );
    m_cube->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to white
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void TextureScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t );

    // Rotate the cube
    if ( m_rotateCube )
    {
        m_theta += 0.5f;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
    m_modelMatrix.rotate( 90.0f, 1.0f, 0.0f, 0.0f );
}

void TextureScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_cube->material()->bind();
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.7f, 0.7f, 0.7f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.4f, 0.4f, 0.4f ) );
    shader->setUniformValue( "material.shininess", 10.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_cube->render();
}

MaterialPtr TextureScene::createTextureMaterial( const QString& fileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/texture.vert",
                          ":/shaders/texture.frag" );

    // Create a texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( fileName ) );

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by many textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the texture with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
#else
    // OS X does not (yet) support OpenGL 3.3 and Samplers so we set the
    // filtering options on the texture object itself
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_LINEAR );
    texture0->setMagnificationFilter( GL_LINEAR );

    // Associate the texture with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
#endif

    return material;
}
