#include "GradDescSaveLoad.h"

#include <QString>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>

static void GradDescParseJson(tf_gd_lib::GradDescent &_gd, const QJsonObject &_jsonObject, const QJsonParseError &_parseError)
{
    if ( !_jsonObject["GeneralSettings"].isObject() )
    {
        qDebug() << _parseError.errorString();
        return;
    }

    const QJsonObject &GeneralSettingsObject = _jsonObject["GeneralSettings"].toObject();

    _gd.SetAlpha(GeneralSettingsObject["Alpha"].toDouble(-1));
    _gd.SetEps(GeneralSettingsObject["Eps"].toDouble(-1));
    _gd.SetEta_FirstJump(GeneralSettingsObject["Eta_FirstJump"].toDouble(-1));
    _gd.SetEta_k_inc(GeneralSettingsObject["Eta_k_inc"].toDouble(-1));
    _gd.SetEta_k_dec(GeneralSettingsObject["Eta_k_dec"].toDouble(-1));
    _gd.SetMin_Eta(GeneralSettingsObject["Min_Eta"].toDouble(-1));
    _gd.SetFinDifMethod(GeneralSettingsObject["FinDifMethod"].toBool(false));
    _gd.SetMaxIters(GeneralSettingsObject["MaxIters"].toInt(0));
    _gd.SetMaxTime(GeneralSettingsObject["MaxTime"].toDouble(-1));
    _gd.SetCallBackFreq(GeneralSettingsObject["CallBackFreq"].toInt(1));
}
//----------------------------------------------------------

bool GradDescLoadFromFile(tf_gd_lib::GradDescent &_gd, const QString &_fileName)
{
    QFile json(_fileName);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
//                Name = _fileName.mid(0, _fileName.lastIndexOf('.'));
                GradDescParseJson(_gd, jsonDoc.object(), parseError);
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

bool GradDescSaveToFile(const tf_gd_lib::GradDescent &_gd, const QString &_fileName)
{
    if (_fileName == "")
    {
        qDebug() << (_fileName + " not found");
        return false;
    }

    QJsonDocument jsonDoc;
    QJsonObject mainObject;

    QJsonObject GeneralSettingsObject;

    GeneralSettingsObject.insert("Alpha", (double)_gd.GetAlpha());
    GeneralSettingsObject.insert("CallBackFreq", (int)_gd.GetCallBackFreq());
    GeneralSettingsObject.insert("Eps", (double)_gd.GetEps());
    GeneralSettingsObject.insert("Eta_FirstJump", (double)_gd.GetEta_FirstJump());
    GeneralSettingsObject.insert("Eta_k_dec", (double)_gd.GetEta_k_dec());
    GeneralSettingsObject.insert("Eta_k_inc", (double)_gd.GetEta_k_inc());
    GeneralSettingsObject.insert("FinDifMethod", _gd.GetFinDifMethod());
    GeneralSettingsObject.insert("MaxIters", (int)_gd.GetMaxIters());
    GeneralSettingsObject.insert("MaxTime", (double)_gd.GetMaxTime());
    GeneralSettingsObject.insert("Min_Eta", (double)_gd.GetMin_Eta());

    mainObject.insert("GeneralSettings", GeneralSettingsObject);

    jsonDoc.setObject(mainObject);

    QFile jsonFile(_fileName);
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
