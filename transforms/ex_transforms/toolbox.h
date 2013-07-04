#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QMatrix4x4>
#include <QWidget>
#include <QGridLayout>

namespace Ui {
    class Toolbox;
}

class TransformScene;
class ObserverScene;

class Toolbox : public QWidget
{
    Q_OBJECT
public:
    Toolbox(TransformScene* ts, ObserverScene* obs);
    virtual ~Toolbox();

private slots:
    void onResetAll();
    void onDemoSideBySide();
    void onRandomiseModels();
    
    void onModifyProjection();
    void onResetAspectRatio();
    
    void onSelectModel();
    void onResetMatrix();
    void onRotate();
    void onTranslate();

    void onObserverPerspectiveToggled(bool on);    
    void onObserverPlanView();
    void onObserverFrontView();
    void onObserverSideView();
    
private:
    void initMatrix();
    void showMatrix(const QMatrix4x4& mat);
    
    QMatrix4x4 m_liveMatrix;
    QGridLayout* m_grid;
    
    void copyLiveMatrix();
    
    QScopedPointer<Ui::Toolbox> m_ui;
    TransformScene* m_scene;
    ObserverScene* m_observer;
};


#endif