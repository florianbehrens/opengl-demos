#include "uvmapscene.h"

#include "axisalignedboundingbox.h"
#include "mesh.h"
#include "objloader.h"

#include <QImage>
#include <QOpenGLFunctions>
#include <QPropertyAnimation>

#include <math.h>

UVmapScene::UVmapScene( QObject* parent )
    : CameraScene( parent ),
        m_theta( 0.0f )
{
    m_rotate = true;

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 2.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void UVmapScene::initialise()
{
    // Set the clear color to neutral grey-blue
    glClearColor( 0.4f, 0.4f, 0.6f, 1.0f );

    // Create a material that performs multi-texturing
    MaterialPtr material = createTextureMaterial( "assets/meshes/chest/diffuse.obj");

    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( true );
    loader.setTangentGenerationEnabled( true );
    loader.load( "assets/meshes/chest/chest2.obj" );

    m_mesh = new Mesh;
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    m_funcs = m_context->functions();
    m_funcs->initializeOpenGLFunctions();
    m_funcs->glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
}

void UVmapScene::update( float t )
{

    // Rotate the cube
    if ( m_rotate )
    {
        m_theta += t * 10;
        if ( m_theta > 360.0f )
            m_theta -= 360.0f;
    }
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

}

void UVmapScene::render()
{

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_mesh->material()->bind();
    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();
    m_camera->setStandardUniforms(shader, m_modelMatrix);

    m_mesh->render();

}

MaterialPtr UVmapScene::createTextureMaterial( const QString& diffuseFileName )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/simpletexture.vert",
                          ":/shaders/simpletexture.frag" );

    // Create a diffuse texture
    TexturePtr texture0( new Texture( Texture::Texture2D ) );
    texture0->create();
    texture0->bind();
    texture0->setImage( QImage( diffuseFileName ) );
    texture0->generateMipMaps();

    // Create a sampler. This can be shared by many textures
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setMinificationFilter( GL_LINEAR_MIPMAP_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture0, sampler, QByteArrayLiteral( "texture0" ) );

    return material;
}

void UVmapScene::toggleAnimation()
{
    m_rotate = !m_rotate;
}
