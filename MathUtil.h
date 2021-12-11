#pragma once

#include <cmath>

#include <cstdlib>

//#include <Eigen/Dense>

//using Eigen::Vector2d;
//using Eigen::Vector3d;

struct MathUtil
{
	static double random(double a = 0, double b = 1)// ,большее разнообразие результатов случайных чисел
	{
		const double k = 0.0001;
		int v = rand() % 10000, u = rand() % 10001;

		return a + (b - a) * (k * v + k * k * u);   
	}

	static bool randomDecision(double p)//принятие решения в зависимости от вероятности р
	{
		return random() < p;
	}

	static bool randomDecision(double a, double b)//принимает решение высчитывая вероятность (отношение благоприятных исходов ко всем)
	{
		if (b <= 0) return false;
		return randomDecision(a / b);
	}

	static bool expRandomDecision(double h, int j)
	{
		return randomDecision(exp(-h * j));
	}

	static bool gaussianRandomDecision(double s, int j)
	{
		return randomDecision(exp(-j * j / (2 * s)));
	}

	static bool antiGaussianDecision(double s, int j)
	{
		return randomDecision(1 - exp(-j * j / (2 * s)));
	}

	static double clamp(double x, double a, double b)//если х за пределами диапазона возвращает ближайшую границу
	{
		if (x < a) return a;
		if (x > b) return b;
		return x;
	}

	static double clamp(double x, double b) // a = 0
	{
		return clamp(x, 0, b);
	}


	// Геометрия

//	static Vector3d _V_(const Vector2d& p, double z = 0)//конструирует 3-х мерный вектор на базе трехмерного
//	{
//		return Vector3d(p.x(), p.y(), z);
//	}

};
