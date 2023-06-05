#include <QString>
#include"MainWindowView.h"
#pragma once
class Utils
{
public:
	static QString toUncPath(QString path);
	static void incrementJobName(MainWindowView* mainWindow);
};

