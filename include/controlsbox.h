#ifndef CONTROLSBOX_H
#define CONTROLSBOX_H

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QByteArray>
#include <QTabWidget>

#include "frameworker.h"
#include "lvtabapplication.h"
#include "ctkrangeslider.h"

class ControlsBox : public QWidget
{
    Q_OBJECT
public:
    explicit ControlsBox(FrameWorker *fw, QTabWidget *tw, QWidget *parent = NULL);
    ~ControlsBox();

public slots:
    void resetDir();
    void collectDSFMask();
    void tabChanged(int);

private slots:
    void updateFPS(float frameRate);
    void setPrecision(bool isPrecise);

private:
    FrameWorker* frame_handler;
    QLabel* fpsLabel;

    QTabWidget* tab_handler;

    LVTabApplication* getCurrentTab();
    LVTabApplication* viewWidget;

    ctkRangeSlider* rangeSlider;
    QCheckBox* precisionBox;

    QLineEdit* dirEdit;

    QPushButton* maskButton;
    bool collecting_mask;
};

#endif // CONTROLSBOX_H
