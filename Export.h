#ifndef EXPORT_H
#define EXPORT_H

#include <vector>

#include <QVector2D>
#include <QString>

class QTableWidget;

//struct ExportGeneralInfo
//{
//    int id = -1;
//    double w = -1;
//    double w0 = -1;
//    QString LastOp = "unknown";
//};

//struct ExportGeneralData
//{
//    double Time = -1;

//    std::vector<ExportGeneralInfo> GeneralInfos;
//};

struct ExportDetailedInfo
{
    int nConfig = -1;
    double Time = -1;

    std::vector<QVector2D> PeopleCoords;
    std::vector<QVector2D> NodeCoords;
    std::vector<float> SignalPowers;
    std::vector<float> DistToNearestNode;
};

void CopySelectedTableToClipboard(QTableWidget *_table);

QString QVector2D_ToQString(const QVector2D &_v);


#endif // EXPORT_H
