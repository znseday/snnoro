#include "FormRelief.h"
#include "ui_FormRelief.h"

#include <cmath>

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>

void MyPicWidget::mousePressEvent(QMouseEvent *pe)
{
    qDebug() << __PRETTY_FUNCTION__;

    OldX = pe->pos().x();
    OldY = pe->pos().y();

    FrameRect.setTopLeft({OldX, OldY});
    FrameRect.setBottomRight({OldX, OldY});

    IsMouseDown = true;
}
//-------------------------------------------------------------

void MyPicWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    IsMouseDown = false;
    emit SignalSendRectFrame(QRect(QPoint(OldX, OldY), QPoint(pe->pos().x(), pe->pos().y())));
    repaint();
}
//-------------------------------------------------------------

void MyPicWidget::mouseMoveEvent(QMouseEvent *pe)
{
    FrameRect.setTopLeft({OldX, OldY});
    FrameRect.setBottomRight({pe->pos().x(), pe->pos().y()});
    repaint();
}
//-------------------------------------------------------------

void MyPicWidget::paintEvent([[maybe_unused]] QPaintEvent *pe)
{
    QPainter painter(this);
    painter.drawImage(0, 0, ImgSrc);

    if (IsMouseDown)
        painter.drawRect(FrameRect);
    //painter.drawLine(100,100,200,200);
}
//-------------------------------------------------------------
//-------------------------------------------------------------

FormRelief::FormRelief(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FormRelief)
{
    ui->setupUi(this);

    wgtForScrollArea = new QWidget;
    QVBoxLayout *layouForScrollArea = new QVBoxLayout;

    lblPicSrc = new MyPicWidget("Test", ImgReliefSrc);
    lblPicDst = new QLabel("Result");
    layouForScrollArea->addWidget(lblPicSrc);
    layouForScrollArea->addWidget(lblPicDst);

    wgtForScrollArea->setLayout(layouForScrollArea);
    ui->scrollImageArea->setWidget(wgtForScrollArea);

    connect(lblPicSrc, SIGNAL(SignalSendRectFrame(QRect)),
            this, SLOT(SlotReceiveRectFrame(QRect)));


    //ui->tableColors->setColumnCount(2);

    //connect(ui->tableColors, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(SlotColorItemDblClicked(QTableWidgetItem *)));

    dlgColor.setWindowTitle("Select color");
}
//-------------------------------------------------------------

FormRelief::~FormRelief()
{
    delete ui;
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Open_Image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Image file", ".", "Image Files (*.png *jpg *bmp)");

    if (fileName.isEmpty())
        return;

    ImgReliefSrc.load(fileName);
    ImgReliefSrc = ImgReliefSrc.convertToFormat(QImage::Format_ARGB32);

    ImgReliefDst = ImgReliefSrc;

    //ImgReliefDst = ImgReliefDst.convertToFormat(QImage::Format_ARGB32); // ????????

//    ImgRelief = ImgRelief.convertToFormat(QImage::Format_RGB888);  // 24bit

    //ui->horizontalLayout_2->sets

//    ui->lblImage->setFixedSize(ImgRelief.width(), ImgRelief.height());
//    ui->lblImage->setPixmap(QPixmap::fromImage(ImgRelief));

    wgtForScrollArea->setFixedSize(ImgReliefSrc.width(), ImgReliefSrc.height()*2);

    lblPicSrc->setFixedSize(ImgReliefSrc.width(), ImgReliefSrc.height());
    lblPicDst->setFixedSize(ImgReliefSrc.width(), ImgReliefSrc.height());

    //lblPic->setPixmap(QPixmap::fromImage(ImgRelief));
}
//-------------------------------------------------------------

void FormRelief::on_btnApply_clicked()
{
    //ui->tableColors->clearContents();

    int rowCount = ui->EditColorCount->text().toInt();

    int oldRowCount = ui->tableColors->rowCount();

    ui->tableColors->setRowCount(rowCount);

    for (int i = oldRowCount; i < rowCount; ++i)
    {
        QTableWidgetItem *pItem = new QTableWidgetItem;
        pItem->setBackground(QBrush(QColor(rand()%255, rand()%255, rand()%255)));
        pItem->setFlags(Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/);
        ui->tableColors->setItem(i, 0, pItem);

        QTableWidgetItem *pItem2 = new QTableWidgetItem(QString().setNum(i*100));
        ui->tableColors->setItem(i, 1, pItem2);
    }
}
//-------------------------------------------------------------

void FormRelief::on_actionRelief_Calc_triggered()
{
    if (ImgReliefSrc.isNull())
    {
        qDebug() << "ImgReliefSrc.isNull()";
        return;
    }

    CalcLegendColor();

    int rows = ui->EditRowCount->text().toInt(); // Размер сетки
    int cols = ui->EditColCount->text().toInt(); // Размер сетки

    int w = ImgReliefSrc.width();  // в пикселах
    int h = ImgReliefSrc.height(); // в пикселах

    double dxPic = w/double(cols); // в пикселах
    double dyPic = h/double(rows); // в пикселах

    Relief.Clear();
    double l = ui->EditXStart->text().toDouble();
    double b = ui->EditYStart->text().toDouble();
    double r = l + ui->EditWidth->text().toDouble();
    double t = b + ui->EditHeight->text().toDouble();
    Relief.SetArea(l, b, r, t);

    if (!Relief.GetArea().isValid())
        throw std::runtime_error("!Relief.GetArea().isValid()");

    double dxReal = Relief.GetArea().width()/double(cols);  // в пикселах
    double dyReal = Relief.GetArea().height()/double(rows); // в пикселах

    for (int i = 0; i < rows; ++i)
    {
        std::vector<std::pair<int, int>> row;
        double yReal = b + (rows - i - 1)*dyReal;

        for (int j = 0; j < cols; ++j)
        {
            int Z = AnalyseImageAreaForZ(j*dxPic, i*dyPic, (j+1)*dxPic, (i+1)*dyPic);
            double xReal = l + dxReal/2 + j*dxReal;

            row.emplace_back(xReal, Z);
            //Relief.AddPoint(xReal, yReal, Z);
        }
        Relief.AddRow(yReal + dyReal/2, row);
    }

    //lblPicDst->repaint();

    lblPicDst->setPixmap(QPixmap::fromImage(ImgReliefDst));
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Close_triggered()
{
    this->close();
}
//-------------------------------------------------------------

void FormRelief::SlotColorDblClicked(QTableWidgetItem *pItem)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (pItem->column() != 0)
        return;

    dlgColor.setCurrentColor(pItem->background().color());

    if (dlgColor.exec() == QDialog::Accepted)
    {
        pItem->setBackground(QBrush(dlgColor.currentColor()));
    }
}
//-------------------------------------------------------------

void FormRelief::SlotColorCellPressed(int r, [[maybe_unused]] int c)
{
    LastSelectedColorRow = r;
}
//-------------------------------------------------------------

void FormRelief::CalcLegendColor()
{
    int colorCount = ui->tableColors->rowCount();

    LegendColor.Colors.clear();

    for (int i = 0; i < colorCount; ++i)
    {
        QColor c = ui->tableColors->item(i, 0)->background().color();
        int h = ui->tableColors->item(i, 1)->text().toInt();
        LegendColor.Colors.emplace_back(rgbaType(c.blue(), c.green(), c.red(), c.alpha()), h);
    }

    double averSim = 0;

    for (size_t i = 0; i < LegendColor.Colors.size()-1; ++i)
    {
        double r1 = LegendColor.Colors.at(i).first.r;
        double g1 = LegendColor.Colors.at(i).first.g;
        double b1 = LegendColor.Colors.at(i).first.b;

        double r2 = LegendColor.Colors.at(i+1).first.r;
        double g2 = LegendColor.Colors.at(i+1).first.g;
        double b2 = LegendColor.Colors.at(i+1).first.b;

        double dc = sqrt((r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2));

        averSim += dc;
    }
    averSim /= double(LegendColor.Colors.size() - 1);

    bool okFlag = false;
    double garbageFilter = ui->EditGarbageFilter->text().toDouble(&okFlag);
    if (!okFlag)
    {
        QMessageBox::critical(this, "Incorrect input", "GarbageFilter has an incorrect value. It will be set as 1.0.");
        garbageFilter = 1.0;
    }

    LegendColor.averSimilarity = averSim*garbageFilter;
}
//-------------------------------------------------------------

int FormRelief::FindNearestColorIndex(int r, int g, int b)
{
    double dcMin = 1e9;
    int resInd = -1;
    for (size_t i = 0; i < LegendColor.Colors.size(); ++i)
    {
        double r1 = LegendColor.Colors.at(i).first.r;
        double g1 = LegendColor.Colors.at(i).first.g;
        double b1 = LegendColor.Colors.at(i).first.b;

        double dc = sqrt((r1-r)*(r1-r) + (g1-g)*(g1-g) + (b1-b)*(b1-b));
        if (dc < dcMin)
        {
            dcMin = dc;
            resInd = i;
        }
    }

    if (dcMin > LegendColor.averSimilarity)
        resInd = -1;

    return resInd;
}
//-------------------------------------------------------------

int FormRelief::AnalyseImageAreaForZ(int xStart, int yStart, int xEnd, int yEnd)
{
    //int res = 0;

    if (xEnd > ImgReliefSrc.width())
    {
        qDebug() << "xEnd =" << xEnd;
        qDebug() << "ImgRelief.width() =" << ImgReliefSrc.width();
        throw std::runtime_error("xEnd > ImgReliefSrc.width()");
    }

    if (yEnd > ImgReliefSrc.height())
        throw std::runtime_error("yEnd > ImgReliefSrc.height()");


//    double averR = 0;
//    double averG = 0;
//    double averB = 0;
    //double averA = 0;

    std::vector<int> indCounts(LegendColor.Colors.size(), 0);

    int effPixelCount = 0;

    for (int y = yStart; y < yEnd; ++y)
    {
        const rgbaType *tempLine = reinterpret_cast<rgbaType*>(ImgReliefSrc.scanLine(y));

        tempLine += xStart;
        for (int x = xStart; x < xEnd; ++x)
        {
            int ind = FindNearestColorIndex(tempLine->r, tempLine->g, tempLine->b);
            if (ind >= 0)
            {
                indCounts.at(ind)++;
                effPixelCount++;
            }

            tempLine++;
        }
    }

    //qDebug() << "rate of effective pixels = " << double(effPixelCount)/(double(yEnd-yStart)*(xEnd-xStart));

    int indRes = -1;
    double maxCount = 0;

    if (effPixelCount > 0)
    {
        for (size_t i = 0; i < indCounts.size(); ++i)
            if (indCounts[i] > maxCount)
            {
                maxCount = indCounts[i];
                indRes = i;
            }
    }

//    if (indRes < 0)
//    {
//        qDebug() << "ERROR: Color for Area not found!";
//        return 0;
//    }

//    averR /= (yEnd-yStart)*(xEnd-xStart);
//    averG /= (yEnd-yStart)*(xEnd-xStart);
//    averB /= (yEnd-yStart)*(xEnd-xStart);


    for (int y = yStart; y < yEnd; ++y)
    {
//        QRgb *tempLine = reinterpret_cast<QRgb*>(ImgRelief.scanLine(y));
        rgbaType *tempLine = reinterpret_cast<rgbaType*>(ImgReliefDst.scanLine(y));
        tempLine += xStart;
        for (int x = xStart; x < xEnd; ++x)
        {
            if (indRes >= 0)
            {
                tempLine->r = LegendColor.Colors.at(indRes).first.r;
                tempLine->g = LegendColor.Colors.at(indRes).first.g;
                tempLine->b = LegendColor.Colors.at(indRes).first.b;
            }
            else
            {
                tempLine->r = 0;
                tempLine->g = 0;
                tempLine->b = 0;
            }


            tempLine++;
        }
    }

    if (indRes >= 0)
        return LegendColor.Colors.at(indRes).second;
    else
        return 0;
}
//-------------------------------------------------------------

rgbaType FormRelief::AnalyseImageAreaForColor(int xStart, int yStart, int xEnd, int yEnd)
{
    double averR = 0;
    double averG = 0;
    double averB = 0;
    //double averA = 0;

    for (int y = yStart; y < yEnd; ++y)
    {
        const rgbaType *tempLine = reinterpret_cast<rgbaType*>(ImgReliefSrc.scanLine(y));

        tempLine += xStart;
        for (int x = xStart; x < xEnd; ++x)
        {
            averR += tempLine->r;
            averG += tempLine->g;
            averB += tempLine->b;

            tempLine++;
        }
    }

    averR /= (yEnd-yStart)*(xEnd-xStart);
    averG /= (yEnd-yStart)*(xEnd-xStart);
    averB /= (yEnd-yStart)*(xEnd-xStart);

    return {(unsigned char)averB, (unsigned char)averG, (unsigned char)averR, 255};
}
//-------------------------------------------------------------

void FormRelief::SlotReceiveRectFrame(QRect _rect)
{
//    QModelIndexList list =  ui->tableColors->selectionModel()->selectedIndexes();

//    if (list.isEmpty())
//        return;

//    int firstRow = list.first().row();

    int firstRow = LastSelectedColorRow;
    if (firstRow < 0 || firstRow >= ui->tableColors->rowCount())
    {
        qDebug() << "firstRow < 0 || firstRow >= ui->tableColors->rowCount()";
        return;
    }

    _rect = _rect.normalized();
    rgbaType color = AnalyseImageAreaForColor(_rect.left()+1, _rect.top()+1, _rect.right(), _rect.bottom());

//    ui->tableColors->item(firstRow, 0)->background().setColor(QColor(color.r, color.g, color.b));

    ui->tableColors->item(firstRow, 0)->setBackground(QBrush(QColor(color.r, color.g, color.b)));

}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Save_Legend_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Legend file", ".", "Legend Files (*.json)");

    if (fileName == "")
    {
        qDebug() << (fileName + " not found");
        return;
    }

    QJsonDocument jsonDoc;
    QJsonObject resultObject;
    resultObject.insert("Count", ui->tableColors->rowCount());

    QJsonArray colorsArray;
    for (int i = 0; i < ui->tableColors->rowCount(); ++i)
    {
        QJsonObject colorInfoObject;
        colorInfoObject.insert("Color", ui->tableColors->item(i, 0)->background().color().name());
        colorInfoObject.insert("Height", ui->tableColors->item(i, 1)->text().toDouble());
        colorsArray.append(QJsonValue(colorInfoObject));
    }

    resultObject.insert("Colors", colorsArray);

    jsonDoc.setObject(resultObject);

    QFile jsonFile(fileName);
    if (jsonFile.open(QFile::WriteOnly))
        jsonFile.write(jsonDoc.toJson());
    else
        qDebug() << "json file not open to write";
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Load_Legend_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Legend file", ".", "Legend Files (*.json)");

    if (fileName == "")
    {
        qDebug() << (fileName + " not found");
        return;
    }

    QFile json(fileName);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
                QJsonObject mainObject = jsonDoc.object();

                int Count = mainObject["Count"].toInt(0);
                ui->EditColorCount->setText(QString().setNum(Count));
                on_btnApply_clicked();

                int i = 0;
                const QJsonArray &layersArray = mainObject["Colors"].toArray();
                for (auto it = layersArray.begin(); it != layersArray.end(); ++it, ++i)
                {
                    const QJsonObject &colorInfoObject = it->toObject();
                    QColor c;
                    c.setNamedColor(colorInfoObject["Color"].toString("red"));
                    ui->tableColors->item(i, 0)->setBackground(QBrush(c));
                    double h = colorInfoObject["Height"].toDouble();
                    ui->tableColors->item(i, 1)->setText(QString().setNum(h));
                }
                if (Count != i)
                    throw std::runtime_error("Count != i in FormRelief::on_actionFile_Load_Legend_triggered()");
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


}
//-------------------------------------------------------------

void FormRelief::on_chbColorToLegend_stateChanged([[maybe_unused]] int arg1)
{
//    if (ui->chbColorToLegend->isChecked())
//        lblPicSrc->setEnabled(true);
//    else
//        lblPicSrc->setEnabled(false);

    lblPicSrc->setEnabled(ui->chbColorToLegend->isChecked());
}
//-------------------------------------------------------------

void FormRelief::on_actionRelief_Save_Relief_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Relief file", ".", "Relief Files (*.json)");

    if (fileName == "")
    {
        qDebug() << (fileName + " not found");
        return;
    }

    Relief.SaveToFile(fileName);
}
//-------------------------------------------------------------



