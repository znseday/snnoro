#ifndef TARGETFUNCTIONSSECONDPHASE_H
#define TARGETFUNCTIONSSECONDPHASE_H

#include "TargetFunctionBase.h"

class TargetFuncAdditiveSphereSecondPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
    ~TargetFuncAdditiveSphereSecondPhase(); // just to be sure
};
//-------------------------------------------------------------


class TargetFuncProbabilisticSphereSecondPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
};
//-------------------------------------------------------------


class TargetFuncAdditiveConeSecondPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
};
//-------------------------------------------------------------


class TargetFuncProbabilisticConeSecondPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
};

#endif // TARGETFUNCTIONSSECONDPHASE_H
