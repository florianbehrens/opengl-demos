#include "quadtessellationscene.h"

#include "mesh.h"
#include "objloader.h"

#include <QObject>
#include <QOpenGLFunctions_4_0_Core>

#include <math.h>

QuadTessellationScene::QuadTessellationScene( QObject* parent )
    : AbstractScene( parent ),
      m_modelMatrix(),
      m_viewMatrix(),
      m_projectionMatrix(),
      m_quadBuffer( QOpenGLBuffer::VertexBuffer ),
      m_theta( 0.0f ),
      m_tessLevel( 2.0f )
{
    m_modelMatrix.setToIdentity();
}

void QuadTessellationScene::initialise()
{
    m_material = MaterialPtr( new Material );
    m_material->setShaders( ":/shaders/quadtessellation.vert",
                            ":/shaders/quadtessellation.tcs",
                            ":/shaders/quadtessellation.tes",
                            ":/shaders/quadtessellation.geom",
                            ":/shaders/quadtessellation.frag" );

    // Create a renderable object
    prepareVertexBuffers();
    prepareVertexArrayObject();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    m_funcs = m_context->versionFunctions<QOpenGLFunctions_4_0_Core>();
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();
}

void QuadTessellationScene::update( float t )
{
    Q_UNUSED( t );
    //m_theta += 0.2;
    if ( m_theta > 360.0f )
        m_theta -= 360.0f;
    static float dTessLevel = 0.05f;
    if ( m_tessLevel >= 64.0 || m_tessLevel < 2.0 )
        dTessLevel = -dTessLevel;
    m_tessLevel += dTessLevel;

    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 0.0f, 1.0f );
}

void QuadTessellationScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    // Pass in the model matrix to transform our quad
    shader->setUniformValue( "modelMatrix", m_modelMatrix );

    // Set the inner and outer tessellation levels
    shader->setUniformValue( "inner", m_tessLevel );
    shader->setUniformValue( "outer", m_tessLevel );

    // We also need the viewport matrix (to transform into screen-space)
    shader->setUniformValue( "viewportMatrix", m_viewportMatrix );

    // Set the wireframe line properties
    shader->setUniformValue( "line.width", 1.0f );
    shader->setUniformValue( "line.color", QVector4D( 0.0f, 1.0f, 0.0f, 1.0f ) );

    // Render the quad as a patch
    m_vao.bind();
    m_funcs->glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawArrays( GL_PATCHES, 0, 4 );
}

void QuadTessellationScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the viewport matrix
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    m_viewportMatrix.setToIdentity();
    m_viewportMatrix.setColumn( 0, QVector4D( w2, 0.0f, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 1, QVector4D( 0.0f, h2, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 2, QVector4D( 0.0f, 0.0f, 1.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 3, QVector4D( w2, h2, 0.0f, 1.0f ) );
}

void QuadTessellationScene::prepareVertexBuffers()
{
    float positionData[] = {
        -0.8f, -0.8f, 0.0f,
         0.8f, -0.8f, 0.0f,
         0.8f,  0.8f, 0.0f,
        -0.8f,  0.8f, 0.0f
    };

    m_quadBuffer.create();
    m_quadBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_quadBuffer.bind();
    m_quadBuffer.allocate( positionData, 4 * 3 * sizeof( float ) );
    m_quadBuffer.release();
}

void QuadTessellationScene::prepareVertexArrayObject()
{
    // Create a VAO for this "object"
    m_vao.create();
    m_vao.bind();

    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();
    m_quadBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_vao.release();
}
