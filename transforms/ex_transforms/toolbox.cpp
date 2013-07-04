#include "toolbox.h"

#include "transformscene.h"
#include "observerscene.h"

#include "ui_toolbox.h"

// random number between -1 and 1
static double signedNormRand()
{
    int i = qrand() - (RAND_MAX >> 1);
    return i / static_cast<double>(RAND_MAX);
}

Toolbox::Toolbox(TransformScene* ts, ObserverScene* obs) :
    m_scene(ts),
    m_observer(obs)
{
    m_ui.reset(new Ui::Toolbox);
    m_ui->setupUi(this);

    initMatrix();

    m_ui->radioPerspective->setChecked(true);
    m_ui->observerPerspective->setChecked(true);

    connect(m_ui->resetEverythingButton, &QPushButton::clicked, this, &Toolbox::onResetAll);
    connect(m_ui->sideBySideDemo, &QPushButton::clicked, this, &Toolbox::onDemoSideBySide);
    connect(m_ui->randomiseModels, &QPushButton::clicked, this, &Toolbox::onRandomiseModels);

    // QDoubleSpinBox::valueChanges is overloaded, hence the old style connect
    connect(m_ui->fov, SIGNAL(valueChanged(double)), this, SLOT(onModifyProjection()));
    connect(m_ui->nearClip, SIGNAL(valueChanged(double)), this, SLOT(onModifyProjection()));
    connect(m_ui->aspectRatio, SIGNAL(valueChanged(double)), this, SLOT(onModifyProjection()));
    connect(m_ui->farClip, SIGNAL(valueChanged(double)), this, SLOT(onModifyProjection()));

    connect(m_ui->resetAspect, &QPushButton::clicked, this, &Toolbox::onResetAspectRatio);

    connect(m_ui->radioPerspective, &QRadioButton::toggled, this, &Toolbox::onModifyProjection);
    connect(m_ui->radioOrtho, &QRadioButton::toggled, this, &Toolbox::onModifyProjection);

    // QSpinBox::valueChanges is overloaded, hence the old style connect
    connect(m_ui->modelIndex, SIGNAL(valueChanged(int)), this, SLOT(onSelectModel()));

    connect(m_ui->resetTransform, &QPushButton::clicked, this, &Toolbox::onResetMatrix);
    connect(m_ui->translateButton, &QPushButton::clicked, this, &Toolbox::onTranslate);
    connect(m_ui->rotateButton, &QPushButton::clicked, this, &Toolbox::onRotate);

    connect(m_ui->observerPlanView, &QPushButton::clicked, this, &Toolbox::onObserverPlanView);
    connect(m_ui->observerFrontView, &QPushButton::clicked, this, &Toolbox::onObserverFrontView);
    connect(m_ui->observerSideView, &QPushButton::clicked, this, &Toolbox::onObserverSideView);

    connect(m_ui->observerPerspective, &QCheckBox::toggled, this, &Toolbox::onObserverPerspectiveToggled);

    onResetAll();
}

Toolbox::~Toolbox()
{

}

void Toolbox::onResetAll()
{
    m_ui->nearClip->setValue(1.0);
    m_ui->farClip->setValue(1000);
    onResetAspectRatio();
    m_ui->fov->setValue(90.0);


    m_scene->camera()->resetViewToIdentity();

    onRandomiseModels();
    onModifyProjection();
    onObserverPlanView();
}

void Toolbox::onDemoSideBySide()
{
    onResetAll();

    m_scene->camera()->resetViewToIdentity();

    QMatrix4x4 model1;
    model1.setToIdentity();
    model1.translate(-1, 0, -2);
    model1.scale(1.0 / 3.0);

    QMatrix4x4 model2;
    model2.setToIdentity();
    model2.translate(3, 0, -6);

    m_scene->setModelMat(0, model1);
    m_scene->setModelMat(1, model2);

    for (int i=2; i< m_scene->numObjects(); ++i) {
    // move the other models outt he way so they don't distract
        QMatrix4x4 m;
        m.setToIdentity();
        m.translate(0, 0, -300);
        m_scene->setModelMat(i, m);
    }
}

void Toolbox::onRandomiseModels()
{
    double range = 30.0;

    for (int i=0; i<m_scene->numObjects(); ++i) {
        QMatrix4x4 m;
        m.setToIdentity();
        m.translate(range * signedNormRand(), 0.0, range * signedNormRand());
        m_scene->setModelMat(i, m);
    }
}

void Toolbox::onResetAspectRatio()
{
    QSize sz = m_scene->size();
    double aspect = sz.width() / static_cast<double>(sz.height());
    m_ui->aspectRatio->setValue(aspect);
}

void Toolbox::onModifyProjection()
{
    QMatrix4x4 proj;
    if (m_ui->radioPerspective->isChecked()) {
        m_scene->camera()->setPerspectiveProjection(
                    m_ui->fov->value(),
                    m_ui->aspectRatio->value(),
                    m_ui->nearClip->value(),
                    m_ui->farClip->value()
                    );
    } else {
        const double dim = 20;
        double aspect = m_ui->aspectRatio->value();
        m_scene->camera()->setOrthographicProjection(
                    -dim * aspect, dim * aspect, -dim, dim,
                    m_ui->nearClip->value(),
                    m_ui->farClip->value());
    }

    showMatrix(m_scene->camera()->projectionMatrix());
}

void Toolbox::onSelectModel()
{
    m_liveMatrix = m_scene->modelMatrix(m_ui->modelIndex->value());
    showMatrix(m_liveMatrix);
}

void Toolbox::copyLiveMatrix()
{
    int index = m_ui->modelIndex->value();
    m_scene->setModelMat(index, m_liveMatrix);
    showMatrix(m_liveMatrix);
}

void Toolbox::onResetMatrix()
{
    m_liveMatrix.setToIdentity();
    copyLiveMatrix();
}

void Toolbox::onRotate()
{
    QVector3D rotationAxis;
    int selectedAxis = m_ui->rotationAxes->currentIndex();
    switch (selectedAxis) {
    case 0: rotationAxis.setX(1); break;
    case 1: rotationAxis.setY(1); break;
    case 2: rotationAxis.setZ(1); break;
    }

    m_liveMatrix.rotate(m_ui->degreesRotation->value(), rotationAxis);
    copyLiveMatrix();
}

void Toolbox::onTranslate()
{
    m_liveMatrix.translate(m_ui->xTranslate->value(),
        m_ui->yTranslate->value(), m_ui->zTranslate->value() );
    copyLiveMatrix();
}

void Toolbox::onObserverPerspectiveToggled(bool on)
{
    double nearClip = 1.0;
    double farClip = 10000;
    double fov = 90.0;
    QMatrix4x4 proj;
    proj.setToIdentity();
    QSize sz = m_observer->size();
    double aspect = sz.width() / static_cast<double>(sz.height());

    if (on) {
        m_observer->camera()->setPerspectiveProjection(fov, aspect, nearClip, farClip);
    } else {
        const double dim = 20;
        m_observer->camera()->setOrthographicProjection(-dim * aspect, dim * aspect, -dim, dim, nearClip, farClip);
    }
}

void Toolbox::onObserverPlanView()
{
    m_observer->camera()->setViewCenter( QVector3D(0, 0, 0) );
    m_observer->camera()->setPosition( QVector3D(0, 10, 0) );
    m_observer->camera()->setUpVector(  QVector3D(0, 0, 1) );
    showMatrix( m_observer->camera()->viewMatrix() );
}

void Toolbox::onObserverFrontView()
{
    m_observer->camera()->setViewCenter( QVector3D(0, 0, 0) );
    m_observer->camera()->setPosition( QVector3D(0, 0, -10 ) );
    m_observer->camera()->setUpVector(  QVector3D(0, 1, 0) );
    showMatrix( m_observer->camera()->viewMatrix() );
}

void Toolbox::onObserverSideView()
{
    m_observer->camera()->setViewCenter( QVector3D(0, 0, 0) );
    m_observer->camera()->setPosition( QVector3D(-10, 0, 0 ) );
    m_observer->camera()->setUpVector(  QVector3D(0, 1, 0) );
    showMatrix( m_observer->camera()->viewMatrix() );
}

void Toolbox::initMatrix()
{
    QGroupBox* gb = m_ui->matrixGroup;
    m_grid = new QGridLayout;
    gb->setLayout(m_grid);

    for (int i=0; i<4; ++i) {
        for (int j=0; j<4; ++j) {
            m_grid->addWidget(new QLabel, i, j);
        }
    }
}

void Toolbox::showMatrix(const QMatrix4x4& mat)
{
    for (int i=0; i<4; ++i) {
        QVector4D row = mat.row(i);
        double rowData[4];
        rowData[0] = row.x();
        rowData[1] = row.y();
        rowData[2] = row.z();
        rowData[3] = row.w();

        for (int j=0; j<4; ++j) {
            QLabel* l = qobject_cast<QLabel*>(m_grid->itemAtPosition(i, j)->widget());
            l->setText(QString::number(rowData[j], 'f', 3));
        }
    }
}
