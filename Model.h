#pragma once

#include "IModel.h"
#include "Configuration.h"

#include <vector>
#include <QObject>
#include <iostream>

#include <chrono>

using ClockType = std::chrono::steady_clock;

class Model : public QObject, public IModel
{
    Q_OBJECT
private:

    void init();

    bool isInited = false;

    std::vector<ExportGeneralData> ExportAllGeneralData;
    std::vector<std::vector<ExportDetailedInfo>> ExportAllDetailedInfo;

public:

    const auto & GetExportAllGeneralData()  {return ExportAllGeneralData;}
    const auto & GetExportAllDetailedInfo() {return ExportAllDetailedInfo;}

    bool IsInited() {return isInited;}

    int nCView; // при старте 4
    mutable std::vector<int> cViewIds;

    bool fDrawCfg3d;

    double alpha, beta;

    double timeK;

    int uiActiveElement;

    double t, tPrev;

    std::vector<Configuration*> population;

    mutable std::vector<int> pA;

    Model();

    QVector3D cViewPos(int k) const;

    int nSpecial() const override { return fDrawCfg3d ? nCView : 0; }

    double getSpecialViewW() const { return nCView == 1 ? 0.8 : 0.4; }

    void getSpecialViewParams(int k, double &x0, double &y0, double &w, double &h) const override;

    void draw() const override;

    void drawSpecial(int k) const override;

    void drawWindows() const;

	// ----
	// ”правл¤ющие параметры модели


    std::string getParamStr(const std::string &name) const;

//    std::string info() const override;

//    int objectId() const override;

	// ----
    // Интерактивность

    //void onKeyPress(unsigned char key);

//    void onSpecialKeyPress(int key);

    bool onMouseMove(double x, double y);

    bool onMouseClick(double x, double y);


    void openConfigWindows(int id);

	// ----
    // Математическа¤ модель

    void dynamics();

    void analyze() const;

    void sortConfiguration() const;

	// ----
	// ƒинамическа¤ структура алгоритма

    Configuration* protoCfg = nullptr;

    void start();

    void createPopulation(); // здесь создаетс¤ начальна¤ попул¤ци¤ конфигураций

    void algStep();

    void timeStep();

    void algMove();

    void algMutate();

    void algCrossover();
    void algClearAndRenew();

    void deleteConfig(Configuration* old1);

	// ----
	// √еометри¤

    static QVector3D _P_(double x, double y) { return QVector3D(x, y, 0); }

    static QVector3D _P_(const QVector3D & p)  { return QVector3D(p.x(), p.y(), 0);  }

    static double physicalTime(double* out_dt = nullptr)
    {
        static bool inited = false;
        static double t = 0;
        //static clock_t c0;
        static std::chrono::time_point<ClockType> c0;// = ClockType::now();


        if (!inited)
        {
//            c0 = clock();
            c0 = ClockType::now();
            inited = true;
            if (out_dt)
                *out_dt = 0;
            return t;
        }

        //clock_t c1 = clock();
        std::chrono::time_point<ClockType> c1 = ClockType::now();

//        if (c1 - c0 == 0)
//            std::cout << "dt = 0" << std::endl;

//        double dt = 0.001*(c1 - c0);

        double dt = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(c1 - c0).count();
        dt /= 1.0e9;

        //std::cout << "physicalTime: dt = " << dt << std::endl;

        c0 = c1;

        t += dt;

        if (out_dt)
            *out_dt = dt;

        return t;
    }

    static Model & Instance();  // Singletone

signals:

    void signalRepaint() const;

    void signalSendGeneralData(ExportGeneralData);
    void signalSendDetailedInfo(ExportDetailedInfo);

};
