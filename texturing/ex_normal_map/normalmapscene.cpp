#include "normalmapscene.h"

#include <camera.h>
#include <mesh.h>
#include <objloader.h>

#include <QGLWidget>
#include <QImage>

NormalMapScene::NormalMapScene( QObject* parent )
    : CameraScene( parent ),
      m_mesh( 0 ),
      m_theta( 0.0f ),
      m_rotate( false )
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
                               "../../../opengl/assets/meshes/ogrehead/ogrehead_diffuse.png",
                               "../../../opengl/assets/meshes/ogrehead/ogre_normalmap.png" );
#else
    MaterialPtr material = createTextureMaterial(
                               "../../../../../../opengl/assets/meshes/ogrehead/ogrehead_diffuse.png",
                               "../../../../../../opengl/assets/meshes/ogrehead/ogre_normalmap.png" );
#endif

    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( true );
    loader.setTangentGenerationEnabled( true );
#if !defined(Q_OS_MAC)
    loader.load( "../../../opengl/assets/meshes/ogrehead/ogrehead.obj" );
#else
    loader.load( "../../../../../../opengl/assets/meshes/ogrehead/ogrehead.obj" );
#endif

    m_mesh = new Mesh( this );
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

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
    if ( m_rotate )
    {
        m_theta += 0.5f;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );
}

void NormalMapScene::render()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_mesh->material()->bind();
    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.9f, 0.9f, 0.9f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.2f, 0.2f, 0.2f ) );
    shader->setUniformValue( "material.shininess", 20.0f );

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();
}

MaterialPtr NormalMapScene::createTextureMaterial( const QString& diffuseFileName,
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

    // Create a normal map
    TexturePtr texture1( new Texture( Texture::Texture2D ) );
    texture1->create();
    texture1->bind();
    texture1->setImage( QImage( normalFileName ) );
    texture1->generateMipMaps();

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

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, QByteArrayLiteral( "texture1" ) );
#endif

    return material;
}
