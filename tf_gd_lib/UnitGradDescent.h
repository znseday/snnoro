//          Copyright Sergey Tsynikin 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

//---------------------------------------------------------------------------
#ifndef UnitGradDescentH
#define UnitGradDescentH
//---------------------------------------------------------------------------

#include <chrono>
#include <vector>

#include "UnitTableFunctions.h"

namespace tf_gd_lib
{

using DstFunctionType = std::function<long double(long double x, const std::vector<long double>&)>;
using CallbackType = std::function<void()>;

using UserTargetFunctionType = std::function<long double(const std::vector<long double>&)>;

using ClockType = std::chrono::steady_clock;

enum class GradErrorType
{
	Success,
	VectorSizesNotTheSame,
	CanceledByUser,
	TimeOut,
	ItersOverflow
};

class GradDescent
{
private:
protected:

    long double Min_Eta = 1e-6;
    long double Eta_k_inc = 1.1;
    long double Eta_k_dec = 2.0;
    long double Eta_FirstJump = 10.0;
    long double Alpha = 0.25;
    long double MaxTime = 20;

    long double LastCost = -1.0;
    long double LastTime = -1.0;

    long double Eps = 0.000001;

    std::vector<long double> Cur_Eta;
    std::vector<long double> dCost_dp;

    std::vector<long double> Params;
    std::vector<long double> MinConstrains;
    std::vector<long double> MaxConstrains;
    std::vector<long double> RelConstrains;

	std::vector<bool>   TypeConstrains;

    size_t MaxIters = 15000;
	size_t CallBackFreq = 10;
    size_t LastIters = 0;

	TableFunction SrcFunction;
	DstFunctionType DstFunction = nullptr; // is it ok to use nullptr for std::function?
	UserTargetFunctionType UserTargetFunction = nullptr;
	
	bool IsUseUserTargetFunction = false;
    bool IsCalculating = false;
    bool FinDifMethod = true;

	std::chrono::time_point<ClockType> TimeStart, TimeEnd; // default values?

	CallbackType Callback = nullptr;

    void CalcCost();

public:
	GradDescent() = default;
	~GradDescent() = default;

    GradDescent(const GradDescent&) = default;
    GradDescent(GradDescent&&) = default;

    GradDescent& operator=(const GradDescent&) = default;
    GradDescent& operator=(GradDescent&&) = default;

    void SetMin_Eta(long double _Min_Eta)             { Min_Eta = _Min_Eta; }
    //void SetCur_Eta(std::vector<long double>)       {;} // read only
    void SetEta_k_inc(long double _Eta_k_inc)         { Eta_k_inc = _Eta_k_inc; }
    void SetEta_k_dec(long double _Eta_k_dec)         { Eta_k_dec = _Eta_k_dec; }
    void SetEta_FirstJump(long double _Eta_FirstJump) { Eta_FirstJump = _Eta_FirstJump; }
    void SetAlpha(long double _Alpha)                 { Alpha = _Alpha; }
	void SetMaxIters(size_t _MaxIters)           { MaxIters = _MaxIters; }
    void SetMaxTime(long double _MaxTime)             { MaxTime = _MaxTime; }
	void SetCallBackFreq(size_t _CallBackFreq)   { CallBackFreq = _CallBackFreq; }

    long double GetMin_Eta() const                      { return Min_Eta; }
    const std::vector<long double> & GetCur_Eta() const { return Cur_Eta; }

    const std::vector<long double> & Get_dCost_dp() const { return dCost_dp; }

    long double GetEta_k_inc() const            { return Eta_k_inc; }
    long double GetEta_k_dec() const            { return Eta_k_dec; }
    long double GetEta_FirstJump() const        { return Eta_FirstJump; }
    long double GetAlpha() const                { return Alpha; }
	size_t GetMaxIters() const             { return MaxIters; }
    long double GetMaxTime() const              { return MaxTime; }
	size_t GetCallBackFreq() const         { return CallBackFreq; }

    void SetParams(const std::vector<long double>& _Params) { Params = _Params; }
    void SetMinConstrains(const std::vector<long double>& _MinConstrains) { MinConstrains = _MinConstrains; }
    void SetMaxConstrains(const std::vector<long double>& _MaxConstrains) { MaxConstrains = _MaxConstrains; }
    void SetRelConstrains(const std::vector<long double>& _RelConstrains) { RelConstrains = _RelConstrains; }
	void SetTypeConstrains(const std::vector<bool>& _TypeConstrains) { TypeConstrains = _TypeConstrains; }

    const std::vector<long double> & GetParams() const         { return Params; }
    const std::vector<long double> & GetMinConstrains() const  { return MinConstrains; }
    const std::vector<long double> & GetMaxConstrains() const  { return MaxConstrains; }
    const std::vector<long double> & GetRelConstrains() const  { return RelConstrains; }
    const std::vector<bool>   & GetTypeConstrains() const { return TypeConstrains; }

	void SetSrcFunction(const TableFunction& _SrcFunction) { SrcFunction = _SrcFunction; }

	// to do: consider perfect forwarding?
	void SetDstFunction(const DstFunctionType& _DstFunction) { DstFunction = _DstFunction; }
	//DstFunctionType GetDstFunction() {return DstFunction;} // write only

	void SetCallback(const CallbackType& _Callback) { Callback = _Callback; }

	void SetUseUserTargetFunction(const UserTargetFunctionType& _UserTargetFunction) { UserTargetFunction = _UserTargetFunction; }
	void SetIsUseUserTargetFunction(bool _IsUseUserTargetFunction) { IsUseUserTargetFunction = _IsUseUserTargetFunction; }
	
    long double GetY(size_t i) const
	{
		if (DstFunction && SrcFunction.Size())
			return DstFunction(SrcFunction.GetX(i), Params);
		else
			return 0;
	}

    long double GetRandomY(long double x) const
	{
		if (DstFunction)
			return DstFunction(x, Params);
		else
			return 0;
	}

    long double GetLastCost() const { return LastCost; }

	GradErrorType Go();

	void Stop() { IsCalculating = false; }

	size_t GetLastIters() const { return LastIters; }
    long double GetLastTime() const { return LastTime; }

    void SetEps(long double _eps) { Eps = _eps; }
    long double GetEps() const { return Eps; }

	void SetFinDifMethod(bool _FinDifMethod) { FinDifMethod = _FinDifMethod; }
	bool GetFinDifMethod() const { return FinDifMethod; }
};


}

#endif
