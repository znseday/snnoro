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
#include <QMouseEvent>

#include <QDirIterator>


void MyPicSrcWidget::mousePressEvent(QMouseEvent *pe)
{
    qDebug() << __PRETTY_FUNCTION__;

    OldX = pe->pos().x();
    OldY = pe->pos().y();

    FrameRect.setTopLeft({OldX, OldY});
    FrameRect.setBottomRight({OldX, OldY});

    if (pe->button() == Qt::LeftButton)
    {
        IsMouseDown = true;
    }
}
//-------------------------------------------------------------

void MyPicSrcWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    IsMouseDown = false;
    emit SignalSendRectFrame(QRect(QPoint(OldX, OldY), QPoint(pe->pos().x(), pe->pos().y())));
    repaint();
}
//-------------------------------------------------------------

void MyPicSrcWidget::mouseMoveEvent(QMouseEvent *pe)
{
    FrameRect.setTopLeft({OldX, OldY});
    FrameRect.setBottomRight({pe->pos().x(), pe->pos().y()});
    repaint();
}
//-------------------------------------------------------------

void MyPicSrcWidget::paintEvent([[maybe_unused]] QPaintEvent *pe)
{
    if (!ImgSrc.isNull())
    {
        QPainter painter(this);
        painter.drawImage(0, 0, ImgSrc);

        if (IsMouseDown)
            painter.drawRect(FrameRect);
    }
}
//-------------------------------------------------------------
//-------------------------------------------------------------

void MyPicDstWidget::mousePressEvent(QMouseEvent *pe)
{
    qDebug() << __PRETTY_FUNCTION__;

    OldX = pe->pos().x();
    OldY = pe->pos().y();

    if (pe->button() == Qt::RightButton)
    {
        emit SignalSendChangePoint(OldX, OldY);
        repaint();
    }
}
//-------------------------------------------------------------

void MyPicDstWidget::paintEvent([[maybe_unused]] QPaintEvent *pe)
{
    if (!ImgDst.isNull())
    {
        QPainter painter(this);
        painter.drawImage(0, 0, ImgDst);
    }
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

    lblPicSrc = new MyPicSrcWidget("Test", ImgReliefSrc);
    lblPicDst = new MyPicDstWidget("Result", ImgReliefDst);
    layouForScrollArea->addWidget(lblPicSrc);
    layouForScrollArea->addWidget(lblPicDst);

    wgtForScrollArea->setLayout(layouForScrollArea);
    ui->scrollImageArea->setWidget(wgtForScrollArea);

    connect(lblPicSrc, SIGNAL(SignalSendRectFrame(QRect)),
            this, SLOT(SlotReceiveRectFrame(QRect)));

    connect(lblPicDst, SIGNAL(SignalSendChangePoint(int,int)),
            this, SLOT(SlotReceiveChangePoint(int,int)));

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

bool FormRelief::CheckIsLegendSavedAndSaveIfNecessary()
{
    if ( !IsLegendSaved )
    {
        auto res = QMessageBox::question(this, "Question",
                                         "Legend is not saved. Would you like to save it?",
                                         QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (res == QMessageBox::Yes)
        {
            on_actionFile_Save_Legend_As_triggered();  // try save
            return IsLegendSaved;
        }
        else if (res == QMessageBox::No)
            return true;
        else if (res == QMessageBox::Cancel)
            return false;
        else
        {
            QMessageBox::critical(this, "Error", "Something wrong with QMessageBox::question result");
            return false;
        }
    }
    else
        return true;
}
//-------------------------------------------------------------

bool FormRelief::CheckIsReliefSavedAndSaveIfNecessary()
{
    if ( !IsReliefSaved )
    {
        auto res = QMessageBox::question(this, "Question",
                                         "Relief is not saved. Would you like to save it?",
                                         QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (res == QMessageBox::Yes)
        {
            on_actionRelief_Calc_Relief_And_Save_As_triggered(); // try save
            return IsLegendSaved;
        }
        else if (res == QMessageBox::No)
            return true;
        else if (res == QMessageBox::Cancel)
            return false;
        else
        {
            QMessageBox::critical(this, "Error", "Something wrong with QMessageBox::question result");
            return false;
        }
    }
    else
        return true;
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Open_Image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Image file", ".", "Image Files (" + ReliefsImagesExtension + ")");

    if (fileName.isEmpty())
        return;

    if (!LoadSrcImage(fileName))
        QMessageBox::critical(this, "Error", "Image's been not loaded");

}
//-------------------------------------------------------------

void FormRelief::on_btnApply_clicked()
{
    //ui->tableColors->clearContents();
//    IsLegendReadyToBeSavedToTemp = false;

    SaveCurrentLegendToTemp();

    int rowCount = ui->EditColorCount->text().toInt();

    OldRowCount = ui->tableColors->rowCount();

    ui->tableColors->setRowCount(rowCount);

    for (int i = OldRowCount; i < rowCount; ++i)
    {
        QTableWidgetItem *pItem = new QTableWidgetItem;
        pItem->setBackground(QBrush(QColor(rand()%255, rand()%255, rand()%255)));
        pItem->setFlags(Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/);
        ui->tableColors->setItem(i, 0, pItem);

        QTableWidgetItem *pItem2 = new QTableWidgetItem(QString().setNum(i*100));
        ui->tableColors->setItem(i, 1, pItem2);
    }

    OldRowCount = rowCount;

//    SaveCurrentLegendToTemp();

//    IsLegendReadyToBeSavedToTemp = true;

    CalcLegendColor();

    IsLegendSaved = false;
}
//-------------------------------------------------------------

void FormRelief::on_actionRelief_Calc_Discrete_Img_triggered()
{
    if (ImgReliefSrc.isNull())
    {
        qDebug() << "ImgReliefSrc.isNull()";
        return;
    }
    int colorCount = ui->tableColors->rowCount();
    if (colorCount < 2)
    {
        QMessageBox::critical(this, "Error", "There must be at least 2 colors in the legend");
        return;
    }

    TempGrid.clear();

    CalcLegendColor();

    int rows = ui->EditRowCount->text().toInt(); // Размер сетки
    int cols = ui->EditColCount->text().toInt(); // Размер сетки

    int w = ImgReliefSrc.width();  // в пикселах
    int h = ImgReliefSrc.height(); // в пикселах

    double dxPic = w/double(cols); // в пикселах
    double dyPic = h/double(rows); // в пикселах

    for (int i = 0; i < rows; ++i)
    {
        TempGrid.push_back({});

        for (int j = 0; j < cols; ++j)
        {
            int xStart = j*dxPic;
            int yStart = i*dyPic;
            int xEnd = (j+1)*dxPic;
            int yEnd = (i+1)*dyPic;

            auto res = AnalyseImageAreaForZ(xStart, yStart, xEnd, yEnd);

            TempGrid.back().emplace_back(res);
        }

    }

    PrintImgReliefDstFromTempGrid();

    ui->actionRelief_Calc_Relief_And_Save_As->setEnabled(true);

    IsReliefSaved = false;
}
//-------------------------------------------------------------

void FormRelief::PrintImgReliefDstFromTempGrid()
{
    if (TempGrid.empty())
    {
        qDebug() << "TempGrid is empty in PrintImgReliefDstFromTempGrid()";
        return;
    }

    int rows = TempGrid.size();         // Размер сетки
    int cols = TempGrid.front().size(); // Размер сетки

    int w = ImgReliefDst.width();  // в пикселах
    int h = ImgReliefDst.height(); // в пикселах

    double dxPic = w/double(cols); // в пикселах
    double dyPic = h/double(rows); // в пикселах

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            int xStart = j*dxPic;
            int yStart = i*dyPic;
            int xEnd = (j+1)*dxPic;
            int yEnd = (i+1)*dyPic;

            for (int y = yStart; y < yEnd; ++y)
            {
                const auto & color = TempGrid.at(i).at(j).first;
                rgbaType *tempLine = reinterpret_cast<rgbaType*>(ImgReliefDst.scanLine(y));
                tempLine += xStart;
                for (int x = xStart; x < xEnd; ++x)
                {
                    tempLine->r = color.r;
                    tempLine->g = color.g;
                    tempLine->b = color.b;

                    tempLine++;
                }
            }
        }
    }

    lblPicDst->repaint();
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Close_triggered()
{
    this->close();
}
//-------------------------------------------------------------

void FormRelief::SlotColorDblClicked(QTableWidgetItem *pItem)
{
//    qDebug() << __PRETTY_FUNCTION__;

    if (pItem->column() != 0)
        return;

    dlgColor.setCurrentColor(pItem->background().color());

    if (dlgColor.exec() == QDialog::Accepted)
    {
        SaveCurrentLegendToTemp();
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

    if (colorCount < 2)
    {
        QMessageBox::critical(this, "Error", "There must be at least 2 colors in the legend");
        return;
    }

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

CorolAndZ_pair FormRelief::AnalyseImageAreaForZ(int xStart, int yStart, int xEnd, int yEnd)
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

    if (indRes < 0)
    {
        qDebug() << "Warning: Appropriate Color for Area not found!";
        return {};
    }

//    averR /= (yEnd-yStart)*(xEnd-xStart);
//    averG /= (yEnd-yStart)*(xEnd-xStart);
//    averB /= (yEnd-yStart)*(xEnd-xStart);

//-------------------------------------------------------------
//    for (int y = yStart; y < yEnd; ++y)
//    {
//        rgbaType *tempLine = reinterpret_cast<rgbaType*>(ImgReliefDst.scanLine(y));
//        tempLine += xStart;
//        for (int x = xStart; x < xEnd; ++x)
//        {
//            if (indRes >= 0)
//            {
//                tempLine->r = LegendColor.Colors.at(indRes).first.r;
//                tempLine->g = LegendColor.Colors.at(indRes).first.g;
//                tempLine->b = LegendColor.Colors.at(indRes).first.b;
//            }
//            else
//            {
//                tempLine->r = 0;
//                tempLine->g = 0;
//                tempLine->b = 0;
//            }
//            tempLine++;
//        }
//    }
//---------------------------------------------------------------

    if (indRes >= 0)
        return LegendColor.Colors.at(indRes);
    else
        return {};
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

void FormRelief::SaveCurrentTempGridToTempGridList()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (TempGrid.empty())
        return;

    TempGridList.push_back(TempGrid);

    if (TempGridList.size() > MaxReliefsForUndo)
    {
        TempGridList.pop_front();
    }

    ui->actionRelief_Undo->setEnabled(true);
}
//-------------------------------------------------------------

void FormRelief::UndoTempGridChanges()
{
    if (TempGridList.empty())
    {
        qDebug() << "TempGridList is empty()";
        return;
    }

    TempGrid = std::move(TempGridList.back());
    TempGridList.pop_back();

    PrintImgReliefDstFromTempGrid();
    IsReliefSaved = false;

    if (TempGridList.empty())
    {
        ui->actionRelief_Undo->setEnabled(false);
    }
}
//-------------------------------------------------------------

bool FormRelief::LoadSrcImage(const QString &_fn)
{
    if (!ImgReliefSrc.load(_fn))
        return false;

    ImageSrcFileName = _fn;

    ImgReliefSrc = ImgReliefSrc.convertToFormat(QImage::Format_ARGB32);

    ImgReliefDst = ImgReliefSrc;

    wgtForScrollArea->setFixedSize(ImgReliefSrc.width(), ImgReliefSrc.height()*2);

    lblPicSrc->setFixedSize(ImgReliefSrc.width(), ImgReliefSrc.height());
    lblPicDst->setFixedSize(ImgReliefSrc.width(), ImgReliefSrc.height());

    ui->actionRelief_Calc_Discrete_Img->setEnabled(true);

    return true;
}
//-------------------------------------------------------------

bool FormRelief::LoadLegend(const QString &_fn)
{
    QFile json(_fn);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            LegendFileName = _fn;

            if (jsonDoc.isObject())
            {
                QJsonObject mainObject = jsonDoc.object();

                int Count = mainObject["Count"].toInt(0);
                ui->EditColorCount->setText(QString().setNum(Count));
                on_btnApply_clicked();
                IsLegendReadyToBeSavedToTemp = false;

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
            return false;
        }
    }
    else
    {
        qDebug() << "json file not open";
        return false;
    }

    IsLegendSaved = true;
    IsLegendReadyToBeSavedToTemp = true;
    SaveCurrentLegendToTemp();
    return true;
}
//-------------------------------------------------------------

void FormRelief::CalcDiscreteImgByExistentRelief()
{
    if (ImgReliefSrc.isNull())
    {
        qDebug() << "ImgReliefSrc.isNull()";
        return;
    }
//    int colorCount = ui->tableColors->rowCount();
//    if (colorCount < 2)
//    {
//        QMessageBox::critical(this, "Error", "There must be at least 2 colors in the legend");
//        return;
//    }

    SaveCurrentTempGridToTempGridList();

    TempGrid.clear();

    CalcLegendColor();

    int rows = ui->EditRowCount->text().toInt(); // Размер сетки
    int cols = ui->EditColCount->text().toInt(); // Размер сетки

    if (rows != (int)Relief.GetReliefMap().size())
    {
        throw std::runtime_error("rows != (int)Relief.GetReliefMap().size() in FormRelief::CalcDiscreteImgByExistentRelief()");
    }

    if (cols != (int)Relief.GetReliefMap().cbegin()->second.size())
    {
        throw std::runtime_error("cols != (int)Relief.GetReliefMap().cbegin()->second.size() in FormRelief::CalcDiscreteImgByExistentRelief()");
    }

//    for (const auto & rs : Relief.GetReliefMap())  // rows
    for (auto r_it = Relief.GetReliefMap().crbegin(); r_it != Relief.GetReliefMap().crend(); ++r_it)
    {
        TempGrid.push_back({});

        for (const auto & cs : r_it->second)  // cols
        {
//            int xStart = j*dxPic;
//            int yStart = i*dyPic;
//            int xEnd = (j+1)*dxPic;
//            int yEnd = (i+1)*dyPic;
//            auto res = AnalyseImageAreaForZ(xStart, yStart, xEnd, yEnd);

            int z = cs.second;

            auto it = std::find_if(LegendColor.Colors.cbegin(), LegendColor.Colors.cend(), [z](const auto & colorPair)
                {
                    return colorPair.second == z;
                });

            if (it == LegendColor.Colors.cend())
            {
                throw std::runtime_error("Z in Relief not found in Legend");
            }

            CorolAndZ_pair res = *it;

            TempGrid.back().emplace_back(res);
        }

    }

    PrintImgReliefDstFromTempGrid();

    ui->actionRelief_Calc_Relief_And_Save_As->setEnabled(true);
}
//-------------------------------------------------------------

void FormRelief::SlotReceiveRectFrame(QRect _rect)
{
//    QModelIndexList list =  ui->tableColors->selectionModel()->selectedIndexes();
//    if (list.isEmpty())
//        return;
//    int firstRow = list.first().row();
//    int firstRow = LastSelectedColorRow;
    if (LastSelectedColorRow < 0 || LastSelectedColorRow >= ui->tableColors->rowCount())
    {
        qDebug() << "LastSelectedColorRow < 0 || LastSelectedColorRow >= ui->tableColors->rowCount()";
        return;
    }

    _rect = _rect.normalized();
    rgbaType color = AnalyseImageAreaForColor(_rect.left()+1, _rect.top()+1, _rect.right(), _rect.bottom());

//    ui->tableColors->item(firstRow, 0)->background().setColor(QColor(color.r, color.g, color.b));

    ui->tableColors->item(LastSelectedColorRow, 0)->setBackground(QBrush(QColor(color.r, color.g, color.b)));

    SaveCurrentLegendToTemp();

    IsLegendSaved = false;
}
//-------------------------------------------------------------

void FormRelief::SlotReceiveChangePoint(int x, int y)
{
    if (LastSelectedColorRow < 0 || LastSelectedColorRow >= ui->tableColors->rowCount())
    {
        QMessageBox::critical(this, "Error", "There must be a selected color in the legend");
        qDebug() << "LastSelectedColorRow < 0 || LastSelectedColorRow >= ui->tableColors->rowCount()";
        return;
    }
    if (TempGrid.empty())
    {
        QMessageBox::critical(this, "Error", "TempGrid is empty in SlotReceiveChangePoint()");
        qDebug() << "TempGrid is empty in SlotReceiveChangePoint()";
        return;
    }

    SaveCurrentTempGridToTempGridList();

    QColor c = ui->tableColors->item(LastSelectedColorRow, 0)->background().color();
    if (!c.isValid())
    {
        qDebug() << "Selected color is invalid!";
        return;
    }

    int w = ImgReliefDst.width();
    int h = ImgReliefDst.height();

    int row = y / (double)h * TempGrid.size();
    int col = x / (double)w * TempGrid.front().size();

    qDebug() << "row =" << row << "col =" << col;

    TempGrid.at(row).at(col) = LegendColor.Colors.at(LastSelectedColorRow);

    PrintImgReliefDstFromTempGrid();

    IsReliefSaved = false;
}
//-------------------------------------------------------------

void FormRelief::closeEvent(QCloseEvent *event)
{
    if (CheckIsLegendSavedAndSaveIfNecessary() && CheckIsReliefSavedAndSaveIfNecessary())
        event->accept();
    else
        event->ignore();
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Save_Legend_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Legend file", ".", "Legend Files (" + ReliefsLegendsExtension + ")");

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
    {
        qDebug() << "json file not open to write";
        return;
    }

    IsLegendSaved = true;
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Load_Legend_triggered()
{
    if (!CheckIsLegendSavedAndSaveIfNecessary())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Legend file", ".", "Legend Files (" + ReliefsLegendsExtension + ")");

    if (fileName == "")
    {
        qDebug() << (fileName + " not found");
        return;
    }

    if (!LoadLegend(fileName))
        QMessageBox::critical(this, "Error", "Legend's been not loaded");

    IsLegendSaved = true;
}
//-------------------------------------------------------------

void FormRelief::on_chbColorToLegend_stateChanged([[maybe_unused]] int arg1)
{
    lblPicSrc->setEnabled(ui->chbColorToLegend->isChecked());
}
//-------------------------------------------------------------

void FormRelief::on_actionRelief_Calc_Relief_And_Save_As_triggered()
{
    if (TempGrid.empty())
    {
        throw std::logic_error("TempGrid is empty in PrintImgReliefDstFromTempGrid()");
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Relief file", ".", "Relief Files (" + ReliefsExtension + ")");

    if (fileName == "")
    {
        qDebug() << (fileName + " not found");
        return;
    }

    int rows = TempGrid.size();         // Размер сетки
    int cols = TempGrid.front().size(); // Размер сетки

    Relief.Clear();

    Relief.SetImageFileName(ImageSrcFileName);
    Relief.SetLegendFileName(LegendFileName);

    double l = ui->EditXStart->text().toDouble();
    double b = ui->EditYStart->text().toDouble();
    double r = l + ui->EditWidth->text().toDouble();
    double t = b + ui->EditHeight->text().toDouble();
    Relief.SetArea(l, b, r, t);

    if (!Relief.GetArea().isValid())
        throw std::runtime_error("!Relief.GetArea().isValid()");

    double dxReal = Relief.GetArea().width()/double(cols);  // в метрах
    double dyReal = Relief.GetArea().height()/double(rows); // в метрах

    for (int i = 0; i < rows; ++i)
    {
        std::vector<std::pair<int, int>> row;
        double yReal = b + dyReal/2 + (rows - i - 1)*dyReal;

        for (int j = 0; j < cols; ++j)
        {
            double xReal = l + dxReal/2 + j*dxReal;
            auto Z = TempGrid.at(i).at(j).second;
            row.emplace_back(xReal, Z);
        }

        Relief.AddRow(yReal, row);
    }

    Relief.SaveToFile(fileName);

    IsReliefSaved = true;
}
//-------------------------------------------------------------

void FormRelief::on_actionFile_Open_Relief_triggered()
{
    if (!CheckIsReliefSavedAndSaveIfNecessary())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Relief file", ".", "Relief Files (" + ReliefsExtension + ")");

    if (fileName.isEmpty())
        return;

    if (!Relief.LoadFromFile(fileName))
        throw std::runtime_error("Relief File Not Found or Couldn't be read");

    ImageSrcFileName = Relief.GetImageFileName();
    bool isImageSrcFileNameCorrected = CorrectFileNameIfDoesntExist(ImageSrcFileName, ReliefsImagesDefaultDir, "Image for Relief", ReliefsImagesExtension);

    Relief.SetImageFileName(ImageSrcFileName);
    qDebug() << "Relief.GetImageFileName(); =" << Relief.GetImageFileName();
    if (!LoadSrcImage(Relief.GetImageFileName()))
        QMessageBox::critical(this, "Error", "Image's been not loaded");

    LegendFileName = Relief.GetLegendFileName();
    bool isLegendFileNameCorrected = CorrectFileNameIfDoesntExist(LegendFileName, ReliefsLegendsDefaultDir, "Legend for Relief", ReliefsLegendsExtension);
    Relief.SetLegendFileName(LegendFileName);
    qDebug() << "Relief.GetLegendFileName(); =" << Relief.GetLegendFileName();
    if (!LoadLegend(Relief.GetLegendFileName()))
        QMessageBox::critical(this, "Error", "Legend's been not loaded");

    ui->EditXStart->setText(QString().setNum(Relief.GetArea().left()));
    ui->EditYStart->setText(QString().setNum(Relief.GetArea().top()));
    ui->EditWidth->setText(QString().setNum(Relief.GetArea().width()));
    ui->EditHeight->setText(QString().setNum(Relief.GetArea().height()));

    ui->EditRowCount->setText(QString().setNum(Relief.GetReliefMap().size()));
    ui->EditColCount->setText(QString().setNum(Relief.GetReliefMap().cbegin()->second.size()));

    CalcDiscreteImgByExistentRelief();

    if (isImageSrcFileNameCorrected || isLegendFileNameCorrected)
        IsReliefSaved = false;
    else
        IsReliefSaved = true;

}
//-------------------------------------------------------------

void FormRelief::on_tableColors_itemChanged(QTableWidgetItem *item) // срабатывает при загрузки легенды из файла
{                                                 // надо что-то с этим сделать
    (void)item;
//    qDebug() << __PRETTY_FUNCTION__ << item->text();

//    if (IsLegendReadyToBeSavedToTemp)
//        SaveCurrentLegendToTemp();

//    IsLegendSaved = false;
}
//-------------------------------------------------------------

void FormRelief::on_actionRelief_Undo_triggered()
{
    UndoTempGridChanges();
//    ui->actionRelief_Redo->setEnabled(true);
//    ui->actionRelief_Undo->setEnabled(false);
}
//-------------------------------------------------------------

void FormRelief::on_actionRelief_Redo_triggered()
{

//    ui->actionRelief_Undo->setEnabled(true);
//    ui->actionRelief_Redo->setEnabled(false);
}
//-------------------------------------------------------------

void FormRelief::on_actionLegend_Undo_triggered()
{
    UndoLegendChanges();
//    ui->actionLegend_Redo->setEnabled(true);
//    ui->actionLegend_Undo->setEnabled(false);
}
//-------------------------------------------------------------

void FormRelief::on_actionLegend_Redo_triggered()
{

//    ui->actionLegend_Undo->setEnabled(true);
//    ui->actionLegend_Redo->setEnabled(false);
}
//-------------------------------------------------------------

void FormRelief::SaveCurrentLegendToTemp()
{
    qDebug() << __PRETTY_FUNCTION__;

//    TempLegendColor.Colors.clear();

    LegendColorType TempLegendColor;

    for (int i = 0; i <  ui->tableColors->rowCount(); ++i)
    {
        auto item0 = ui->tableColors->item(i, 0);
        if (!item0)
            break;

        QColor c = item0->background().color();

        auto item1 = ui->tableColors->item(i, 1);
        if (!item1)
            break;

        int h = item1->text().toInt();
        TempLegendColor.Colors.emplace_back(rgbaType(c.red(), c.green(), c.blue(), c.alpha()), h);
    }

    LegendColorList.push_back(std::move(TempLegendColor));

    if (LegendColorList.size() > MaxLegendsForUndo)
    {
        LegendColorList.pop_front();
    }

    ui->actionLegend_Undo->setEnabled(true);
}
//-------------------------------------------------------------

void FormRelief::UndoLegendChanges()
{
    if (LegendColorList.empty())
    {
        qDebug() << "LegendColorList is empty()";
        return;
    }

    IsLegendReadyToBeSavedToTemp = false;

    auto TempLegendColor = std::move(LegendColorList.back());
    LegendColorList.pop_back();

//    assert((int)TempLegendColor.Colors.size() == ui->tableColors->rowCount());

    OldRowCount = ui->tableColors->rowCount();
    ui->tableColors->setRowCount(TempLegendColor.Colors.size());


    for (int i = 0; i < (int)TempLegendColor.Colors.size(); ++i)
    {
        QTableWidgetItem *pItem;
        if (i >= OldRowCount)
        {
            pItem = new QTableWidgetItem;
            pItem->setFlags(Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/);
            ui->tableColors->setItem(i, 0, pItem);
        }
        else
            pItem = ui->tableColors->item(i, 0);

        auto c = TempLegendColor.Colors[i].first;

        pItem->setBackground(QBrush({c.b, c.g, c.r, c.a}));
//        ui->tableColors->setItem(i, 0, pItem);

        QTableWidgetItem *pItem2;

        if (i >= OldRowCount)
        {
            pItem2 = new QTableWidgetItem;
            ui->tableColors->setItem(i, 1, pItem2);
        }
        else
            pItem2 = ui->tableColors->item(i, 1);

        pItem2->setText(QString().setNum(TempLegendColor.Colors[i].second));
//        QTableWidgetItem *pItem2 = new QTableWidgetItem(QString().setNum(i*100));
//        ui->tableColors->setItem(i, 1, pItem2);
    }

    IsLegendReadyToBeSavedToTemp = true;

    if (LegendColorList.empty())
    {
        ui->actionLegend_Undo->setEnabled(false);
    }
}
//-------------------------------------------------------------

void FormRelief::on_tableColors_cellChanged(int row, int column)
{
    (void)row;  (void)column;

    IsLegendSaved = false;

//    if (row == 0)
//        return;

//    qDebug() << __PRETTY_FUNCTION__;

//    if (IsLegendReadyToBeSavedToTemp)
//        SaveCurrentLegendToTemp();
}
//-------------------------------------------------------------

void FormRelief::showEvent(QShowEvent *event)
{
    if (event->spontaneous() )
    {
//        qDebug() << __PRETTY_FUNCTION__ << "  spontaneous";
        return;
    }

//    QString dir = "./Reliefs/Legends";

//    ui->cbLegends->blockSignals(true);
    ui->cbLegends->clear();

//    QDirIterator it(dir, QStringList() << "*.rlgd", QDir::Files, QDirIterator::Subdirectories);
//    while (it.hasNext())
//    {
//        qDebug() << it.next();
//    }

    QDirIterator it(ReliefsLegendsDefaultDir, QStringList() << "*.rlgd", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
//        QString fn = it.next();
        it.next();
        ui->cbLegends->addItem(it.fileName());
    }
//    ui->cbLegends->blockSignals(false);
}
//-------------------------------------------------------------

void FormRelief::on_cbLegends_currentIndexChanged(int index)
{
    if (index < 0)
    {
        qDebug() << __PRETTY_FUNCTION__ << " with index < 0 !!!";
        return;
    }

    QString fn = ReliefsLegendsDefaultDir + "/" + ui->cbLegends->itemText(index);
    qDebug() << fn;

    if (!LoadLegend(fn))
    {
        QMessageBox::critical(this, "Error", "Selected legend not loaded");
    }
}
//-------------------------------------------------------------

