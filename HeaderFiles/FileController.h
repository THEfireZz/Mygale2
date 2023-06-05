#pragma once
#include"MainWindowView.h"
#include "FileManager.h"
#include <QFile>
class FileController : public QObject
{
	Q_OBJECT
public:
	FileController(MainWindowView* mainWindowView, QObject* parent = nullptr);
	void createDefaultJob(QString priority);
	
	
private:
	MainWindowView* m_mainWindowView;
	QString m_jobFolderPath;
	void connecting();
	QString getPreviousJobId(const QString& output);
	void addRessubmission(FileManager& fileManager);

	QString getJobType();
	QString getJobName();
	QString getScene();
	QString getImageFolder();
	QString getImagesIndex();
	QString getFirstImageIndex();
	QString getLastImageIndex();
	QString getName();
	QString getFormat();
	QString getRawFormat();
	QString getjobTypeOption(QString option);
	QString getCamera();
	QString getSubmissionOption();
	QString getNumberOfCpu();
	QString getMinimumMemory();
	QString getPcPoolManagementChoice();
	QString getMaxCPU();

public slots:
	void handleStandartPushButtonCliked();
};

