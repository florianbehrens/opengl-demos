#include "instancedhistogramscene.h"

#include <camera.h>
#include <cube.h>
#include <torus.h>
#include <material.h>
#include <sphere.h>

#include <QObject>
#include <QOpenGLContext>

#if !defined(Q_OS_MAC)
#include <QOpenGLFunctions_3_3_Compatibility>
#else
#include <QOpenGLFunctions_2_1>
#include <QOpenGLExtensions>
#endif

#include <math.h>

static const int xPoints = 100;
static const int zPoints = 100;
static const int pointCount = xPoints * zPoints;

InstancedHistogramScene::InstancedHistogramScene( QObject* parent )
    : AbstractScene( parent ),
      m_funcs( 0 ),
#if defined(Q_OS_MAC)
      m_instanceFuncs( 0 ),
      m_drawInstanced( 0 ),
#endif
      m_camera( new Camera( this ) ),
      m_vx( 0.0f ),
      m_vy( 0.0f ),
      m_vz( 0.0f ),
      m_viewCenterFixed( false ),
      m_theta( 0.0f ),
      m_modelMatrix(),
      m_data( 3 * pointCount ),
      m_sphere( 0 ),
      m_sphereModelMatrix(),
      m_time( 0.0f ),
      m_updatesEnabled( true ),
      m_frequency( 2.5f ),
      m_spatialFrequency( 1.0f ),
      m_scale( 2.0f ),
      m_timeScale( 0.05f ),
      m_redBias( 0.5f ),
      m_greenBias( 1.0f ),
      m_blueBias( 1.5f )
{
    m_modelMatrix.setToIdentity();
    update( 0.0f );

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 25.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );

    m_camera->translateWorld( QVector3D( 0.0f, 7.0f, 0.0f ), Camera::DontTranslateViewCenter );
}

void InstancedHistogramScene::initialise()
{
#if !defined(Q_OS_MAC)
    // Resolve the OpenGL 3.3 functions that we need for instanced rendering
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_3_Compatibility>();
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();
#else
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_2_1>();
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();

    m_instanceFuncs = new QOpenGLExtension_ARB_instanced_arrays();
    m_instanceFuncs->initializeOpenGLFunctions();

    m_drawInstanced = new QOpenGLExtension_ARB_draw_instanced();
    m_drawInstanced->initializeOpenGLFunctions();
#endif

    // Create a Material we can use to operate on instanced geometry
    MaterialPtr material( new Material );
#if !defined(Q_OS_MAC)
    material->setShaders( ":/shaders/instancedhistogram.vert",
                          ":/shaders/instancedhistogram.frag" );
#else
    material->setShaders( ":/shaders/instancedhistogram_2_1.vert",
                          ":/shaders/instancedhistogram_2_1.frag" );
#endif

    // Create a cube
    m_cube = new Cube;
    m_cube->setMaterial( material );
    m_cube->create();

    // Create a VBO ready to hold our data
    prepareVertexBuffers();

    // Tell OpenGL how to pass the data VBOs to the shader program
    prepareVertexArrayObject();

    // Use a sphere for the background - we'll render the scene
    // inside the sphere
    MaterialPtr noiseMaterial( new Material );
#if !defined(Q_OS_MAC)
    noiseMaterial->setShaders( ":/shaders/noise.vert",
                               ":/shaders/noise.frag" );
#else
    noiseMaterial->setShaders( ":/shaders/noise_2_1.vert",
                               ":/shaders/noise_2_1.frag" );
#endif

    m_sphere = new Sphere;
    m_sphere->setMaterial( noiseMaterial );
    m_sphere->create();

    m_sphereModelMatrix.setToIdentity();
    m_sphereModelMatrix.scale( 30.0f );

    // Enable depth testing to prevent artifacts
    glEnable( GL_DEPTH_TEST );
}

void InstancedHistogramScene::cleanup()
{
    delete m_cube;
    delete m_sphere;
}

void InstancedHistogramScene::update( float t )
{
    QMutexLocker locker( &m_mutex );

    // Update the data being plotted
    if ( m_updatesEnabled )
    {
        const float xMin = -15.0f, xMax = 15.0f;
        const float zMin = -15.0f, zMax = 15.0f;
        const float dx = ( xMax - xMin ) / static_cast<float>( xPoints - 1 );
        const float dz = ( zMax - zMin ) / static_cast<float>( zPoints - 1 );
        int i = 0;
        const float A = 5.0;
        for ( int zi = 0; zi < zPoints; ++zi )
        {
            float z = zMin + static_cast<float>( zi ) * dz;

            for ( int xi = 0; xi < xPoints; ++xi )
            {
                float x = xMin + static_cast<float>( xi ) * dx;

                double r = sqrt( x * x + z * z );
                float y = A * ( sinf( m_frequency * t ) + cosf( m_frequency * t ) ) * j0( m_spatialFrequency * r );

                m_data[3 * i]     = x;
                m_data[3 * i + 1] = y;
                m_data[3 * i + 2] = z;
                ++i;
            }
        }
    }

    // Store the time to pass into the shader
    const float dt = t - m_time;
    m_time = t;

    // Update the camera position and orientation
    Camera::CameraTranslationOption option = m_viewCenterFixed
                                           ? Camera::DontTranslateViewCenter
                                           : Camera::TranslateViewCenter;
    m_camera->translate( QVector3D( m_vx * dt, m_vy * dt, m_vz * dt ), option );

    if ( !qFuzzyIsNull( m_panAngle ) )
    {
        m_camera->pan( m_panAngle * dt );
        m_panAngle = 0.0f;
    }

    if ( !qFuzzyIsNull( m_tiltAngle ) )
    {
        m_camera->tilt( m_tiltAngle * dt );
        m_tiltAngle = 0.0f;
    }
}

void InstancedHistogramScene::render()
{
    glDepthMask( true );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    {
        QMutexLocker locker( &m_mutex );
        glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
        m_dataBuffer.bind();
        m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );
        glPopClientAttrib();

        // Bind the shader program
        m_cube->material()->bind();
        QOpenGLShaderProgramPtr shader = m_cube->material()->shader();

        // Calculate needed matrices
        m_modelMatrix.setToIdentity();
        m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

        QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_modelMatrix;
        QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
        shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
        shader->setUniformValue( "normalMatrix", normalMatrix );
        shader->setUniformValue( "projectionMatrix", m_camera->projectionMatrix() );
    }

    // Scale the x-z dimensions of the cuboids
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    shader->setUniformValue( "cubeScale", QVector2D( 0.25f, 0.25f ) );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "material.ks", QVector3D( 0.95f, 0.95f, 0.95f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.shininess", 100.0f );

    // Draw the cuboids
#if !defined(Q_OS_MAC)
    m_cube->vertexArrayObject()->bind();
    m_funcs->glDrawElementsInstanced( GL_TRIANGLES, m_cube->indexCount(), GL_UNSIGNED_INT, 0, pointCount );
#else
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
    m_cube->bindBuffers();

    m_dataBuffer.bind();
    shader->enableAttributeArray( "point" );
    shader->setAttributeBuffer( "point", GL_FLOAT, 0, 3 );
    GLuint pointLocation = shader->attributeLocation( "point" );
    m_instanceFuncs->glVertexAttribDivisorARB( pointLocation, 1 );

    m_drawInstanced->glDrawElementsInstancedARB( GL_TRIANGLES, m_cube->indexCount(), GL_UNSIGNED_INT, 0, pointCount );
    glPopClientAttrib();
#endif

    // Draw the background sphere
    m_sphere->material()->bind();
    shader = m_sphere->material()->shader();

    {
        QMutexLocker locker( &m_mutex );

        // Set the mvp matrix
        QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_sphereModelMatrix;
        shader->setUniformValue( "mvp", mvp );
        shader->setUniformValue( "time", m_time );
        shader->setUniformValue( "texCoordScale", m_scale );
        shader->setUniformValue( "texCoordTimeScale", m_timeScale );
        QVector3D colorBias( m_redBias, m_greenBias, m_blueBias );
        shader->setUniformValue( "colorBias", colorBias );
    }

    // Let the mesh setup the remainder of its state and draw itself
#if !defined(Q_OS_MAC)
    m_sphere->render();
#else
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
    m_sphere->bindBuffers();
    m_sphere->render();
    glPopClientAttrib();
#endif

    shader->release();
}

void InstancedHistogramScene::resize( int w, int h )
{
    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 60.0f, aspect, 0.3, 100.0f );
}

void InstancedHistogramScene::prepareVertexBuffers()
{
    // Populate the data buffer object
    m_dataBuffer.create();
    m_dataBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );
    m_dataBuffer.release();
}

void InstancedHistogramScene::prepareVertexArrayObject()
{
#if !defined(Q_OS_MAC)
    // Bind the marker's VAO
    m_cube->vertexArrayObject()->bind();

    // Enable the data buffer and add it to the marker's VAO
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    shader->bind();
    m_dataBuffer.bind();
    shader->enableAttributeArray( "point" );
    shader->setAttributeBuffer( "point", GL_FLOAT, 0, 3 );

    // We only vary the point attribute once per instance
    GLuint pointLocation = shader->attributeLocation( "point" );

    m_funcs->glVertexAttribDivisor( pointLocation, 1 );
    m_cube->vertexArrayObject()->release();
#endif
}

void InstancedHistogramScene::setUpdatesEnabled( bool enabled )
{
    if ( m_updatesEnabled == enabled )
        return;
    QMutexLocker locker( &m_mutex );
    m_updatesEnabled = enabled;
    emit updatesEnabledChanged();
}

bool InstancedHistogramScene::updatesEnabled() const
{
    return m_updatesEnabled;
}

void InstancedHistogramScene::setFrequency( float frequency )
{
    if ( qFuzzyCompare( m_frequency, frequency ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_frequency = frequency;
    emit frequencyChanged();
}

float InstancedHistogramScene::frequency() const
{
    return m_frequency;
}

void InstancedHistogramScene::setSpatialFrequency( float spatialFrequency )
{
    if ( qFuzzyCompare( m_spatialFrequency, spatialFrequency ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_spatialFrequency = spatialFrequency;
    emit spatialFrequencyChanged();
}

float InstancedHistogramScene::spatialFrequency() const
{
    return m_spatialFrequency;
}

void InstancedHistogramScene::setScale( float scale )
{
    if ( qFuzzyCompare( m_scale, scale ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_scale = scale;
    emit scaleChanged();
}

float InstancedHistogramScene::scale() const
{
    return m_scale;
}

void InstancedHistogramScene::setTimeScale( float timeScale )
{
    if ( qFuzzyCompare( m_timeScale, timeScale ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_timeScale = timeScale;
    emit timeScaleChanged();
}

float InstancedHistogramScene::timeScale() const
{
    return m_timeScale;
}

void InstancedHistogramScene::setRedBias( float redBias )
{
    if ( qFuzzyCompare( m_redBias, redBias ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_redBias = redBias;
    emit redBiasChanged();
}

float InstancedHistogramScene::redBias() const
{
    return m_redBias;
}

void InstancedHistogramScene::setGreenBias( float greenBias )
{
    if ( qFuzzyCompare( m_greenBias, greenBias ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_greenBias = greenBias;
    emit greenBiasChanged();
}

float InstancedHistogramScene::greenBias() const
{
    return m_greenBias;
}

void InstancedHistogramScene::setBlueBias( float blueBias )
{
    if ( qFuzzyCompare( m_blueBias, blueBias ) )
        return;
    QMutexLocker locker( &m_mutex );
    m_blueBias = blueBias;
    emit blueBiasChanged();
}

float InstancedHistogramScene::blueBias() const
{
    return m_blueBias;
}
