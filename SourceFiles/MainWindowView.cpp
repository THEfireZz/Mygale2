#include "../HeaderFiles/MainWindowView.h"
#include "../HeaderFiles/InterfaceController.h"
#include <QSettings>
#include <QDebug>

QSettings settings("Stellantis", "Mygale");

MainWindowView::MainWindowView(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

MainWindowView::~MainWindowView()
{}

void MainWindowView::startConnection()
{
    connect(ui.StandartPushButton, &QPushButton::clicked, this, &MainWindowView::standartPushButtonCliked);
    connect(ui.PriorityStartPushButton, &QPushButton::clicked, this, &MainWindowView::priorityPushButtonCliked);
    connect(ui.JobNamepushButton, &QPushButton::clicked, this, &MainWindowView::jobNamePushButtonCliked);
    connect(ui.SceneSelectionPushButton, &QPushButton::clicked, this, &MainWindowView::sceneSelectionPushButtonCliked);
    connect(ui.ImageDirectoryPushButton, &QPushButton::clicked, this, &MainWindowView::imageDirectoryPushButtonCliked);
    connect(ui.JobTypeComboBox, &QComboBox::currentIndexChanged, this, &MainWindowView::jobTypeComboBoxChanged);
    connect(ui.JobNamelineEdit, &QLineEdit::textChanged, this, &MainWindowView::jobNameLineEditChanged);
    connect(ui.SceneSelectionlineEdit, &QLineEdit::textChanged, this, &MainWindowView::sceneLineEditChanged);

    connect(ui.ImageDirectorylineEdit, &QLineEdit::textChanged, this, &MainWindowView::imageDirectoryLineEditChanged);
    connect(ui.AnimationradioButton, &QRadioButton::toggled, this, &MainWindowView::animationRadioButtonChanged);
    connect(ui.FixedImageCalculationSpinBox, &QSpinBox::valueChanged, this, &MainWindowView::fixedImageSpinBoxChanged);
    connect(ui.AnimationMinspinBox, &QSpinBox::valueChanged, this, &MainWindowView::animationMinSpinBoxChanged);
    connect(ui.AnimationMaxspinBox, &QSpinBox::valueChanged, this, &MainWindowView::animationMaxSpinBoxChanged);
    connect(ui.NameCheckBox, &QCheckBox::stateChanged, this, &MainWindowView::nameCheckBoxCliked);
    connect(ui.NameLineEdit, &QLineEdit::textChanged, this, &MainWindowView::nameLineEditChanged);
    connect(ui.FormatCheckBox, &QCheckBox::stateChanged, this, &MainWindowView::formatCheckBoxCliked);
    connect(ui.FormatComboBox, &QComboBox::currentIndexChanged, this, &MainWindowView::formatComboBoxChanged);
    connect(ui.CameraGroupBox, &QGroupBox::toggled, this, &MainWindowView::cameraGroupBoxClicked);
    connect(ui.CameralineEdit, &QLineEdit::textChanged, this, &MainWindowView::cameraLineEditChanged);
    connect(ui.CPUcheckBox, &QCheckBox::stateChanged, this, &MainWindowView::cPUCheckBoxClicked);
    connect(ui.MinCPUSpinBox, &QSpinBox::valueChanged, this, &MainWindowView::minCPUSpinBoxChanged);
    connect(ui.MaxCPUspinBox, &QSpinBox::valueChanged, this, &MainWindowView::maxCPUSpinBoxChanged);
    connect(ui.MemoryCheckBox, &QCheckBox::stateChanged, this, &MainWindowView::memoryCheckBoxClicked);
    connect(ui.MemorydoubleSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindowView::memoryDoubleSpinBoxChanged);
    connect(ui.AnalysischeckBox, &QCheckBox::stateChanged, this, &MainWindowView::analysisCheckBoxClicked);
    connect(ui.ResubmissionCheckBox, &QCheckBox::stateChanged, this, &MainWindowView::ressubmissionCheckBoxClicked);

    for (int i = 0; i < ui.gridLayout->count(); ++i) {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(ui.gridLayout->itemAt(i)->widget());
        if (checkBox) {
            connect(checkBox, &QCheckBox::stateChanged, this, &MainWindowView::saveGridLayout);
        }

    }
}

//getter
QComboBox* MainWindowView::getJobTypeComboBox()
{
    return ui.JobTypeComboBox;
}

QLineEdit* MainWindowView::getJobNamelineEdit()
{
    return ui.JobNamelineEdit;
}

QLineEdit* MainWindowView::getSceneSelectionlineEdit()
{
    return ui.SceneSelectionlineEdit;
}

QLineEdit* MainWindowView::getImageDirectorylineEdit()
{
    return ui.ImageDirectorylineEdit;
}

QRadioButton* MainWindowView::getFixeImageradioButton()
{
    return ui.FixedImageRadioButton;
}

QRadioButton* MainWindowView::getAnimationradioButton()
{
    return ui.AnimationradioButton;
}

QSpinBox* MainWindowView::getFixedImageCalculationSpinBox()
{
    return  ui.FixedImageCalculationSpinBox;
}

QSpinBox* MainWindowView::getAnimationMinspinBox()
{
    return ui.AnimationMinspinBox;
}

QSpinBox* MainWindowView::getAnimationMaxspinBox()
{
    return ui.AnimationMaxspinBox;
}

QCheckBox* MainWindowView::getNameCheckBox()
{
    return ui.NameCheckBox;
}

QLineEdit* MainWindowView::getNameLineEdit()
{
    return  ui.NameLineEdit;
}

QCheckBox* MainWindowView::getFormatCheckBox()
{
    return  ui.FormatCheckBox;
}

QComboBox* MainWindowView::getFormatComboBox()
{
    return ui.FormatComboBox;
}

QGroupBox* MainWindowView::getCameraGroupBox()
{
    return ui.CameraGroupBox;
}

QLineEdit* MainWindowView::getCameralineEdit()
{
    return ui.CameralineEdit;
}

QCheckBox* MainWindowView::getCPUcheckBox()
{
    return ui.CPUcheckBox;
}

QSpinBox* MainWindowView::getMinCPUSpinBox()
{
    return ui.MinCPUSpinBox;
}

QSpinBox* MainWindowView::getMaxCPUspinBox()
{
    return ui.MaxCPUspinBox;
}

QCheckBox* MainWindowView::getMemoryCheckBox()
{
    return ui.MemoryCheckBox;
}

QDoubleSpinBox* MainWindowView::getMemorydoubleSpinBox()
{
    return ui.MemorydoubleSpinBox;
}

QCheckBox* MainWindowView::getAnalysischeckBox()
{
    return ui.AnalysischeckBox;
}

QCheckBox* MainWindowView::getResubmissionCheckBox()
{
    return ui.ResubmissionCheckBox;
}

QGridLayout* MainWindowView::getGridLayout()
{
    return ui.gridLayout;
}


void MainWindowView::saveGridLayout()
{
 
    
    QGridLayout* gridLayout = ui.gridLayout;
    for (int i = 0; i < gridLayout->count(); i++) {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(gridLayout->itemAt(i)->widget());
        if (checkBox) {
            
            QString checkBoxName = checkBox->objectName();
            bool checkBoxValue = checkBox->isChecked();

            settings.setValue(checkBoxName, checkBoxValue);

        }
    }
   
}


void MainWindowView::onStandartPushButtonCliked()
{
    emit standartPushButtonCliked();
}
void MainWindowView::onPriorityPushButtonCliked()
{
    emit priorityPushButtonCliked();
}

void MainWindowView::onJobNamePushButtonCliked()
{
    emit jobNamePushButtonCliked();
}

void MainWindowView::onSceneSelectionPushButtonCliked()
{
    emit sceneSelectionPushButtonCliked();
}

void MainWindowView::onImageDirectoryPushButtonCliked()
{
    emit imageDirectoryPushButtonCliked();
}

void MainWindowView::onJobTypeComboBoxChanged()
{
    qDebug() << "job type changer";
    emit jobTypeComboBoxChanged();
}

void MainWindowView::onJobNameLineEditChanged()
{
    emit jobNameLineEditChanged();
}

void MainWindowView::onSceneLineEditChanged()
{
    emit sceneLineEditChanged();
}

void MainWindowView::onImageDirectoryLineEditChanged()
{
    emit imageDirectoryLineEditChanged();
}

void MainWindowView::onAnimationRadioButtonChanged()
{
    emit animationRadioButtonChanged();
}

void MainWindowView::onFixedImageSpinBoxChanged()
{
    emit fixedImageSpinBoxChanged();
}

void MainWindowView::onAnimationMinSpinBoxChanged()
{
    emit animationMinSpinBoxChanged();
}

void MainWindowView::onAnimationMaxSpinBoxChanged()
{
    emit animationMaxSpinBoxChanged();
}

void MainWindowView::onNameCheckBoxCliked()
{
    emit nameCheckBoxCliked();
}

void MainWindowView::onNameLineEditChanged()
{
    emit nameLineEditChanged();
}

void MainWindowView::onFormatCheckBoxCLiked()
{
    emit formatCheckBoxCliked();
}

void MainWindowView::onFormatComboBoxChanged()
{
    emit formatComboBoxChanged();
}

void MainWindowView::onCameraGroupBoxClicked()
{
    emit cameraGroupBoxClicked();
}

void MainWindowView::onCameraLineEditChanged()
{
    emit cameraLineEditChanged();
}

void MainWindowView::onCPUCheckBoxClicked()
{
    emit cPUCheckBoxClicked();
}

void MainWindowView::onMinCPUSpinBoxChanged()
{
    emit minCPUSpinBoxChanged();
}

void MainWindowView::onMaxCPUSpinBoxChanged()
{
    emit maxCPUSpinBoxChanged();
}

void MainWindowView::onMemoryCheckBoxClicked()
{
    emit memoryCheckBoxClicked();
}

void MainWindowView::onMemoryDoubleSpinBoxChanged()
{
    emit memoryDoubleSpinBoxChanged();
}

void MainWindowView::onAnalysisCheckBoxClicked()
{
    emit analysisCheckBoxClicked();
}

void MainWindowView::onRessubmissionCheckBoxClicked()
{
    emit ressubmissionCheckBoxClicked();
}


