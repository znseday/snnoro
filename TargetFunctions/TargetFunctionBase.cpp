#include "TargetFunctionBase.h"
#include "Types.h"

#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>

void TargetFunctionBase::InitMembersByGlobalSettings(const TargetFuncSettingsStruct &_targetFuncSettings)
{
    Aarf = _targetFuncSettings.Aarf;
    A2 = _targetFuncSettings.A2;
    p = _targetFuncSettings.p;
    offX = _targetFuncSettings.offX;
    k_step_ot = _targetFuncSettings.k_step_ot;
    R_nodeOverlap = _targetFuncSettings.R_nodeOverlap;
    IsUseCoveredFlag = _targetFuncSettings.IsUseCoveredFlag;
    IsUseLineBetweenTwoPoints = _targetFuncSettings.IsUseLineBetweenTwoPoints;
}
//----------------------------------------------------------
//----------------------------------------------------------

void TargetFuncSettingsStruct::ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError)
{
    if ( !_jsonObject["General"].isObject() )
    {
        qDebug() << _parseError.errorString();
        return;
    }

    const QJsonObject &GeneralSettingsObject = _jsonObject["GeneralSettings"].toObject();

    Aarf =             GeneralObject["Aarf"].toDouble();
    A2 =               GeneralObject["A2"].toDouble();
    p =                GeneralObject["p"].toDouble();
    offX =             GeneralObject["offX"].toDouble();
    k_step_ot =        GeneralObject["k_step_ot"].toDouble();
    R_nodeOverlap =    GeneralObject["R_nodeOverlap"].toDouble();
    IsUseCoveredFlag = GeneralObject["IsUseCoveredFlag"].toBool();
    IsUseLineBetweenTwoPoints = GeneralObject["IsUseLineBetweenTwoPoints"].toBool();
}
//----------------------------------------------------------

bool TargetFuncSettingsStruct::LoadFromFile(const QString &_fileName)
{
    // Решить вопрос относительных и абсолютных путей

    QFile json(_fileName);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
                FileName = _fileName;
//                Name = _fileName.mid(0, _fileName.lastIndexOf('.'));
                ParseJson(jsonDoc.object(), parseError);
            }
        }
        else
        {
            qDebug() << parseError.errorString();
            return false;
        }

    }
    else
    {
        qDebug() << "json file not open";
        return false;
    }

    return true;
}
//----------------------------------------------------------

bool TargetFuncSettingsStruct::SaveToFile(const QString &_fileName)
{
    if (_fileName == "")
    {
        qDebug() << (_fileName + " not found");
        return false;
    }

    QJsonDocument jsonDoc;
    QJsonObject mainObject;

    QJsonObject GeneralSettingsObject;

    GeneralSettingsObject.insert("A2", A2);
    GeneralSettingsObject.insert("Aarf", Aarf);
    GeneralSettingsObject.insert("IsUseCoveredFlag", IsUseCoveredFlag);
    GeneralSettingsObject.insert("R_nodeOverlap", R_nodeOverlap);
    GeneralSettingsObject.insert("k_step_ot", k_step_ot);
    GeneralSettingsObject.insert("offX", offX);
    GeneralSettingsObject.insert("p", p);
    GeneralSettingsObject.insert("IsUseLineBetweenTwoPoints", IsUseLineBetweenTwoPoints);

    mainObject.insert("GeneralSettings", GeneralSettingsObject);

    jsonDoc.setObject(mainObject);

    QFile jsonFile(FileName);
    if (jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(jsonDoc.toJson());
    }
    else
    {
        qDebug() << "json file not open to write";
        return false;
    }

//    IsSaved = true;
    return true;
}
//----------------------------------------------------------
