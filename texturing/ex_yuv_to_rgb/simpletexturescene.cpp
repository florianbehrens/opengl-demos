#include "simpletexturescene.h"

#include "axisalignedboundingbox.h"
#include "mesh.h"
#include "objloader.h"

#include <QFile>
#include <QGLWidget>
#include <QImage>
#include <QOpenGLFunctions>
#include <QPropertyAnimation>

#include <math.h>

SimpleTextureScene::SimpleTextureScene( QObject* parent )
    : AbstractScene( parent ),
      m_shader(),
      m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_textureId( 0 ),
      m_uvTextureId( 0 ),
      m_projectionMatrix(),
      m_modelViewMatrix(),
      m_propertyAnimation( new QPropertyAnimation( this, "filterPosition", this ) ),
      m_filterPosition( 1.0 )
{
    // Setup a property animation to update the filter position
    m_propertyAnimation->setStartValue( 1.0 );
    m_propertyAnimation->setEndValue( 0.0 );
    m_propertyAnimation->setDuration( 4000 );
    m_propertyAnimation->setEasingCurve( QEasingCurve( QEasingCurve::SineCurve ) );
    m_propertyAnimation->setLoopCount( -1 );
}

void SimpleTextureScene::initialise()
{
    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Prepare a complete shader program...
    prepareShaderProgram( ":/shaders/simpletexture.vert",
                          ":/shaders/simpletexture.frag" );

    // Prepare the vertex, texture and index buffers
    prepareVertexBuffers();

    // Prepare the VAO
    prepareVertexArrayObject();

    // Prepare the texture data itself
    prepareTexture( ":/assets/test.nv12" );

    // Link texture unit 0 to the "textureSampler" variable in the fragment shader
    m_shader.setUniformValue( "ySampler", 0 );
    m_shader.setUniformValue( "uvSampler", 1 );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    m_funcs = m_context->functions();
    m_funcs->initializeOpenGLFunctions();
}

void SimpleTextureScene::update( float t )
{
    // Nothing to do here
    Q_UNUSED( t )
}

void SimpleTextureScene::render()
{
    m_shader.bind();

    // Set the shader start position uniform variable
    m_shader.setUniformValue( "filterPosition", m_filterPosition );

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Draw stuff
    glDrawElements( GL_TRIANGLES,       // Type of primitive to draw
                    6,                  // The number of indices in our vertex buffer we wish to draw
                    GL_UNSIGNED_INT,    // The element type of the index buffer
                    0 );                // Offset from the start of our index buffer of where to begin
}

void SimpleTextureScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    // Setup an orthogonal projection matrix
    qreal nearPlane = 1.0f;
    qreal farPlane = 100.0f;
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.ortho( -float( w ) / 2.0 , float( w ) / 2.0,
                              -float( h ) / 2.0, float( h ) / 2.0,
                              nearPlane, farPlane );
    m_shader.setUniformValue( "projectionMatrix", m_projectionMatrix );

    // Setup the modelview matrix. We can do it here in this simple example
    // as it is constant. Normally we would do this in paintGL or in some event handler
    QVector3D eyePosition( 0.0, 0.0, 5.0 );
    QVector3D targetPosition( 0.0, 0.0, 0.0 );
    QVector3D upDirection( 0.0, 1.0, 0.0 );
    m_modelViewMatrix.setToIdentity();
    m_modelViewMatrix.lookAt( eyePosition, targetPosition, upDirection );
    m_shader.setUniformValue( "modelViewMatrix", m_modelViewMatrix );
}

void SimpleTextureScene::setFilterPosition( float x )
{
    // Just store a local copy of the filter position.
    // our render function will pick up this value the next
    // time it is called by the window timer.
    m_filterPosition = x;
}

float SimpleTextureScene::filterPosition()
{
    return m_filterPosition;
}

void SimpleTextureScene::toggleAnimation()
{
    if ( m_propertyAnimation->state() == QAbstractAnimation::Stopped )
        m_propertyAnimation->start();
    else if ( m_propertyAnimation->state() == QAbstractAnimation::Paused )
        m_propertyAnimation->resume();
    else
        m_propertyAnimation->pause();
}

void SimpleTextureScene::prepareShaderProgram( const QString& vertexShader,
                                               const QString& fragmentShader )
{
    if ( !m_shader.addShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << "Could not compile vertex shader. Log:" << m_shader.log();

    if ( !m_shader.addShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << "Could not compile fragment shader. Log:" << m_shader.log();

    if ( !m_shader.link() )
        qCritical() << "Could not link shader program. Log:" << m_shader.log();
}

void SimpleTextureScene::prepareVertexBuffers()
{
    // Vertex positions
    float vertices[] = { -600.0f, -337.0f, 0.0f, 1.0f,
                         -600.0f,  337.0f, 0.0f, 1.0f,
                          600.0f,  337.0f, 0.0f, 1.0f,
                          600.0f, -337.0f, 0.0f, 1.0f };
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate( vertices, 4 * 4 * sizeof( float ) );

    // Texture coordinates
    float textureCoords[] = { 0.0f, 0.0f,
                              0.0f, 1.0f,
                              1.0f, 1.0f,
                              1.0f, 0.0f };
    m_textureBuffer.create();
    m_textureBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_textureBuffer.bind();
    m_textureBuffer.allocate( textureCoords, 4 * 2 * sizeof( float ) );

    // Indices
    GLuint indices[] = { 2, 1, 0, 0, 3, 2 };
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( indices, 6 * sizeof( GLuint ) );
}

void SimpleTextureScene::prepareVertexArrayObject()
{
    // Create a vertex array object (VAO) - more on this later!
    m_vao.create();
    m_vao.bind();

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    m_shader.bind();

    // Enable the "vertex" attribute to bind it to our vertex buffer
    m_vertexBuffer.bind();
    m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
    m_shader.enableAttributeArray( "vertex" );

    // Enable the "textureCoords" attribute to bind it to our texture coords buffer
    m_textureBuffer.bind();
    m_shader.setAttributeBuffer( "textureCoords", GL_FLOAT, 0, 2 );
    m_shader.enableAttributeArray( "textureCoords" );

    // Bind the index buffer ready for drawing
    m_indexBuffer.bind();
}

void SimpleTextureScene::prepareTexture( const QString& fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
        qWarning() << "Could not open file";
    QByteArray yuvData = file.readAll();
    qDebug() << yuvData.size();

    const int width = 1024;
    const int height = 768;

    // Create an OpenGL texture object
    glGenTextures( 1, &m_textureId );

    // Activate Texture Unit 0
    m_funcs->glActiveTexture( GL_TEXTURE0 );

    // Make our texure object active
    glBindTexture( GL_TEXTURE_2D, m_textureId );

    // Provide image data to the texture
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, width, height, 0,
                  GL_RED, GL_UNSIGNED_BYTE, yuvData.data() );
    qDebug() << glGetError();

    // Set filtering options for this texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );


    // Create an OpenGL texture object
    glGenTextures( 1, &m_uvTextureId );

    // Activate Texture Unit 1
    m_funcs->glActiveTexture( GL_TEXTURE1 );

    // Make our texure object active
    glBindTexture( GL_TEXTURE_2D, m_uvTextureId );

    // Provide image data to the texture
    char* uvData = yuvData.data() + width * height;
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RG, width / 2, height / 2, 0,
                  GL_RG, GL_UNSIGNED_BYTE, uvData );
    qDebug() << glGetError();

    // Set filtering options for this texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}
