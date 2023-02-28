#ifndef MYSTATEMACHINE_H
#define MYSTATEMACHINE_H

#include <QObject>
#include <QStateMachine>

namespace Ui {
class MainWindow;
}

class MyStateMachine : QObject
{
    Q_OBJECT
private:

    QStateMachine StateMachine;
    QState *StateNothing;
    QState *StateNormal;
    QState *StateGradDesc;
    QState *StateShowAbonents;
    QState *StateNewRoute;
    QState *StateCurPosOrAngleEditting;

public:
    MyStateMachine();

    void InitStateMachine(Ui::MainWindow *ui);

    void ToNormal()               {emit SignalStateToNormal();}
    void ToGradDesc()             {emit SignalStateToGradDesc();}
    void ToShowAbonents()         {emit SignalStateToShowAbonents();}
    void ToAddNewRoute()          {emit SignalStateToAddNewRoute();}
    void ToCurPosOrAngleEditing() {emit SignalStateToCurPosOrAngleEditing();}

    bool IsRunning() const {return StateMachine.isRunning();}

signals:

    void SignalStateToNormal();
    void SignalStateToGradDesc();
    void SignalStateToShowAbonents();
    void SignalStateToAddNewRoute();
    void SignalStateToCurPosOrAngleEditing();
};

#endif // MYSTATEMACHINE_H
