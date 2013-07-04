#include "transformscene.h"

#include "mesh.h"
#include "objloader.h"

#include <QObject>

#include <math.h>

TransformScene::TransformScene()
    : CameraScene(),
      m_program( new QOpenGLShaderProgram ),
      m_gridProgram( new QOpenGLShaderProgram )
{
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 4.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );

    int n = 5;

    for (int i=0; i<n; ++i) {
        QMatrix4x4 m;
        m.setToIdentity();
        m_modelMats.append(m);
    }
}

void TransformScene::initialise()
{

    // Create a material
    prepareShaderProgram( m_program, ":/shaders/diffuse.vert", ":/shaders/diffuse.frag" );
    MaterialPtr material( new Material );
    material->setShader( m_program );

    ObjLoader loader;
    loader.setTangentGenerationEnabled( false );
#if 1
    loader.load( "../../../opengl/assets/meshes/cow1.obj" );
#else
    loader.load( "../../../../../assets/meshes/cow1.obj" );
#endif

    m_mesh = new Mesh();
    m_mesh->setMaterial( material );
    m_mesh->setMeshData(loader);

    ObjLoader loader2;
    loader2.setTangentGenerationEnabled( false );
#if 1
    loader2.load( "../../../opengl/assets/meshes/origin.obj" );
#else
    loader2.load( "../../../../../assets/meshes/origin.obj" );
#endif

    m_originMarker = new Mesh();
    m_originMarker->setMaterial( material );
    m_originMarker->setMeshData(loader2);

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );


// Create a material
    prepareShaderProgram( m_gridProgram, ":/shaders/grid.vert", ":/shaders/grid.frag" );
    MaterialPtr material2( new Material );
    material2->setShader( m_gridProgram );

    m_grid = new Grid(20.0, 20.0, 20, 20);

    m_grid->setMaterial(material2);
    m_grid->create();
}

void TransformScene::update( float t )
{
    Q_UNUSED( t );
}

void TransformScene::drawMesh(Mesh* mesh, const QMatrix4x4& at)
{
    m_camera->setStandardUniforms( mesh->material()->shader(), at );
    // Let the mesh setup the remainder of its state and draw itself
    mesh->render();
}

void TransformScene::setupRenderState()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_program->bind();

    // Set the lighting parameters
    m_program->setUniformValue( "lightPosition", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    m_program->setUniformValue( "lightIntensity", QVector3D( 1.0f, 1.0f, 1.0f ) );

    // Set the diffuse reflectivity of the object
    m_program->setUniformValue( "kd", QVector3D( 0.9f, 0.5f, 0.3f ) );
}

void TransformScene::render()
{
    setupRenderState();

    // draw loop
    for (int i=0; i<m_modelMats.count(); ++i) {
        drawMesh(m_mesh, m_modelMats[i]);
    }

    // draw the origin, at the origin!
    QMatrix4x4 m;
    m.setToIdentity();
    drawMesh(m_originMarker, m);

    m_gridProgram->bind();
    m_gridProgram->setUniformValue( "color", QVector4D(0.0, 1.0, 1.0, 0.5) );
    m_gridProgram->setUniformValue( "mvp", m_camera->viewProjectionMatrix() );

    // draw the XZ grid (ground plane)
    m_grid->render();
}

void TransformScene::resize( int w, int h )
{
    CameraScene::resize(w, h);
    m_size = QSize(w, h);
}

void TransformScene::prepareShaderProgram( QOpenGLShaderProgramPtr program, const QString& vertexShader,
                                         const QString& fragmentShader )
{
    // Load and compile the vertex shader
    if ( !program->addShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << "Could not compile vertex shader. Log:" << program->log();

    // Load and compile the fragment shader
    if ( !program->addShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << "Could not compile fragment shader. Log:" << program->log();

    // Link the shaders together into a complete shader program (pipeline)
    if ( !program->link() )
        qCritical() << "Could not link shader program. Log:" << program->log();
}

void TransformScene::setModelMat(int index, const QMatrix4x4& mat)
{
    m_modelMats[index] = mat;
}

