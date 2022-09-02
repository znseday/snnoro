#ifndef GRADDESCSAVELOAD_H
#define GRADDESCSAVELOAD_H

class QJsonObject;
//class QJsonParseError;

class QString;

#include "UnitGradDescent.h"

//void GradDescParseJson(tf_gd_lib::GradDescent &_gd, const QJsonObject &_jsonObject, const QJsonParseError &_parseError);
bool GradDescLoadFromFile(tf_gd_lib::GradDescent &_gd, const QString &_fileName);
bool GradDescSaveToFile(const tf_gd_lib::GradDescent &_gd, const QString &_fileName);



#endif // GRADDESCSAVELOAD_H
