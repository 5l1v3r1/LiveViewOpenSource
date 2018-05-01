#include "controlsbox.h"

ControlsBox::ControlsBox(FrameWorker *fw, QTabWidget *tw, QWidget *parent) :
    QWidget(parent), collecting_mask(false)
{
    frame_handler = fw;
    connect(frame_handler, SIGNAL(updateFPS(float)), this, SLOT(updateFPS(float)));
    tab_handler = tw;
    connect(tab_handler, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    viewWidget = getCurrentTab();

    fpsLabel = new QLabel("Warning: No Frames Received");
    fpsLabel->setFixedWidth(200);

    rangeSlider = new ctkRangeSlider();
    rangeSlider->setOrientation(Qt::Horizontal);
    connect(rangeSlider, SIGNAL(minimumPositionChanged(int)), viewWidget, SLOT(setFloorPos(int)));
    connect(rangeSlider, SIGNAL(maximumPositionChanged(int)), viewWidget, SLOT(setCeilingPos(int)));

    precisionBox = new QCheckBox("Precision Slider");
    connect(precisionBox, SIGNAL(toggled(bool)), this, SLOT(setPrecision(bool)));

    dirEdit = new QLineEdit();
    dirEdit->setFixedWidth(750);
    QPushButton *resetButton = new QPushButton("&Reset");
    connect(resetButton, SIGNAL(released()), this, SLOT(resetDir()));

    maskButton = new QPushButton("&Collect Mask Frames");
    connect(maskButton, SIGNAL(released()), this, SLOT(collectDSFMask()));

    QGridLayout *cboxLayout = new QGridLayout();
    cboxLayout->addWidget(fpsLabel, 0, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Range:"), 0, 1, 1, 1);
    cboxLayout->addWidget(rangeSlider, 0, 2, 1, 5);
    cboxLayout->addWidget(precisionBox, 0, 7, 1, 2);
    cboxLayout->addWidget(maskButton, 1, 0, 1, 1);
    cboxLayout->addWidget(new QLabel("Input directory:"), 1, 1, 1, 1);
    cboxLayout->addWidget(dirEdit, 1, 2, 1, 5);
    cboxLayout->addWidget(resetButton, 1, 7, 1, 1);
    this->setLayout(cboxLayout);
    this->setMaximumHeight(100);
}

ControlsBox::~ControlsBox()
{
}

void ControlsBox::tabChanged(int index)
{
    Q_UNUSED( index );

    // disconnect the signals associated with the pointer to the old tab
    disconnect(rangeSlider, SIGNAL(minimumPositionChanged(int)), viewWidget, SLOT(setFloorPos(int)));
    disconnect(rangeSlider, SIGNAL(maximumPositionChanged(int)), viewWidget, SLOT(setCeilingPos(int)));

    // associate the GUI items with the pointer to the new tab
    viewWidget = getCurrentTab();
    connect(rangeSlider, SIGNAL(minimumPositionChanged(int)), viewWidget, SLOT(setFloorPos(int)));
    connect(rangeSlider, SIGNAL(maximumPositionChanged(int)),  viewWidget, SLOT(setCeilingPos(int)));

    precisionBox->setChecked(viewWidget->isPrecisionMode());

    // update the range slider positions
    rangeSlider->setPositions((int)(viewWidget->getFloor() / viewWidget->getDataMax() * 100.0),
                              (int)(viewWidget->getCeiling() / viewWidget->getDataMax() * 100.0));
}

void ControlsBox::setPrecision(bool isPrecise)
{
    viewWidget->setPrecision(isPrecise);

    if (isPrecise) {
        // Set the minimum position out of 100, so the slider can go as low as -dataMax
        rangeSlider->setMinimum(-100);
    } else {
        rangeSlider->setMinimum(0);
    }

    // update the range slider positions
    rangeSlider->setPositions(viewWidget->getFloor() / viewWidget->getDataMax() * 100,
                              viewWidget->getCeiling() / viewWidget->getDataMax() * 100);
}

void ControlsBox::resetDir()
{
    QString dirname = dirEdit->text();
    QByteArray ba = dirname.toLatin1();
    const char *dname = ba.data();
    frame_handler->resetDir(dname);
}

void ControlsBox::collectDSFMask()
{
    collecting_mask = !collecting_mask;
    if (collecting_mask) {
        maskButton->setText("&Stop Collecting Mask");
        frame_handler->DSFilter->start_mask_collection();
    } else {
        maskButton->setText("&Collect Mask Frames");
        frame_handler->DSFilter->finish_mask_collection();
    }

}

void ControlsBox::updateFPS(float frameRate)
{
    if ((int)frameRate == -1) {
        fpsLabel->setText(QString("Warning: No Frames Received"));
    } else {
        fpsLabel->setText(QString("FPS @ backend: %1").arg(QString::number(frameRate, 'f', 1)));
    }
}

LVTabApplication* ControlsBox::getCurrentTab()
{
    return qobject_cast<LVTabApplication*>(tab_handler->widget(tab_handler->currentIndex()));
}
