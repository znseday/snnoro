#include "MathRelief.h"
#include "MathUtils.h"

#include <cmath>

using namespace std;

ostream & operator<<(ostream &s, const ReliefMatInfoStruct ob)
{
    s << "IsUseReliefRandomSeed = " << ob.IsUseReliefRandomSeed << endl;
    s << "ReliefRandomSeed = " << ob.ReliefRandomSeed << endl;
    s << "A_r1 = " << ob.A_r1 << endl;
    s << "A_r2 = " << ob.A_r2 << endl;
    return s;
}


MathRelief::MathRelief()
{

}

void MathRelief::CreateMathRelief(const ReliefMatInfoStruct &_reliefCoeffs)
{
    if (_reliefCoeffs.IsUseReliefRandomSeed)
        srand(_reliefCoeffs.ReliefRandomSeed);
    else
        srand((unsigned)time(nullptr));

    constexpr size_t nReliefs = 250;

    A.resize(nReliefs);
    B.resize(nReliefs);
    C.resize(nReliefs);
    D.resize(nReliefs);
    E.resize(nReliefs);
    F.resize(nReliefs);
    X0.resize(nReliefs);
    Y0.resize(nReliefs);

    S1.resize(nReliefs);
    S2.resize(nReliefs);
    //S12.resize(nReliefs);
    ro.resize(nReliefs);

    for (size_t i = 0; i < nReliefs; ++i)
    {
        A[i] = 0.2; SimpleRandom(0.5, 2.5);
        B[i] = SimpleRandom(0.4, 0.5);
        C[i] = SimpleRandom(0.4, 0.5);
        D[i] = SimpleRandom(0.4, 0.5);
        E[i] = SimpleRandom(0.4, 0.5);
        F[i] = SimpleRandom(0.4, 0.5);

        X0[i] = SimpleRandom(-1, 1);
        Y0[i] = SimpleRandom(-1, 1);

        S1[i] =  SimpleRandom(0.08, 0.12);
        S2[i] =  SimpleRandom(0.08, 0.12);
        //S12[i] =  SimpleRandom(0.6, 0.8);

        ro[i] = 0; // S12[i]/(S1[i]*S2[i]);
    }

//    A.emplace_back(0.5);
//    B.emplace_back(0.2);
//    C.emplace_back(0.9);
//    D.emplace_back(1.2);
//    E.emplace_back(1.2);
//    F.emplace_back(0.9);

//    X0.emplace_back(0);
//    Y0.emplace_back(0);

//    constexpr size_t nRelief = 12;
//    reliefA.resize(nRelief);
//    reliefB.resize(nRelief);
//    reliefH.resize(nRelief);

//    for (size_t i = 0; i < 4; i++)
//    {
////        double A   = SimpleRandom(_reliefCoeffs.A_r1, _reliefCoeffs.A_r2);
//        double A   = SimpleRandom(-1, -1);
//        double phi = SimpleRandom(0, 2 * M_PI);

//        reliefA[i] = A * cos(phi);
//        reliefB[i] = A * sin(phi);
//        reliefH[i] = SimpleRandom(150, 3300);
//    }

//    for (size_t i = 4; i < 8; i++)
//    {
//        double A   = SimpleRandom(0.0005, 0.002);
//        double phi = SimpleRandom(0, 2 * M_PI);

//        reliefA[i] = A * cos(phi);
//        reliefB[i] = A * sin(phi);
//        reliefH[i] = SimpleRandom(50, 250);
//    }

//    for (size_t i = 8; i < nRelief; i++)
//    {
//        double A   = SimpleRandom(0.004, 0.008);
//        double phi = SimpleRandom(0, 2 * M_PI);

//        reliefA[i] = A * cos(phi);
//        reliefB[i] = A * sin(phi);
//        reliefH[i] = SimpleRandom(75, 175);
//    }

//    CalcDeltaDiag();
//    srand((unsigned)time(0));
//    IsReliefCreated = true;
}
//----------------------------------------------------------

double MathRelief::CalcRealZbyNormXY(double x, double y) const
{
    double z = 0;

    for (size_t i = 0; i < A.size(); ++i)
    {
//        z += A[i]*(x-X0[i])*(x-X0[i]) + B[i]*(x-X0[i])*(y-Y0[i]) + C[i]*(y-Y0[i])*(y-Y0[i]) + D[i]*(x-X0[i]) + E[i]*(y-Y0[i]);// + F[i];
        z += A[i]*(1/(2*M_PI*S1[i]*S2[i]*sqrt(1-ro[i]*ro[i])))*exp(-0.5/(1-ro[i]*ro[i])*( (x-X0[i])*(x-X0[i])/(S1[i]*S1[i]) - ro[i]*2*(x-X0[i])*(y-Y0[i])/(S1[i]*S2[i]) + (y-Y0[i])*(y-Y0[i])/(S2[i]*S2[i]) ));
    }

    return z;
}
//----------------------------------------------------------

double MathRelief::CalcNormZbyNormXY(double x, double y) const
{
    (void)x; (void)y;
    return 0; // CalcRealZbyNormXY(x, y)/maxZ;
}
//----------------------------------------------------------

void MathRelief::BuildReliefToGL(bool _is2d)
{
    (void)_is2d;
/*
    int RowCount = 128;
    int ColCount = 128;

    const double aspect = Area.width()/Area.height();

    if (aspect > 1)
    {
        wInside = 2.0;
        hInside = wInside/aspect;
        xStartInside = -1.0;
        yStartInside = -1.0 + (2.0 - hInside)/2.0;
    }
    else
    {
        hInside = 2.0;
        wInside = hInside*aspect;
        yStartInside = -1.0;
        xStartInside = -1.0 + (2.0 - wInside)/2.0;
    }

    const double dx = wInside / (ColCount-1);
    const double dy = hInside / (RowCount-1);


    std::vector<QVector3D> oneRow(ColCount);
    std::vector<std::vector<QVector3D>> points(RowCount, oneRow);

    std::vector<QColor> oneRowColor(ColCount);
    std::vector<std::vector<QColor>> colors(RowCount, oneRowColor);

    if (!_is2d)
        glNewList(ReliefCompileList, GL_COMPILE);
    else
        glNewList(Relief2dCompileList, GL_COMPILE);

    maxZ = 1;
    minZ = numeric_limits<double>::max();
    double mz = -1;
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
        {
            points[i][j] = QVector3D(xStartInside + j * dx, yStartInside + i * dy, 0);

            if (points[i][j].x() < -1)
                qDebug() << "points[i][j].x() < -1";

            if (points[i][j].x() > 1)
                qDebug() << "points[i][j].x() > 1";

            if (points[i][j].y() < -1)
                qDebug() << "points[i][j].y() < -1";

            if (points[i][j].y() > 1)
                qDebug() << "points[i][j].y() > 1";


             points[i][j].setZ(CalcNormZbyNormXY(points[i][j].x(), points[i][j].y()));


            if (points[i][j].z() > mz)
                mz = points[i][j].z();

            if (points[i][j].z() < minZ)
                minZ = points[i][j].z();
        }
    }

    maxZ = mz; // meters

    for (int i = 0; i < RowCount; i++)
        for (int j = 0; j < ColCount; j++)
        {
            colors[i][j] = CalcColorByZ(points[i][j].z());
            //qDebug() << colors[i][j] ;
        }


    Global_kz = 2.0/max(Area.width(), Area.height());

    AverZ = 0;
    for (int i = 0; i < RowCount; i++)
        for (int j = 0; j < ColCount; j++)
        {
            double z = points[i][j].z()*Global_kz;
            AverZ += z;
            points[i][j].setZ(z);
        }

    AverZ /= (RowCount*ColCount);

//    Global_kz = 1/maxZ;

    glBegin(GL_QUADS);
    for (int i = 0; i < RowCount-1; i++)
    {
        for (int j = 0; j < ColCount-1; j++)
        {
            QVector3D
                p0 = points[i][j],
                p1 = points[i][j+1],
                p2 = points[i+1][j+1],
                p3 = points[i+1][j];

            QColor
                c0 = colors[i][j],
                c1 = colors[i][j+1],
                c2 = colors[i+1][j+1],
                c3 = colors[i+1][j];

            glColor3f(c0.redF(), c0.greenF(), c0.blueF());
            glVertex3f(p0.x(), p0.y(), _is2d ? 0 : p0.z());

            //qDebug() << "p0.z() = " << p0.z();

            glColor3f(c1.redF(), c1.greenF(), c1.blueF());
            glVertex3f(p1.x(), p1.y(), _is2d ? 0 : p1.z());

            glColor3f(c2.redF(), c2.greenF(), c2.blueF());
            glVertex3f(p2.x(), p2.y(), _is2d ? 0 : p2.z());

            glColor3f(c3.redF(), c3.greenF(), c3.blueF());
            glVertex3f(p3.x(), p3.y(), _is2d ? 0 : p3.z());
        }
    }
    glEnd();

    glEndList(); // закончить список

    if (!_is2d)
        IsReliefBuilt = true;
    else
        IsRelief2dBuilt = true;

*/
}
//----------------------------------------------------------

void MathRelief::Clear()
{
//    Area.setCoords(0, 0, 0, 0);
//    FileName.clear();

//    A.clear(); B.clear(); C.clear(); D.clear(); E.clear(); F.clear();
//    X0.clear(); Y0.clear();

//    S1.clear(), S2.clear();
//    ro.clear();

//    ReliefCompileList = 0;
//    IsReliefBuilt = false;
//    Relief2dCompileList = 0;
//    IsRelief2dBuilt = false;

//    IsReliefCreated = false;

//    MinX = std::numeric_limits<double>::max();
//    MinY = std::numeric_limits<double>::max();

//    minZ = std::numeric_limits<double>::max();
//    maxZ = -1;

//    ReliefMap.clear();

//    if (ReliefCompileList)
//        glDeleteLists(ReliefCompileList, 1);
//    if (Relief2dCompileList)
//        glDeleteLists(Relief2dCompileList, 1);
}
//----------------------------------------------------------

void MathRelief::CalcDeltaDiag()
{
    //DeltaDiag = 0;
}
//----------------------------------------------------------


