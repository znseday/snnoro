#include "Model.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Settings.h"

using namespace std;


void Model::init()
{
    isInited = true;

    t = 0;
    //
    timeK = 125;
    //
    alpha = 33, beta = 60; // угля для 3d просмотра
    //
    uiActiveElement = 0;
}


Model::Model()
{
    init();
}


QVector3D Model::cViewPos(int k) const
{
    if (nCView == 1)
    {
        return _P_(0.1, 0.1);
    }
    //
    if (k == 0) return _P_(0.05, 0.55);
    else if (k == 1) return _P_(0.55, 0.55);
    else if (k == 2) return _P_(0.05, 0.05);
    // else if (k == 3)
        return _P_(0.55, 0.05);
}


void Model::getSpecialViewParams(int k, double &x0, double &y0, double &w, double &h) const
{
    QVector3D p0 = cViewPos(k);
    x0 = p0.x();
    y0 = p0.y();
    //if (nCView == 1)
    //	*x0 = 0.1, *y0 = 0.1;
    w = getSpecialViewW();
    h = getSpecialViewW(); // W vs H ?
}

void Model::draw() const
{
//    cout << __PRETTY_FUNCTION__ << endl;

    // конфигурации
    //
    for (int k = 0; k < nCView; k++)
    {
        QVector3D p0 = cViewPos(k);
        glPushMatrix();
        glTranslated(p0.x(), p0.y(), p0.z());
        double m = getSpecialViewW();
        //
        if (uiActiveElement == 10 + k)
        {
            glColor3d(1, 0.4, 0);
            glBegin(GL_LINE_STRIP);
                glVertex2d(-0.01, -0.01);
                glVertex2d(m + 0.01, -0.01);
                glVertex2d(m + 0.01, m + 0.01);
                glVertex2d(-0.01, m + 0.01);
                glVertex2d(-0.01, -0.01);
            glEnd();
        }
        //
        glScaled(m, m, 1);
//        auto cfg = population[cViewIds[k]];

        auto _id = cViewIds[k];

        auto cfg = population.at(_id);

        cfg->draw(t, fDrawCfg3d);
        glPopMatrix();
    }


}

void Model::drawSpecial(int k) const
{
    if (fDrawCfg3d)
    {
        auto cfg = population[cViewIds[k]];
        cfg->draw3d(t, alpha, beta);
        //glutPostRedisplay();
        emit signalRepaint();
    }
}


// ----
// ”правл¤ющие параметры модели

std::string Model::getParamStr(const std::string &name) const
{
    if (name == "model-params-0")
    {
        char text[1024];
        sprintf(text, "tK = %.3lf", (double)timeK);
        return text;
    }
    if (name == "model-info-text")
    {
        char text[1024];
        sprintf(text, "Model parameters\n \ntK = %.3lf", (double)timeK);
        return text;
    }
    return "";
}

//std::string Model::info() const
//{
//    return getParamStr("model-info-text");
//}

//int Model::objectId() const
//{
//    return 1;
//}


// ----
// Интерактивность

bool Model::onMouseMove(double x, double y)
{
    int a = 0;

    // конфигурации
    //
    for (int k = 0; k < nCView; k++)
    {
        QVector3D p0 = cViewPos(k);
        double m = getSpecialViewW();
        if (x >= p0.x() && x <= p0.x() + m && y >= p0.y() && y <= p0.y() + m)
        {
            a = 10 + k;
            break;
        }
    }
    //
    bool update = false;
    if (a != uiActiveElement)
        update = true;
    //
    uiActiveElement = a;
    //
    return update;
}

bool Model::onMouseClick([[maybe_unused]] double x, [[maybe_unused]] double y)
{
    //onMouseMove(x, y);

    cout << __PRETTY_FUNCTION__ << endl;

    //
    return true;
}


// ----
// ћатематическа¤ модель

void Model::dynamics()
{
    algStep();

//    emit signalSendData(population.front()->GetId());
//    emit signalSendData(QString().setNum(population.size()) + "\n" + QString().setNum(population.front()->lifeTime()));

    //QString lt = QString().setNum(population.front()->lifeTime(), 'f', 10);
    //cout << "lt = " << lt.toStdString() << endl;
    //emit signalSendData(lt);

    static size_t n_step = 0;

    if (n_step % 58 == 0)
    {
        ExportGeneralData generalData;
        generalData.Time = physicalTime();
        generalData.GeneralInfos.reserve(4);

        ExportDetailedInfo detailedInfo;

        for (size_t i = 0; i < 4; i++)
        {
            generalData.GeneralInfos.emplace_back(population[i]->PrepareExportGeneralData());
            generalData.GeneralInfos[i].id = pA[i];

            detailedInfo = population[i]->PrepareExportDetailedInfo();
            detailedInfo.Time = generalData.Time;
            detailedInfo.nConfig = generalData.GeneralInfos[i].id;

            //emit signalSendDetailedInfo(detailedInfo);
            ExportAllDetailedInfo[i].emplace_back(detailedInfo);
        }

        //emit signalSendGeneralData(generalData);
        ExportAllGeneralData.emplace_back(generalData);
    }

    n_step++;
}

void Model::analyze() const
{
    static double _t = 0;
    bool fRefresh = false;
    double _t1 = physicalTime();
    if (_t1 > _t + 4.9)
    {
        fRefresh = true;
        _t = _t1;
    }
    //
    for (size_t j = 0; j < population.size(); j++)
    {
        population.at(j)->updateResults(t);
    }
    //
    if (fRefresh)
    {
        sortConfiguration();
        for (size_t j = 0; j < population.size(); j++)
        {
            population.at(j)->dropHints();
        }
    }
}

void Model::sortConfiguration() const
{
    int n = population.size();
    for (int r = n-1; r >= 1; r--)
    {
        for (int k = 0; k <= r - 1; k++)//сортировка конфигурации,  –а это индекс.
        {
            double w1 = population[pA[k]]->Weight(), w2 = population[pA[k + 1]]->Weight();
            if (w1 > w2)
            {
                int t = pA[k];
                pA[k] = pA[k + 1];
                pA[k + 1] = t;
            }
        }
    }
    // Поменять на std::sort ???

    //
    int t;
    for (size_t k = 0; k < population.size() / 2; k++) // Что это?
    {
        t = pA[population.size()-1-k];
        pA[population.size() - 1 - k] = pA[k];
        pA[k] = t;
    }
    //
    for (int k = 0; k < (int)population.size(); k++)
    {
        population[pA[k]]->SetCurrentIndex(k);
    }
    //
    for (int k = 0; k < nCView; k++)
    {
        cViewIds[k] = pA[k];
    }
}

// ----
// Динамическая структура алгоритма

void Model::start()
{
    nCView = 4; // это сколько отображается конфигураций
    for (int k = 0; k < nCView; k++)
    {
        cViewIds.push_back(k);
    }
    //

    ExportAllDetailedInfo.resize(4);

    createPopulation();
}

void Model::createPopulation() // здесь создаетс¤ начальна¤ попул¤ци¤ конфигураций
{
    protoCfg = new Configuration();
    protoCfg->initProto();
    //
    for (size_t j = 0; j < Settings::Inst().Get().Count; j++)
    {
        Configuration* cfg = new Configuration(*protoCfg);
        cfg->initState();
//        cfg->id = j + 1;
        cfg->SetId(j+1);

        population.push_back(cfg);
        pA.push_back(j);
    }
}


void Model::algStep()
{
    timeStep();

    algMove();

    //algMutate();

    //if (MathUtil::randomDecision(Settings::Inst().Get().pCrossover))
    //    algCrossover();

   // algClearAndRenew();
    //
    analyze();
}


void Model::timeStep()
{
    double dt;
    [[maybe_unused]] double t1 = physicalTime(&dt);

    //cout << "timeStep: dt = " << dt  << endl;

    tPrev = t;
    t += timeK * dt;
}

void Model::algMove()
{
    int nCfg = (int)population.size();
    for (int j = 0; j < nCfg; j++)
    {
        Configuration *cfg = population[j];
        cfg->move(t);
    }
}

void Model::algMutate()
{
    Configuration* child = new Configuration(*protoCfg);
    child->initState();
    Configuration* old1 = population[0];
    population[0] = child;
    deleteConfig(old1);

    int nCfg = (int)population.size();
    for (int j = 0; j < nCfg; j++)
    {
        Configuration* cfg = population[j];
        cfg->mutate();
    }
}

void Model::algCrossover()
{
    int n = (int)population.size();
    int i = rand() % n, j = rand() % n;//берем два случайных индекса
    if (i == j) return;//одинаковые не берем - ВЛИЯЕТ НА ВЕРОТНОСТЬ КРОССОВЕРА
    double Wmax = population[pA[0]]->Weight();//берем конфигурацию с максимальном весе, он в 0-м экз, так как они отсортированы
    if (Wmax < 0.05)
        return; //одинаковые не берем - ВЛИЯЕТ НА ВЕРОТНОСТЬ КРОССОВЕРА
    Configuration *cfg1 = population[i], *cfg2 = population[j]; //берем конфигурации с выбранными случайными номерами
    if (cfg1->Weight() < 0.75 * Wmax && cfg2->Weight() < 0.75 * Wmax)//вес хот¤ бы одной конфигурации должен быть 3/4 от максимума
        return; //одинаковые не берем - ВЛИЯЕТ НА ВЕРОТНОСТЬ КРОССОВЕРА
    //
    Configuration* child = new Configuration(*protoCfg); //создаем случайную конфигурацию стандартным образом, как при запуске, по шаблону (protocfg)
    ////child->initState();
    Configuration* old1 = population[n - 1]; //помещаем в конец, удал¤¤ самую плохую
    population[n - 1] = child;
    deleteConfig(old1);
    //
    child->crossover(cfg1, cfg2); //должен произвести редакцию, унаследовав хар-ки родителей, но сейчас оно не работает, просто создаетс¤ случайна¤ конфигураци¤
    //
    child->Set_fHintChild(true);
    cfg1->Set_fHintCO(true);
    cfg2->Set_fHintCO(true);
}

void Model::algClearAndRenew()
{
    if (!MathUtil::randomDecision(0.012))
        return;

    //
    int n = (int)population.size();
    auto W = population[n - 1]->Weight();
    if (W > 0 && W < 0.25)
    {
        bool f = MathUtil::randomDecision(0.15);
        auto W0 = population[0]->Weight();
        if (W0 > 0)
        {
            if (W0 > W)
                if (W > 0.8 * W0)
                    f = MathUtil::randomDecision(0.15);
        }
        if (f)
        {
            Configuration* child = new Configuration(*protoCfg);
            child->initState();
            Configuration* old1 = population[n - 1];
            population[n - 1] = child;
            deleteConfig(old1);
        }
    }
}

void Model::deleteConfig(Configuration* old1)
{
    delete old1; // !
}

Model & Model::Instance()  // Singletone
{
    static Model s_Model; // Срабатывает один раз в жизни
    return s_Model;       // потом постоянно возвращаем ссылку на этот объект
}

