#include "../HeaderFiles/Utils.h"
#include <Windows.h>
#include <QDebug>
#include<QDir>
#include<QMessageBox>
QString Utils::toUncPath(QString path)
{
    QString uncPath = path;
    uncPath.replace("/", "\\");

    QFileInfo fileInfo(uncPath);
    QString drivePath = fileInfo.absolutePath();
    QString rootPath = drivePath.left(2);
    DWORD driveType = GetDriveTypeW((LPCWSTR)rootPath.utf16());

    if (driveType == DRIVE_REMOTE) {
        WCHAR buf[MAX_PATH];
        DWORD buflen = MAX_PATH;
        DWORD ret = WNetGetConnectionW((LPCWSTR)rootPath.utf16(), buf, &buflen);

        if (ret == NO_ERROR) {
            QString pathUnc = QString::fromWCharArray(buf);
            uncPath.replace(0, 2, pathUnc);
        }
        else {
            qWarning() << "WNetGetConnectionW failed with error code" << ret;
            
            return "";
        }
        
    }
    else
    {
        QMessageBox::critical(nullptr, "Drive error", "The path is not a DRIVE_REMOTE");
        return "";
    }
    return uncPath;
}

void Utils::incrementJobName(MainWindowView* mainWindow)
{
    QString baseText = mainWindow->getJobNamelineEdit()->text();
    if (baseText.isEmpty()) {
        return;
    }

    int suffix = 0;
    QRegularExpression regex("_([0-9]+)$");
    QRegularExpressionMatch match = regex.match(baseText);
    if (match.hasMatch()) {
        suffix = match.captured(1).toInt();
        baseText.chop(match.captured(1).length() + 1);
    }

    ++suffix;
    QString newText = QString("%1_%2").arg(baseText).arg(QString::number(suffix), 2, '0');

    mainWindow->getJobNamelineEdit()->setText(newText);
}

