// Файл для старой модели
#include "Configuration.h"
#include "MathUtil.h"
#include "Settings.h"

#include <ctime>

using namespace std;

#define GR ((sqrt(5)+1)/2.0)
#define K_GR ((0.5) / (GR))

void Configuration::clear()
{
    tStart = -1;
    //
    currentIndex = 0;
    fHintM = fHintCO = fHintChild = false;
    //
    nRelief = 0;
    reliefA.clear();
    reliefB.clear();
    reliefH.clear();

    Tr.clear();

    Soty.clear();

    // Динамические характеристики
    _refreshTime = 0;
    for (int j = 0; j < M0; j++) // это просто очищения архива исторических записей
    {
        _AR[j].clear();
    }

    W = 0;
}


Configuration::Configuration()
{
    clear();
}

Configuration::Configuration(const Configuration &cfg2)
{
    clear();
    clone(cfg2);
}

QVector2D Configuration::tScreenToPlane(double X, double Y) const
{
    return QVector2D(
        (1 - X) * b0.x() + X * b1.x(),
        (1 - Y) * b0.y() + Y * b1.y()
        );
}

QVector2D Configuration::tScreenToPlane(const QVector2D & p) const
{
    return tScreenToPlane(p.x(), p.y());
}

QVector2D Configuration::randomPoint() const // точка лежит в прямоугольнике b0 b1
{
    return QVector2D(MathUtil::random(b0.x(), b1.x()), MathUtil::random(b0.y(), b1.y()));
}


string Configuration::info() const
{
    return "Configuration " + to_string(id) + "\n";
}


void Configuration::initProto()
{
    b0 = QVector2D(Settings::Inst().Get().bArea.b0.x, Settings::Inst().Get().bArea.b0.y);
    b1 = QVector2D(Settings::Inst().Get().bArea.b1.x, Settings::Inst().Get().bArea.b1.y);

    if (Settings::Inst().Get().IsUseReliefRandomSeed)
        srand(Settings::Inst().Get().ReliefRandomSeed);
    else
        srand((unsigned)time(nullptr));

    nRelief = 12;
    reliefA.resize(nRelief);
    reliefB.resize(nRelief);
    reliefH.resize(nRelief);

//    decltype(Settings::Inst().Get().ReliefCoeffs)  ReliefCoeffs = Settings::Inst().Get().ReliefCoeffs;
    const auto & ReliefCoeffs = Settings::Inst().Get().ReliefCoeffs;

    for (int i = 0; i < 4; i++)
    {
        double A   = MathUtil::random(ReliefCoeffs.A_r1, ReliefCoeffs.A_r2);
        double phi = MathUtil::random(0, 2 * M_PI);

        reliefA[i] = A * cos(phi);
        reliefB[i] = A * sin(phi);
        reliefH[i] = MathUtil::random(150, 3300);
    }

    for (int i = 4; i < 8; i++)
    {
        double A   = MathUtil::random(0.0005, 0.002);
        double phi = MathUtil::random(0, 2 * M_PI);

        reliefA[i] = A * cos(phi);
        reliefB[i] = A * sin(phi);
        reliefH[i] = MathUtil::random(50, 250);
    }

    for (int i = 8; i < nRelief; i++)
    {
        double A   = MathUtil::random(0.004, 0.008);
        double phi = MathUtil::random(0, 2 * M_PI);

        reliefA[i] = A * cos(phi);
        reliefB[i] = A * sin(phi);
        reliefH[i] = MathUtil::random(75, 175);
    }

    srand((unsigned)time(0));

    // Маршруты
    if (Settings::Inst().Get().IsRandomTrajectories)  // Генерируем случаные траектории
    {
        for (size_t j = 0; j < Settings::Inst().Get().TrajectoryCount; j++)
        {
            double v = 1000 * MathUtil::random(1, 5) /* м/с */ / 3600; //скорость

            QVector2D p0 = randomPoint(), p1 = randomPoint();
            // цикл проверяет отсутствие очень коротких маршрутов
            while ((p1 - p0).length() < 1000)
                p1 = randomPoint();

            Tr.emplace_back(p0, p1, v);
        }
    }
    else // Используем траектории из файла настроек
    {
        for (const auto & tr : Settings::Inst().Get().Trajectories)
        {
            Tr.emplace_back(QVector2D(tr.p0.x, tr.p0.y), QVector2D(tr.p1.x, tr.p1.y), tr.v);
        }
    }

    // Узлы

    if (Settings::Inst().Get().IsRandomNodes)  // Генерируем случаные узлы
    {
        for (size_t j = 0; j < Settings::Inst().Get().NodeCount; j++)
        {
            // double v = 300 * MathUtil::random(1, 5);       
            double R = randomSotaR(); // это у нас радиус точек доступа
            QVector2D p0; // null vector
            Soty.emplace_back(p0, R);
        }
    }
    else
    {
        for (const auto & node : Settings::Inst().Get().Nodes)
        {
            Soty.emplace_back(QVector2D(), node.R);
        }
    }

    BuildAllReliefToGL(*this);
}

double Configuration::randomSotaR()
{
    return MathUtil::random(500, 1500);
}

void Configuration::initState()
{
    for (auto & sota : Soty)
    {
        sota.p0 = randomPoint();
    }
}

void Configuration::clone(const Configuration& cfg2) // полностью переписать на operator= ???
{
    b0 = cfg2.b0, b1 = cfg2.b1;
    //
    nRelief = cfg2.nRelief;
    for (int i = 0; i < nRelief; i++)
    {
        reliefA.push_back(cfg2.reliefA[i]);
        reliefB.push_back(cfg2.reliefB[i]);
        reliefH.push_back(cfg2.reliefH[i]);
    }

    Tr = cfg2.Tr;
    Soty = cfg2.Soty;
}

double Configuration::reliefZ(double x, double y) const
{
    double z = 0;
    for (int i = 0; i < nRelief; i++)
    {
        if (i >= 12)
            break;

        double v = i % 2 ? sin(reliefA[i] * x + reliefB[i] * y) : cos(reliefA[i] * x + reliefB[i] * y);
        if (i < 4 && false)
            v *= 0.88 + 0.12 * sin(0.33 * (reliefA[4+i] * x + reliefB[7-i] * y));

        z += (i < 4) ? reliefH[i] * pow(v, 24) :
             (i < 8) ? reliefH[i] * 0.5 * (1 + v) :
             reliefH[i] * pow(v, 8);
    }
    return z;
}


QVector3D Configuration::colorByH(double z) const
{ //цвет рельефа
    QVector3D cUpHill(1, 0, 0), cMiddle(1, 0.75, 0), cValley(0, 0.75, 0);
    double t = z / 1000;
    t = t / (1 + t);
    return (t < 0.5) ?
        (1 - 2*t) * cValley + 2*t * cMiddle :
        (2 - 2*t) * cMiddle + (2*t - 1) * cUpHill;
}

bool Configuration::IsReliefBuilt = false;
GLuint Configuration::ReliefCompileList = 0;
GLuint Configuration::Relief3dCompileList = 1;

void Configuration::BuildAllReliefToGL(const Configuration &_protoConf) // static метод
{
    if (IsReliefBuilt)
        throw runtime_error("Relief is already built into GL");

    ReliefCompileList = glGenLists(1);
    BuildReliefToGL(false, ReliefCompileList, _protoConf);

    Relief3dCompileList = glGenLists(1);
    BuildReliefToGL(true, Relief3dCompileList, _protoConf);

    IsReliefBuilt = true;
}

void Configuration::BuildReliefToGL(bool _fDrawCfg3d, GLuint _nList, const Configuration &_protoConf) // static метод
{
    const int M = 12;
    const double dx = 1.0 / M, dy = 1.0 / M;

    QVector3D points[M][M];
    QVector3D colors[M][M];

    // Начать список
    glNewList(_nList, GL_COMPILE);

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            QVector2D p = _protoConf.tScreenToPlane(j * dx, i * dy);
            auto z1 = _protoConf.reliefZ(p);

            points[i][j] = QVector3D(p.x(), p.y(), _fDrawCfg3d? K_GR*z1 : 0);
            colors[i][j] = _protoConf.colorByH(z1);
        }
    }
    //
    glBegin(GL_QUADS);
    for (int i = 0; i < M-1; i++)
    {
        for (int j = 0; j < M-1; j++)
        {
            QVector3D
                p0 = points[i][j],
                p1 = points[i][j+1],
                p2 = points[i+1][j+1],
                p3 = points[i+1][j];

            QVector3D
                c0 = colors[i][j],
                c1 = colors[i][j+1],
                c2 = colors[i+1][j+1],
                c3 = colors[i+1][j];

            glColor3f(c0.x(), c0.y(), c0.z());
            glVertex3f(p0.x(), p0.y(), p0.z());

            glColor3f(c1.x(), c1.y(), c1.z());
            glVertex3f(p1.x(), p1.y(), p1.z());

            glColor3f(c2.x(), c2.y(), c2.z());
            glVertex3f(p2.x(), p2.y(), p2.z());

            glColor3f(c3.x(), c3.y(), c3.z());
            glVertex3f(p3.x(), p3.y(), p3.z());
        }
    }
    glEnd();


    glEndList(); // закончить список
}

void Configuration::_draw_scene(bool fDrawCfg3d) const
{
    double gSx = b1.x() - b0.x();
    double gSy = b1.y() - b0.y();

    glPushMatrix();

    if (!fDrawCfg3d)
    {
        glTranslated(b0.x(), b0.y(), 0);
        glScaled(1 / gSx, 1 / gSy, 1);
    }
    else // 3d
    {
        glTranslated(-0.5*gSx, -0.5*gSy, 0);
    }

    // Рельеф
    {

        if (fDrawCfg3d)
            glCallList(Relief3dCompileList);
        else
            glCallList(ReliefCompileList);

        // Отображение атрибутов конфигурации
        //
        string text = getHeaderStr();
        glColor3f(0.8, 1, 1);
        //GrUtil::drawText(0, gSy, text);

        // Маршруты
        //
        double _h0 = fDrawCfg3d? 100 : 0;

        for (const auto & tr : Tr)
        {
            glColor3d(0.3, 0.3, 1);
            //
            double z0 = fDrawCfg3d ? K_GR*reliefZ(tr.p0) : 0;
            double z1 = fDrawCfg3d ? K_GR*reliefZ(tr.p1) : 0;
            //
            const int M = 24;
            glBegin(GL_LINES);
            for (int j = 0; j < M; j++)
            {
                double t1 = 1.0 * j / M, t2 = 1.0 * (j + 1) / M;
                auto q1 = tr.p0 * (1 - t1) + tr.p1 * t1;
                auto q2 = tr.p0 * (1 - t2) + tr.p1 * t2;
                auto _z1 = fDrawCfg3d ? K_GR * reliefZ(q1) + _h0 : 0;
                auto _z2 = fDrawCfg3d ? K_GR * reliefZ(q2) + _h0 : 0;

                auto __v1 = _V_(q1, _z1 + 0.01);
                glVertex3f(__v1.x(), __v1.y(), __v1.z());

                auto __v2 = _V_(q2, _z2 + 0.01);
                glVertex3f(__v2.x(), __v2.y(), __v2.z());

                //glVertex3dv(_V_(tr->p0 * (1 - t1) + tr->p1 * t1, z0 * (1 - t1) + z1 * t1 + 0.01 + _h0).data());
                //glVertex3dv(_V_(tr->p0 * (1 - t2) + tr->p1 * t2, z0 * (1 - t2) + z1 * t2 + 0.01 + _h0).data());
            }
            glEnd();
            //
            glColor3d(0.3, 0.3, 1);
            //
            glPushMatrix();
            glTranslated(tr.p0.x(), tr.p0.y(), z0 + 0.01 + _h0);
            if (fDrawCfg3d)
                gluSphere(Quadric(), 50, 12, 12);
            else
                gluDisk(Quadric(), 0, 50, 12, 12);
            glPopMatrix();
            //
            glPushMatrix();
            glTranslated(tr.p1.x(), tr.p1.y(), z1 + 0.01 + _h0);
            if (fDrawCfg3d)
                gluSphere(Quadric(), 50, 12, 12);
            else
                gluDisk(Quadric(), 0, 50, 12, 12);
            glPopMatrix();
            //
            QVector2D p = tr.location(t);
            double t = MathUtil::clamp(accessRate(p), 1);
            t = t * t;
            glColor3d(t, 1 - t, 1 - t);
            //
            glPushMatrix();
            auto z = fDrawCfg3d ? K_GR * reliefZ(p) + _h0 + 1.2 : 0;
            glTranslated(p.x(), p.y(), z + 0.01);
            if (fDrawCfg3d)
                gluSphere(Quadric(), 75, 12, 12);
            else
                gluDisk(Quadric(), 0, 75, 12, 12);
            glPopMatrix();
        }


        // Узлы связи
        //
        for (const auto & node : Soty)
        {
            glColor3d(1, 0.8, 0);
            //
            glPushMatrix();
            double _z0 = fDrawCfg3d ? K_GR * reliefZ(node.p0) + _h0 : 0;
            glTranslated(node.p0.x(), node.p0.y(), _z0 + 0.02);
            if (fDrawCfg3d)
                gluSphere(Quadric(), 75, 12, 12);
            else
                gluDisk(Quadric(), 0, 75, 12, 12);
            glPopMatrix();
            //
            glColor3d(1, 0, 0);
            //
            glPushMatrix();
            glTranslated(node.p0.x(), node.p0.y(), 0.02);
            int M = 24; // nCView == 1 ? 36 : 24;
            const double da = 2 * M_PI / M;
            double R = node.R;
            double _nz0 = fDrawCfg3d ? _h0 + 3 + K_GR * reliefZ(node.p0) : 0;
            glBegin(GL_LINES);
            for (int i = 0; i < M; i++)
            {
                double a0 = i * da, a1 = a0 + da;
                glVertex3d(R * cos(a0), R * sin(a0), _nz0 + 0.02);
                glVertex3d(R * cos(a1), R * sin(a1), _nz0 + 0.02);
            }
            glEnd();
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void Configuration::draw([[maybe_unused]] double t, bool fDrawCfg3d) const
{
    glColor3d(0, 0, 0.75);
    glBegin(GL_LINE_STRIP);
        glVertex2d(0, 0);
        glVertex2d(1, 0);
        glVertex2d(1, 1);
        glVertex2d(0, 1);
        glVertex2d(0, 0);
    glEnd();

    //
    // Атрибуты
    //
    //glColor3d(0, 0, 0.8);
    //glBegin(GL_QUADS);
    //glVertex3d(0.1, -0.2, -1);
    //glVertex3d(0.5, -0.2, -1);
    //glVertex3d(0.5, 1.106, -1);
    //glVertex3d(0.1, 1.106, -1);
    //glEnd();
    //
    //
    // --> Физические координаты
    if (!fDrawCfg3d)
    {
        _draw_scene(false);
    }

    // Атрибуты
    //
    glColor3d(0.3, 0.3, 1);

    glBegin(GL_QUADS);
        glVertex3d(0.04, 0.90, 0);
        glVertex3d(0.14, 0.90, 0);
        glVertex3d(0.14, 0.97, 0);
        glVertex3d(0.04, 0.97, 0);
    glEnd();
    //
    glColor3d(1, 0.7, 0);
    //GrUtil::drawText(0.06, 0.92, getParamStr("id"));
}

void Configuration::draw3d([[maybe_unused]] double t, double alpha, double beta)
{
    const double R0 = 5500;
    double _a = M_PI * alpha / 180, _b = M_PI * beta / 180;
    //
    glPushMatrix();
    //
    gluLookAt(R0*cos(_a)*cos(_b), R0*sin(_a)*cos(_b), R0*sin(_b), 0, 0, 0, 0, 0, 1);
    // glRotated(alpha, 0, 0, 1);
    //
    _draw_scene(true/*3d*/);
    //
    glTranslated(0, 0, -250);
    glScaled(3000, 3000, 3000);
    glColor3d(0, 0, 0.2);

    glBegin(GL_QUADS);
        glVertex2d(-0.5, -0.5);
        glVertex2d(0.5, -0.5);
        glVertex2d(0.5, 0.5);
        glVertex2d(-0.5, 0.5);
    glEnd();

    glPopMatrix();
}


// --------------------------------------------------------------
// Свойства конфигурации

string Configuration::getParamStr(const string &name) const
{
    // вот тут выводится вес каждой конфигурации
    if (name == "W-text")
    {
        char s[256];
        sprintf(s, "W = %.2lf (%.2lf)", (double)W, W0);
        return s;
    }
    else if (name == "lt-text")
    {
        char s[256];
        sprintf(s, "lt = %.2lf h", (double)(lifeTime()/3600.0));
        return s;
    }
    else if (name == "id")
    {
        char s[256];
        sprintf(s, "%d", (int)(id));
        return s;
    }
    else if (name == "current-index")
    {
        char s[256];
        sprintf(s, "%d", (int)(currentIndex));
        return s;
    }
    else if (name == "header")
    {
        string hint = lastHint();
        return getParamStr("W-text") + ", " + getParamStr("lt-text")
            + (hint.length() > 0 ? ", " + hint : "")
            + ", i:" + getParamStr("current-index");
    }

    return "";
}

string Configuration::getHeaderStr() const
{
    return getParamStr("header");
}

string Configuration::lastHint() const
{
    string hint = "";
    if (fHintM) hint += string(hint.length() > 0 ? "," : "") + "M";
    if (fHintCO) hint += string(hint.length() > 0 ? "," : "") + "CO";
    if (fHintChild) hint += string(hint.length() > 0 ? "," : "") + "child";
    if (lastHintText.length() > 0) hint += string(hint.length() > 0 ? "/" : "") + lastHintText;
    return hint;
}

double Configuration::lifeTime() const
{
    return t - tStart;
}

// --------------------------------------------------------------
// Алгоритм

void Configuration::move(double t)
{
    this->t = t;
//    Configuration* me = const_cast<Configuration*>(this);
    Configuration *me = this;

    if (this->tStart < 0)
        me->tStart = t;

    QVector2D *a = new QVector2D[Soty.size()];

    double s = 0;
    for (size_t k = 0; k < Soty.size(); k++)
    {
        const Sota* node = &Soty[k];

        double s1 = 0;
        QVector2D u = nodeAccessGrad(t, node, &s1);
        a[k] = u;
        s += s1;
    }
    //
    if (s < 0.0000001)
        return;
    //
    double rho = 1 / s;
    for (size_t k = 0; k < Soty.size(); k++)
    {
        QVector2D u = rho * a[k];
        u = -150 * u;
        a[k] = u;
    }

    // Градиенты отталкивания
    //
    for (size_t k = 0; k < Soty.size(); k++)
    {
        const Sota &node = Soty[k];

        QVector2D u;
        for (size_t k2 = 0; k2 < Soty.size(); k2++)
        {
            if (k2 == k)
                continue;

            const Sota &node2 = Soty[k2];

            QVector2D dp = node.p0 - node2.p0;
            double len = dp.length();
            double b = 0.4 * exp(- 0.004 * len) * (100/(1 + 0.25*len));
            u += b * dp;
        }
        a[k] += u;
    }

    // Градиент, контролирующий высоту
    //
    for (size_t k = 0; k < Soty.size(); k++)
    {
        const Sota &node = Soty[k];

        QVector2D u = a[k];
        QVector2D u1 = u.normalized();
        QVector2D p = node.p0;

        double m = 8, h0 = reliefZ(p);
        double dz0 = (
            (reliefZ(p + u1) - h0) +
            (reliefZ(p + m*u1) - h0)/m +
            (reliefZ(p + m*m*u1) - h0)/(m*m)
            ) / 3;
        QVector2D du = (- pow(h0 / 7500, 2) * (dz0 / 0.5)) * u;

        a[k] += du;
    }

    // Цикл формирования шага для каждого узла
    //
    for (size_t k = 0; k < Soty.size(); k++)
    {
        Sota &node = Soty[k];

        QVector2D u = a[k];

        const double a1 = 5;
        double dx = MathUtil::random(-a1, a1), dy = MathUtil::random(-a1, a1); // шаг у нас тут от одного до 5, а дальше что происходит?
        u += QVector2D(dx, dy);
        //
        node.p0 = checkPoint(node.p0 + u);
    }
}

void Configuration::mutate()
{
    if (MathUtil::gaussianRandomDecision(2, currentIndex))
        return;

    // mutating
    //
    bool fH = false;

    for (auto & node : Soty)  // мы пробегаем по всем точкам доступа и для каждой может произойти мутация
    {

        if (MathUtil::randomDecision(Settings::Inst().Get().pMutation)) //вероятность мутации, технически могут мутировать и две, но шансов у нас этого практически нет
        {
            node.p0 = randomPoint(); // перемещение в случайную точку и там меняются все координаты
            fH = true;
            //break;//тогда у нас только одна мутирует
        }
    }
    //
    if (fH)
    {
        fHintM = true;
    }
}

void Configuration::removeNodes()
{
    Soty.clear();
}

void Configuration::crossover(Configuration* cfg1, Configuration* cfg2) //должен быть алгоритм, который отбирает пару, а здесь для них уже обработка
{
    //
    for (size_t j = 0; j < Soty.size(); j++)
    {
        const Sota &node = cfg1->Soty[j];

        for (size_t i = 0; i < Soty.size(); i++)
        {
            if (i != j)
                continue;

            const Sota &node2 = cfg2->Soty[i];

            Sota &node1 = Soty[i];

            // Сравнение характеристик...
            //
            if ((node.p0 - node2.p0).length() < 1.2000)
            {
                double t = 0.2 + 0.1 * (rand() % 7);
                node1.p0 = t * node.p0 + (1 - t)*node2.p0; //  node->R); // t*node->R + (1 - t)*node2->R
            }
            else
            {
                if (rand() % 2)
                    node1.p0 = node.p0;
                else
                    node1.p0 = node2.p0;
            }
        }
    }
}

// Узлы связи

double Configuration::accessRate(const QVector3D & p) const
{
    double s = 0;

    for (const auto & node : Soty)
    {
        s += node.accessRateF(p);
    }
    return s / sqrt(Soty.size());
}

QVector2D Configuration::nodeAccessGrad(double t, const Sota* node, double* s_ /*= nullptr*/) const
{
    if (!node)
        return QVector2D();

    double s = 0;
    QVector2D u;

    for (const auto & tr : Tr)
    {
        double y;
        u += accessGrad(node, tr, t, &y);
        s += y;
    }


    if (s <= 0.0000001)
    {
        if (s_) *s_ = s;
        return QVector2D();
    }
    *s_ = s;
    return u;
}

QVector2D Configuration::accessGradByAllNodes(const QVector3D &p, double* y) const
{
    QVector2D v;

    for (const auto & node : Soty)
    {
        v += node.accessGradF(p, y);
    }
    return v / sqrt(Soty.size());
}

int Configuration::dyn_I(int j) const
{
    int i = phase0 + j;
    while (i < 0)
        i += M0;
    return i % M0;
}


// Геометрия

QVector2D Configuration::checkPoint(const QVector2D & p) const
{
    return QVector2D(
            MathUtil::clamp(p.x(), b0.x(), b1.x()),
            MathUtil::clamp(p.y(), b0.y(), b1.y())
        );
}


// ~~~
// Анализ конфигурации

//вот тут высчитывается вес каждой конфигурации
double Configuration::CalcWeight_H()
{
    //if (dyn_AR(-1).W < 0.1*W)
    //	return W;
    //
    const int n = HIS_N;
    double q = HIS_MULT; // HIS_N = 80 - это количество швгов в прошлое  HIS_MULT (1 - 1.0/HIS_N) - это коэффициент с которым мы складываем
    double hW = 0, S = 0;
    int j;
    for (j = 0; j < n && dyn_AR(-j).W > 0.1*W; j++) // -j значит что мы идем в прошлое ... судя по всему это && dyn_AR(-j).W > 0.1*W просто какая-то проверка корректности
        hW += q * dyn_AR(-j).W, S += q, q *= HIS_MULT; // мы берем текущее с весом 1, предыдущее с меньшим весом, предпред идущее с еще меньшим весом и так всего 80 шагом назад учитываем q это динамическая переменная, мы каждый раз умножаем ее на his mult..

    if (j == 0)
        W0 = W;

    W0 = hW / S;

    return W0;
}

#define QT_NO_VECTOR2D

void Configuration::updateResults(double t)
{
    double A = 0, s = 0;

    for (auto & tr : Tr) // это цикл по людям nTr - это количество траекторий
    {
        double y = 0;
        double r, rMin = numeric_limits<double>::max();

        for (const auto & node : Soty)//это цикл по точкам доступа. сейчас складывается сумма между всеми точками доступа, возможно, стоит использоать максимум... если мы возьмем lp -
        {
            QVector2D p1 = tr.location(t);// по длине вектора между человеком и точкой доступа

            //y += node.accessRateF(p1); //используется расстояние от агента в точке p1 до узла node, мы двигаемся от одной точке доступа

            y += node.accessRateF({p1, 0}); //используется расстояние от агента в точке p1 до узла node, мы двигаемся от одной точке доступа


            // к другой, для каждой точки рассчитываем уровень доступа к сети и все это суммируем в y

            r = p1.distanceToPoint(node.p0);
            if (r < rMin)
                rMin = r;
        }
        tr.SignalPower = y;
        tr.DistToNearestNode = rMin;
        s += y; // а здесь мы точно так же двигаемся от одного туриста к следующему и накапливаем s как сумму y
    }
    //
    const double k1 = 5, k2 = 0.01;
    A += k1 * s; // мы здесь допускаем добавление новых коэффициентов
    //
    W = k2 * A / (1 + k2 * A); // мы посчитали вес в кучу всех значений и вот этой функцией мы преобразуем его в значение от 0 до примерно 0,7
    //коэффициент k1 нормирует, а k2 переводит в шкалу 0,1, 5 взяли просто для удобства (см картинку на почте)

    // Сохранение  конфигурации



    bool fUpdate = false;
    if (t > _refreshTime + 0.25 * 3600)
    {
        _refreshTime = t;
        fUpdate = true;
    }

    if (fUpdate)
    {
        phase0++;
        HRecord &record = dyn_AR(0);
        record.clear();
        record.W = W;
    }


    CalcWeight_H();
}

void Configuration::dropHints()
{
    fHintM = fHintCO = fHintChild = false;
    lastHintText = "";
}

GLUquadric* Configuration::Quadric()
{
    static GLUquadric* q = nullptr;
    if (!q) q = gluNewQuadric();
    return q;
}
//------------------------------------------------------------

ExportGeneralInfo Configuration::PrepareExportGeneralData()
{
    ExportGeneralInfo res;

    res.w  = W;
    res.w0 = W0;

    //res.id = id;
    res.id = currentIndex;

//    bool fHintM;
//    bool fHintCO, fHintChild;

    if (!fHintM && !fHintCO && !fHintChild)
        res.LastOp = ":new";
    else
    {
        if (fHintM)
            res.LastOp = ":Mutated";
        if (fHintCO)
            res.LastOp += ":CO";
        if (fHintChild)
            res.LastOp += ":Child";
    }

    return res;
}
//------------------------------------------------------------

ExportDetailedInfo Configuration::PrepareExportDetailedInfo()
{
    ExportDetailedInfo res;

    res.PeopleCoords.reserve(Tr.size());
    res.SignalPowers.reserve(Tr.size());
    res.DistToNearestNode.reserve(Tr.size());
    for (const auto & tr : Tr)
    {
        res.PeopleCoords.emplace_back(tr.location(t));

        res.SignalPowers.emplace_back(tr.SignalPower);

        res.DistToNearestNode.emplace_back(tr.DistToNearestNode);
    }

    res.NodeCoords.reserve(Soty.size());
    for (const auto & node : Soty)
    {
        res.NodeCoords.emplace_back(node.p0);
    }

    return res;
}
//------------------------------------------------------------








