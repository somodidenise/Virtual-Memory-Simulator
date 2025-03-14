#include "VirtualMemorySimulator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VirtualMemorySimulator w;
    w.show();
    return a.exec();
}
