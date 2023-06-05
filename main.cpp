#include "HeaderFiles/MainWindowView.h"
#include "HeaderFiles/InterfaceController.h"
#include "HeaderFiles/FileController.h"
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowView w;
    InterfaceController interface(&w);
    FileController fileController(&w);
    w.startConnection();
    w.show();
    return a.exec();
}
