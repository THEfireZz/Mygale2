#pragma once
#include <QString>
#include <fstream>
#include <QFile>
class FileManager
{
public:
    
    void insertLineInFile(QString line);
    QString replaceLine(QString& line);
    QString execute();
    void toRessubmission();
    void extractConfigLine(const QString& filePath);
    bool hasRequiredElement();
    class Builder
    {
    public:
        Builder& jobType(QString jobType);
        Builder& jobName(QString jobName);
        Builder& scene(QString scene);
        Builder& dossierImage(QString dossierImage);
        Builder& format(QString format);
        Builder& name(QString name);
        Builder& camera(QString imgIndex);
        Builder& imageIndex(QString camera);
        Builder& firstImg(QString firstImg);
        Builder& lastImg(QString lastImg);
        Builder& fileName(QString fileName);
        Builder& submissionOption(QString submissionOption);
        Builder& priority(QString priority);
        Builder& maxCPU(QString maxCPU);
        Builder& previousJobId(QString previousJobId);

        FileManager build();
    private:
        QString m_jobType;
        QString m_jobName;
        QString m_scene;
        QString m_dossierImage;
        QString m_name;
        QString m_format;
        QString m_camera;
        QString m_imgIndex;
        QString m_firstImg;
        QString m_lastImg;
        QString m_fileName;
        QString m_submissionOption;
        QString m_priority;
        QString m_maxCPU;
        QString m_previousJobId;
    };



protected:
	
	std::ifstream m_inputFileStream;
    QFile m_outputFile;
	QString m_outputFilePath;
	
    QString m_jobType;
    QString m_jobName;
    QString m_iUnc;
    QString m_scene;
    QString m_dossierImage;
    QString m_name;
    QString m_format;
    QString m_camera;
    QString m_imgIndex;
    QString m_firstImg;
    QString m_lastImg;
    QString m_skeletonName;
    QString m_submissionOption;
    QString m_maxCPU;
    QString m_lanceType;
    QString m_previousJobId;
    QString m_priority;
    
    QString m_fileName;
private:
    FileManager(QString joType, QString jobName, QString scene, QString dossierImage, QString name, QString format, QString camera, QString imgIndex, QString firstImg, QString lastImg, QString fileName, QString submissionOption, QString priority, QString maxCPU, QString previousJobId);

    void createOutputFolder();
};

