#include "stdafx.h"
#include "virapp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    VIRApp window;
    window.show();
    return app.exec();
}
