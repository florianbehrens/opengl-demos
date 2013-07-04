#include "instancedgeometryscene.h"

#include <camera.h>
#include <cube.h>
#include <torus.h>
#include <material.h>

#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFunctions_3_2_Core>
#if defined(Q_OS_MAC)
#include <QOpenGLExtensions>
#endif

#include <math.h>

static const int pointCount = 400;

InstancedGeometryScene::InstancedGeometryScene( QObject* parent )
    : AbstractScene( parent ),
      m_funcs( 0 ),
#if defined(Q_OS_MAC)
      m_instanceFuncs( 0 ),
#endif
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_torus( 0 ),
      m_theta( 0.0f ),
      m_modelMatrix(),
      m_data( 3 * pointCount ),
      m_data2( 3 * pointCount )
{
    m_modelMatrix.setToIdentity();
    update( 0.0f );

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 20.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void InstancedGeometryScene::initialise()
{
    // Resolve the OpenGL functions that we need for instanced rendering
#if !defined(Q_OS_MAC)
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();
#else
    m_instanceFuncs = new QOpenGLExtension_ARB_instanced_arrays;
    if ( !m_instanceFuncs->initializeOpenGLFunctions() )
        qFatal( "Could not resolve GL_ARB_instanced_arrays functions" );

    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_2_Core>();
#endif
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();

    // Create a Material we can use to operate on instanced geometry
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/instancedgeometry.vert",
                          ":/shaders/instancedgeometry.frag" );

    // Create a torus
    m_torus = new Torus( this );
    m_torus->setMinorRadius( 0.25 );
    m_torus->setMajorRadius( 0.5 );
    m_torus->setMaterial( material );
    m_torus->create();

    // Create a cube
    m_cube = new Cube( this );
    m_cube->setMaterial( material );
    m_cube->create();

    // Create a pair of VBOs ready to hold our data
    prepareVertexBuffers();

    // Tell OpenGL how to pass the data VBOs to the shader program
    prepareVertexArrayObject();

    // Enable depth testing to prevent artifacts
    glEnable( GL_DEPTH_TEST );

    // Cull back facing triangles to save the gpu some work
    glEnable( GL_CULL_FACE );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}

void InstancedGeometryScene::update( float t )
{
    const float xMin = -150.0f, xMax = 150.0f;
    const float dx = ( xMax - xMin ) / static_cast<float>( pointCount - 1 );
    for ( int i = 0; i < pointCount; ++i )
    {
        float x = xMin + static_cast<float>( i ) * dx;
        float y1 = 6.0f * sin( 0.4f * x - 0.4 * t );
        float y2 = 4.0f * cos( 0.7f * x - 0.2 * t );

        m_data[3 * i]     = x;
        m_data[3 * i + 1] = y1;
        m_data[3 * i + 2] = 0.0f;

        m_data2[3 * i]     = x;
        m_data2[3 * i + 1] = y2;
        m_data2[3 * i + 2] = 3.0f;
    }

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

void InstancedGeometryScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );

    m_dataBuffer2.bind();
    m_dataBuffer2.allocate( m_data2.data(), m_data2.size() * sizeof( float ) );

    // Bind the shader program
    QOpenGLShaderProgramPtr shader = m_torus->material()->shader();

    // Calculate needed matrices
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "material.kd", QVector3D( 0.9f, 0.0f, 0.0f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.shininess", 100.0f );

    // Draw the torus
    m_torus->vertexArrayObject()->bind();
    m_funcs->glDrawElementsInstanced( GL_TRIANGLES, m_torus->indexCount(), GL_UNSIGNED_INT, 0, pointCount );

    // Change the material for the cubes. No need to bind it as it is the same material
    // shared with the torus. We just need to alter some uniform variables
    shader->setUniformValue( "material.kd", QVector3D( 0.0f, 0.6f, 0.1f ) );
    m_cube->vertexArrayObject()->bind();
    m_funcs->glDrawElementsInstanced( GL_TRIANGLES, m_cube->indexCount(), GL_UNSIGNED_INT, 0, pointCount );
}

void InstancedGeometryScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.3, 1000.0f );
}

void InstancedGeometryScene::prepareVertexBuffers()
{
    // Populate the data buffer object
    m_dataBuffer.create();
    m_dataBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );

    m_dataBuffer2.create();
    m_dataBuffer2.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_dataBuffer2.bind();
    m_dataBuffer2.allocate( m_data2.data(), m_data2.size() * sizeof( float ) );
}

void InstancedGeometryScene::prepareVertexArrayObject()
{
    // Bind the marker's VAO
    m_torus->vertexArrayObject()->bind();

    // Enable the data buffer and add it to the marker's VAO
    QOpenGLShaderProgramPtr shader = m_torus->material()->shader();
    shader->bind();
    m_dataBuffer.bind();
    shader->enableAttributeArray( "point" );
    shader->setAttributeBuffer( "point", GL_FLOAT, 0, 3 );

    // We only vary the point attribute once per instance
    GLuint pointLocation = shader->attributeLocation( "point" );
#if !defined(Q_OS_MAC)
    m_funcs->glVertexAttribDivisor( pointLocation, 1 );
#else
    m_instanceFuncs->glVertexAttribDivisorARB( pointLocation, 1 );
#endif
    m_torus->vertexArrayObject()->release();


    // Bind the second data series marker's VAO
    m_cube->vertexArrayObject()->bind();

    // Enable the data buffer and add it to the marker's VAO
    m_dataBuffer2.bind();
    shader->enableAttributeArray( "point" );
    shader->setAttributeBuffer( "point", GL_FLOAT, 0, 3 );

    // We only vary the point attribute once per instance
#if !defined(Q_OS_MAC)
    m_funcs->glVertexAttribDivisor( pointLocation, 1 );
#else
    m_instanceFuncs->glVertexAttribDivisorARB( pointLocation, 1 );
#endif
    m_cube->vertexArrayObject()->release();
}
