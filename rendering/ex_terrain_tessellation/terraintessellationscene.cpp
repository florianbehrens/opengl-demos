#include "terraintessellationscene.h"

#include "camera.h"
#include "mesh.h"
#include "objloader.h"

#include <QImage>
#include <QGLWidget>
#include <QObject>
#include <QOpenGLFunctions_4_0_Core>

#include <math.h>

TerrainTessellationScene::TerrainTessellationScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_patchBuffer( QOpenGLBuffer::VertexBuffer ),
      m_modelMatrix(),
      m_horizontalScale( 10.0f ),
      m_verticalScale( 0.4f ),
      m_theta( 0.0f )
{
    m_modelMatrix.setToIdentity();

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 10.0f, 0.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 10.0f, 1.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void TerrainTessellationScene::initialise()
{
    m_material = MaterialPtr( new Material );
    m_material->setShaders( ":/shaders/terraintessellation.vert",
                            ":/shaders/terraintessellation.tcs",
                            ":/shaders/terraintessellation.tes",
                            ":/shaders/terraintessellation.geom",
                            ":/shaders/terraintessellation.frag" );

    QImage heightMapImage( "../../../opengl/assets/textures/heightmap-1024x1024.png" );
    TexturePtr heightMap( new Texture );
    heightMap->create();
    heightMap->bind();
    heightMap->setImage( heightMapImage );

    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );

    m_material->setTextureUnitConfiguration( 0, heightMap, sampler, QByteArrayLiteral( "heightMap" ) );


    // Create a renderable object
    prepareVertexBuffers( heightMapImage.size() );
    prepareVertexArrayObject();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    m_funcs = m_context->versionFunctions<QOpenGLFunctions_4_0_Core>();
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();
}

void TerrainTessellationScene::update( float t )
{
    Q_UNUSED( t );
    m_theta += 0.2;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;

    m_modelMatrix.setToIdentity();
    m_modelMatrix.scale( 50.0f );
    m_modelMatrix.translate( -5.0f, 0.0f, 0.0f );

    // Update the camera position and orientation
    Camera::CameraTranslationOption option = m_viewCenterFixed
                                           ? Camera::DontTranslateViewCenter
                                           : Camera::TranslateViewCenter;
    m_camera->translate( QVector3D( m_vx, m_vy, m_vz ), option );

    if ( !qFuzzyIsNull( m_panAngle ) )
    {
        m_camera->pan( m_panAngle );
        m_panAngle = 0.0f;
    }

    if ( !qFuzzyIsNull( m_tiltAngle ) )
    {
        m_camera->tilt( m_tiltAngle );
        m_tiltAngle = 0.0f;
    }
}

void TerrainTessellationScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_material->bind();
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    // Set the horizontal and vertical scales applied in the tess eval shader
    shader->setUniformValue( "horizontalScale", m_horizontalScale );
    shader->setUniformValue( "verticalScale", m_verticalScale );

    // Pass in the model matrix to transform our grid
    QMatrix4x4 viewMatrix = m_camera->viewMatrix();
    QMatrix4x4 modelViewMatrix = viewMatrix * m_modelMatrix;
    QMatrix4x4 mvp = m_camera->projectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "mvp", mvp );

    // We also need the viewport matrix (to transform into screen-space)
    shader->setUniformValue( "viewportMatrix", m_viewportMatrix );

    // Set the wireframe line properties
    shader->setUniformValue( "line.width", 1.0f );
    shader->setUniformValue( "line.color", QVector4D( 0.0f, 1.0f, 0.0f, 1.0f ) );

    // Render the quad as a patch
    m_vao.bind();
    m_funcs->glPatchParameteri( GL_PATCH_VERTICES, 1 );
    glDrawArrays( GL_PATCHES, 0, m_patchCount );
}

void TerrainTessellationScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.1, 128.0f );

    // Update the viewport matrix
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    m_viewportMatrix.setToIdentity();
    m_viewportMatrix.setColumn( 0, QVector4D( w2, 0.0f, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 1, QVector4D( 0.0f, h2, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 2, QVector4D( 0.0f, 0.0f, 1.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 3, QVector4D( w2, h2, 0.0f, 1.0f ) );
}

void TerrainTessellationScene::prepareVertexBuffers( QSize heightMapSize )
{
    // Generate patch primitive data to cover the heightmap texture assuming
    // one heightmap sample per triangle at maximum tessellation level.

    // Each patch consists of a single point located at the lower-left corner
    // of a rectangle (in the xz-plane)
    const int maxTessellationLevel = 64;
    const int trianglesPerHeightSample = 3;
    const int xDivisions = trianglesPerHeightSample * heightMapSize.width() / maxTessellationLevel;
    const int zDivisions = trianglesPerHeightSample * heightMapSize.height() / maxTessellationLevel;
    m_patchCount = xDivisions * zDivisions;
    QVector<float> positionData( 2 * m_patchCount ); // 2 floats per vertex
    qDebug() << "Total number of patches =" << m_patchCount;

    const float dx = 1.0f / static_cast<float>( xDivisions );
    const float dz = 1.0f / static_cast<float>( zDivisions );

    for ( int j = 0; j < 2 * zDivisions; j += 2 ) {
        float z = static_cast<float>( j ) * dz * 0.5;
        for ( int i = 0; i < 2 * xDivisions; i += 2 ) {
            float x = static_cast<float>( i ) * dx * 0.5;
            const int index = xDivisions * j + i;
            positionData[index]     = x;
            positionData[index + 1] = z;
        }
    }

    m_patchBuffer.create();
    m_patchBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_patchBuffer.bind();
    m_patchBuffer.allocate( positionData.data(), positionData.size() * sizeof( float ) );
    m_patchBuffer.release();
}

void TerrainTessellationScene::prepareVertexArrayObject()
{
    // Create a VAO for this "object"
    m_vao.create();
    m_vao.bind();

    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();
    m_patchBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 2 );

    m_vao.release();
}

