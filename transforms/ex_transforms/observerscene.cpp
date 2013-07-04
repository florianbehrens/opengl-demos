#include "observerscene.h"

#include "grid.h"

ObserverScene* foo = NULL;

ObserverScene::ObserverScene(TransformScene* peer) :
    TransformScene(),
    m_peer(peer)
{

}

ObserverScene::~ObserverScene()
{

}

float positionData[] = {
    -0.8f, -0.8f, -3.0f,
     0.8f, -0.8f, -2.0f,
     0.0f,  0.8f, 2.0f
};

void ObserverScene::initialise()
{
    TransformScene::initialise();


    m_cameraVerts.resize(6);



    m_cameraVerts[0] = 0.0f;
    m_cameraVerts[1] = 0.0f;
    m_cameraVerts[2] = 0.0f;
    m_cameraVerts[3] = 0.0f;
    m_cameraVerts[4] = 0.0f;
    m_cameraVerts[5] = -100.0f;

 //   m_cameraPositionBuffer.create();
//    m_cameraPositionBuffer.setUsagePattern( QOpenGLBuffer::StreamDraw );
//    m_cameraPositionBuffer.bind();
   // m_cameraPositionBuffer.allocate( m_cameraVerts.data(), m_cameraVerts.size() * sizeof( float ) );


 //    m_cameraPositionBuffer.allocate( positionData, 9 * sizeof( float ) );
}

void ObserverScene::render()
{
    setupRenderState();

    for (int i=0; i<m_peer->numObjects(); ++i) {
        QMatrix4x4 mat = m_peer->modelMatrix(i);
        drawMesh(m_mesh, mat);
    }

    // draw the origin, at the origin!
    QMatrix4x4 m;
    m.setToIdentity();
    drawMesh(m_originMarker, m);

/////////////////////////////////////

    // draw the primary scene's camera
    m_gridProgram->bind();

    QMatrix4x4 mvp = m_camera->viewProjectionMatrix() * m_peer->camera()->viewMatrix();
    m_gridProgram->setUniformValue( "mvp", mvp );
    m_gridProgram->setUniformValue( "color", QVector4D(1.0, 0.0, 1.0, 0.2) );


  //  qDebug() << glGetError();


    m_gridProgram->setAttributeArray("vertexPosition", GL_FLOAT, positionData,  3 );

    glDrawArrays(GL_TRIANGLES, 0, 3);

///////////////
    mvp = m_camera->viewProjectionMatrix();
    m_gridProgram->setUniformValue( "mvp", mvp );
    m_gridProgram->setUniformValue( "color", QVector4D(0.0, 1.0, 1.0, 0.2) );

    // draw the XZ grid (ground plane)
    m_grid->render();
}
