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
