#include "Settings.h"
#include <stdexcept>
//#include <iostream>

#include <QJsonArray>
#include <QFile>
#include <QDebug>

using namespace std;

void Settings::ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError)
{
    qDebug() << "Cnf.Name = " << Cnf.Name.c_str();

    if ( !_jsonObject["Configuration"].isObject() )
    {
        qDebug() << _parseError.errorString(); return;
    }

    const QJsonObject &configObject = _jsonObject["Configuration"].toObject();

    qDebug() << "Keys of Configuration";
    for (auto iter = configObject.begin(); iter != configObject.end(); ++iter)
    {
        qDebug() << iter.key();
    }

    Cnf.Count = configObject["Count"].toInt(0);

    Cnf.bArea.b0.x = configObject["b0.x"].toDouble(-1);
    Cnf.bArea.b0.y = configObject["b0.y"].toDouble(-1);
    Cnf.bArea.b1.x = configObject["b1.x"].toDouble(-1);
    Cnf.bArea.b1.y = configObject["b1.y"].toDouble(-1);

    Cnf.IsUseReliefRandomSeed = configObject["IsUseReliefRandomSeed"].toBool(false);
    Cnf.ReliefRandomSeed = configObject["ReliefRandomSeed"].toInt(200);

    const QJsonObject &reliefObject = configObject["ReliefCoeffs"].toObject();

    Cnf.ReliefCoeffs.A_r1 = reliefObject["A_r1"].toDouble(-1);
    Cnf.ReliefCoeffs.A_r2 = reliefObject["A_r2"].toDouble(-1);

    Cnf.TrajectoryCount = configObject["TrajectoryCount"].toInt(0);
    Cnf.IsRandomTrajectories = configObject["IsRandomTrajectories"].toBool(true);

    const QJsonArray &trArray = configObject["Trajectories"].toArray();
    for (auto it = trArray.begin(); it != trArray.end();  ++it)
    {
//        qDebug()<<(*iter).toString();
        const QJsonObject &trObject = it->toObject();
        ConfigurationSettings::MyTr temp;
        temp.p0.x = trObject["p0.x"].toDouble(-1);
        temp.p0.y = trObject["p0.y"].toDouble(-1);
        temp.p1.x = trObject["p1.x"].toDouble(-1);
        temp.p1.y = trObject["p1.y"].toDouble(-1);
        temp.v    = trObject["v"].toDouble(-1);
        Cnf.Trajectories.emplace_back(temp);
    }
    if (!Cnf.IsRandomTrajectories && Cnf.TrajectoryCount != Cnf.Trajectories.size())
    {
        throw std::runtime_error("Cnf.TrajectoryCount != Cnf.Trajectories.size()");
    }

    Cnf.NodeCount = configObject["NodeCount"].toInt(0);
    Cnf.IsRandomNodes = configObject["IsRandomNodes"].toBool(true);
    const QJsonArray &nodesArray = configObject["Nodes"].toArray();
    for (auto it = nodesArray.begin(); it != nodesArray.end();  ++it)
    {
//        qDebug()<<(*iter).toString();
        const QJsonObject &nodeObject = it->toObject();
        ConfigurationSettings::MyNode temp;
        temp.R = nodeObject["R"].toDouble(-1);
        Cnf.Nodes.emplace_back(temp);
    }
    if (!Cnf.IsRandomNodes && Cnf.NodeCount != Cnf.Nodes.size())
    {
        throw std::runtime_error("Cnf.NodeCount != Cnf.Nodes.size()");
    }

    Cnf.pMutation  = configObject["pMutation"].toDouble(-1);
    Cnf.pCrossover = configObject["pCrossover"].toDouble(-1);

    Cnf.TargetCostFunction = configObject["TargetCostFunction"].toString().toStdString();
}
//----------------------------------------------------------

bool Settings::LoadFromJson(const std::string &fn)
{
    if (IsLoaded)
    {
        throw std::runtime_error("Settings is already Loaded");
        return false;
    }

    QFile json(fn.c_str());
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
                Cnf.Name = fn.substr(0, fn.rfind('.'));
                ParseJson(jsonDoc.object(), parseError);
            }
        }
        else
        {
            qDebug() << parseError.errorString();
        }

    }
    else
    {
        qDebug() << "json file not open";
    }


    IsLoaded = true;
    return true;
}
//----------------------------------------------------------

Settings & Settings::Inst()  // Singletone
{
    static Settings s_Settings; // Срабатывает один раз в жизни
    return s_Settings;          // потом постоянно возвращаем ссылку на этот объект
}
//----------------------------------------------------------

ostream & operator<<(ostream & s, const Settings &ob)
{
    s << boolalpha;

    s << "Configuration:" << endl;
    s << "\tName = " << ob.Cnf.Name << endl;

    s << "\tCount = " << ob.Cnf.Count << endl;
    s << "\tbArea:" << endl;
    s << "\t\tb0 = "  << ob.Cnf.bArea.b0 << endl;
    s << "\t\tb1 = "  << ob.Cnf.bArea.b1 << endl;

    s << "\tIsUseReliefRandomSeed = "  << ob.Cnf.IsUseReliefRandomSeed << endl;
    s << "\tReliefRandomSeed = "  << ob.Cnf.ReliefRandomSeed << endl;

    s << "\tReliefCoeffs:" << endl;
    s << "\t\tA_r1 = "  << ob.Cnf.ReliefCoeffs.A_r1 << endl;
    s << "\t\tA_r2 = "  << ob.Cnf.ReliefCoeffs.A_r2 << endl;

    s << "\tTrajectoryCount = "  << ob.Cnf.TrajectoryCount << endl;
    s << "\tIsRandomTrajectories = "  << ob.Cnf.IsRandomTrajectories << endl;

    s << "\tTrajectories:" << endl;
    for (const auto & tr : ob.Cnf.Trajectories)
    {
        s << "\t\tp0 = " << tr.p0 << endl;
        s << "\t\tp1 = " << tr.p1 << endl;
        s << "\t\tv = "  << tr.v << endl;
    }


    s << "\tNodeCount = "  << ob.Cnf.NodeCount << endl;
    s << "\tIsRandomNodes = "  << ob.Cnf.IsRandomNodes << endl;

    s << "\tNodes:" << endl;
    for (const auto & node : ob.Cnf.Nodes)
    {
        s << "\t\tR = " << node.R << endl;
    }

    s << "\tpMutation = "  << ob.Cnf.pMutation << endl;
    s << "\tpCrossover = "  << ob.Cnf.pCrossover << endl;

    s << "\tTargetCostFunction = "  << ob.Cnf.TargetCostFunction << endl;

    return s;
}

