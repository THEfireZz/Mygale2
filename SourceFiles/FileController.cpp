#include "../HeaderFiles/FileController.h"
#include "../HeaderFiles/FileManager.h"
#include "../HeaderFiles/Utils.h"
#include <QtWidgets>
#include<QSettings>
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <QMessageBox>

FileController::FileController(MainWindowView* mainWindowView, QObject* parent) :m_mainWindowView(mainWindowView), QObject(parent)
{
	// Stocke dans les parametres de l'application le chemin vers le lecteur I:/
	QSettings settings("Stellantis", "Mygale");

	settings.beginGroup("UserSetting");
	settings.setValue("UserTempFolder", Utils::toUncPath("I:\\Mygale\\TEMP"));
	settings.endGroup();
	m_jobFolderPath = settings.value("UserSetting/UserTempFolder").toString();

	connecting();
	
	
}

void FileController::createDefaultJob(QString priority)
{
	QSettings settings("Stellantis", "Mygale");
	
	QString jobType = getJobType();
	QString jobName = getJobName();
	if (jobName.isEmpty())
	{
		QMessageBox::critical(m_mainWindowView, "jobName error", "The job name is required");
		return;
	}

	QString scene = getScene();
	if (scene.isEmpty())
	{
		return;
	}

	QString imageFolder = getImageFolder();
	
	if (imageFolder.isEmpty())
	{
		return;
	}

	QString imagesIndex =getImagesIndex();


	QString format =getFormat();;
	
	if (format.isEmpty())
	{
		if (m_mainWindowView->getAnalysischeckBox()->isChecked()) {
			QMessageBox::critical(m_mainWindowView, "Analysis error", "The format is required to do an analysis");
			return;
		}
	}

	QString name = getName();
	QString camera = getCamera();

	QString submissionOption = getSubmissionOption();
	if (submissionOption.isEmpty())
	{
		return;
	}

	QString maxCpu = getMaxCPU();
	
	FileManager defaultSkeleton = FileManager::Builder().jobType(jobType).jobName(jobName).scene(scene).dossierImage(imageFolder).name(name).format(format).camera(camera).fileName(jobName).build();
	FileManager defaultLance = FileManager::Builder().jobName(jobName).submissionOption(submissionOption).imageIndex(imagesIndex).priority(priority).maxCPU(maxCpu).fileName("lance").build();
	//QString jobName, QString submissionOption, QString firstImg, QString lastImg, QString priority, QString maxCPU, QString previousIdJob, QString fileName
		
	QString skeletonPath = settings.value("UserSetting/UserConfigFolder").toString() + "skeleton\\" + m_mainWindowView->getJobTypeComboBox()->currentData(Qt::UserRole).toString();
	QString lancePath = settings.value("UserSetting/UserConfigFolder").toString() + "lance.txt";
	defaultSkeleton.extractConfigLine(skeletonPath);
	defaultLance.extractConfigLine(lancePath);

	if (!defaultSkeleton.hasRequiredElement())
	{
		return;
	}
	else {
		qDebug() << "has required";
	}

	QString defaultOutput =defaultLance.execute();
	QString previousJobId = getPreviousJobId(defaultOutput);
	//QString defaultOutput ="non";
	//²QString previousJobId = getPreviousJobId(0);
	
	if (m_mainWindowView->getAnalysischeckBox()->isChecked())
	{
		FileManager analysisSkeleton = FileManager::Builder().jobName(jobName).scene(scene).dossierImage(imageFolder).name(getRawName()).format(getRawFormat()).firstImg(getFirstImageIndex()).lastImg(getLastImageIndex()).priority(priority).maxCPU(maxCpu).submissionOption(submissionOption).fileName(jobName + "_analysis").build();
		FileManager analysisLance = FileManager::Builder().jobName(jobName).submissionOption(submissionOption).maxCPU(maxCpu).previousJobId(previousJobId).fileName("lance_analysis").build();
		

		QString skeletonAnalysisPath = settings.value("UserSetting/UserConfigFolder").toString() + "skeleton\\squeletteAnalysis.txt";
		QString lanceAnalysisPath = settings.value("UserSetting/UserConfigFolder").toString() + "lanceAnalysis.txt";
		
		analysisSkeleton.extractConfigLine(skeletonAnalysisPath);

		if (m_mainWindowView->getResubmissionCheckBox()->isChecked())
		{
			
			FileManager ressubmissionSkeleton = FileManager::Builder().jobType(jobType).jobName(jobName).scene(scene).dossierImage(imageFolder).name(name).format(format).camera(camera).fileName(jobName + "_ressubmission").build();
			addRessubmission(analysisSkeleton);
			ressubmissionSkeleton.extractConfigLine(skeletonPath);
			ressubmissionSkeleton.toRessubmission();
		}
		analysisLance.extractConfigLine(lanceAnalysisPath);
		analysisLance.execute();
	}
	Utils::incrementJobName(m_mainWindowView);
	QMessageBox::information(m_mainWindowView, "Job submitted", defaultOutput, QMessageBox::NoIcon, QMessageBox::Ok);
}

void FileController::connecting()
{
	connect(m_mainWindowView, &MainWindowView::standartPushButtonCliked, this, &FileController::handleStandartPushButtonCliked);
	connect(m_mainWindowView, &MainWindowView::priorityPushButtonCliked, this, &FileController::handlePriorityPushButtonCliked);
}

QString FileController::getPreviousJobId(const QString& output)
{
	QRegularExpression regex("\\d+");
	QString jobId;
	QRegularExpressionMatch match = regex.match(output);
	if (match.hasMatch()) {
		jobId = match.captured();
	}
	qDebug() << "Previous job ID: " << jobId;
	return jobId;
}

void FileController::addRessubmission(FileManager& fileManager)
{
	fileManager.insertLineInFile(R"(rem lance ressubmission)");
	fileManager.insertLineInFile(R"(bsub -a lspasswd -q mygale -sp <priority> -n 1,<maxCPU>  -R "span[hosts=1]  rusage[] select[ <submissionOption>]" -J <jobName>_ressubmission[1-%resubmissionCount%] -o <path>mygale\TEMP\<jobName>\lsf\LSF_Ressubmission_log.txt <path>mygale\TEMP\<jobName>\lsf\<jobName>_ressubmission.bat)");
}

void FileController::handleStandartPushButtonCliked()
{
	createDefaultJob("50");
}
void FileController::handlePriorityPushButtonCliked()
{
	createDefaultJob("75");
}


QString FileController::getJobType()
{
	QString jobType = m_mainWindowView->getJobTypeComboBox()->currentText();
	if (!jobType.isEmpty())
	{
		return jobType;
	}
	return "";
}

QString FileController::getJobName()
{
	QString jobName = m_mainWindowView->getJobNamelineEdit()->text();
	if (!jobName.isEmpty())
	{
		return jobName;
	}
	return "";
}

QString FileController::getScene()
{

	QString sceneLineEdit = m_mainWindowView->getSceneSelectionlineEdit()->text();
	if (!sceneLineEdit.isEmpty())
	{
		QFileInfo fileInfo(sceneLineEdit);
		if (!fileInfo.exists()&&!fileInfo.isFile())
		{
			QMessageBox::critical(m_mainWindowView, "Scene error", "The selected scene is invalid or does not exist");
		}
		else
		{
			sceneLineEdit = Utils::toUncPath(fileInfo.absoluteFilePath());
		}
	}
	else {
		QMessageBox::critical(m_mainWindowView, "Scene error", "No scene selected for the job");
		return sceneLineEdit;
	}
	return sceneLineEdit;
}

QString FileController::getImageFolder()
{
	QString ImageFolder = m_mainWindowView->getImageDirectorylineEdit()->text();
	if (!ImageFolder.isEmpty())
	{
		QFileInfo fileInfo(ImageFolder);
		if (!fileInfo.exists() && !fileInfo.isDir())
		{
			QMessageBox::critical(m_mainWindowView, "Render directory error", "The rendering directory does not exist or is invalid");
		}
		else
		{
			ImageFolder = Utils::toUncPath(fileInfo.absoluteFilePath());
		}
	}
	else
	{
		QMessageBox::critical(m_mainWindowView, "Render directory error", "No rendering directory selected");
		return ImageFolder;
	}
	return ImageFolder;
}

QString FileController::getImagesIndex()
{
	QString imagesIndex;
	if (m_mainWindowView->getFixeImageradioButton()->isChecked())
	{
		imagesIndex = "["+m_mainWindowView->getFixedImageCalculationSpinBox()->text()+"-"+ m_mainWindowView->getFixedImageCalculationSpinBox()->text()+"]";
	}
	else
	{
		imagesIndex = "[" + getFirstImageIndex() + "-" + getLastImageIndex() + "]";
	}
	qDebug() << " index :" + imagesIndex;
	return imagesIndex;
}

QString FileController::getFirstImageIndex()
{
	QString firstImageIndex;
	if (m_mainWindowView->getFixeImageradioButton()->isChecked())
	{
		firstImageIndex = m_mainWindowView->getFixedImageCalculationSpinBox()->text();
	}
	else
	{
		firstImageIndex = m_mainWindowView->getAnimationMinspinBox()->text();
	}

	if (!firstImageIndex.isEmpty())
	{
		return firstImageIndex;
	}
	return "";
}

QString FileController::getLastImageIndex()
{
	QString lastImageIndex;
	if (m_mainWindowView->getFixeImageradioButton()->isChecked())
	{
		lastImageIndex = m_mainWindowView->getFixedImageCalculationSpinBox()->text();
	}
	else
	{
		lastImageIndex = m_mainWindowView->getAnimationMaxspinBox()->text();
	}

	if (!lastImageIndex.isEmpty())
	{
		return lastImageIndex;
	}
	return "";
}

QString FileController::getName()
{
	if (m_mainWindowView->getNameCheckBox()->isChecked())
	{
		QString nameOption = getjobTypeOption("NameOption");
		QString name = getRawName();
		if (!name.isEmpty())
		{
			qDebug() << "name option : " + (nameOption + name);
			return nameOption + name;
		}
		
	}
	return "";
}

QString FileController::getRawName()
{
	return m_mainWindowView->getNameLineEdit()->text();
}

QString FileController::getFormat()
{
	if (m_mainWindowView->getFormatCheckBox()->isChecked()) {
		QString formatOption = getjobTypeOption("FormatOption");
		QString selection = m_mainWindowView->getFormatComboBox()->currentText();
		QString result;
		if (!formatOption.isEmpty())
		{
			if (m_mainWindowView->getJobTypeComboBox()->currentText() != "Blender")
			{
				result = getRawFormat();
			}
			else
			{
				QRegularExpression regex(R"(\b(.*?)\s*\[\*\.)");
				QRegularExpressionMatch match = regex.match(selection);

				if (match.hasMatch()) {
					result = match.captured(1);
				}
			}
			
		}
		else
		{
			result = getRawFormat();
		}
	
		qDebug() << "format option : " + (formatOption + result);
		return formatOption + result;
	}
	return "";
}

QString FileController::getRawFormat()
{
	QString selection = m_mainWindowView->getFormatComboBox()->currentText();

	QRegularExpression regex(R"(\[\*\.(\w+)\])");
	QRegularExpressionMatch match = regex.match(selection);
	if (match.hasMatch()) {
		qDebug()<<"raw Format : " + match.captured(1);
		return match.captured(1);
	}
	return "";
}

QString FileController::getjobTypeOption(QString option)
{
	QSettings settings("Stellantis", "Mygale");
	QFile configFile(settings.value("UserSetting/UserConfigFolder").toString() + "mainConfig.xml");
	QString currentJobType = m_mainWindowView->getJobTypeComboBox()->currentText();
	QString findOption;
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Erreur lors de l'ouverture du fichier config.xml LOAD CONFIG FILE";
		return findOption;
	}

	QXmlStreamReader configReader(&configFile);
	bool foundName = false;
	while (!configReader.atEnd() && !configReader.hasError()) {
		if (configReader.isStartElement()) {

			if (configReader.name().toString() == "name") {

				QString name = configReader.readElementText();
				if (name == currentJobType) {
					foundName = true;
				}
			}

			else if (configReader.name().toString() == option && foundName) {
				findOption = configReader.readElementText();
				configFile.close();
				return findOption;
			}
		}

		if (configReader.hasError()) {
			qDebug() << "Erreur lors de la lecture du document XML, loadFormat :" << configReader.errorString();
			configFile.close();
			return findOption;
		}
		configReader.readNext();
	}
	return findOption;
	configFile.close();
}

QString FileController::getCamera()
{
	QString cameraOption = getjobTypeOption("CameraOption");
	QString camera = m_mainWindowView->getCameralineEdit()->text();
	if (!camera.isEmpty())
	{
		return cameraOption+camera;
	}
	return "";
}

QString FileController::getSubmissionOption()
{
	QString cpuClause = getNumberOfCpu();
	QString memoryClause = getMinimumMemory();
	QString parcStyleClause = getPcPoolManagementChoice();
	QStringList clauses;
	if (!cpuClause.isEmpty()) {
		clauses << cpuClause;
	}
	if (!memoryClause.isEmpty()) {
		clauses << memoryClause;
	}
	if (!parcStyleClause.isEmpty()) {
		clauses << parcStyleClause;
	}
	else {
		QMessageBox::critical(m_mainWindowView, "Pc pool error", "You should choose at least one PC Pool");
		return "";
	}
	return clauses.join(" && ");

}

QString FileController::getNumberOfCpu()
{
	if (m_mainWindowView->getCPUcheckBox()->isChecked()) {
		return "ncpus >= " + m_mainWindowView->getMinCPUSpinBox()->text() + " && ncpus <= " + m_mainWindowView->getMaxCPUspinBox()->text();
	}
	return "";
}

QString FileController::getMinimumMemory()
{
	if (m_mainWindowView->getMemoryCheckBox()->isChecked())
	{
		double value = m_mainWindowView->getMemorydoubleSpinBox()->value() * 1024; //excat value
		return "mem > " + QString::number(value, 'f', 1);
	}
	return "";
}

QString FileController::getPcPoolManagementChoice()
{
	QStringList parcStyles;

	for (int i = 0; i < m_mainWindowView->getGridLayout()->count(); ++i) {
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(m_mainWindowView->getGridLayout()->itemAt(i)->widget());
		if (checkBox->isChecked()) {
			QString propertyValue = checkBox->property("LFS").toString();
			parcStyles << "parc_style == " + propertyValue;
		}
	}
	if (parcStyles.isEmpty()) {
		return "";
	}
	else if (parcStyles.size() == 1) {
		return parcStyles.first();
	}
	else {
		return "(" + parcStyles.join(" || ") + ")";
	}
}

QString FileController::getMaxCPU()
{
	if (m_mainWindowView->getCPUcheckBox()->isChecked()) {
		return m_mainWindowView->getMaxCPUspinBox()->text();
	}
	else
	{
		return "32"; //default MaxCpu
	}
}