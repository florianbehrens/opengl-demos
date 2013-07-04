#include "simpletexturescene.h"

#include "axisalignedboundingbox.h"
#include "mesh.h"
#include "objloader.h"

#include <QFile>
#include <QGLWidget>
#include <QImage>
#include <QOpenGLFunctions>
#include <QRegularExpression>

#include <math.h>

SimpleTextureScene::SimpleTextureScene( const QString &movieFile, QObject* parent )
    : AbstractScene( parent ),
      m_shader(),
      m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordinateBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_yTextureBuffer( QOpenGLBuffer::PixelUnpackBuffer ),
      m_uvTextureBuffer( QOpenGLBuffer::PixelUnpackBuffer ),
      m_yTextureId( 0 ),
      m_uvTextureId( 0 ),
      m_movieFile(movieFile)
{
}

void SimpleTextureScene::initialise()
{
    m_funcs = m_context->functions();
    m_funcs->initializeOpenGLFunctions();

    // mplayer's output is definitely easier to parse than ffmpeg's...
    QProcess movieFileParameters;
    movieFileParameters.start("mplayer",
                              QStringList()
                              << "-identify"
                              << "-vo" << "null"
                              << "-ao" << "null"
                              << "-frames" << "0"
                              << "-vc" << "null"
                              << "--"
                              << m_movieFile,
                              QIODevice::ReadOnly);
    movieFileParameters.waitForFinished();
    QString mplayerOutput = QString::fromLocal8Bit(movieFileParameters.readAllStandardOutput());

    QRegularExpression widthRe("^ID_VIDEO_WIDTH=(.*)", QRegularExpression::MultilineOption);
    QRegularExpressionMatch widthMatch = widthRe.match(mplayerOutput);
    if (widthMatch.hasMatch())
        m_frameSize.setWidth(widthMatch.captured(1).toInt());

    QRegularExpression heightRe("^ID_VIDEO_HEIGHT=(.*)", QRegularExpression::MultilineOption);
    QRegularExpressionMatch heightMatch = heightRe.match(mplayerOutput);
    if (heightMatch.hasMatch())
        m_frameSize.setHeight(heightMatch.captured(1).toInt());

    if (m_frameSize.width() <= 0 || m_frameSize.height() <= 0)
        qFatal("Cannot determine the input file frame size!");

    qDebug() << "Got frame size:" << m_frameSize;

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Prepare a complete shader program...
    prepareShaderProgram( ":/shaders/simpletexture.vert",
                          ":/shaders/simpletexture.frag" );

    // Prepare the vertex, texture and index buffers
    prepareVertexBuffers();

    // Prepare the VAO
    prepareVertexArrayObject();

    // Prepare the texture data itself (textures and pixel unpack buffer objects)
    prepareTexture();

    // Link texture unit 0 to the "ySampler" variable in the fragment shader
    m_shader.setUniformValue( "ySampler", 0 );

    // Link texture unit 1 to the "uvSampler" variable in the fragment shader
    m_shader.setUniformValue( "uvSampler", 1 );

    m_videoDecoder.start("ffmpeg",
                         QStringList()
                         << "-i" << m_movieFile
                         << "-f" << "rawvideo"
                         << "-vcodec" << "rawvideo"
                         << "-pix_fmt" << "nv12"
                         << "-an"
                         << "-ss" << "180" // jump to 3m
                         << "-",
                         QIODevice::ReadOnly);

    m_videoDecoder.closeWriteChannel();
    m_videoDecoder.closeReadChannel(QProcess::StandardError);
}

void SimpleTextureScene::update( float t )
{
    Q_UNUSED(t);
    m_NV12Buffer += m_videoDecoder.readAllStandardOutput();

    if (m_NV12Buffer.size() >= frameByteSize()) {
        // time to update our textures
        void *bufferPtr;

        m_yTextureBuffer.bind();
        bufferPtr = m_yTextureBuffer.map(QOpenGLBuffer::WriteOnly);
        memcpy(bufferPtr,
               m_NV12Buffer.constData(),
               m_frameSize.width() * m_frameSize.height());
        m_yTextureBuffer.unmap();

        m_funcs->glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_yTextureId );
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        m_frameSize.width(),
                        m_frameSize.height(),
                        GL_RED,
                        GL_UNSIGNED_BYTE,
                        NULL);

        m_yTextureBuffer.release();


        m_uvTextureBuffer.bind();
        bufferPtr = m_uvTextureBuffer.map(QOpenGLBuffer::WriteOnly);
        memcpy(bufferPtr,
               m_NV12Buffer.constData() + m_frameSize.width() * m_frameSize.height(),
               m_frameSize.width() * m_frameSize.height() / 2);
        m_uvTextureBuffer.unmap();

        m_funcs->glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, m_uvTextureId );
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        m_frameSize.width() / 2,
                        m_frameSize.height() / 2,
                        GL_RG,
                        GL_UNSIGNED_BYTE,
                        NULL);

        m_uvTextureBuffer.release();

        m_NV12Buffer.remove(0, frameByteSize());
    }
}

void SimpleTextureScene::render()
{
    m_shader.bind();

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT );

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

    // We are simply going to map the video on a quad ranging from (-1, -1 to 1, 1).
    // So no projection is actually needed here, everything is already in clip space.
}

void SimpleTextureScene::toggleAnimation()
{
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
//    float vertices[] = { -600.0f, -337.0f, 0.0f, 1.0f,
//                         -600.0f,  337.0f, 0.0f, 1.0f,
//                          600.0f,  337.0f, 0.0f, 1.0f,
//                          600.0f, -337.0f, 0.0f, 1.0f };
    float vertices[] = { -1.0f,  1.0f, 0.0f, 1.0f,
                          1.0f, -1.0f, 0.0f, 1.0f,
                         -1.0f, -1.0f, 0.0f, 1.0f,
                          1.0f,  1.0f, 0.0f, 1.0f };
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate( vertices, 4 * 4 * sizeof( float ) );

    // Texture coordinates
    float textureCoords[] = { 0.0f, 0.0f,
                              1.0f, 1.0f,
                              0.0f, 1.0f,
                              1.0f, 0.0f };
    m_textureCoordinateBuffer.create();
    m_textureCoordinateBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_textureCoordinateBuffer.bind();
    m_textureCoordinateBuffer.allocate( textureCoords, 4 * 2 * sizeof( float ) );

    // Indices
    GLuint indices[] = { 0, 1, 2, 0, 3, 1 };
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
    m_textureCoordinateBuffer.bind();
    m_shader.setAttributeBuffer( "textureCoords", GL_FLOAT, 0, 2 );
    m_shader.enableAttributeArray( "textureCoords" );

    // Bind the index buffer ready for drawing
    m_indexBuffer.bind();
}

void SimpleTextureScene::prepareTexture()
{
    const int width = m_frameSize.width();
    const int height = m_frameSize.height();

    // Create an OpenGL texture object
    glGenTextures( 1, &m_yTextureId );

    // Activate Texture Unit 0
    m_funcs->glActiveTexture( GL_TEXTURE0 );

    // Make our texure object active
    glBindTexture( GL_TEXTURE_2D, m_yTextureId );

    // Create the texture without any image data (notice the last argument)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, width, height, 0,
                  GL_RED, GL_UNSIGNED_BYTE, NULL );
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

    // Create the texture without any image data (notice the last argument)
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RG, width / 2, height / 2, 0,
                  GL_RG, GL_UNSIGNED_BYTE, NULL );
    qDebug() << glGetError();

    // Set filtering options for this texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    // Prepare the pixel unpack buffers
    m_yTextureBuffer.create();
    m_yTextureBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_yTextureBuffer.bind();
    m_yTextureBuffer.allocate(width * height);

    m_uvTextureBuffer.create();
    m_uvTextureBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_uvTextureBuffer.bind();
    m_uvTextureBuffer.allocate(width * height / 2);
}

int SimpleTextureScene::frameByteSize() const
{
    return m_frameSize.width() * m_frameSize.height() * 6 / 4;
}
