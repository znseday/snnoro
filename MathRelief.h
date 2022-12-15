#ifndef MATHRELIEF_H
#define MATHRELIEF_H

#include <iostream>
#include <vector>

struct ReliefMatInfoStruct
{
    bool IsUseReliefRandomSeed = true;
    unsigned ReliefRandomSeed = 100;

    double A_r1 = 0.01;
    double A_r2 = 0.02;

    friend std::ostream & operator<<(std::ostream &s, const ReliefMatInfoStruct ob);
};



class MathRelief
{
protected:

//    std::vector<double> reliefA, reliefB, reliefH;
    std::vector<double> A, B, C, D, E, F;
    std::vector<double> X0, Y0;

    std::vector<double> S1, S2;
//    std::vector<double> S12;
    std::vector<double> ro;

public:
    MathRelief();

    void CreateMathRelief(const ReliefMatInfoStruct &_reliefCoeffs);
    double CalcNormZbyNormXY(double x, double y) const;
    double CalcRealZbyNormXY(double x, double y) const;
    void BuildReliefToGL(bool _is2d);
    void Clear();
    void CalcDeltaDiag();
};



#endif // MATHRELIEF_H
