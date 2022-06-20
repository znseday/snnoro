#ifndef TARGETFUNCTIONSFIRSTPHASE_H
#define TARGETFUNCTIONSFIRSTPHASE_H

#include "TargetFunctionBase.h"

class TargetFuncAdditiveSphereFirstPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
    ~TargetFuncAdditiveSphereFirstPhase(); // just to be sure
};
//-------------------------------------------------------------


class TargetFuncProbabilisticSphereFirstPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
};
//-------------------------------------------------------------


class TargetFuncAdditiveConeFirstPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
};
//-------------------------------------------------------------


class TargetFuncProbabilisticConeFirstPhase : public TargetFunctionBase
{
public:
    void Init(MyConfig *_myConfig) override;
    double operator()(const std::vector<double> &params) const override;
};
//-------------------------------------------------------------


#endif // TARGETFUNCTIONSFIRSTPHASE_H
