#include "Export.h"

#include <QApplication>
#include <QClipboard>

void CopySelectedTableToClipboard(QTableWidget *_table)
{
    QString cbStr;
    QClipboard *cb = QApplication::clipboard();
    QModelIndexList list = _table->selectionModel()->selectedIndexes();

    if (list.isEmpty())
        return;

    int firstRow = list.first().row();
    int lastRow  = list.last().row();
    int rowCount = lastRow - firstRow + 1;

    int ColCount = list.last().column() - list.first().column() + 1;

    int n = 0;
    for (int i = 0; i < rowCount; ++i, cbStr += '\n')
        for (int j = i; j < list.count(); j += rowCount)
        {
            cbStr += _table->model()->data(list[j], Qt::EditRole).toString();

            if (n%ColCount != (ColCount-1))
            {
                cbStr += '\t';
            }
            n++;
        }

    cb->setText(cbStr);
}
//----------------------------------------------------------

QString QVector2D_ToQString(const QVector2D &_v)
{
    return QString().setNum(_v.x(), 'f', 2) + "; " + QString().setNum(_v.y(), 'f', 2);
}
//----------------------------------------------------------










