#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>

#include "Model.h"
#include "View.h"
#include "Settings.h"

#include "FormSettingsSelector.h"
#include "ui_FormSettingsSelector.h"

using namespace std;

#include <QtWidgets>

#include "MainWindow.h"
#include "FormGeneral.h"
#include "FormConfigInfo.h"

#include "Test_tf_gd_lib.h"

int main(int argc, char * argv[])
{
 // проблема при изменении размера популяции на меньшее чем вьюпортов
    MyLibTest(); // Просто тестирование библиотеки

    srand((unsigned int)time(0));

    QApplication app(argc, argv);

    QSurfaceFormat sfmt;
    sfmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(sfmt);

//    FormSettingsSelector frmSettingsSelector;
//    frmSettingsSelector.show();
//    frmSettingsSelector.exec();
//    if (frmSettingsSelector.ui->listWidget->count() > 0)
//        fn = frmSettingsSelector.ui->listWidget->currentItem()->text();
//    else
//    {
//        app.closeAllWindows();
//        return 0;
//    }

    QString fn = "Settings.json";
    cout << "fn = " << fn.toStdString() << endl;

    Settings::Inst().LoadFromJson(fn.toStdString());
    cout << endl << Settings::Inst() << endl;

    MainWindow mainWindow;
    mainWindow.resize(1200, 600);
    mainWindow.show();
    mainWindow.AfterShow();

//    Model::Instance().start();

//    View& view = View::Instance();
//    view.initialize();
//    view.setModel(&Model::Instance());
//    view.reshape(mainWindow.GetGLWidth(), mainWindow.GetGLHeight());
//    view.update();

//    FormGeneral formGeneral;
//    formGeneral.resize(1100, 500);
//    formGeneral.show();


//    QObject::connect(&Model::Instance(), SIGNAL(signalSendGeneralData(ExportGeneralData)),
//                     &formGeneral, SLOT(slotReceiveGeneralData(ExportGeneralData)));

//    FormConfigInfo formConfigInfo;
//    formConfigInfo.resize(1350, 500);
//    formConfigInfo.Set_nConfig(0);
//    formConfigInfo.show();

//    QObject::connect(&Model::Instance(), SIGNAL(signalSendDetailedInfo(ExportDetailedInfo)),
//                     &formConfigInfo, SLOT(slotReceiveDetailedInfo(ExportDetailedInfo)));

//    QObject::connect(&Model::Instance(), SIGNAL(signalRepaint()),
//                     &mainGLWidget, SLOT(slotReceiveRepaintSignal()));

    app.exec();

    return 0;
}



