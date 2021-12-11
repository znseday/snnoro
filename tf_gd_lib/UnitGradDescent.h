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

using DstFunctionType = std::function<double(double x, const std::vector<double>&)>;
using CallbackType = std::function<void()>;

using UserTargetFunctionType = std::function<double(const std::vector<double>&)>;

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

	double Min_Eta = 1e-6;

	std::vector<double> Cur_Eta;

	double Eta_k_inc = 1.1;
	double Eta_k_dec = 2.0;
	double Eta_FirstJump = 10.0;

	double Alpha = 0.25;

	size_t MaxIters = 15000;
	double MaxTime = 20;

	std::vector<double> Params;
	std::vector<double> MinConstrains;
	std::vector<double> MaxConstrains;
	std::vector<double> RelConstrains;
	std::vector<bool>   TypeConstrains;

	size_t CallBackFreq = 10;

	TableFunction SrcFunction;
	DstFunctionType DstFunction = nullptr; // is it ok to use nullptr for std::function?
	UserTargetFunctionType UserTargetFunction = nullptr;
	
	bool IsUseUserTargetFunction = false;

	double LastCost = -1.0;

	size_t LastIters = 0;
	double LastTime = -1.0;

	void CalcCost();

	std::chrono::time_point<ClockType> TimeStart, TimeEnd; // default values?

	bool IsCalculating = false;

	double Eps = 0.000001;
	bool FinDifMethod = true;

	CallbackType Callback = nullptr;

public:
	GradDescent() = default;
	~GradDescent() = default;

    GradDescent(const GradDescent&) = default;
    GradDescent(GradDescent&&) = default;

    GradDescent& operator=(const GradDescent&) = default;
    GradDescent& operator=(GradDescent&&) = default;

	void SetMin_Eta(double _Min_Eta)             { Min_Eta = _Min_Eta; }
	//void SetCur_Eta(std::vector<double>)       {;} // read only
	void SetEta_k_inc(double _Eta_k_inc)         { Eta_k_inc = _Eta_k_inc; }
	void SetEta_k_dec(double _Eta_k_dec)         { Eta_k_dec = _Eta_k_dec; }
	void SetEta_FirstJump(double _Eta_FirstJump) { Eta_FirstJump = _Eta_FirstJump; }
	void SetAlpha(double _Alpha)                 { Alpha = _Alpha; }
	void SetMaxIters(size_t _MaxIters)           { MaxIters = _MaxIters; }
	void SetMaxTime(double _MaxTime)             { MaxTime = _MaxTime; }
	void SetCallBackFreq(size_t _CallBackFreq)   { CallBackFreq = _CallBackFreq; }

	double GetMin_Eta() const              { return Min_Eta; }
	std::vector<double> GetCur_Eta() const { return Cur_Eta; }
	double GetEta_k_inc() const            { return Eta_k_inc; }
	double GetEta_k_dec() const            { return Eta_k_dec; }
	double GetEta_FirstJump() const        { return Eta_FirstJump; }
	double GetAlpha() const                { return Alpha; }
	size_t GetMaxIters() const             { return MaxIters; }
	double GetMaxTime() const              { return MaxTime; }
	size_t GetCallBackFreq() const         { return CallBackFreq; }

	void SetParams(const std::vector<double>& _Params) { Params = _Params; }
	void SetMinConstrains(const std::vector<double>& _MinConstrains) { MinConstrains = _MinConstrains; }
	void SetMaxConstrains(const std::vector<double>& _MaxConstrains) { MaxConstrains = _MaxConstrains; }
	void SetRelConstrains(const std::vector<double>& _RelConstrains) { RelConstrains = _RelConstrains; }
	void SetTypeConstrains(const std::vector<bool>& _TypeConstrains) { TypeConstrains = _TypeConstrains; }

	std::vector<double> GetParams() const         { return Params; }
	std::vector<double> GetMinConstrains() const  { return MinConstrains; }
	std::vector<double> GetMaxConstrains() const  { return MaxConstrains; }
	std::vector<double> GetRelConstrains() const  { return RelConstrains; }
	std::vector<bool>   GetTypeConstrains() const { return TypeConstrains; }


	void SetSrcFunction(const TableFunction& _SrcFunction) { SrcFunction = _SrcFunction; }

	// to do: consider perfect forwarding?
	void SetDstFunction(const DstFunctionType& _DstFunction) { DstFunction = _DstFunction; }
	//DstFunctionType GetDstFunction() {return DstFunction;} // write only

	void SetCallback(const CallbackType& _Callback) { Callback = _Callback; }

	void SetUseUserTargetFunction(const UserTargetFunctionType& _UserTargetFunction) { UserTargetFunction = _UserTargetFunction; }
	void SetIsUseUserTargetFunction(bool _IsUseUserTargetFunction) { IsUseUserTargetFunction = _IsUseUserTargetFunction; }
	
	double GetY(size_t i) const
	{
		if (DstFunction && SrcFunction.Size())
			return DstFunction(SrcFunction.GetX(i), Params);
		else
			return 0;
	}

	double GetRandomY(double x) const
	{
		if (DstFunction)
			return DstFunction(x, Params);
		else
			return 0;
	}

	double GetLastCost() const { return LastCost; }

	GradErrorType Go();

	void Stop() { IsCalculating = false; }

	size_t GetLastIters() const { return LastIters; }
	double GetLastTime() const { return LastTime; }

	void SetEps(double _eps) { Eps = _eps; }
        double GetEps() const { return Eps; }

	void SetFinDifMethod(bool _FinDifMethod) { FinDifMethod = _FinDifMethod; }
	bool GetFinDifMethod() const { return FinDifMethod; }
};


}

#endif
