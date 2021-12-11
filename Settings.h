#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <iostream>

#include <QJsonObject>
#include <QJsonDocument>

#include "Types.h"

struct ConfigurationSettings
{
    std::string Name; // from filename

    struct MyPoint2d
    {
        double x = 0, y = 0;
        friend std::ostream & operator<<(std::ostream &s, const MyPoint2d &p) {s << "{" << p.x << "; " << p.y << "}"; return s;}
    };

    size_t Count = 4;
    struct
    {
        MyPoint2d b0, b1;
    } bArea;

    bool IsUseReliefRandomSeed = true;
    unsigned int ReliefRandomSeed = 100;

    struct ReliefCoeffsStruct
    {
        double A_r1 = 0.01;
        double A_r2 = 0.02;
    };
    ReliefCoeffsStruct ReliefCoeffs;

    size_t TrajectoryCount = 4;
    bool IsRandomTrajectories = false;

    struct MyTr
    {
        MyPoint2d p0, p1;
        double v = 0;
    };

    std::vector<MyTr> Trajectories;

    size_t NodeCount = 2;
    bool IsRandomNodes = false;

    struct MyNode
    {
        double R = 0;
    };

    std::vector<MyNode> Nodes;

    double pMutation = 0.012345;
    double pCrossover = 0.0156789;

    std::string TargetCostFunction = "Empty"; // not used yet
};

class Settings
{
private:

    bool IsLoaded = false;

    void ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError);

    ConfigurationSettings Cnf;

public:
    Settings() = default;

    const ConfigurationSettings & Get() {return Cnf;}

    bool LoadFromJson(const std::string &fn);

    static Settings & Inst();  // Singletone

    friend std::ostream & operator<<(std::ostream & s, const Settings &ob);
};

std::ostream & operator<<(std::ostream & s, const Settings &ob);

#endif // SETTINGS_H
