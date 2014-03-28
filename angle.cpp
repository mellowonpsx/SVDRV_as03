#include "angle.h"
#include <math.h>
#include <iostream>

Angle::Angle()
{
    angle= 0.0;
}

Angle::Angle(double value)
{
    angle= 0.0;
    (*this) += value;
}

Angle& Angle::operator+= (double angleToSum)
{
    this->angle += angleToSum;
    double integerPart = floor(this->angle/360.0);
    double divisionReminder = angle-(integerPart*360.0);
    this->angle = divisionReminder;
    return *this;
}

Angle& Angle::operator-= (double angleToDiff)
{
    angleToDiff = -angleToDiff;
    (*this) += angleToDiff;
    return *this;
}

Angle& Angle::operator= (double angleToSet)
{
    this->angle = 0.0;
    (*this) += angleToSet;
    return *this;
}

Angle& Angle::operator+= (Angle angleToSum)
{
    (*this) += angleToSum.getAngle();
    return *this;
}

Angle& Angle::operator-= (Angle angleToDiff)
{
    (*this) += angleToDiff.getAngle();
    return *this;
}

Angle& Angle::operator= (Angle angleToSet)
{
    (*this) = angleToSet.getAngle();
    return *this;
}

double Angle::getAngle()
{
    return this->angle;
}

double Angle::getRadiantAngle()
{
    return (this->angle*M_PI/180.0);
}
