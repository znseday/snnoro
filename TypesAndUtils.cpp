#include "TypesAndUtils.h"

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>

bool CorrectFileNameIfDoesntExist(QString &_fileName,
                                  const QString &_defDir,
                                  const QString &_what,
                                  const QString &_extension)
{
    if ( !QFile::exists(_fileName) )
    {
        QFileInfo fileInfo(_fileName);
        _fileName = _defDir + "/" + fileInfo.fileName(); // автоматическая корректировка
    }

    if ( !QFile::exists(_fileName) )
    {
        auto res = QMessageBox::question(nullptr, "Question",
                                         _what + " File not Found. Would you like to choose " + _what + " file?");
        if (res == QMessageBox::Yes)
        {
            _fileName = QFileDialog::getOpenFileName(nullptr,
                                      "Choose " + _what + " file", ".",
                                      _what + " Files (" + _extension + ")");

            return true; // выполнили корректировку вручную

            if (_fileName.isEmpty())
            {
                QMessageBox::critical(nullptr, "Error", _what + " file not set and won't be loaded");
            }

        }
        else
        {
            QMessageBox::critical(nullptr, "Error", _what + " file not set and won't be loaded");
        }
    }
    else
    {
        return true; // было достаточно автоматической корректировки
    }


    return false; // Корректировка не потребовалась
}
//-------------------------------------------------------------
//-------------------------------------------------------------

QJsonObject WhatShowStruct::RepresentAsJsonObject() const
{
    QJsonObject res;
//    res.insert("ShowGrid", ShowGrid);
    res.insert("ShowCones", ShowCones);
    res.insert("ShowRadii", ShowRadii);
    res.insert("ShowEllipses", ShowEllipses);
    res.insert("ShowLinesBetweenSNandPoints", ShowLinesBetweenSNandPoints);
    res.insert("ShowPointsOnRadii", ShowPointsOnRadii);
    res.insert("ShowAreaForRandomNodes", ShowAreaForRandomNodes);
    res.insert("ShowAreaForGradDesc", ShowAreaForGradDesc);
//    res.insert("ShowIsolinesOfAccessRate", ShowIsolinesOfAccessRate);
    res.insert("ShowRoutes", ShowRoutes);

    return res;
}
//-------------------------------------------------------------

void WhatShowStruct::LoadFromJsonObject(const QJsonObject &_jsonObject)
{
//    ShowGrid                    = _jsonObject["ShowGrid"].toBool(false);
    ShowCones                   = _jsonObject["ShowCones"].toBool(false);
    ShowRadii                   = _jsonObject["ShowRadii"].toBool(false);
    ShowEllipses                = _jsonObject["ShowEllipses"].toBool(false);
    ShowLinesBetweenSNandPoints = _jsonObject["ShowLinesBetweenSNandPoints"].toBool(false);
    ShowPointsOnRadii           = _jsonObject["ShowPointsOnRadii"].toBool(false);
    ShowAreaForRandomNodes      = _jsonObject["ShowAreaForRandomNodes"].toBool(false);
    ShowAreaForGradDesc         = _jsonObject["ShowAreaForGradDesc"].toBool(false);  
//    ShowIsolinesOfAccessRate    = _jsonObject["ShowIsolinesOfAccessRate"].toBool(false);
    ShowRoutes                  = _jsonObject["ShowRoutes"].toBool(false);
}
//-------------------------------------------------------------
//-------------------------------------------------------------
