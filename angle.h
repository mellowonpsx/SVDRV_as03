#ifndef ANGLE_H
#define ANGLE_H

#include <iostream>

class Angle
{
private:
    double angle;
public:
    Angle();
    Angle(double);
    Angle& operator+= (double);
    Angle& operator-= (double);
    Angle& operator= (double);
    Angle& operator+= (Angle);
    Angle& operator-= (Angle);
    Angle& operator= (Angle);
    double getAngle();
    double getRadiantAngle();
    friend std::ostream& operator<<(std::ostream& os, const Angle& angle);
};

inline std::ostream& operator<<(std::ostream& os, const Angle& angle)
{
    os << angle.angle;
    return os;
}

#endif // ANGLE_H
