#include "Test_tf_gd_lib.h"

#include "UnitTableFunctions.h"
#include "UnitGradDescent.h"

using namespace tf_gd_lib;

#include <iostream>

using namespace std;

long double user_target_function(const std::vector<long double>& p)
{
    long double r1 = sqrt((p[0] - 0) * (p[0] - 0) + (p[1] - 1000) * (p[1] - 1000));
    long double r2 = sqrt((p[0] - 0) * (p[0] - 0) + (p[1] - 0) * (p[1] - 0));
    long double r3 = sqrt((p[2] - p[0]) * (p[2] - p[0]) + (p[3] - p[1]) * (p[3] - p[1]));
    long double r4 = sqrt((1000 - p[2]) * (1000 - p[2]) + (1000 - p[3]) * (1000 - p[3]));
    long double r5 = sqrt((1000 - p[2]) * (1000 - p[2]) + (0 - p[3]) * (0 - p[3]));

    return r1 + r2 + r3 + r4 + r5;
}

//   *                *(1000,1000)
//    \              /
//     \            /
//      \          /
//       *--------*        //  Point1(p[0], p[1]) ,  Point2(p[2], p[3])
//      /          \
//     /            \
//    /              \
//   *(0,0)           *

void MyLibTest()
{
    GradDescent gd;

    gd.SetIsUseUserTargetFunction(true);
    gd.SetUseUserTargetFunction(user_target_function);

    gd.SetAlpha(0.25);   // value for momentum
    gd.SetEps(0.0001); // value to derivative
    gd.SetEta_FirstJump(10);
    gd.SetEta_k_inc(1.08);
    gd.SetEta_k_dec(2.0);
    gd.SetMin_Eta(1e-9);       // min descent rate (will be multiplied by FirstJump before get started)
    gd.SetFinDifMethod(false); // using a plain central derivative
    gd.SetMaxIters(10000);     // iteration limit
    gd.SetMaxTime(3);          // time limit (seconds)

    const int param_count = 4;
    vector<long double> params(param_count);
    vector<long double> min_constrains(param_count);
    vector<long double> max_constrains(param_count);
    vector<long double> rel_constrains(param_count); // relative constrains in %
    vector<bool> type_constrains(param_count);  // set 'false' to use absolute constrains, 'true' - for relative

    //params[0] = 900;
    //params[1] = 999;
    //params[2] = 1;
    //params[3] = 100;

    srand(time(NULL));
    params[0] = 1 + rand() % 1000;
    params[1] = 1 + rand() % 1000;
    params[2] = 1 + rand() % 1000;
    params[3] = 1 + rand() % 1000;

    min_constrains[0] = 0;      max_constrains[0] = 1000;
    min_constrains[1] = 0;      max_constrains[1] = 1000;
    min_constrains[2] = 0;      max_constrains[2] = 1000;
    min_constrains[3] = 0;      max_constrains[3] = 1000;

    rel_constrains[0] = rel_constrains[1] = rel_constrains[2] = rel_constrains[3] = 0; // not used
    type_constrains[0] = type_constrains[1] = type_constrains[2] = type_constrains[3] = false; // use absolute constrains

    gd.SetParams(params);
    gd.SetMinConstrains(min_constrains);
    gd.SetMaxConstrains(max_constrains);
    gd.SetRelConstrains(rel_constrains);
    gd.SetTypeConstrains(type_constrains);

    gd.SetCallBackFreq(25);  // call callback function every 25 iterations

    std::ios_base::fmtflags ff;
    ff = std::cout.flags();
    ff |= std::cout.fixed;
    std::cout.flags(ff);
    cout.precision(10);

    gd.SetCallback([&gd]() {
        cout << "Last Cost = " << gd.GetLastCost() << endl;
        });

    GradErrorType res = gd.Go();

    cout << "gd_user_target_function result: ";
    switch (res)
    {
    case GradErrorType::Success:
        cout << "Success" << endl;
        break;
    case GradErrorType::VectorSizesNotTheSame:
        cout << "VectorSizesNotTheSame" << endl;
        break;
    case GradErrorType::CanceledByUser:
        cout << "CanceledByUser" << endl;
        break;
    case GradErrorType::TimeOut:
        cout << "TimeOut" << endl;
        break;
    case GradErrorType::ItersOverflow:
        cout << "ItersOverflow" << endl;
        break;
    }

    cout << "gd.GetLastCost() = " << gd.GetLastCost() << endl;

    params = gd.GetParams();

    cout << "Found Params: " << endl;
    for (auto p : params)
        cout << p << endl;
}

