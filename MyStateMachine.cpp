#include "MyStateMachine.h"

#include "ui_MainWindow.h"

MyStateMachine::MyStateMachine()
{

}

void MyStateMachine::InitStateMachine(Ui::MainWindow *ui)
{
    if (StateMachine.isRunning())
    {
        qDebug() << "StateMachine is already running";
        throw std::runtime_error("StateMachine is already running");
    }

    StateNothing = new QState(&StateMachine);
    StateNormal = new QState(&StateMachine);
    StateGradDesc = new QState(&StateMachine);
    StateShowAbonents = new QState(&StateMachine);
    StateNewRoute = new QState(&StateMachine);
    StateCurPosOrAngleEditting = new QState(&StateMachine);

    StateNothing->assignProperty(ui->actionFileNew_Grad_Config, "enabled", true);
    StateNothing->assignProperty(ui->actionFileOpen_Grad_Descent, "enabled", true);
    StateNothing->assignProperty(ui->actionFileSave_Grad_Config, "enabled", false);
    StateNothing->assignProperty(ui->actionFileSave_Grad_Config_As, "enabled", false);
    StateNothing->assignProperty(ui->actionWorld_Show_Abonents, "enabled", false);
    StateNothing->assignProperty(ui->actionWorld_Show_Grid, "enabled", false);
    StateNothing->assignProperty(ui->actionGradSwitch_Show_One_All, "enabled", false);
    StateNothing->assignProperty(ui->menuGrad_Count_of_Viewports, "enabled", false);
    StateNothing->assignProperty(ui->actionGrad_SetDrawCount_Custom, "enabled", false);
    StateNothing->assignProperty(ui->actionGradSwitch_Pespective_for_Current, "enabled", false);
    StateNothing->assignProperty(ui->actionGradSwitch_Pespective_for_All, "enabled", false);
    StateNothing->assignProperty(ui->actionGradReset_View_Point_for_Current, "enabled", false);
    StateNothing->assignProperty(ui->actionGradReset_View_Point_for_All, "enabled", false);
    StateNothing->assignProperty(ui->actionGradChange_Population_size, "enabled", false);
    StateNothing->assignProperty(ui->actionGradSettings, "enabled", false);
    StateNothing->assignProperty(ui->actionGradCalc_Bonds, "enabled", false);
    StateNothing->assignProperty(ui->actionGradStart_Phase_1, "enabled", false);
    StateNothing->assignProperty(ui->actionGradStart_Phase_1_for_Current_Config, "enabled", false);
    StateNothing->assignProperty(ui->actionGradRemove_Uncovered, "enabled", false);
    StateNothing->assignProperty(ui->actionGradStart_Phase_2, "enabled", false);
    StateNothing->assignProperty(ui->actionGradStart_Phase_2_for_Current_Config, "enabled", false);
    StateNothing->assignProperty(ui->actionGradStop, "enabled", false);
    StateNothing->assignProperty(ui->actionGradShow_Tables, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Add_New_Route, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Finish_Route, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Delete_Route, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Select_Cur_Node, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Editing_Pos_Cur_Node, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Editing_Angle_Cur_Node, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_All, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_Current, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Edit_Routes, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Change_Count_of_Nodes, "enabled", false);
    StateNothing->assignProperty(ui->actionEdit_Apply_Cur_Node_to_All_Configs, "enabled", false);
    StateNothing->assignProperty(ui->actionDebug_TwoLines, "enabled", false);
    StateNothing->assignProperty(ui->actionDebug_Get_Last_Cost_For_Current, "enabled", false);
    StateNothing->assignProperty(ui->actionDebug_Calc_Access_Rate_for_current, "enabled", false);

    StateMachine.setInitialState(StateNothing);

    StateNormal->assignProperty(ui->actionFileNew_Grad_Config, "enabled", true);
    StateNormal->assignProperty(ui->actionFileOpen_Grad_Descent, "enabled", true);
    StateNormal->assignProperty(ui->actionFileSave_Grad_Config, "enabled", true);
    StateNormal->assignProperty(ui->actionFileSave_Grad_Config_As, "enabled", true);
    StateNormal->assignProperty(ui->actionWorld_Show_Abonents, "enabled", true);
    StateNormal->assignProperty(ui->actionWorld_Show_Grid, "enabled", true);
    StateNormal->assignProperty(ui->actionGradSwitch_Show_One_All, "enabled", true);
    StateNormal->assignProperty(ui->menuGrad_Count_of_Viewports, "enabled", true);
    StateNormal->assignProperty(ui->actionGrad_SetDrawCount_Custom, "enabled", true);
    StateNormal->assignProperty(ui->actionGradSwitch_Pespective_for_Current, "enabled", true);
    StateNormal->assignProperty(ui->actionGradSwitch_Pespective_for_All, "enabled", true);
    StateNormal->assignProperty(ui->actionGradReset_View_Point_for_Current, "enabled", true);
    StateNormal->assignProperty(ui->actionGradReset_View_Point_for_All, "enabled", true);
    StateNormal->assignProperty(ui->actionGradChange_Population_size, "enabled", true);
    StateNormal->assignProperty(ui->actionGradSettings, "enabled", true);
    StateNormal->assignProperty(ui->actionGradCalc_Bonds, "enabled", true);
    StateNormal->assignProperty(ui->actionGradStart_Phase_1, "enabled", true);
    StateNormal->assignProperty(ui->actionGradStart_Phase_1_for_Current_Config, "enabled", true);
    StateNormal->assignProperty(ui->actionGradRemove_Uncovered, "enabled", true);
    StateNormal->assignProperty(ui->actionGradStart_Phase_2, "enabled", true);
    StateNormal->assignProperty(ui->actionGradStart_Phase_2_for_Current_Config, "enabled", true);
    StateNormal->assignProperty(ui->actionGradStop, "enabled", false);
    StateNormal->assignProperty(ui->actionGradShow_Tables, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Add_New_Route, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Finish_Route, "enabled", false);
    StateNormal->assignProperty(ui->actionEdit_Delete_Route, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Select_Cur_Node, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Editing_Pos_Cur_Node, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Editing_Angle_Cur_Node, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_All, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_Current, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Edit_Routes, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Change_Count_of_Nodes, "enabled", true);
    StateNormal->assignProperty(ui->actionEdit_Apply_Cur_Node_to_All_Configs, "enabled", true);
    StateNormal->assignProperty(ui->actionDebug_TwoLines, "enabled", true);
    StateNormal->assignProperty(ui->actionDebug_Get_Last_Cost_For_Current, "enabled", true);
    StateNormal->assignProperty(ui->actionDebug_Calc_Access_Rate_for_current, "enabled", true);

//    StateNothing->addTransition(ui->actionFileOpen_Grad_Descent, SIGNAL(triggered()), StateNormal);
    StateNothing->addTransition(this, SIGNAL(SignalStateToNormal()), StateNormal);


    StateGradDesc->assignProperty(ui->actionFileNew_Grad_Config, "enabled", false);
    StateGradDesc->assignProperty(ui->actionFileOpen_Grad_Descent, "enabled", false);
    StateGradDesc->assignProperty(ui->actionFileSave_Grad_Config, "enabled", true);
    StateGradDesc->assignProperty(ui->actionFileSave_Grad_Config_As, "enabled", true);
    StateGradDesc->assignProperty(ui->actionWorld_Show_Abonents, "enabled", false);
    StateGradDesc->assignProperty(ui->actionWorld_Show_Grid, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradSwitch_Show_One_All, "enabled", true);
    StateGradDesc->assignProperty(ui->menuGrad_Count_of_Viewports, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGrad_SetDrawCount_Custom, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradSwitch_Pespective_for_Current, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradSwitch_Pespective_for_All, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradReset_View_Point_for_Current, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradReset_View_Point_for_All, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradChange_Population_size, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradSettings, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradCalc_Bonds, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradStart_Phase_1, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradStart_Phase_1_for_Current_Config, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradRemove_Uncovered, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradStart_Phase_2, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradStart_Phase_2_for_Current_Config, "enabled", false);
    StateGradDesc->assignProperty(ui->actionGradStop, "enabled", true);
    StateGradDesc->assignProperty(ui->actionGradShow_Tables, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Add_New_Route, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Finish_Route, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Delete_Route, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Select_Cur_Node, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Editing_Pos_Cur_Node, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Editing_Angle_Cur_Node, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_All, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_Current, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Edit_Routes, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Change_Count_of_Nodes, "enabled", false);
    StateGradDesc->assignProperty(ui->actionEdit_Apply_Cur_Node_to_All_Configs, "enabled", false);
    StateGradDesc->assignProperty(ui->actionDebug_TwoLines, "enabled", false);
    StateGradDesc->assignProperty(ui->actionDebug_Get_Last_Cost_For_Current, "enabled", false);
    StateGradDesc->assignProperty(ui->actionDebug_Calc_Access_Rate_for_current, "enabled", false);

//    StateNormal->addTransition(ui->actionGradStart_Phase_1_for_Current_Config, SIGNAL(triggered()), StateGradDesc);
//    StateNormal->addTransition(ui->actionGradStart_Phase_2_for_Current_Config, SIGNAL(triggered()), StateGradDesc);
//    StateNormal->addTransition(ui->actionGradStart_Phase_1, SIGNAL(triggered()), StateGradDesc);
//    StateNormal->addTransition(ui->actionGradStart_Phase_2, SIGNAL(triggered()), StateGradDesc);

    StateNormal->addTransition(this, SIGNAL(SignalStateToGradDesc()), StateGradDesc);
    StateGradDesc->addTransition(this, SIGNAL(SignalStateToNormal()), StateNormal);

    StateShowAbonents->assignProperty(ui->actionFileNew_Grad_Config, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionFileOpen_Grad_Descent, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionFileSave_Grad_Config, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionFileSave_Grad_Config_As, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionWorld_Show_Abonents, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionWorld_Show_Grid, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGradSwitch_Show_One_All, "enabled", true);
    StateShowAbonents->assignProperty(ui->menuGrad_Count_of_Viewports, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGrad_SetDrawCount_Custom, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGradSwitch_Pespective_for_Current, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGradSwitch_Pespective_for_All, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGradReset_View_Point_for_Current, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGradReset_View_Point_for_All, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionGradChange_Population_size, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradSettings, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradCalc_Bonds, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradStart_Phase_1, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradStart_Phase_1_for_Current_Config, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradRemove_Uncovered, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradStart_Phase_2, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradStart_Phase_2_for_Current_Config, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradStop, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionGradShow_Tables, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionEdit_Add_New_Route, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Finish_Route, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Delete_Route, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Select_Cur_Node, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Editing_Pos_Cur_Node, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Editing_Angle_Cur_Node, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_All, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_Current, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Edit_Routes, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Change_Count_of_Nodes, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionEdit_Apply_Cur_Node_to_All_Configs, "enabled", false);
    StateShowAbonents->assignProperty(ui->actionDebug_TwoLines, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionDebug_Get_Last_Cost_For_Current, "enabled", true);
    StateShowAbonents->assignProperty(ui->actionDebug_Calc_Access_Rate_for_current, "enabled", true);

    StateNormal->addTransition(this, SIGNAL(SignalStateToShowAbonents()), StateShowAbonents);
    StateShowAbonents->addTransition(this, SIGNAL(SignalStateToNormal()), StateNormal);

    StateNewRoute->assignProperty(ui->actionFileNew_Grad_Config, "enabled", false);
    StateNewRoute->assignProperty(ui->actionFileOpen_Grad_Descent, "enabled", false);
    StateNewRoute->assignProperty(ui->actionFileSave_Grad_Config, "enabled", true);
    StateNewRoute->assignProperty(ui->actionFileSave_Grad_Config_As, "enabled", true);
    StateNewRoute->assignProperty(ui->actionWorld_Show_Abonents, "enabled", false);
    StateNewRoute->assignProperty(ui->actionWorld_Show_Grid, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGradSwitch_Show_One_All, "enabled", true);
    StateNewRoute->assignProperty(ui->menuGrad_Count_of_Viewports, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGrad_SetDrawCount_Custom, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGradSwitch_Pespective_for_Current, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGradSwitch_Pespective_for_All, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGradReset_View_Point_for_Current, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGradReset_View_Point_for_All, "enabled", true);
    StateNewRoute->assignProperty(ui->actionGradChange_Population_size, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradSettings, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradCalc_Bonds, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradStart_Phase_1, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradStart_Phase_1_for_Current_Config, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradRemove_Uncovered, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradStart_Phase_2, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradStart_Phase_2_for_Current_Config, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradStop, "enabled", false);
    StateNewRoute->assignProperty(ui->actionGradShow_Tables, "enabled", true);
    StateNewRoute->assignProperty(ui->actionEdit_Add_New_Route, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Finish_Route, "enabled", true);
    StateNewRoute->assignProperty(ui->actionEdit_Delete_Route, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Select_Cur_Node, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Editing_Pos_Cur_Node, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Editing_Angle_Cur_Node, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_All, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_Current, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Edit_Routes, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Change_Count_of_Nodes, "enabled", false);
    StateNewRoute->assignProperty(ui->actionEdit_Apply_Cur_Node_to_All_Configs, "enabled", false);
    StateNewRoute->assignProperty(ui->actionDebug_TwoLines, "enabled", true);
    StateNewRoute->assignProperty(ui->actionDebug_Get_Last_Cost_For_Current, "enabled", true);
    StateNewRoute->assignProperty(ui->actionDebug_Calc_Access_Rate_for_current, "enabled", true);

    StateNormal->addTransition(this, SIGNAL(SignalStateToAddNewRoute()), StateNewRoute);
    StateNewRoute->addTransition(this, SIGNAL(SignalStateToNormal()), StateNormal);


    StateCurPosOrAngleEditting->assignProperty(ui->actionFileNew_Grad_Config, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionFileOpen_Grad_Descent, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionFileSave_Grad_Config, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionFileSave_Grad_Config_As, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionWorld_Show_Abonents, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionWorld_Show_Grid, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradSwitch_Show_One_All, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->menuGrad_Count_of_Viewports, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGrad_SetDrawCount_Custom, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradSwitch_Pespective_for_Current, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradSwitch_Pespective_for_All, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradReset_View_Point_for_Current, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradReset_View_Point_for_All, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradChange_Population_size, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradSettings, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradCalc_Bonds, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradStart_Phase_1, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradStart_Phase_1_for_Current_Config, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradRemove_Uncovered, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradStart_Phase_2, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradStart_Phase_2_for_Current_Config, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradStop, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionGradShow_Tables, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Add_New_Route, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Finish_Route, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Delete_Route, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Select_Cur_Node, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Editing_Pos_Cur_Node, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Editing_Angle_Cur_Node, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_All, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Edit_Signal_Nodes_for_Current, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Edit_Routes, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Change_Count_of_Nodes, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionEdit_Apply_Cur_Node_to_All_Configs, "enabled", false);
    StateCurPosOrAngleEditting->assignProperty(ui->actionDebug_TwoLines, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionDebug_Get_Last_Cost_For_Current, "enabled", true);
    StateCurPosOrAngleEditting->assignProperty(ui->actionDebug_Calc_Access_Rate_for_current, "enabled", true);


    StateNormal->addTransition(this, SIGNAL(SignalStateToCurPosOrAngleEditing()), StateCurPosOrAngleEditting);
    StateCurPosOrAngleEditting->addTransition(this, SIGNAL(SignalStateToNormal()), StateNormal);

    StateMachine.start();
}
