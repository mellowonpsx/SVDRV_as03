#include "angle.h"
#include <math.h>
#include <iostream>

Angle::Angle()
{
    this->angle= 0.0;
    this->limited = false;
}

Angle::Angle(double value)
{
    this->angle= 0.0;
    this->limited = false;
    (*this) += value;
}

void Angle::setAngleLimit(double newAngleMin, double newAngleMax)
{
    if(newAngleMax>newAngleMin)
    {
        this->angleMin = newAngleMin;
        this->angleMax = newAngleMax;
        this->limited = true;
    }
    else
    {
        this->limited = false;
    }
}

Angle& Angle::operator+= (double angleToSum)
{
    if(this->limited)
    {
        if((this->angle+angleToSum) >= this->angleMax)
        {
            this->angle = this->angleMax;
        }
        else
        {
            if((this->angle+angleToSum) <= this->angleMin)
            {
                this->angle = this->angleMin;
            }
            else this->angle += angleToSum;
        }
    }
    else
    {
        this->angle += angleToSum;
        double integerPart = floor(this->angle/360.0);
        double divisionReminder = this->angle-(integerPart*(360.0));
        this->angle = divisionReminder;
    }
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
/*
Angle& Angle::operator= (Angle angleToSet)
{
    //(*this) = angleToSet;
    return *this;
}*/

double Angle::getAngle()
{
    return this->angle;
}

double Angle::getRadiantAngle()
{
    return (this->angle*M_PI/180.0);
}
