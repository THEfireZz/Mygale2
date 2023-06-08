#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindowView.h"

class MainWindowView : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowView(QWidget *parent = nullptr);
    ~MainWindowView();
    void startConnection();

    QComboBox* getJobTypeComboBox();
    QLineEdit* getJobNamelineEdit();
    QLineEdit* getSceneSelectionlineEdit();
    QLineEdit* getImageDirectorylineEdit();
    QRadioButton* getFixeImageradioButton();
    QRadioButton* getAnimationradioButton();
    QSpinBox* getFixedImageCalculationSpinBox();
    QSpinBox* getAnimationMinspinBox();
    QSpinBox* getAnimationMaxspinBox();
    QCheckBox* getNameCheckBox();
    QLineEdit* getNameLineEdit();
    QCheckBox* getFormatCheckBox();
    QComboBox* getFormatComboBox();
    QGroupBox* getCameraGroupBox();
    QLineEdit* getCameralineEdit();
    QCheckBox* getCPUcheckBox();
    QSpinBox* getMinCPUSpinBox();
    QSpinBox* getMaxCPUspinBox();
    QCheckBox* getMemoryCheckBox();
    QDoubleSpinBox* getMemorydoubleSpinBox();
    QCheckBox* getAnalysischeckBox();
    QCheckBox* getResubmissionCheckBox();
    QGridLayout* getGridLayout();

private:
    Ui::MainWindowViewClass ui;
    void saveGridLayout();
public slots:

    void onStandartPushButtonCliked();
    void onPriorityPushButtonCliked();
    void onJobNamePushButtonCliked();
    void onSceneSelectionPushButtonCliked();
    void onImageDirectoryPushButtonCliked();
    void onJobTypeComboBoxChanged();
    void onJobNameLineEditChanged();
    void onSceneLineEditChanged();
    void onImageDirectoryLineEditChanged();
    void onAnimationRadioButtonChanged();
    void onFixedImageSpinBoxChanged();
    void onAnimationMinSpinBoxChanged();
    void onAnimationMaxSpinBoxChanged();
    void onNameCheckBoxCliked();
    void onNameLineEditChanged();
    void onFormatCheckBoxCLiked();
    void onFormatComboBoxChanged();
    void onCameraGroupBoxClicked();
    void onCameraLineEditChanged();
    void onCPUCheckBoxClicked();
    void onMinCPUSpinBoxChanged();
    void onMaxCPUSpinBoxChanged();
    void onMemoryCheckBoxClicked();
    void onMemoryDoubleSpinBoxChanged();
    void onAnalysisCheckBoxClicked();
    void onRessubmissionCheckBoxClicked();

signals:
    void standartPushButtonCliked();
    void priorityPushButtonCliked();
    void jobNamePushButtonCliked();
    void sceneSelectionPushButtonCliked();
    void imageDirectoryPushButtonCliked();
    void jobTypeComboBoxChanged();
    void jobNameLineEditChanged();
    void sceneLineEditChanged();
    void imageDirectoryLineEditChanged();
    void animationRadioButtonChanged();
    void fixedImageSpinBoxChanged();
    void animationMinSpinBoxChanged();
    void animationMaxSpinBoxChanged();
    void nameCheckBoxCliked();
    void nameLineEditChanged();
    void formatCheckBoxCliked();
    void formatComboBoxChanged();
    void cameraGroupBoxClicked();
    void cameraLineEditChanged();
    void cPUCheckBoxClicked();
    void minCPUSpinBoxChanged();
    void maxCPUSpinBoxChanged();
    void memoryCheckBoxClicked();
    void memoryDoubleSpinBoxChanged();
    void analysisCheckBoxClicked();
    void ressubmissionCheckBoxClicked();
};
