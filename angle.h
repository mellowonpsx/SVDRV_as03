#ifndef ANGLE_H
#define ANGLE_H

#include <iostream>

class Angle
{
private:
    double angle;
    double angleMin;
    double angleMax;
    bool limited;
public:
    Angle();
    Angle(double newAngle);
    Angle& operator+= (double angleToAdd);
    Angle& operator-= (double angleToSub);
    Angle& operator= (double angleToAssign);
    Angle& operator+= (Angle angleToAdd);
    Angle& operator-= (Angle angleToSub);
    //Angle& operator= (Angle angleToAssign);
    double getAngle();
    double getRadiantAngle();
    void setAngleLimit(double newAngleMin, double newAngleMax);
    friend std::ostream& operator<<(std::ostream& os, const Angle& angle);
};

inline std::ostream& operator<<(std::ostream& os, const Angle& angle)
{
    os << angle.angle;
    return os;
}

#endif // ANGLE_H
