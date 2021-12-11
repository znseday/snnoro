//          Copyright Sergey Tsynikin 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

//---------------------------------------------------------------------------
#ifndef UnitTableFunctionsH
#define UnitTableFunctionsH
//---------------------------------------------------------------------------

#include <vector>
#include <functional>
#include <cmath>
#include <string>

#include "UnitSpline.h"

namespace tf_gd_lib
{

inline double LineInterpol(double x, double x1, double x2, double y1, double y2);
inline double LineInterpolSafeMiddleVal(double x, double x1, double x2, double y1, double y2);

class TableFunction
{
private:
protected:

	std::vector<SinglePoint> Points;

	double MinX = 0.0, MaxX = 0.0;
	double MinY = 0.0, MaxY = 0.0;

	double x_ForMinY = 0.0, x_ForMaxY = 0.0;
	size_t i_ForMinY = 0, i_ForMaxY = 0;

	std::string Name;

	mutable size_t iCache = 0;

public:
	TableFunction() = default;
	~TableFunction() = default;

	TableFunction(const TableFunction&) = default;
	TableFunction& operator=(const TableFunction&) = default;

	TableFunction(TableFunction&&) = default;
	TableFunction& operator=(TableFunction&&) = default;


	size_t Size() const { return Points.size(); }

	double GetX(size_t i) const { iCache = i; return Points[i].x; };
	double GetY(size_t i) const { iCache = i; return Points[i].y; };

	std::tuple<double &, double &> operator[](size_t i);

	void SetPointByNumber(size_t i, const std::tuple<double, double>& point);
	void SetValAtPoint(size_t i, double y);

	double GetMinX() const { return MinX; };
	double GetMaxX() const { return MaxX; };

	double GetMinY() const { return MinY; };
	double GetMaxY() const { return MaxY; };

	double Get_x_ForMinY() const { return x_ForMinY; };
	double Get_x_ForMaxY() const { return x_ForMaxY; };

	size_t Get_i_ForMinY() const { return i_ForMinY; };
	size_t Get_i_ForMaxY() const { return i_ForMaxY; };

	void SetName(const std::string& name) { Name = name; }
	std::string GetName() const { return Name; }

	double GetValByBSearchFromX(double x) const;
	double operator()(double x) const { return GetValByBSearchFromX(x); }

	double GetValFromRightX(double x) const;
	double GetValFromLeftX(double x) const;

	void ClearAll();

	void CreateNewFunction(size_t n, const std::string& _name = "NewFunc");
	void SetPoint(size_t i, double x, double y) { Points[i].x = x; Points[i].y = y; }

	static double TestFunc(double x) { return std::sin(10.0 * x); }

	void CreateDemoFunction(size_t n, double a, double dx,
		std::function<double(double)> f = TestFunc /* = std::sin*/, const std::string& _name = "DemoFunc");	                                                   

	void KillDuplicates();
	void Sort();
	void CalcStat();

	bool LoadFromFile(const std::string& FileName);
	void LoadFromStream(std::istream& Stream);

	double GetBackX() { return Points.back().x; }

	CubicSpline Spline;
	bool BuildSpline();

};
//---------------------------------------------------------------------------

} // namespace

#endif
