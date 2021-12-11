//          Copyright Sergey Tsynikin 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

//---------------------------------------------------------------------------
#ifndef UnitSplineH
#define UnitSplineH
//---------------------------------------------------------------------------

#include <vector>

namespace tf_gd_lib
{

struct SinglePoint
{
	double x = 0;
	double y = 0;
	SinglePoint(double _x, double _y) : x(_x), y(_y) {}
};

class CubicSpline
{
private:

	struct SplinePart
	{
		double a, b, c, d, x;
	};

	std::vector<SplinePart> Splines;

public:

	bool BuildSpline(const std::vector<SinglePoint>& Points);

	double operator()(double x) const;

	bool IsSplineExists() const { return !Splines.empty(); }

	void Clear() { Splines.clear(); };
	void ClearAndRelease() { Splines.clear(); Splines.shrink_to_fit(); };
};


} // namespace

#endif
