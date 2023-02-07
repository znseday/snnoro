#ifndef FORMRELIEF_H
#define FORMRELIEF_H

#include <QMainWindow>

#include <QImage>
#include <QLabel>
//#include <QTableWidgetItem>
class QTableWidgetItem;

#include <QColorDialog>

class QMouseEvent;

#include "Relief.h"
//#include <vector>
//#include <stack>

struct rgbaType
{
    unsigned char b = 0, g = 0, r = 0, a = 0;
    rgbaType() = default;
    rgbaType(unsigned char _b, unsigned char _g, unsigned char _r, unsigned char _a) : b(_b), g(_g), r(_r), a(_a) {}
};

using CorolAndZ_pair = std::pair<rgbaType, int>;

struct LegendColorType
{
    std::vector<CorolAndZ_pair> Colors;
    double averSimilarity = 0;
};
//-------------------------------------------------------------

class MyPicSrcWidget : public QLabel
{
    Q_OBJECT
private:
    int OldX, OldY;
    QRect FrameRect;
    const QImage &ImgSrc;
    bool IsMouseDown = false;

signals:
    void SignalSendRectFrame(QRect);
    void SignalSendChangePoint(int, int);

protected:
    void mousePressEvent(QMouseEvent *pe);
    void mouseReleaseEvent(QMouseEvent *pe);
    void mouseMoveEvent(QMouseEvent *pe);
    void paintEvent(QPaintEvent *pe);

public:
    MyPicSrcWidget(const QString &text, const QImage &_imgSrc, QWidget *parent = nullptr) : QLabel(text, parent), ImgSrc(_imgSrc) {};
};
//-------------------------------------------------------------

class MyPicDstWidget : public QLabel
{
    Q_OBJECT
private:
    int OldX, OldY;
    const QImage &ImgDst;
    bool IsMouseDown = false;

signals:
    void SignalSendChangePoint(int, int);

protected:
    void mousePressEvent(QMouseEvent *pe);
//    void mouseReleaseEvent(QMouseEvent *pe);
//    void mouseMoveEvent(QMouseEvent *pe);
    void paintEvent(QPaintEvent *pe);

public:
    MyPicDstWidget(const QString &text, const QImage &_imgDst, QWidget *parent = nullptr) : QLabel(text, parent), ImgDst(_imgDst) {};
};
//-------------------------------------------------------------
//-------------------------------------------------------------

namespace Ui {
class FormRelief;
}

class FormRelief : public QMainWindow
{
    Q_OBJECT

public:
    explicit FormRelief(QWidget *parent = nullptr);
    ~FormRelief();

private slots:
    void on_actionFile_Open_Image_triggered();

    void on_btnApply_clicked();

    void on_actionFile_Close_triggered();

    void SlotColorDblClicked(QTableWidgetItem *pItem);
    void SlotColorCellPressed(int r, int c);

    void on_actionFile_Save_Legend_As_triggered();

    void on_actionFile_Load_Legend_triggered();

    void on_chbColorToLegend_stateChanged(int arg1);

    void on_actionRelief_Calc_Discrete_Img_triggered();
    void on_actionRelief_Calc_Relief_And_Save_As_triggered();

    void on_actionFile_Open_Relief_triggered();

    void on_tableColors_itemChanged(QTableWidgetItem *item);

    void on_actionRelief_Undo_triggered();

    void on_actionRelief_Redo_triggered();

    void on_actionLegend_Undo_triggered();

    void on_actionLegend_Redo_triggered();

    void on_tableColors_cellChanged(int row, int column);

private:
    Ui::FormRelief *ui;

    QWidget *wgtForScrollArea;
    QImage ImgReliefSrc;
    QImage ImgReliefDst;

    QString ImageSrcFileName;
    QString LegendFileName;

    MyPicSrcWidget *lblPicSrc;
    MyPicDstWidget *lblPicDst;

    QColorDialog dlgColor;

    int LastSelectedColorRow = -1;

    int OldRowCount = 0;
    LegendColorType LegendColor;

    bool IsLegendReadyToBeSavedToTemp = false;
    std::list<LegendColorType> LegendColorList; // for Undo/Redo  // Подумать - возможно сделать массив

    void SaveCurrentLegendToTemp();
    void UndoLegendChanges();

    void CalcLegendColor();
    int FindNearestColorIndex(int r, int g, int b);

    CorolAndZ_pair AnalyseImageAreaForZ(int xStart, int yStart, int xEnd, int yEnd);
    rgbaType AnalyseImageAreaForColor(int xStart, int yStart, int xEnd, int yEnd);

    Relief3D Relief;

    std::vector<std::vector<CorolAndZ_pair>> TempGrid;

    bool IsLegendSaved = true;
    bool IsReliefSaved = true;

    bool LoadSrcImage(const QString &_fn);
    bool LoadLegend(const QString &_fn);

    void CalcDiscreteImgByExistentRelief();

    void PrintImgReliefDstFromTempGrid();

    bool CheckIsLegendSavedAndSaveIfNecessary();
    bool CheckIsReliefSavedAndSaveIfNecessary();

public slots:

    void SlotReceiveRectFrame(QRect _rect);
    void SlotReceiveChangePoint(int x, int y);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // FORMRELIEF_H
