#include"MainWindowView.h"
#include <QFile>
#pragma once
class InterfaceController : public QObject 
{
	Q_OBJECT
public:
	InterfaceController(MainWindowView* mainWindowView,QObject *parent=nullptr);
	
	void loadPreviousSession();
	
private:
	MainWindowView* m_mainWindowView;
	QString m_configFolderPath;
	
	void loadConfigFile();
	void extractConfig(QFile& configFile);
	void loadFormat(QFile& configFile);
	void changeConfigFilesPermissions(const QString& path);
	
public slots:
	void handleJobNamePushButtonClicked();
	void handleImageDirectoryPushButtonClicked();
	void handleSceneSelectionPushButtonClicked();
	void handleJobTypeComboBoxChanged();
	void handleJobNameLineEditChanged();
	void handleSceneLineEditChanged();

	void handleImageDirectoryLineEditChanged();
	void handleAnimationRadioButtonChanged();
	void handleFixedImageSpinBoxChanged();
	void handleAnimationMinSpinBoxChanged();
	void handleAnimationMaxSpinBoxChanged();
	void handleNameCheckBoxClicked();
	void handleNameLineEditChanged();
	void handleFormatCheckBoxClicked();
	void handleFormatComboBoxChanged();
	void handleCameraGroupBoxClicked();
	void handleCameraLineEditChanged();
	void handleCPUCheckBoxClicked();
	void handleMinCPUSpinBoxChanged();
	void handleMaxCPUSpinBoxChanged();
	void handleMemoryCheckBoxClicked();
	void handleMemoryDoubleSpinBoxChanged();
	void handleAnalysisCheckBoxClicked();
	void handleRessubmissionCheckBoxClicked();
};

