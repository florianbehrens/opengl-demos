#include "normalmapscene.h"

#include <camera.h>
#include <cube.h>
#include <plane.h>

#include <QGLWidget>
#include <QImage>

NormalMapScene::NormalMapScene( QObject* parent )
    : CameraScene( parent ),
      m_obj( 0 ),
      m_theta( 0.0f ),
      m_rotateCube( false )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 4.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void NormalMapScene::initialise()
{
    // Create a material that performs multi-texturing
#if !defined(Q_OS_MAC)
    MaterialPtr material = createTextureMaterial(
                               "../../../opengl/assets/textures/pattern_133/diffuse.png",
                               "../../../opengl/assets/textures/pattern_133/specular.png",
                               "../../../opengl/assets/textures/pattern_133/normal.png" );
#else
    MaterialPtr material = createTextureMaterial(
                               "../../../../../../opengl/assets/textures/pattern_133/diffuse.png",
                               "../../../../../../opengl/assets/textures/pattern_133/specular.png",
                               "../../../../../../opengl/assets/textures/pattern_133/normal.png" );
#endif

    // Create a cube and set the material on it
    m_obj = new Cube( this );
    m_obj->setMaterial( material );
    m_obj->create();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    // Set the clear color to white
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void NormalMapScene::update( float t )
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

void NormalMapScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_obj->material()->bind();
    QOpenGLShaderProgramPtr shader = m_obj->material()->shader();
    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 10.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.shininess", 10.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_obj->render();
}

MaterialPtr NormalMapScene::createTextureMaterial( const QString& diffuseFileName,
                                                   const QString& specularFileName,
                                                   const QString& normalFileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/normalmap.vert",
                          ":/shaders/normalmap.frag" );

    // Create a diffuse texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( diffuseFileName ) );
    texture0->generateMipMaps();

    // Create a specular texture map
    TexturePtr texture1( new Texture( Texture::Texture2D ) );
    texture1->create();
    texture1->bind();
    texture1->setImage( QImage( specularFileName ) );
    texture1->generateMipMaps();

    // Create a normal map
    TexturePtr texture2( new Texture( Texture::Texture2D ) );
    texture2->create();
    texture2->bind();
    texture2->setImage( QImage( normalFileName ) );
    texture2->generateMipMaps();

#if !defined(Q_OS_MAC)
    // Create a sampler. This can be shared by many textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, sampler, QByteArrayLiteral( "texture1" ) );
    material->setTextureUnitConfiguration( 2, texture2, sampler, QByteArrayLiteral( "texture2" ) );
#else
    texture0->bind();
    texture0->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture0->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture0->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture0->setMagnificationFilter( GL_LINEAR );

    texture1->bind();
    texture1->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture1->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture1->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture1->setMagnificationFilter( GL_LINEAR );

    texture2->bind();
    texture2->setWrapMode( Texture::DirectionS, GL_CLAMP_TO_EDGE );
    texture2->setWrapMode( Texture::DirectionT, GL_CLAMP_TO_EDGE );
    texture2->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    texture2->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, QByteArrayLiteral( "texture1" ) );
    material->setTextureUnitConfiguration( 2, texture2, QByteArrayLiteral( "texture2" ) );
#endif

    return material;
}
