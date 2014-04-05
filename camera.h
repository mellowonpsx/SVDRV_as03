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

    double minX, maxX, minY, maxY, minZ, maxZ;
    // position and angle of vision of camera
    double x,y,z,r,depth;
    Angle tetaAngle, phiAngle;
    bool hasChanged;
    void updateLookValue();
public:
    Camera();
    Camera(double newX, double newY, double newZ, double newR,  Angle newTetaAngle, Angle newPhiAngle, double newDepth);
    void setCameraBound(double newMinX, double newMaxX, double newMinY, double newMaxY, double newMinZ, double newMaxZ);
    void setR(double newR);
    void checkBounds();
    // static camera movement, used in test
    void moveCameraForward();
    void moveCameraBackward();
    void moveCameraLeft();
    void moveCameraRight();
    void moveCameraUp();
    void moveCameraDown();

    // dynamic camera movement
    // head movement vs body movement
    // body movement
    void moveForward();
    void moveForward(double step);
    void moveBackward();
    void moveBackward(double step);
    void moveAlongDirection(double step); //maybe better private?
    void strafeLeft();
    void strafeRight();
    void strafeLeft(double step);
    void strafeRight(double step);
    void strafeHoriziontal(double step); //maybe better private?
    void strafeUp();
    void strafeDown();
    void strafeVertical(double step); //maybe better private?
    // head movement
    void rotateCameraLeft();
    void rotateCameraLeft(double step);
    void rotateCameraRight();
    void rotateCameraRight(double step);
    void rotateHotiziontal(double step); //maybe better private?
    void rotateCameraUp();
    void rotateCameraUp(double step);
    void rotateCameraDown();
    void rotateCameraDown(double step);
    void rotateVertical(double step); //maybe better private?

    // get methods
    double getCameraX();
    double getCameraY();
    double getCameraZ();
    double getLookAtX();
    double getLookAtY();
    double getLookAtZ();
    double getUpX();
    double getUpY();
    double getUpZ();
    double getDepth();
};

#endif // CAMERA_H
