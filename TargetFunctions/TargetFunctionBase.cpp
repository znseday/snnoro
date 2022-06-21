#include "TargetFunctionBase.h"
#include "Types.h"

void TargetFunctionBase::InitMembersByGlobalSettings(const TargetFuncSettingsStruct &_targetFuncSettings)
{
    Aarf = _targetFuncSettings.Aarf;
    A2 = _targetFuncSettings.A2;
    p = _targetFuncSettings.p;
    offX = _targetFuncSettings.offX;
    k_step_ot = _targetFuncSettings.k_step_ot;
    R_nodeOverlap = _targetFuncSettings.R_nodeOverlap;
    IsUseCoveredFlag = _targetFuncSettings.IsUseCoveredFlag;
    IsUseLineBetweenTwoPoints = _targetFuncSettings.IsUseLineBetweenTwoPoints;
}
