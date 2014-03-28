#ifndef CAMERA_H
#define CAMERA_H

// Camera is a simple class to manage camera
// We memorize position as angle and position
// And then we traslate to lookat function format
#include "angle.h"

class Camera
{
private:
    // data used by lookAt function
    double cameraX,cameraY,cameraZ; // camera position
    double lookAtX,lookAtY,lookAtZ; // camera look-at
    double upX,upY,upZ; // camera up

    // position and angle of vision of camera
    double x,y,z,r;
    Angle tetaAngle, phiAngle;
    bool hasChanged;
    void updateLookValue();
public:
    Camera();
    Camera(double, double, double);
    // static camera movemet
    void moveCameraForward();
    void moveCameraBackward();
    void moveCameraLeft();
    void moveCameraRight();
    void moveCameraUp();
    void moveCameraDown();
    void rotateCameraLeft();
    void rotateCameraRight();
    void rotateCameraUp();
    void rotateCameraDown();

    // dynamic camera movement
    // FPS Style
    void strafeLeft();
    void strafeRight();
    void moveForward();
    void moveBackward();
    void strafeUp();
    void strafeDown();
    //void staticForward();
    //void staticMoveBackward();

    /*void moveForward();
    void moveBackward();
    void moveLeft();
    void moveRight();
    void rotateUp();
    void rotateDown();
    void rotateUp();
    void rotateDown();*/


    double getCameraX();
    double getCameraY();
    double getCameraZ();
    double getLookAtX();
    double getLookAtY();
    double getLookAtZ();
    double getUpX();
    double getUpY();
    double getUpZ();
};

#endif // CAMERA_H
