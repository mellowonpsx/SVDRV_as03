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
    this->z= -1.0;
    this->r = 1.0;
    this->tetaAngle = 0.0;
    this->phiAngle = 0.0;
    this->depth = -1.0;
    this->hasChanged = true;
}

Camera::Camera(double newX, double newY, double newZ, double newR, Angle newTetaAngle, Angle newPhiAngle, double newDepth)
{
    this->x = newX;
    this->y = newY;
    this->z = newZ;
    this->r = newR;
    this->tetaAngle = newTetaAngle;
    this->phiAngle = newPhiAngle;
    this->depth = newDepth;
    this->hasChanged = true;
}

void Camera::setCameraBound(double newMinX, double newMaxX, double newMinY, double newMaxY, double newMinZ, double newMaxZ)
{
    this->minX = newMinX;
    this->maxX = newMaxX;
    this->minY = newMinY;
    this->maxY = newMaxY;
    this->minZ = newMinZ;
    this->maxZ = newMaxZ;
}

void Camera::setR(double newR)
{
    this->r = newR;
    this->hasChanged = true;
}

void Camera::updateLookValue()
{
    if(this->hasChanged)
    {
        this->checkBounds();
        // camera position is my position
        this->cameraX = x;
        this->cameraY = y;
        this->cameraZ = z;

        // from wiki i have this conversion tips
        // but my sistem is different
        //x = r sin teta cos phi
        //y = r sin teta sin phi
        //z = r cos teta

        // SEE PICTURE!!!
        // x, y, z => (-z), x, y
        // teta = teta-90°
        // this make 0.0 in front of you, increase to look up, decrease to look down
        // phi = 180°-phi
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
        this->hasChanged = false;
    }
}

void Camera::moveCameraForward()
{
    this->hasChanged = true;
    this->z -= Z_MOVE_STEP;
}

void Camera::checkBounds()
{
    this->hasChanged = true;
    // remember z is strange, it is negative
    if(this-> z < this->maxZ) this->z = this->maxZ;
    if(this-> z > this->minZ) this->z = this->minZ;
    // other are normal
    if(this-> x > this->maxX) this->x = this->maxX;
    if(this-> x < this->minX) this->x = this->minX;
    if(this-> y > this->maxY) this->y = this->maxY;
    if(this-> y < this->minY) this->y = this->minY;
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

void Camera::strafeHoriziontal(double step)
{
    this->hasChanged = true;

    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    // I strafe left, so my phi angle is 90° more
    Angle phiVisionAngle(180.0+90.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    z = z-(step)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    x = x+(step)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    // when i strafe i don't modify my y value
    //y = y+(-STRAFE_STEP)*cos(tetaVisionRadiant);
}

void Camera::strafeLeft()
{
    this->hasChanged = true;
    this->strafeHoriziontal(-STRAFE_STEP);
}

void Camera::strafeLeft(double step)
{
    this->hasChanged = true;
    this->strafeHoriziontal(-fabs(step));
}

void Camera::strafeRight()
{
    this->hasChanged = true;
    this->strafeHoriziontal(STRAFE_STEP);
}

void Camera::strafeRight(double step)
{
    this->hasChanged = true;
    this->strafeHoriziontal(fabs(step));
}

void Camera::strafeVertical(double step)
{
    this->hasChanged = true;
    this->y += step;
}

void Camera::strafeUp()
{
    // strafe up (like jump)
    this->hasChanged = true;
    this->strafeVertical(STRAFE_STEP);
}

void Camera::strafeDown()
{
    // strafe down (like fall down)
    this->hasChanged = true;
    this->strafeVertical(-STRAFE_STEP);
}

void Camera::moveAlongDirection(double step)
{
    this->hasChanged = true;

    Angle tetaVisionAngle(-90.0);
    tetaVisionAngle += tetaAngle;

    Angle phiVisionAngle(180.0);
    phiVisionAngle += phiAngle;
    double tetaVisionRadiant = tetaVisionAngle.getRadiantAngle();
    double phiVisionRadiant = phiVisionAngle.getRadiantAngle();

    this->z = this->z-(step)*sin(tetaVisionRadiant)*cos(phiVisionRadiant);
    this->x = this->x+(step)*sin(tetaVisionRadiant)*sin(phiVisionRadiant);
    this->y = this->y+(step)*cos(tetaVisionRadiant);
}

void Camera::moveForward()
{
    this->hasChanged = true;
    this->moveAlongDirection(R_MOVE_STEP);
}

void Camera::moveForward(double step)
{
    this->hasChanged = true;
    this->moveAlongDirection(fabs(step));
}

void Camera::moveBackward()
{
    this->hasChanged = true;
    this->moveAlongDirection(-R_MOVE_STEP);
}

void Camera::moveBackward(double step)
{
    this->hasChanged = true;
    this->moveAlongDirection(-fabs(step));
}

void Camera::rotateHotiziontal(double step)
{
    this->hasChanged = true;
    this->phiAngle += step;
}

void Camera::rotateVertical(double step)
{
    this->hasChanged = true;
    this->tetaAngle += step;
}

void Camera::rotateCameraLeft()
{
    this->hasChanged = true;
    this->rotateHotiziontal(-ORIZ_ANGLE_STEP);
}

void Camera::rotateCameraLeft(double step)
{
    this->hasChanged = true;
    this->rotateHotiziontal(-fabs(step));
}

void Camera::rotateCameraRight()
{
    this->hasChanged = true;
    this->rotateHotiziontal(ORIZ_ANGLE_STEP);
}

void Camera::rotateCameraRight(double step)
{
    this->hasChanged = true;
    this->rotateHotiziontal(fabs(step));
}

void Camera::rotateCameraUp()
{
    this->hasChanged = true;
    this->rotateVertical(VERT_ANGLE_STEP);
}

void Camera::rotateCameraUp(double step)
{
    this->hasChanged = true;
    this->rotateVertical(fabs(step));
}

void Camera::rotateCameraDown()
{
    this->hasChanged = true;
    this->rotateVertical(-VERT_ANGLE_STEP);
}

void Camera::rotateCameraDown(double step)
{
    this->hasChanged = true;
    this->rotateVertical(-fabs(step));
}

// get methods

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

double Camera::getDepth()
{
    return this->depth;
}
