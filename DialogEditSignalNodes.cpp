#include "DialogEditSignalNodes.h"
#include "ui_DialogEditSignalNodes.h"

#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>

#include "Relief.h"

class MyButtonWithNumber : public QPushButton
{
protected:
    int Number = -1;
public:
    explicit MyButtonWithNumber(const QString &c, int n, QWidget *p = nullptr) :
        QPushButton(c, p), Number(n) {}
    int GetNumber() const {return Number;}
};
//----------------------------------------------------------



DialogEditSignalNodes::DialogEditSignalNodes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditSignalNodes)
{
    ui->setupUi(this);
}
//----------------------------------------------------------

DialogEditSignalNodes::~DialogEditSignalNodes()
{
    delete ui;
}
//----------------------------------------------------------

void DialogEditSignalNodes::InitDialog_ForAll(SignalNodeType _snt, const std::vector<SignalNode> &_signalNodes)
{
//    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
   // ui->Table->setFlags(  ui->Table->flags() & ~Qt::ItemIsEditable);

    ui->Table->clear();

    ui->Table->setRowCount(1 + _signalNodes.size());

    if (_snt == SignalNodeType::Sphere)
        ui->Table->setColumnCount(2+2);
    else if (_snt == SignalNodeType::Cone)
        ui->Table->setColumnCount(3+2);
    else
        QMessageBox::critical(this, "Error", "SignalNodeType is Unknown");

    CurNodeType = _snt;
//    pNodes = &_signalNodes;

//    ui->Table->setCellWidget(0, 0+0, new QPushButton("Load"));

//    ui->Table->setItem(0, 0+0, new QTableWidgetItem(new QPushButton("Load")));
    ui->Table->setColumnWidth(0+0, 50);

//    ui->Table->setItem(0, 0+1, new QPushButton("Save"));
    ui->Table->setColumnWidth(0+1, 50);

    ui->Table->setItem(0, 0+2, new QTableWidgetItem("n"));
    ui->Table->setColumnWidth(0+2, 60);
    ui->Table->item(0, 0+2)->setFlags( ui->Table->item(0, 0+2)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 1+2, new QTableWidgetItem("R, m"));
    ui->Table->setColumnWidth(1+2, 100);
    ui->Table->item(0, 1+2)->setFlags( ui->Table->item(0, 1+2)->flags() & ~Qt::ItemIsEditable );

    if (_snt == SignalNodeType::Cone)
    {
        ui->Table->setItem(0, 2+2, new QTableWidgetItem("Betha, deg"));
        ui->Table->setColumnWidth(2+2, 100);
        ui->Table->item(0, 2+2)->setFlags( ui->Table->item(0, 2+2)->flags() & ~Qt::ItemIsEditable );
    }

    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
        QPushButton *btnLoad = new MyButtonWithNumber("Load", i);
        ui->Table->setCellWidget(1 + i, 0+0, btnLoad);

        connect(btnLoad, SIGNAL(clicked()),
                this, SLOT(SlotLoadButtonClicked()));

        QPushButton *btnSave = new MyButtonWithNumber("Save", i);
        ui->Table->setCellWidget(1 + i, 0+1, btnSave);

        connect(btnSave, SIGNAL(clicked()),
                this, SLOT(SlotSaveButtonClicked()));

        ui->Table->setItem(1 + i, 0+2, new QTableWidgetItem(QString().setNum( i )));
//        ui->Table->setItem(1 + i, 1+2, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).R )));

//        if (_snt == SignalNodeType::Cone)
//            ui->Table->setItem(1 + i, 2+2, new QTableWidgetItem(QString().setNum( qRadiansToDegrees(_signalNodes.at(i).Beta) )));


        SetRowInTableFromSignalNode(_signalNodes.at(i), i);

        ui->Table->item(1 + i, 0+2)->setFlags( ui->Table->item(1 + i, 0+2)->flags() & ~Qt::ItemIsEditable );
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::InitDialog_ForCurrent(SignalNodeType _snt, const std::vector<SignalNode> &_signalNodes,
                                                  const Relief3D & _relief)
{
    const auto &_area = _relief.GetArea();

    ui->lblAreaInfo->setText("Area: l = " + QString().setNum(_area.left()) +
                             "; r = " + QString().setNum(_area.right()) +
                             "; b = " + QString().setNum(_area.bottom()) +
                             "; t = " + QString().setNum(_area.top()) );

    InitDialog_ForAll(_snt, _signalNodes);

    if (_snt == SignalNodeType::Sphere)
        ui->Table->setColumnCount(5+2);
    else if (_snt == SignalNodeType::Cone)
    {
        ui->Table->setColumnCount(7+2);

        ui->Table->setItem(0, 3+2, new QTableWidgetItem("Alpha, deg"));
        ui->Table->setColumnWidth(3+2, 100);
        ui->Table->item(0, 3+2)->setFlags( ui->Table->item(0, 3+2)->flags() & ~Qt::ItemIsEditable );
    }
    else
        QMessageBox::critical(this, "Error", "SignalNodeType is Unknown");

    int offCol = 0+2;
    if (_snt == SignalNodeType::Cone)
        offCol = 2+2;

    ui->Table->setItem(0, 2 + offCol, new QTableWidgetItem("x, m"));
    ui->Table->setColumnWidth(2 + offCol, 100);
    ui->Table->item(0, 2 + offCol)->setFlags( ui->Table->item(0, 2 + offCol)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 3 + offCol, new QTableWidgetItem("y, m"));
    ui->Table->setColumnWidth(3 + offCol, 100);
    ui->Table->item(0, 3 + offCol)->setFlags( ui->Table->item(0, 3 + offCol)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 4 + offCol, new QTableWidgetItem("z, m"));
    ui->Table->setColumnWidth(4 + offCol, 100);
    ui->Table->item(0, 4 + offCol)->setFlags( ui->Table->item(0, 4 + offCol)->flags() & ~Qt::ItemIsEditable );

    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
        ui->Table->setItem(1 + i, 2 + offCol, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Pos.x() )));
        ui->Table->setItem(1 + i, 3 + offCol, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Pos.y() )));

        ui->Table->setItem(1 + i, 4 + offCol, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Pos.z() )));
        ui->Table->item(1 + i, 4 + offCol)->setFlags( ui->Table->item(1, 4 + offCol)->flags() & ~Qt::ItemIsEditable );

        if (_snt == SignalNodeType::Cone)
            ui->Table->setItem(1 + i, 3+2, new QTableWidgetItem(QString().setNum( qRadiansToDegrees(_signalNodes.at(i).Alpha) )));
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::ChangeSignalNodesParameters_ForAll(SignalNodeType _snt, std::vector<SignalNode> &_signalNodes)
{
    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
        SetSignalNodeFormTable(_signalNodes.at(i), _snt, i);
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::ChangeSignalNodesParameters_ForCurrent(SignalNodeType _snt, std::vector<SignalNode> &_signalNodes,
                                                                   const Relief3D & _relief)
{
//    ChangeSignalNodesParameters_ForAll(SignalNodeType::Sphere, _signalNodes); // специально передаем Sphere, чтобы взять только R
    ChangeSignalNodesParameters_ForAll(_snt, _signalNodes); // специально передаем Sphere, чтобы взять только R

    int offCol = 0+2;
    if (_snt == SignalNodeType::Cone)
        offCol = 2+2;

    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
//        bool isOk = false;
//        double R = ui->Table->item(1 + i, 1)->text().toDouble(&isOk);
//        if (isOk)
//            _signalNodes.at(i).R = R;
//        else
//            qDebug() << "R is not saved for " << i << " signal Node";

        bool isOk_x = false;
        double x = ui->Table->item(1 + i, 2 + offCol)->text().toDouble(&isOk_x);

        bool isOk_y = false;
        double y = ui->Table->item(1 + i, 3 + offCol)->text().toDouble(&isOk_y);

        if (isOk_x && isOk_y)
        {
            if ( !_signalNodes.at(i).SetCoordForPos(_relief, {(float)x, (float)y, 0.0}) )
            {
                qDebug() << "x and y were incorrect according the Area for " << i << " signal Node";
            }
        }
        else
        {
            qDebug() << "x and y are not saved for " << i << " signal Node";
        }

        if (_snt == SignalNodeType::Cone)
        {
            bool isOk_Aplpha = false;
            double Alpha = ui->Table->item(1 + i, 3+2)->text().toDouble(&isOk_Aplpha);
            if (!isOk_Aplpha || Alpha < -360 || Alpha > 360)
            {
                qDebug() << "Alpha was not saved for " << i << " signal Node";
            }
            else
            {
                _signalNodes.at(i).Alpha = qDegreesToRadians(Alpha);
            }
        }
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::SlotLoadButtonClicked()
{
    const auto s = dynamic_cast<MyButtonWithNumber*>(this->sender());
    if (!s)
    {
        throw std::runtime_error("Unknown sender in DialogEditSignalNodes::SlotSaveButtonClicked");
    }

    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Signal Node file", ".", "Open Signal Node file (" + SignalNodesExtension + ")");

    if (fileName == "")
    {
        qDebug() << "Warning: File name is not set. Abort loading.";
        return;
    }

    // to do: load sn

    SignalNode sn;

//    sn.LoadFromJsonObject(???)

    QFile json(fileName);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
//                ParseJson(jsonDoc.object(), parseError);
                const QJsonObject &jsonObject = jsonDoc.object();
                const QJsonObject &nodeObject = jsonObject["SignalNode"].toObject();
                sn.LoadFromJsonObject(nodeObject);
            }
        }
        else
        {
            qDebug() << parseError.errorString();
            return;
        }
    }
    else
    {
        qDebug() << "json file not open";
        return;
    }

    int i = s->GetNumber();

    SetRowInTableFromSignalNode(sn, i);

}
//----------------------------------------------------------

void DialogEditSignalNodes::SetSignalNodeFormTable(SignalNode &_sn, SignalNodeType _snt, int i)
{
    bool isOk = false;
    double R = ui->Table->item(1 + i, 1+2)->text().toDouble(&isOk);
    if (isOk)
        _sn.R = R;
    else
        qDebug() << "R is not saved for " << i << " signal Node";

    if (_snt == SignalNodeType::Cone)
    {
        isOk = false;
        double Beta = ui->Table->item(1 + i, 2+2)->text().toDouble(&isOk);
        if (isOk)
            _sn.Beta = qDegreesToRadians(Beta);
        else
            qDebug() << "Betha is not saved for " << i << " signal Node";
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::SetRowInTableFromSignalNode(const SignalNode &_sn, int i)
{
    ui->Table->setItem(1 + i, 1+2, new QTableWidgetItem(QString().setNum( _sn.R )));

    if (CurNodeType == SignalNodeType::Cone)
        ui->Table->setItem(1 + i, 2+2,
                           new QTableWidgetItem(QString().setNum( qRadiansToDegrees(_sn.Beta) )));
}
//----------------------------------------------------------

void DialogEditSignalNodes::SlotSaveButtonClicked()
{
    const auto s = dynamic_cast<MyButtonWithNumber*>(this->sender());
    if (!s)
    {
        throw std::runtime_error("Unknown sender in DialogEditSignalNodes::SlotSaveButtonClicked");
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Signal Node file", ".", "Save Signal Node file (" + SignalNodesExtension + ")");

    if (fileName == "")
    {
        qDebug() << "Warning: File name is not set. Abort saving.";
        return;
    }

    SignalNode sn;

    int i = s->GetNumber();
    qDebug() << "i =" << i;

    SetSignalNodeFormTable(sn, CurNodeType, i);

    // to do: save it
    sn.RepresentAsJsonObject();

    QJsonDocument jsonDoc;
    QJsonObject resultObject;

    resultObject.insert("SignalNode", sn.RepresentAsJsonObject());

    jsonDoc.setObject(resultObject);

    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDoc.toJson());
}
//----------------------------------------------------------

