#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>



#include <QtWidgets>

#include "MainWindow.h"

#include "Test_tf_gd_lib.h"

int main(int argc, char * argv[])
{
    MyLibTest(); // Просто тестирование библиотеки

    srand((unsigned int)time(0));

    QApplication app(argc, argv);

    QSurfaceFormat sfmt;
    sfmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(sfmt);


    MainWindow mainWindow;
    mainWindow.resize(1200, 600);
    mainWindow.show();
    mainWindow.AfterShow();

    app.exec();

    return 0;
}



