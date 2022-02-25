TEMPLATE = app

#QT += gui

QT += widgets openglwidgets

CONFIG += c++17 console
CONFIG -= app_bundle

LIBS += -lopengl32 -lglu32
#LIBS +=-lglut32
#LIBS += -L.\

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Configuration.cpp \
        DialogAboReport.cpp \
        DialogChangePopulationSize.cpp \
        DialogEditRoutes.cpp \
        DialogEditSignalNodes.cpp \
        DialogGradConfig.cpp \
        DialogNewGradConfig.cpp \
        Export.cpp \
        FormAboCalc.cpp \
        FormConfigInfo.cpp \
        FormGeneral.cpp \
        FormGradConfigDetails.cpp \
        FormGradGeneral.cpp \
        FormRelief.cpp \
        FormSettingsSelector.cpp \
        GradConfig.cpp \
        GradModel.cpp \
        MainGLWidget.cpp \
        MainWindow.cpp \
        MathUtils.cpp \
        Model.cpp \
        Relief.cpp \
        Route.cpp \
        Settings.cpp \
        SignalNode.cpp \
        Test_tf_gd_lib.cpp \
        View.cpp \
        View3d.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Configuration.h \
    DialogAboReport.h \
    DialogChangePopulationSize.h \
    DialogEditRoutes.h \
    DialogEditSignalNodes.h \
    DialogGradConfig.h \
    DialogNewGradConfig.h \
    Export.h \
    FormAboCalc.h \
    FormConfigInfo.h \
    FormGeneral.h \
    FormGradConfigDetails.h \
    FormGradGeneral.h \
    FormRelief.h \
    FormSettingsSelector.h \
    GradConfig.h \
    GradModel.h \
    IModel.h \
    IView.h \
    MainGLWidget.h \
    MainWindow.h \
    MathUtil.h \
    MathUtils.h \
    Model.h \
    Relief.h \
    Route.h \
    Settings.h \
    SignalNode.h \
    Sota.h \
    Test_tf_gd_lib.h \
    Trajectory.h \
    Types.h \
    View.h \
    View3d.h

FORMS += \
    DialogAboReport.ui \
    DialogChangePopulationSize.ui \
    DialogEditRoutes.ui \
    DialogEditSignalNodes.ui \
    DialogGradConfig.ui \
    DialogNewGradConfig.ui \
    FormAboCalc.ui \
    FormConfigInfo.ui \
    FormGeneral.ui \
    FormGradConfigDetails.ui \
    FormGradGeneral.ui \
    FormRelief.ui \
    FormSettingsSelector.ui \
    MainWindow.ui



unix|win32: LIBS += -L$$PWD/tf_gd_lib/ -lTestLibGrad

INCLUDEPATH += $$PWD/tf_gd_lib
DEPENDPATH += $$PWD/tf_gd_lib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/tf_gd_lib/TestLibGrad.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/tf_gd_lib/libTestLibGrad.a
