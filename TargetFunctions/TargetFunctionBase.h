#ifndef TARGETFUNCTIONBASE_H
#define TARGETFUNCTIONBASE_H


#include <vector>
#include <string>

class MyConfig;
//class TargetFuncSettingsStruct;

//constexpr double WierdCoeffAlpha = 0.00001;
constexpr double WierdCoeffAlpha = 1;

class QJsonObject;
class QJsonParseError;

#include <QString>

struct TargetFuncSettingsStruct // Переделать, чтобы у каждый целевой функции
                                // был свой пакет настроек
{
    double Aarf = 1e8;
    double A2 = 4e7;
    double p = 1e-5;
    double offX = 0.01;
    double k_step_ot = 0.03;
    double R_nodeOverlap = 50;
    bool IsUseCoveredFlag = true;
    bool IsUseLineBetweenTwoPoints = false;

    void ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError);
    bool LoadFromFile(const QString &_fileName);
    bool SaveToFile(const QString &_fileName);

    QString FileName;
};

//----------------------------------------------------------

class TargetFunctionBase
{
protected:
    double Aarf = 1e8;
    double A2 = 4e7;
    double p = 1e-5;
    double offX = 0.01;
    double k_step_ot = 0.03;
    double R_nodeOverlap = 50;
    bool IsUseCoveredFlag = true;
    bool IsUseLineBetweenTwoPoints = false;

    MyConfig *myConfig = nullptr;
    std::size_t param_count = 0;
public:
    TargetFunctionBase() = default;
    virtual long double operator()(const std::vector<long double>& params) const = 0;
    virtual ~TargetFunctionBase() = default;

//    void Set_param_count(std::size_t)
    void Set_myConfig(MyConfig *_cnfg) {myConfig = _cnfg;}
    virtual void Init(MyConfig *_myConfig) {myConfig = _myConfig;}
    std::size_t Get_param_count() const {return param_count;}
    bool GetIsUseLineBetweenTwoPoints() const {return IsUseLineBetweenTwoPoints;}

    void InitMembersByGlobalSettings(const TargetFuncSettingsStruct &_targetFuncSettings);
};
//-------------------------------------------------------------


#endif // TARGETFUNCTIONBASE_H
