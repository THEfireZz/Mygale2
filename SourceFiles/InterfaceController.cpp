#include "../HeaderFiles/InterfaceController.h"
#include "../HeaderFiles/Utils.h"
#include<QSettings>
#include <QtWidgets>
#include<QDebug>
#include <QXmlStreamReader>



InterfaceController::InterfaceController(MainWindowView* mainWindowView, QObject* parent) :m_mainWindowView(mainWindowView), QObject(parent)
{

	// Stocke dans les parametres de l'application le chemin vers le lecteur I:/
	QSettings settings("Stellantis", "Mygale");

	settings.beginGroup("UserSetting");
	settings.setValue("UserIPath", Utils::toUncPath("I:/"));
	settings.setValue("UserConfigFolder", Utils::toUncPath("I:\\Mygale\\Config\\"));
	settings.endGroup();

	m_configFolderPath = settings.value("UserSetting/UserConfigFolder").toString();
	
	loadConfigFile();
	connect(mainWindowView, &MainWindowView::jobNamePushButtonCliked, this, &InterfaceController::handleJobNamePushButtonClicked);
	connect(mainWindowView, &MainWindowView::sceneSelectionPushButtonCliked, this, &InterfaceController::handleSceneSelectionPushButtonClicked);
	connect(mainWindowView, &MainWindowView::imageDirectoryPushButtonCliked, this, &InterfaceController::handleImageDirectoryPushButtonClicked);
	connect(mainWindowView, &MainWindowView::jobTypeComboBoxChanged, this, &InterfaceController::handleJobTypeComboBoxChanged);
	connect(mainWindowView, &MainWindowView::jobNameLineEditChanged, this, &InterfaceController::handleJobNameLineEditChanged);
	connect(mainWindowView, &MainWindowView::sceneLineEditChanged, this, &InterfaceController::handleSceneLineEditChanged);

	connect(mainWindowView, &MainWindowView::imageDirectoryLineEditChanged, this, &InterfaceController::handleImageDirectoryLineEditChanged);
	connect(mainWindowView, &MainWindowView::animationRadioButtonChanged, this, &InterfaceController::handleAnimationRadioButtonChanged);
	connect(mainWindowView, &MainWindowView::fixedImageSpinBoxChanged, this, &InterfaceController::handleFixedImageSpinBoxChanged);
	connect(mainWindowView, &MainWindowView::animationMinSpinBoxChanged, this, &InterfaceController::handleAnimationMinSpinBoxChanged);
	connect(mainWindowView, &MainWindowView::animationMaxSpinBoxChanged, this, &InterfaceController::handleAnimationMaxSpinBoxChanged);
	connect(mainWindowView, &MainWindowView::nameCheckBoxCliked, this, &InterfaceController::handleNameCheckBoxClicked);
	connect(mainWindowView, &MainWindowView::nameLineEditChanged, this, &InterfaceController::handleNameLineEditChanged);
	connect(mainWindowView, &MainWindowView::formatCheckBoxCliked, this, &InterfaceController::handleFormatCheckBoxClicked);
	connect(mainWindowView, &MainWindowView::formatComboBoxChanged, this, &InterfaceController::handleFormatComboBoxChanged);
	connect(mainWindowView, &MainWindowView::cameraGroupBoxClicked, this, &InterfaceController::handleCameraGroupBoxClicked);
	connect(mainWindowView, &MainWindowView::cameraLineEditChanged, this, &InterfaceController::handleCameraLineEditChanged);
	connect(mainWindowView, &MainWindowView::cPUCheckBoxClicked, this, &InterfaceController::handleCPUCheckBoxClicked);
	connect(mainWindowView, &MainWindowView::minCPUSpinBoxChanged, this, &InterfaceController::handleMinCPUSpinBoxChanged);
	connect(mainWindowView, &MainWindowView::maxCPUSpinBoxChanged, this, &InterfaceController::handleMaxCPUSpinBoxChanged);
	connect(mainWindowView, &MainWindowView::memoryCheckBoxClicked, this, &InterfaceController::handleMemoryCheckBoxClicked);
	connect(mainWindowView, &MainWindowView::memoryDoubleSpinBoxChanged, this, &InterfaceController::handleMemoryDoubleSpinBoxChanged);
	connect(mainWindowView, &MainWindowView::analysisCheckBoxClicked, this, &InterfaceController::handleAnalysisCheckBoxClicked);
	connect(mainWindowView, &MainWindowView::ressubmissionCheckBoxClicked, this, &InterfaceController::handleRessubmissionCheckBoxClicked);
	changePermissionsRecursively(m_configFolderPath);
	loadPreviousSession();
}

void InterfaceController::loadConfigFile()
{
	if (!QFile::exists(m_configFolderPath + "mainConfig.xml")) {
		qDebug() << m_configFolderPath + "config/mainConfig.xml n'existe pas";
		QString defaultConfigFilePath = ":/config/mainConfig.xml";
		QFileInfo fileInfo(m_configFolderPath + "mainConfig.xml");
		QDir parentDir = fileInfo.dir();
		if (!parentDir.mkpath(".") || !QFile::copy(defaultConfigFilePath, m_configFolderPath + "mainConfig.xml")) {
			qDebug() << "Erreur lors de la creation ou de la copie du fichier de configuration de" << m_configFolderPath + "mainConfig.xml";
			return;
		}
		
	}

	// Open the config file for reading
	QFile configFile(m_configFolderPath + "mainConfig.xml");
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Erreur lors de l'ouverture du fichier config.xml LOAD CONFIG FILE";
		return;
	}
	extractConfig(configFile);
	configFile.seek(0);//position le curseur du fichier à l'emplacement 0
	loadFormat(configFile);

	configFile.close();

	

	if (!QFile::exists(m_configFolderPath + "skeleton\\squeletteAnalysis.txt")) {
		qDebug() << "Le fichier " << m_configFolderPath + "skeleton\\squeletteAnalysis.txt" << " n'existe pas";
		QString defaultConfigFilePath = ":/Skeleton/squeletteAnalysis.txt";
		if (!QFile::copy(defaultConfigFilePath, m_configFolderPath + "skeleton\\squeletteAnalysis.txt")) {
			qDebug() << "Erreur lors de la copie du fichier de configuration par defaut";
			return;
		}

	}
	if (!QFile::exists(m_configFolderPath + "lance.txt")) {
		qDebug() << "Le fichier " << m_configFolderPath + "lance.txt" << " n'existe pas";
		QString defaultConfigFilePath = ":/lance/lance.txt";
		if (!QFile::copy(defaultConfigFilePath, m_configFolderPath + "lance.txt")) {
			qDebug() << "Erreur lors de la copie du fichier de configuration par defaut";
			return;
		}

	}
	if (!QFile::exists(m_configFolderPath + "lanceAnalysis.txt")) {
		qDebug() << "Le fichier " << m_configFolderPath + "lanceAnalysis.txt" << " n'existe pas";
		QString defaultConfigFilePath = ":/lance/lanceAnalysis.txt";
		if (!QFile::copy(defaultConfigFilePath, m_configFolderPath + "lanceAnalysis.txt")) {
			qDebug() << "Erreur lors de la copie du fichier de configuration par defaut";
			return;
		}

	}

}
void InterfaceController::extractConfig(QFile& configFile)
{

	qDebug() << "Debut de l'extraction";
	QXmlStreamReader configReader(&configFile);
	for (QXmlStreamReader::TokenType token = configReader.readNext(); token != QXmlStreamReader::TokenType::EndDocument; token = configReader.readNext()) {
		QString jobType;
		QString skeletonFile;
		while (configReader.readNextStartElement()) {
			if (configReader.name().toString() == "name") {
				jobType = configReader.readElementText();
				qDebug() << "jobType : " << jobType;
			}
			else if (configReader.name().toString() == "skeleton") {
				skeletonFile = configReader.readElementText();
				QString filePath = m_configFolderPath + "skeleton/" + skeletonFile;
				if (!QFile::exists(filePath)) {
					qDebug() << "Le fichier " << filePath << " n'existe pas";
					QString defaultConfigFilePath = ":/Skeleton/" + skeletonFile;
					QDir parentDir = QFileInfo(filePath).dir();
					if (!parentDir.mkpath(".") || !QFile::copy(defaultConfigFilePath, filePath)) {
						qDebug() << "Erreur lors de la copie du fichier de configuration par defaut";
						return;
					}
				}
			}
		}
		if (!jobType.isEmpty() && !skeletonFile.isEmpty()) {
			m_mainWindowView->getJobTypeComboBox()->addItem(jobType, skeletonFile);
			skeletonFile.clear();
			jobType.clear();
		}
	}
	if (configReader.hasError()) {
		qDebug() << "Erreur lors de la lecture du document XML, loadFormat :" << configReader.errorString();
		return;
	}
}
void InterfaceController::loadFormat(QFile& configFile)
{
	QXmlStreamReader configReader(&configFile);
	bool foundName = false;

	while (!configReader.atEnd() && !configReader.hasError()) {
		if (configReader.isStartElement()) {
			if (configReader.name().toString() == "name") {
				//check arg jobtype
				QString name = configReader.readElementText();
				if (name == m_mainWindowView->getJobTypeComboBox()->currentText()){
					foundName = true;
				}
				else {
					foundName = false;
				}
			}
			//load skeleton file 

			else if (configReader.name().toString() == "Format" && foundName) {
				QString format = configReader.readElementText();
				m_mainWindowView->getFormatComboBox()->addItem(format);
			}
		}
		configReader.readNext();
	}
	if (configReader.hasError()) {
		qDebug() << "Erreur lors de la lecture du document XML, loadFormat :" << configReader.errorString();
		return;
	}
}
void InterfaceController::changePermissionsRecursively(const QString& path)
{
	QDir directory(path);
	directory.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	directory.setSorting(QDir::Name);

	QFileInfoList list = directory.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		if (!QFile::setPermissions(fileInfo.absoluteFilePath(), QFile::ReadOwner | QFile::WriteOwner)) {
			qDebug() << "Erreur lors de la modification des permissions du fichier " << fileInfo.absoluteFilePath();
		}
	}

	// Parcourir les sous-dossiers récursivement
	QStringList subdirs = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QString subdir, subdirs) {
		QString subpath = path + QDir::separator() + subdir;
		changePermissionsRecursively(subpath);
	}
}

void InterfaceController::handleJobNamePushButtonClicked()
{
	Utils::incrementJobName(m_mainWindowView);
}

void InterfaceController::handleImageDirectoryPushButtonClicked()
{
	QString folderPath = QFileDialog::getExistingDirectory(m_mainWindowView, tr("Select Directory"), QString::fromStdString("I:/"),
		QFileDialog::DontResolveSymlinks);

	if (!folderPath.isEmpty()) {
		// Utiliser le chemin du dossier selectionne
		
		m_mainWindowView->getImageDirectorylineEdit()->setText(folderPath + "/");
	}
}

void InterfaceController::handleSceneSelectionPushButtonClicked()
{
	QString folderPath = QFileDialog::getOpenFileName(m_mainWindowView, tr("Select TEMP directory"), QString::fromStdString("I:/"), tr("All Files (*.*)"));
	if (!folderPath.isEmpty()) {
		m_mainWindowView->getSceneSelectionlineEdit()->setText(folderPath);
	}
}

void InterfaceController::handleJobTypeComboBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("JobTypeComboBox", m_mainWindowView->getJobTypeComboBox()->currentIndex());

	QFile configFile(m_configFolderPath + "mainConfig.xml");
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Erreur lors de l'ouverture du fichier config.xml LOAD CONFIG FILE";
		return;
	}
	m_mainWindowView->getFormatComboBox()->clear();
	loadFormat(configFile);
}

void InterfaceController::handleJobNameLineEditChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("JobNamelineEdit", m_mainWindowView->getJobNamelineEdit()->text());
}

void InterfaceController::handleSceneLineEditChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("SceneSelectionlineEdit", m_mainWindowView->getSceneSelectionlineEdit()->text());
}

void InterfaceController::handleImageDirectoryLineEditChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("ImageDirectorylineEdit", m_mainWindowView->getImageDirectorylineEdit()->text());
}

void InterfaceController::handleAnimationRadioButtonChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("AnimationradioButton", m_mainWindowView->getAnimationradioButton()->isChecked());
}

void InterfaceController::handleFixedImageSpinBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("FixedImageCalculationSpinBox", m_mainWindowView->getFixedImageCalculationSpinBox()->value());
}

void InterfaceController::handleAnimationMinSpinBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("AnimationMinspinBox", m_mainWindowView->getAnimationMinspinBox()->value());
}

void InterfaceController::handleAnimationMaxSpinBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("AnimationMaxspinBox", m_mainWindowView->getAnimationMaxspinBox()->value());
}

void InterfaceController::handleNameCheckBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("NameCheckBox", m_mainWindowView->getNameCheckBox()->isChecked());
}

void InterfaceController::handleNameLineEditChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("NameLineEdit", m_mainWindowView->getNameLineEdit()->text());
}

void InterfaceController::handleFormatCheckBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("FormatCheckBox", m_mainWindowView->getFormatCheckBox()->isChecked());
}

void InterfaceController::handleFormatComboBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("FormatComboBox", m_mainWindowView->getFormatComboBox()->currentIndex());
	
}

void InterfaceController::handleCameraGroupBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("CameraGroupBox", m_mainWindowView->getCameraGroupBox()->isChecked());
}

void InterfaceController::handleCameraLineEditChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("CameralineEdit", m_mainWindowView->getCameralineEdit()->text());
}

void InterfaceController::handleCPUCheckBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("CPUcheckBox", m_mainWindowView->getCPUcheckBox()->isChecked());
}

void InterfaceController::handleMinCPUSpinBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("MinCPUSpinBox", m_mainWindowView->getMinCPUSpinBox()->value());
}

void InterfaceController::handleMaxCPUSpinBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("MaxCPUspinBox", m_mainWindowView->getMaxCPUspinBox()->value());
}

void InterfaceController::handleMemoryCheckBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("MemoryCheckBox", m_mainWindowView->getMemoryCheckBox()->isChecked());
}

void InterfaceController::handleMemoryDoubleSpinBoxChanged()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("MemorydoubleSpinBox", m_mainWindowView->getMemorydoubleSpinBox()->value());
}

void InterfaceController::handleAnalysisCheckBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("AnalysischeckBox", m_mainWindowView->getAnalysischeckBox()->isChecked());
}

void InterfaceController::handleRessubmissionCheckBoxClicked()
{
	QSettings settings("Stellantis", "Mygale");
	settings.setValue("ResubmissionCheckBox", m_mainWindowView->getResubmissionCheckBox()->isChecked());
}

void InterfaceController::loadPreviousSession()
{
	QSettings settings("Stellantis", "Mygale");

	m_mainWindowView->getJobTypeComboBox()->setCurrentIndex(settings.value("JobTypeComboBox").toInt());
	m_mainWindowView->getJobNamelineEdit()->setText(settings.value("JobNamelineEdit").toString());
	m_mainWindowView->getSceneSelectionlineEdit()->setText(settings.value("SceneSelectionlineEdit").toString());
	m_mainWindowView->getImageDirectorylineEdit()->setText(settings.value("ImageDirectorylineEdit").toString());
	m_mainWindowView->getAnimationradioButton()->setChecked(settings.value("AnimationradioButton").toBool());
	m_mainWindowView->getFixedImageCalculationSpinBox()->setValue(settings.value("FixedImageCalculationSpinBox").toInt());
	m_mainWindowView->getAnimationMinspinBox()->setValue(settings.value("AnimationMinspinBox").toInt());
	m_mainWindowView->getAnimationMaxspinBox()->setValue(settings.value("AnimationMaxspinBox").toInt());
	m_mainWindowView->getNameCheckBox()->setChecked(settings.value("NameCheckBox").toBool());
	m_mainWindowView->getNameLineEdit()->setText(settings.value("NameLineEdit").toString());


	QFile configFile(m_configFolderPath + "mainConfig.xml");
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Erreur lors de l'ouverture du fichier config.xml LOAD CONFIG FILE";
		return;
	}
	m_mainWindowView->getFormatComboBox()->clear();
	loadFormat(configFile);
	configFile.close();
	
	m_mainWindowView->getFormatCheckBox()->setChecked(settings.value("FormatCheckBox").toBool());
	m_mainWindowView->getFormatComboBox()->setCurrentIndex(settings.value("FormatComboBox").toInt());
	m_mainWindowView->getCameraGroupBox()->setChecked(settings.value("CameraGroupBox").toBool());
	m_mainWindowView->getCameralineEdit()->setText(settings.value("CameralineEdit").toString());
	m_mainWindowView->getCPUcheckBox()->setChecked(settings.value("CPUcheckBox").toBool());
	m_mainWindowView->getMinCPUSpinBox()->setValue(settings.value("MinCPUSpinBox").toInt());
	m_mainWindowView->getMaxCPUspinBox()->setValue(settings.value("MaxCPUspinBox").toInt());

	m_mainWindowView->getMemoryCheckBox()->setChecked(settings.value("MemoryCheckBox").toBool());
	m_mainWindowView->getMemorydoubleSpinBox()->setValue(settings.value("MemorydoubleSpinBox").toDouble());

	m_mainWindowView->getAnalysischeckBox()->setChecked(settings.value("AnalysischeckBox").toBool());
	m_mainWindowView->getResubmissionCheckBox()->setChecked(settings.value("ResubmissionCheckBox").toBool());

	QGridLayout* gridLayout = m_mainWindowView->getGridLayout();
	for (int i = 0; i < gridLayout->count(); i++) {
		QWidget* widget = gridLayout->itemAt(i)->widget();
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
		if (checkBox) {
			QString key = checkBox->objectName();
			checkBox->setChecked(settings.value(key).toBool());

		}
	}

}

