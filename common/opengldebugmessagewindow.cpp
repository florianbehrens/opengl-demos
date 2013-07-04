#include "opengldebugmessagewindow.h"
#include "opengldebugmessagemodel.h"

#include <QDialogButtonBox>
#include <QTreeView>
#include <QVBoxLayout>
#include <QPushButton>

OpenGLDebugMessageWindow::OpenGLDebugMessageWindow(QWidget *parent) :
    QDialog(parent)
{
    m_view = new QTreeView;
    m_view->setRootIsDecorated(false);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    QPushButton *closeButton = buttonBox->addButton(QDialogButtonBox::Close);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    m_clearButton = buttonBox->addButton(QDialogButtonBox::Reset);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_view);
    layout->addWidget(buttonBox);
    setLayout(layout);

    resize(600, 300);
}

void OpenGLDebugMessageWindow::setModel(OpenGLDebugMessageModel *model)
{
    if (m_model) {
        disconnect(m_clearButton, &QPushButton::clicked,
                   m_model, &OpenGLDebugMessageModel::clearMessages);
    }

    m_model = model;
    m_view->setModel(model);

    connect(m_clearButton, &QPushButton::clicked,
            m_model, &OpenGLDebugMessageModel::clearMessages);
}


