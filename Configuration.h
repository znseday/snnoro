#pragma once

#include "Sota.h"
#include "Trajectory.h"
//#include "IInfoProvider.h"

#include <string>
#include <vector>


#include <GL/glu.h>

#include <QVector2D>
#include <QVector3D>

#include <QString>

#include "Export.h"

constexpr auto M0 = 1024; // объём исторических записей
constexpr auto HIS_N = 80;
constexpr auto HIS_MULT = (1 - 1.0 / HIS_N);

#define F_DIFFERENT_NODES (0)


class Configuration //: public IInfoProvider
{
private:
    // Область
    QVector2D b0, b1;

    // Рельеф
    int nRelief;
    std::vector<double> reliefA, reliefB, reliefH;

    static GLuint ReliefCompileList;
    static GLuint Relief3dCompileList;
    static bool IsReliefBuilt;
    static void BuildReliefToGL(bool _fDrawCfg3d, GLuint _nList, const Configuration &_protoConf);
    static void BuildAllReliefToGL(const Configuration &_protoConf);

    // Маршруты движения абонентов
    std::vector<Trajectory> Tr;

    // Узлы связи
    std::vector<Sota> Soty;

    // ----
    // Состояние системы
    //int nStNode;
    std::vector<QVector3D> stNodes;

    // ~~~~
    // Динамические характеристики

    double tStart;
    double t;

    // История
    int phase0; // фаза помогает нам по кругу записывать исторические записи, переходя в начало массива когда у нас заканчива
    double _refreshTime;
    //это структура, сотоящая из числа w (вес, удачность конфигурации) и функции clear, котороя это число обнуляет
    struct HRecord
    {
        double W;
        void clear()
        {
            W = 0;
        }
    };

    // пока оставляем mutable из-за кривого кода
    mutable HRecord _AR[M0]; // это массив архивных записей размером 1024 (количество записей)

    // Анализ конфигурации
    double W;
    double W0 = -1;
    void clear();

    int id = 777; // для теста 777. чтобы убедиться, что это значение не испольузется, и можно было оставлть неинициализированным

    int currentIndex;
    std::string lastHintText;

    bool fHintM;
    bool fHintCO, fHintChild;

public:

    int  GetId() const {return id;}
    void SetId(int _id) {id = _id;}

    void SetCurrentIndex(int _currentIndex) {currentIndex  = _currentIndex;}

    void Set_fHintCO(bool _fHintCO) {fHintCO = _fHintCO;}
    void Set_fHintChild(bool _fHintChild) {fHintChild = _fHintChild;}

    Configuration();
    Configuration(const Configuration &cfg2);

    QVector2D tScreenToPlane(double X, double Y) const;

    QVector2D tScreenToPlane(const QVector2D &p) const;

    QVector2D randomPoint() const; // точка лежит в прямоугольнике b0 b1

    std::string info() const;

    int objectId() const { return 100 + id; }

    void initProto();

    double randomSotaR();

    void initState();

    void clone(const Configuration &cfg2);

    double reliefZ(double x, double y) const;

    double reliefZ(const QVector2D &p) const { return reliefZ(p.x(), p.y()); }

    QVector3D colorByH(double z) const;

    static QVector3D _V_(const QVector2D &p, double z = 0) { return QVector3D(p, z); }

    void _draw_scene(bool fDrawCfg3d) const;

    void draw(double t, bool fDrawCfg3d) const;

    static GLUquadric* Quadric();

    void draw3d(double t, double alpha, double beta);

	// --------------------------------------------------------------
	// Свойства конфигурации

    std::string getParamStr(const std::string &name) const;

    std::string getHeaderStr() const;

    std::string lastHint() const;

    double lifeTime() const;


	// --------------------------------------------------------------
	// Алгоритм

    void move(double t); // здесь убираем const

    void mutate(); // убираем const

    void removeNodes();

    void crossover(Configuration* cfg1, Configuration* cfg2); //должен быть алгоритм, который отбирает пару, а здесь для них уже обработка

	// %%%
	// Узлы связи

    double accessRate(const QVector3D &p) const;

    double accessRate(const QVector2D &p0) const
	{
        return accessRate(QVector3D(p0, 0));
	}

    double accessRate(const Trajectory &tr) const
	{
        return accessRate(tr.p0);
	}


    QVector2D nodeAccessGrad(double t, const Sota* node, double *s_ = nullptr) const;

    QVector2D accessGrad(const Sota *node, const QVector3D &p, double *y) const
	{
		return node->accessGradF(p, y);
	}

    QVector2D accessGrad(const Sota *node, const QVector2D &p0, double *y) const
	{
        return accessGrad(node, QVector3D(p0), y);
	}

    QVector2D accessGrad(const Sota *node, const Trajectory &tr, double t, double *y) const
	{
        return accessGrad(node, tr.location(t), y);
	}


    QVector2D accessGradByAllNodes(const QVector3D &p, double *y) const;

    QVector2D accessGradByAllNodes(const QVector2D &p0, double *y) const
	{
        return accessGradByAllNodes(QVector3D(p0), y);
	}

    int dyn_I(int j) const;

    HRecord & dyn_AR(int j) const
	{
		return _AR[dyn_I(j)]; // возвращает dyn_I(j) по счету элемент массива AR
	}

	// Геометрия
    QVector2D checkPoint(const QVector2D & p) const;

    QVector3D _P_(const QVector3D& p) { return QVector3D(p.x(), p.y(), 0); }
    QVector3D _P_(const QVector2D& p) { return QVector3D(p); }

	// ~~~
	// Анализ конфигурации
    double Weight() const { return W; }

	//вот тут высчитывается вес каждой конфигурации
    double CalcWeight_H();

    void updateResults(double t);

    void dropHints();

    ExportGeneralInfo PrepareExportGeneralData();

    ExportDetailedInfo PrepareExportDetailedInfo();
};
