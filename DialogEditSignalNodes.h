#ifndef DIALOGEDITSIGNALNODES_H
#define DIALOGEDITSIGNALNODES_H

#include <QDialog>

#include "SignalNode.h"
#include "Relief.h"

namespace Ui {
class DialogEditSignalNodes;
}

class DialogEditSignalNodes : public QDialog
{
    Q_OBJECT

private:

    // Подумать: сделать указатель на вектор сигнальных узлов ?
    // Сделать мембор с текем состояние таблицы (конусы или сферы)

public:
    explicit DialogEditSignalNodes(QWidget *parent = nullptr);
    ~DialogEditSignalNodes();

    void InitDialog_ForAll(SignalNodeType _snt, const std::vector<SignalNode> &_signalNodes);

    void InitDialog_ForCurrent(SignalNodeType _snt, const std::vector<SignalNode> &_signalNodes,
                               const Relief3D & _relief);

    void ChangeSignalNodesParameters_ForAll(SignalNodeType _snt, std::vector<SignalNode> &_signalNodes);
    void ChangeSignalNodesParameters_ForCurrent(SignalNodeType _snt, std::vector<SignalNode> &_signalNodes,
                                                const Relief3D & _relief);

private:
    Ui::DialogEditSignalNodes *ui;

private slots:
    void SlotLoadButtonClicked();
    void SlotSaveButtonClicked();
};

#endif // DIALOGEDITSIGNALNODES_H
