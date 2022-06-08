#include "paradigms.h"
#include "setupdlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    paradigms w;
//    w.show();
    SetupDlg w;
    w.show();
    return a.exec();
}
