#include "TypesAndUtils.h"

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>

void CorrectFileNameIfDoesntExist(QString &_fileName, const QString &_defDir, const QString &_what)
{
    if ( !QFile::exists(_fileName) )
    {
        QFileInfo fileInfo(_fileName);
        _fileName = _defDir + "/" + fileInfo.fileName();
    }

    if ( !QFile::exists(_fileName) )
    {
        auto res = QMessageBox::question(nullptr, "Question", _what + " File not Found. Would you like to choose " + _what + " file?");
        if (res == QMessageBox::Yes)
        {
            _fileName = QFileDialog::getOpenFileName(nullptr,
                                      "Choose " + _what + " file", ".", _what + " Files (*.json)");

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
}
