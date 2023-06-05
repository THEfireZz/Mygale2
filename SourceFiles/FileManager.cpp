#include "../HeaderFiles/FileManager.h"
#include <QHash>
#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <iostream>
#include <sstream>
#include <QProcess>
#include <QXmlStreamReader>
#include <QMessageBox>

void FileManager::insertLineInFile(QString line)
{
	//try to open output file
	if (!m_outputFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
		qDebug() << "Erreur: Impossible d'ouvrir le fichier " << m_outputFilePath;
		return;
	}
	QTextStream outStream(&m_outputFile);
	outStream << replaceLine(line) << "\n";
	m_outputFile.close();

}

QString FileManager::replaceLine(QString& line)
{
	QHash<QString, QString> replacements{
		{"<jobName>", m_jobName},
		{"<submissionOption>", m_submissionOption},
		{"<imgIndex>", m_imgIndex},
		{"<firstImg>", m_firstImg},
		{"<lastImg>", m_lastImg},
		{"<path>", m_iUnc},
		{"<maxCPU>",m_maxCPU},
		{"<previousJobID>",m_previousJobId},
		{"<priority>", m_priority},
		{"<scene>", m_scene},
		{"<dossierImages>", m_dossierImage},
		{"<format>", m_format},
		{"<name>", m_name},
		{"<camera>",m_camera}

	};
	for (QString& key : replacements.keys()) {
		line.replace(key, replacements.value(key));

	}
	return line;
}

QString FileManager::execute()
{
	QProcess process;
	process.start(m_outputFilePath);
	process.waitForStarted();
	process.waitForFinished();

	QString output = process.readAllStandardOutput();
	output.replace("\n", "");

	qDebug() << "CMD : " << m_outputFilePath;
	qDebug() << "Output : " << output;


	return output.trimmed();
}

void FileManager::toRessubmission()
{
	QString ressubIndexScript = R"(
rem Lire la valeur correspondante dans missingIndice.txt
set /p val=<<path>mygale\TEMP\<jobName>\lsf\missingIndice.txt
set val=!val: =!
set idx=1
for %%i in (!val!) do (
    set val[!idx!]=%%i
    set /a idx+=1
)
echo Valeur lue depuis missingIndice.txt: !val[%LSB_JOBINDEX%]!
rem Utiliser la valeur lue comme valeur pour CALC_IMAGE
set CALC_IMAGE=!val[%LSB_JOBINDEX%]!
)";


	if (!m_outputFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		qDebug() << "Erreur: Impossible d'ouvrir le fichier " << m_outputFilePath;
		return;
	}
	QString content = QTextStream(&m_outputFile).readAll();
	content.replace("set /A CALC_IMAGE=%LSB_JOBINDEX%", replaceLine(ressubIndexScript));

	m_outputFile.resize(0);
	QTextStream outStream(&m_outputFile);
	outStream << content;

	m_outputFile.close();
}

void FileManager::extractConfigLine(const QString& filePath)
{
	QFile inputFile(filePath);
	if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "extractConfigLine Failed : " + filePath + " couldnt be open";
	}
	QTextStream inputStream(&inputFile);
	while (!inputStream.atEnd()) {
		QString line = inputStream.readLine();
		insertLineInFile(line);
	}
	inputFile.close();
}

bool FileManager::hasRequiredElement()
{
	
	QSettings settings("Stellantis", "Mygale");
	QString filePath = settings.value("UserSetting/UserConfigFolder").toString() + "mainConfig.xml";
	QFile configFile(filePath);
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Erreur lors de l'ouverture du fichier config.xml LOAD CONFIG FILE";
		configFile.close();
		return false;
	}
	QXmlStreamReader configReader(&configFile);
	bool foundJobType = false;

	while (!configReader.atEnd() && !configReader.hasError())
	{

		if (configReader.isStartElement()) {

			if (configReader.name().toString() == "name")
			{

				//check arg jobtype
				QString jobType = configReader.readElementText();
				if (jobType == m_jobType) {
					foundJobType = true;

				}
				else {
					foundJobType = false;
				}
			}
			else if (configReader.name().toString() == "Required" && foundJobType) {

				QString requiredElement = configReader.readElementText();
				

				if (requiredElement == "name" && m_name.isEmpty())
				{
					
					QMessageBox::critical(nullptr, m_jobType + " error", "The name is required for " + m_jobType);
					configFile.close();
					return false;
				}
				else if (requiredElement == "format" && m_format.isEmpty())
				{
					
					QMessageBox::critical(nullptr, m_jobType + " error", "The format is required for " + m_jobType);
					configFile.close();
					return false;
				}
				else if (requiredElement == "camera" && m_camera.isEmpty())
				{
					
					QMessageBox::critical(nullptr, m_jobType + " error", "The camera is required for " + m_jobType);
					configFile.close();
					return false;
				}
			}
		}
		configReader.readNext();
	}
	if (configReader.hasError()) {
		qDebug() << "Erreur lors de la lecture du document XML, loadFormat :" << configReader.errorString();
		configFile.close();
		return false;
	}
	configFile.close();
	return true;
}

FileManager::FileManager(QString joType, QString jobName, QString scene, QString dossierImage, QString name, QString format, QString camera, QString imgIndex, QString firstImg, QString lastImg, QString fileName, QString submissionOption, QString priority, QString maxCPU, QString previousJobId)
	: m_jobType(joType), m_jobName(jobName), m_scene(scene), m_dossierImage(dossierImage), m_name(name), m_format(format), m_camera(camera), m_imgIndex(imgIndex), m_firstImg(firstImg), m_lastImg(lastImg), m_fileName(fileName), m_submissionOption(submissionOption), m_priority(priority), m_maxCPU(maxCPU), m_previousJobId(previousJobId)
{
	QSettings settings("Stellantis", "Mygale");
	m_iUnc = settings.value("UserSetting/UserIPath").toString();
	m_outputFilePath = settings.value("UserSetting/UserTempFolder").toString() + "\\" + jobName + "\\lsf\\" + fileName + ".bat";
	m_outputFile.setFileName(m_outputFilePath);
	createOutputFolder();
}


void FileManager::createOutputFolder()
{
	QFileInfo fileInfo(m_outputFilePath);
	QDir dir(fileInfo.absolutePath());
	QDir appliDir(fileInfo.absolutePath() + "\\..");

	if (!dir.exists()) {
		if (!dir.mkpath(dir.path())) {
			qDebug() << "Erreur: Impossible de créer le dossier pour le fichier " << m_outputFilePath;
		}
		appliDir.mkdir("appli");
	}
	if (!m_outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qDebug() << "Erreur: Impossible de créer le fichier " << m_outputFilePath;

	}
	m_outputFile.close();
}

FileManager::Builder& FileManager::Builder::jobType(QString jobType)
{
	m_jobType = jobType;
	return *this;
}

FileManager::Builder& FileManager::Builder::jobName(QString jobName)
{
	m_jobName = jobName;
	return *this;
}

FileManager::Builder& FileManager::Builder::scene(QString scene)
{
	m_scene = scene;
	return *this;
}

FileManager::Builder& FileManager::Builder::dossierImage(QString dossierImage)
{
	m_dossierImage = dossierImage;
	return *this;
}

FileManager::Builder& FileManager::Builder::format(QString format)
{
	m_format = format;
	return *this;
}

FileManager::Builder& FileManager::Builder::name(QString name)
{
	m_name = name;
	return *this;
}

FileManager::Builder& FileManager::Builder::camera(QString camera)
{
	m_camera = camera;
	return *this;
}

FileManager::Builder& FileManager::Builder::imageIndex(QString imgIndex)
{
	m_imgIndex = imgIndex;
	return *this;

}

FileManager::Builder& FileManager::Builder::firstImg(QString firstImg)
{
	m_firstImg = firstImg;
	return *this;
}

FileManager::Builder& FileManager::Builder::lastImg(QString lastImg)
{
	m_lastImg = lastImg;
	return *this;
}

FileManager::Builder& FileManager::Builder::fileName(QString fileName)
{
	m_fileName = fileName;
	return *this;
}

FileManager::Builder& FileManager::Builder::submissionOption(QString submissionOption)
{
	m_submissionOption = submissionOption;
	return *this;
}

FileManager::Builder& FileManager::Builder::priority(QString priority)
{
	m_priority = priority;
	return *this;
}

FileManager::Builder& FileManager::Builder::maxCPU(QString maxCPU)
{
	m_maxCPU = maxCPU;
	return *this;
}

FileManager::Builder& FileManager::Builder::previousJobId(QString previousJobId)
{
	m_previousJobId = previousJobId;
	return *this;
}

FileManager FileManager::Builder::build()
{
	return FileManager(m_jobType, m_jobName, m_scene, m_dossierImage, m_name, m_format, m_camera, m_imgIndex, m_firstImg, m_lastImg, m_fileName, m_submissionOption, m_priority, m_maxCPU, m_previousJobId);
}