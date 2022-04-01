#include "SignalNode.h"
#include "MathUtils.h"

#include "Relief.h"

#include <cmath>
#include <QDebug>

#include "GradConfig.h"

using namespace std;

void SignalNode::SetRandomCoord(const Relief3D &_relief)
{ 
    const auto &_area = _relief.GetArea();

    Pos.setX( SimpleRandom(_area.left(), _area.right()) );
    Pos.setY( SimpleRandom(_area.bottom(), _area.top()) );

    Pos.setZ( _relief.CalcRealZbyRealXY( Pos.x(), Pos.y()) );
}
//----------------------------------------------------------

bool SignalNode::SetCoordForPos(const Relief3D &_relief, const Pos3d &_pos)
{
    const auto &_area = _relief.GetArea();

    if (_area.isValid())
        qDebug() << "Valid";
    else
        qDebug() << "Invalid";


    double x, y;
    x = _pos.x();
    y = _pos.y();
    bool isOk = true;
    if ( x < _area.left() )
    {
        x = _area.left();
        isOk = false;
    }
    if ( x > _area.right() )
    {
        x = _area.right();
        isOk = false;
    }
    if ( y > _area.bottom() )
    {
        y = _area.bottom();
        isOk = false;
    }
    if ( y < _area.top() )
    {
        y = _area.top();
        isOk = false;
    }

    Pos.setX(x);
    Pos.setY(y);

    Pos.setZ( _relief.CalcRealZbyRealXY( Pos.x(), Pos.y()) );

    return isOk;
}
//----------------------------------------------------------

double SignalNode::accessRateF(const QVector3D &p) const
{
    // это квадрат расстояния между текущей позицией и p
    double d2 = (p.x()-Pos.x())*(p.x()-Pos.x()) +
                (p.y()-Pos.y())*(p.y()-Pos.y()) +
                (p.z()-Pos.z())*(p.z()-Pos.z());

    double y = exp(-d2/(2.0*R*R));
    return y;
}
//----------------------------------------------------------

SignalNodeType SignalNode::ConvertStringToSignalNodeType(QString &str)
{
    if (str.toUpper() == "SPHERE")
        return SignalNodeType::Sphere;
    else if (str.toUpper() == "CONE")
        return SignalNodeType::Cone;
    else
        return SignalNodeType::Unknown;
}
//----------------------------------------------------------

QString SignalNode::ConvertSignalNodeTypeToString(SignalNodeType snt)
{
    switch (snt)
    {
    case SignalNodeType::Sphere:
        return "Sphere";
    case SignalNodeType::Cone:
        return "Cone";
    case SignalNodeType::Unknown:
    default:
        return "Unknown";
    }
}
//----------------------------------------------------------

QJsonObject SignalNode::RepresentAsJsonObject() const
{
    QJsonObject nodeObject;
    nodeObject.insert("R", (int)R);
    nodeObject.insert("Beta", qRadiansToDegrees(Beta));
    return nodeObject;
}
//----------------------------------------------------------

void SignalNode::LoadFromJsonObject(const QJsonObject &_jsonObject)
{
    R = _jsonObject["R"].toDouble(-1);
    Beta = qDegreesToRadians(_jsonObject["Beta"].toDouble(0));

}
//----------------------------------------------------------

GLUquadric* SignalNode::Quadric()
{
    static GLUquadric* q = nullptr;
    if (!q) q = gluNewQuadric();
    return q;
}
//----------------------------------------------------------


void SignalNode::DrawIn3D(SignalNodeType _snt, const Relief3D *relief,
                          const Settings3dType & _settings3d) const
{
    constexpr float zOffset = 0.01f;
    const auto & area = relief->GetArea();
    double kx = 2.0/area.width();
    double ky = 2.0/area.height();
    double k = min(kx, ky);

    double hW = area.width()/2.0;
    double hH = area.height()/2.0;

    double offsetX = area.x()+hW; // in meters
    double offsetY = area.y()+hH; // in meters
    double offsetZ = 0;

    glPushMatrix();

    double x = (Pos.x()-offsetX)*k;
    double y = (Pos.y()-offsetY)*k;
    double z;


    if (relief->GetIsMathRelief())
    {
        glTranslatef(x, y, zOffset + (_settings3d.IsPerspective ? relief->CalcNormZbyNormXY(x, y) : 0));
    }
    else
    {
        z = (Pos.z()-offsetZ)*relief->Get_kz();
        glTranslatef(x, y, zOffset + (_settings3d.IsPerspective ? z : 0));
    }
    gluQuadricDrawStyle(Quadric(), GLU_FILL);
    gluSphere(Quadric(), 0.02, 12, 12);
    glPopMatrix();

    const int nr = 32;
    double dfi, fiStart;

    if (_snt == SignalNodeType::Sphere)
    {
        dfi = 2.0*M_PI/nr;
        fiStart = 0;
    }
    else if (_snt == SignalNodeType::Cone)
    {
        dfi = 2.0*Beta / nr;
        fiStart = Alpha - Beta;
    }
    else
    {
        qDebug() << "Error: SignalNodeType is Unknown";
        dfi = 2*M_PI/nr;
        fiStart = 0;
    }

    glBegin(GL_LINE_STRIP);

    if (_snt == SignalNodeType::Cone) // далее будет копипаст этого ифа
    {
        double x = (Pos.x()-offsetX)*k;
        double y = (Pos.y()-offsetY)*k;
        double z;

        if (relief->GetIsMathRelief())
        {
            glVertex3f(x, y, zOffset + (_settings3d.IsPerspective ? relief->CalcNormZbyNormXY(x, y) : 0));
        }
        else
        {
            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(Pos.x(), Pos.y()) : 0);
            glVertex3f(x, y, z);
        }
    }

    for (int i = 0; i <= nr; i++)
    {
        double xt = Pos.x() + R*cos(fiStart + i*dfi);
        double yt = Pos.y() + R*sin(fiStart + i*dfi);
        double x = (xt-offsetX)*k;
        double y = (yt-offsetY)*k;
        double z;

        if (relief->GetIsMathRelief())
        {
            // z = ???;
            glVertex3f(x, y, zOffset + (_settings3d.IsPerspective ? relief->CalcNormZbyNormXY(x, y) : 0));
        }
        else
        {
//                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyRealXY(xt, yt) : 0));
            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(xt, yt) : 0);
            glVertex3f(x, y, z);
        }
    }

    if (_snt == SignalNodeType::Cone) // здесь копипаст
    {
        double x = (Pos.x()-offsetX)*k;
        double y = (Pos.y()-offsetY)*k;
        double z;

        if (relief->GetIsMathRelief())
        {
            glVertex3f(x, y, zOffset + (_settings3d.IsPerspective ? relief->CalcNormZbyNormXY(x, y) : 0));
        }
        else
        {
            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(Pos.x(), Pos.y()) : 0);
            glVertex3f(x, y, z);
        }
    }

    glEnd();


    glColor3f(0.9, 0.1, 0.9);
    if (_snt == SignalNodeType::Cone) // здесь ellipse
    {
        glBegin(GL_LINE_STRIP);



        const int nr = 32;
        double dfi = 2.0*M_PI/(nr-1);

        double asp_ab = M_PI / Beta;
        double a = R*1.2;
        double b = a / sqrt(asp_ab);

        const double c = sqrt(abs(a*a - b*b));

        for (int i = 0; i < nr; i++)
        {
            double xt = a*cos(fiStart + i*dfi) + c;
            double yt = b*sin(fiStart + i*dfi);

            double xt2 = xt*cos(-Alpha) + yt*sin(-Alpha);
            double yt2 = yt*cos(-Alpha) - xt*sin(-Alpha);

            xt = Pos.x() + xt2;
            yt = Pos.y() + yt2;
            double x = (xt-offsetX)*k;
            double y = (yt-offsetY)*k;
            double z;

            if (relief->GetIsMathRelief())
            {
                glVertex3f(x, y, zOffset + (_settings3d.IsPerspective ? relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
                z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(xt, yt) : 0);
                glVertex3f(x, y, z);
            }
        }

        glEnd();
    }
}
//----------------------------------------------------------


ostream & operator<<(ostream & s, const SignalNode &ob) // для отладки
{
    s << "{" << ob.Pos.x() << "; " << ob.Pos.y() <<  "; " << ob.Pos.z() << "}" << "; R = " << ob.R;
    return s;
}
//----------------------------------------------------------
