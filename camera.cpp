#include "camera.h"
#include <math.h>

#define VERT_ANGLE_STEP 10.0
#define ORIZ_ANGLE_STEP 10.0

#define VERT_MOVE_STEP 10.0
#define ORIZ_MOVE_STEP 10.0
#define Z_MOVE_STEP 10.0
#define R_MOVE_STEP 10.0
#define STRAFE_STEP 10.0

Camera::Camera()
{
    this->x= 0;
    this->y= 0;
    this->z= 500.0;
    this->r = 1000.0;
    this->tetaAngle = 0.0;
    this->phiAngle = 0.0;
    this->hasChanged = true;
}

Camera::Camera(double newX, double newY, double newZ)
{
    /*this->x = newX;
    this->y = newY;
    this->z = newZ;
    this->hasChanged = true;*/
    this[0] = Camera();
}

void Camera::updateLookValue()
{
    if(this->hasChanged)
    {
        // test Value
        this->cameraX = x;
        this->cameraY = y;
        this->cameraZ = z;

        // test Value
        //x = r sin teta cos phi
        //y = r sin teta sin phi
        //z = r cos teta

        // SEE PICTURE!!!
        // x, y, z => (-z), x, y
        // teta = teta-90°
        // this make 0.0 in front of you, increase to look up, decrease to look down
        // phi = 180°-thi
        // this make 0.0 in front of you, increase to look right, decrease to look left

        Angle tetaVisionAngle(-90.0);
        tetaVisionAngle += tetaAngle;

        Angle phiVisionAngle(180.0);
        phiVisionAngle += phiAngle;

        double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
        double phiVisionRadiant = phiVisionAngle.getRadiantAngle();
        this->lookAtZ = z-this->r*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
        this->lookAtX = x+this->r*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
        this->lookAtY = y+this->r*cos(tetaVisionRadiant);

        this->upX = 0.0;
        this->upY = 1.0;
        this->upZ = 0.0;
        hasChanged = false;
    }
}

void Camera::moveCameraForward()
{
    this->hasChanged = true;
    this->z -= Z_MOVE_STEP;
}

void Camera::moveCameraBackward()
{
    this->hasChanged = true;
    this->z += Z_MOVE_STEP;
}

void Camera::moveCameraLeft()
{
    this->hasChanged = true;
    this->x -= ORIZ_MOVE_STEP;
}

void Camera::moveCameraRight()
{
    this->hasChanged = true;
    this->x += ORIZ_MOVE_STEP;
}

void Camera::moveCameraUp()
{
    this->hasChanged = true;
    this->y += VERT_MOVE_STEP;
}

void Camera::moveCameraDown()
{
    this->hasChanged = true;
    this->y -= VERT_MOVE_STEP;
}

void Camera::rotateCameraLeft()
{
    this->hasChanged = true;
    this->phiAngle -= ORIZ_ANGLE_STEP;
}

void Camera::rotateCameraRight()
{
    this->hasChanged = true;
    this->phiAngle += ORIZ_ANGLE_STEP;
}

void Camera::rotateCameraUp()
{
    this->hasChanged = true;
    this->tetaAngle += VERT_ANGLE_STEP;
}

void Camera::rotateCameraDown()
{
    this->hasChanged = true;
    this->tetaAngle -= VERT_ANGLE_STEP;
}

void Camera::strafeLeft()
{
    this->hasChanged = true;

    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    // I strafe left, so my phi angle is 90° more
    Angle phiVisionAngle(180.0+90.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    z = z-(-STRAFE_STEP)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    x = x+(-STRAFE_STEP)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    // when i strafe i don't modify my y value
    //y = y+(-STRAFE_STEP)*cos(tetaVisionRadiant);
}

void Camera::strafeRight()
{
    this->hasChanged = true;

    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    // I strafe left, so my phi angle is 90° less
    Angle phiVisionAngle(180.0+90.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    z = z-(STRAFE_STEP)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    x = x+(STRAFE_STEP)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    // when i strafe i don't modify my y value
    //y = y+(STRAFE_STEP)*cos(tetaVisionRadiant);
}

void Camera::strafeUp()
{
    this->hasChanged = true;

    // when i strafe i move parallel on vision angle (?)
    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    // I strafe left, so my phi angle is 90° less
    Angle phiVisionAngle(180.0+90.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    z = z-(STRAFE_STEP)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    x = x+(STRAFE_STEP)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    // when i strafe i don't modify my y value
    //y = y+(STRAFE_STEP)*cos(tetaVisionRadiant);
}

void Camera::strafeDown()
{

}

void Camera::moveForward()
{
    this->hasChanged = true;

    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    Angle phiVisionAngle(180.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    z = z-(R_MOVE_STEP)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    x = x+(R_MOVE_STEP)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    y = y+(R_MOVE_STEP)*cos(tetaVisionRadiant);
}

void Camera::moveBackward()
{
    this->hasChanged = true;

    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    Angle phiVisionAngle(180.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    z = z-(-R_MOVE_STEP)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    x = x+(-R_MOVE_STEP)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    y = y+(-R_MOVE_STEP)*cos(tetaVisionRadiant);
}

double Camera::getCameraX()
{
    this->updateLookValue();
    return this->cameraX;
}

double Camera::getCameraY()
{
    this->updateLookValue();
    return this->cameraY;
}

double Camera::getCameraZ()
{
    this->updateLookValue();
    return this->cameraZ;
}

double Camera::getLookAtX()
{
    this->updateLookValue();
    return this->lookAtX;
}

double Camera::getLookAtY()
{
    this->updateLookValue();
    return this->lookAtY;
}

double Camera::getLookAtZ()
{
    this->updateLookValue();
    return this->lookAtZ;
}

double Camera::getUpX()
{
    this->updateLookValue();
    return this->upX;
}

double Camera::getUpY()
{
    this->updateLookValue();
    return this->upY;
}

double Camera::getUpZ()
{
    this->updateLookValue();
    return this->upZ;
}
