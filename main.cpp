#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <sstream>
#include "camera.h"

using namespace std;

// sphere proprieties
#define SPHERE_SLICE 100
#define SPHERE_STACKS 100

// font
#define DEFAULT_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DEFAULT_FONT_R 1.0
#define DEFAULT_FONT_G 1.0
#define DEFAULT_FONT_B 1.0
#define DEFAULT_FONT_HEIGHT 24.0
#define DEFAULT_DELTA_FRACTION 1;

typedef struct
{
    GLuint textureId; // textureId is used to blind texture
} RectangularTexturedShape;

typedef struct
{
    double x,y,z; // center coordinate
    double dimX, dimY, dimZ; // abs dimension
    GLuint textureId; // textureId is used to blind texture
} TexturedTree;

void handleKeypress(unsigned char key,int x,int y);
void handleSpecialKeypress(int key,int x,int y);
void drawScene();
void idleScene();
void initRendering();
void changeSize(int w, int h);
void initModel();
void draw2DText(double x, double y, string text);
void draw3DText(double x, double y, double z, string text);
void initTextureId(GLuint *textureId, string filename, GLenum rgbMode);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void pressKey(int key, int xx, int yy);
void releaseKey(int key, int x, int y);

// variabiles
bool isChanged, isRunning, showLabels;
Camera camera;
RectangularTexturedShape grass, sky_line, sky_plane;
RectangularTexturedShape window, door, water, border, roof, wall, floorTexture;
TexturedTree alberellobello1, alberellobello2, alberellobello3, alberellobello4;

// load a texture and link to a identifier
void initTextureId(GLuint *textureId, string filename, GLenum rgbMode)
{
    // load file & error check
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (surface==NULL)
    {
        printf("Error: \"%s\"\n",SDL_GetError());
        exit(1);
    }
    // generate a texture with this identifier
    glGenTextures(1,textureId);
    // bind the texture with that identifier
    glBindTexture(GL_TEXTURE_2D,textureId[0]);
    // produce a 2D texture from the loaded image pixel
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, rgbMode,GL_UNSIGNED_BYTE,surface->pixels);
    // set method to filter texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    // free memory from file
    SDL_FreeSurface(surface);
}

int main( int argc, char** argv )
{
    /*Angle angolo = 0.0;
    angolo.setAngleLimit(0.0,90.0);
    angolo-= 1.0;
    cout << angolo << endl;
    return 0;*/

    camera = Camera();
    // Initialize glut
    glutInit(&argc,argv);
    // Initialize display (RGBA has alpha channel
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    // -1 -1 automatic positioning
    glutInitWindowPosition(0, 0); // Window position (from top corner), and size (width and hieght)
    // i want 16:9 "cinema" like
    glutInitWindowSize( 1600, 900);
    // window name
    glutCreateWindow("SVDRV_as03");
    // Initialize OpenGL
    initRendering();
    // Inizialize Model and Inizialize Texture ###MUST BE AFTER INIT RENDERING!!!###
    initModel();

    // Set up callback

    // Handles "normal" ascii symbols
    glutKeyboardFunc(handleKeypress);
    // Handles other key (not used)
    glutSpecialFunc(handleSpecialKeypress);
    //glutIgnoreKeyRepeat(GL_TRUE);
    //glutSpecialFunc(pressKey);
    //glutSpecialUpFunc(releaseKey);

    // mouse handling
    //glutMouseFunc(mouseButton);
    //glutMotionFunc(mouseMove);
    // called in case of resize
    glutReshapeFunc(changeSize);
    // call this whenever window needs redrawing
    glutDisplayFunc(drawScene);
    // call function during execution (to not use glutPostRedisplay();)
    glutIdleFunc(idleScene);

    // start main loop
    fprintf(stdout, "Press esc to exit\n");
    glutMainLoop();

    return(0);	// This line is never reached.
}

void handleKeypress(unsigned char key,int x,int y)
{
    switch(key)
    {
        case 27:
            exit (0);
            break;
        case 'w':
        case 'W':
            camera.moveForward();
            break;
        case 's':
        case 'S':
            camera.moveBackward();
            break;
        case 'a':
        case 'A':
            camera.strafeLeft();
            break;
        case 'd':
        case 'D':
            camera.strafeRight();
            break;
    }
    // used to not have warning (i hate useless warning!)
    return (void)(x+y);
}

void handleSpecialKeypress(int key,int x,int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            camera.rotateCameraUp();
            break;
        case GLUT_KEY_DOWN:
            camera.rotateCameraDown();
            break;
        case GLUT_KEY_LEFT:
            camera.rotateCameraLeft();
            break;
        case GLUT_KEY_RIGHT:
            camera.rotateCameraRight();
            break;
    }
    // used to not have warning (i hate useless warning!)
    return (void)(x+y);
}

void changeSize(int w, int h)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0) h = 1;
    if (w == 0) w = 1;
    float ratio =  w * 1.0 / h;

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

    // Reset Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(60.0, ratio, 1, camera.getDepth()); // like "infinite"

    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}


void initRendering()
{
    glEnable(GL_DEPTH_TEST);	// Depth testing must be turned on
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_NORMALIZE);

    // enable blender
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_ALPHA_TEST) ;
    // depends on image "transparecy"
    glAlphaFunc(GL_GREATER, 0.0f);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // helpfull to undestand which side is
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_COLOR_MATERIAL);
}

void drawLegend()
{
    std::stringstream legend;
    legend << "legend: " << "\"ESC\" for exit";
    draw2DText(1.0, 2.0, legend.str());
}

void drawTimer()
{
    std::stringstream time;

    int millisecond = glutGet(GLUT_ELAPSED_TIME);
    int second = millisecond/1000;
    millisecond = millisecond%1000;
    int minute = second/60;
    second = second%60;
    int hour = minute/60;
    minute = minute%60;
    time << "Running since " << hour << ":" << minute << ":" << second << "." << millisecond;
    draw2DText(1.0,GLUT_WINDOW_HEIGHT*1.89, time.str());
}

void drawRectTexturedFixedY(double xi, double xf, double y, double zi, double zf, double repeatedTextureX, double repeatedTextureZ, GLuint textureId)
{
    glPushMatrix();
        glTranslated(xi,y,zi);
        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureZ);
            glVertex3d(0.0, 0.0, zf);

            glTexCoord2f(repeatedTextureX,repeatedTextureZ);
            glVertex3d(xf, 0.0, zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, 0.0);
        glEnd();
    glPopMatrix();
}

void drawRectTexturedFixedX(double x, double yi, double yf, double zi, double zf, double repeatedTextureY, double repeatedTextureZ, GLuint textureId)
{
    glPushMatrix();
        // rotate to has right texture position
        glTranslated(0.0,yf+yi,0.0);
        glRotated(90.0,1.0,0.0,0.0); // rotate around x axis of texture
        // having rotate the axis system cause x-y swap
        double temp = zi;
        zi = yi;
        yi = temp;
        temp = zf;
        zf = yf;
        yf = temp;
        glTranslated(x,yi,zi);
        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureZ);
            glVertex3d(0.0, 0.0,  zf);

            glTexCoord2f(repeatedTextureY,repeatedTextureZ);
            glVertex3d(0.0, yf,  zf);

            glTexCoord2f(repeatedTextureZ,0.0);
            glVertex3d(0.0, yf, 0.0);
        glEnd();
    glPopMatrix();
}

void drawRectTexturedFixedZ(double xi, double xf, double yi, double yf, double z, double repeatedTextureX, double repeatedTextureY, GLuint textureId)
{
    glPushMatrix();
        // rotate to has right texture position
        glTranslated(0.0,yf+yi,0.0);
        glRotated(180.0,0.0,0.0,1.0); // rotate around x axis of texture
        // no other trasform cause rotate 180 degrees
        glTranslated(xi,yi,z);
        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  0.0);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, 0.0);
        glEnd();
    glPopMatrix();
}

void drawGroundAndSky()
{
    // left side
    drawRectTexturedFixedX(-1010.0, -10.0, 1010.0, -1010.0, 2020.0, 1000.0, 1.0, sky_line.textureId);
    // front side
    drawRectTexturedFixedZ(-1010.0, 2020.0, -10.0, 1010.0, -1010.0, 1000.0, 1.0, sky_line.textureId);
    // right side
    drawRectTexturedFixedX(1010.0, -10.0, 1010.0, -1010.0, 2020.0, 1000.0, 1.0, sky_line.textureId);
    // back side
    drawRectTexturedFixedZ(-1010.0, 2020.0, -10.0, 1010.0, 1010.0, 1000.0, 1.0, sky_line.textureId);
    // roof
    drawRectTexturedFixedY(-1010.0, 2020.0, 1000.0, 1010.0, -2020.0, 1.0, 1.0, sky_plane.textureId);
    // grass ground
    drawRectTexturedFixedY(-1010.0, 2020.0, 0.0, 1010.0, -2020.0, 4000.0, 4000.0, grass.textureId);
    glPopMatrix();
}

void drawHouse()
{
    glPushMatrix();
        // rotate to has right texture position
        double xf, xi, yf, yi, zi, zf, repeatedTextureY, repeatedTextureX;
        xi = 0.0;
        xf = 50.0;
        yi = 0.0;
        yf = 10.0;
        zi = -50.0;
        zf = -50.0;
        repeatedTextureX = 15.0;
        repeatedTextureY = 3.0;
        glTranslated(0.0,yf+yi,0.0);
        glRotated(180.0,0.0,0.0,1.0); // rotate around x axis of texture
        // no other trasform cause rotate 180 degrees
        glTranslated(xi,yi,zi);
        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, wall.textureId);
        glBegin(GL_POLYGON);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  0.0);

            //glVertex3d(xf, 0.0, zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, 0.0);

            glTexCoord2f(repeatedTextureX*0.5,repeatedTextureY);
            glVertex3d(xf*0.5, -yf, 0.0);
        glEnd();

        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(0.0, yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(0.0, 0.0, zf);

        glEnd();

        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(xf, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(xf, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, zf);

        glEnd();

        glBegin(GL_POLYGON);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, zf);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, zf);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  zf);

            //glVertex3d(xf, 0.0, zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, zf);

            glTexCoord2f(repeatedTextureX*0.5,repeatedTextureY);
            glVertex3d(xf*0.5, -yf, zf);
        glEnd();

        // roof
        glBindTexture(GL_TEXTURE_2D, roof.textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(xf*0.5, -yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf*0.5, -yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(0.0, 0.0, zf);

        glEnd();
        //glBindTexture(GL_TEXTURE_2D, roof.textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(xf, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(xf*0.5, -yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf*0.5, -yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, zf);

        glEnd();
        // floor
        glBindTexture(GL_TEXTURE_2D, floorTexture.textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, yf-0.1, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf-0.1, zf);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf-0.1,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, yf-0.1, 0.0);

        glEnd();
        // door
        glBindTexture(GL_TEXTURE_2D, door.textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(xf*0.56, yf*0.1, 0.0+0.1);

            glTexCoord2f(0.0,1.0);
            glVertex3d(xf*0.56, yf, 0.0+0.1);

            glTexCoord2f(1.0,1.0);
            glVertex3d(xf*0.44, yf,  0.0+0.1);

            glTexCoord2f(1.0,0.0);
            glVertex3d(xf*0.44, yf*0.1, 0.0-0.1);

            //inside

            glTexCoord2f(0.0,0.0);
            glVertex3d(xf*0.56, yf*0.1, 0.0-0.1);

            glTexCoord2f(0.0,1.0);
            glVertex3d(xf*0.56, yf, 0.0-0.1);

            glTexCoord2f(1.0,1.0);
            glVertex3d(xf*0.44, yf,  0.0-0.1);

            glTexCoord2f(1.0,0.0);
            glVertex3d(xf*0.44, yf*0.1, 0.0+0.1);
        glEnd();
        // door
        glBindTexture(GL_TEXTURE_2D, window.textureId);
        glBegin(GL_QUADS);
            //1
            glTexCoord2f(0.0,0.0);
            glVertex3d(-0.1, yf*0.7, zf*0.2);

            glTexCoord2f(0.0,1.0);
            glVertex3d(-0.1, yf*0.2, zf*0.2);

            glTexCoord2f(1.0,1.0);
            glVertex3d(-0.1, yf*0.2,  zf*0.3);

            glTexCoord2f(1.0,0.0);
            glVertex3d(-0.1, yf*0.7, zf*0.3);

            //2
            glTexCoord2f(0.0,0.0);
            glVertex3d(-0.1, yf*0.7, zf*0.7);

            glTexCoord2f(0.0,1.0);
            glVertex3d(-0.1, yf*0.2, zf*0.7);

            glTexCoord2f(1.0,1.0);
            glVertex3d(-0.1, yf*0.2,  zf*0.8);

            glTexCoord2f(1.0,0.0);
            glVertex3d(-0.1, yf*0.7, zf*0.8);
            //3
            glTexCoord2f(0.0,0.0);
            glVertex3d(xf+0.1, yf*0.7, zf*0.2);

            glTexCoord2f(0.0,1.0);
            glVertex3d(xf+0.1, yf*0.2, zf*0.2);

            glTexCoord2f(1.0,1.0);
            glVertex3d(xf+0.1, yf*0.2,  zf*0.3);

            glTexCoord2f(1.0,0.0);
            glVertex3d(xf+0.1, yf*0.7, zf*0.3);

            //4
            glTexCoord2f(0.0,0.0);
            glVertex3d(xf+0.1, yf*0.7, zf*0.7);

            glTexCoord2f(0.0,1.0);
            glVertex3d(xf+0.1, yf*0.2, zf*0.7);

            glTexCoord2f(1.0,1.0);
            glVertex3d(xf+0.1, yf*0.2,  zf*0.8);

            glTexCoord2f(1.0,0.0);
            glVertex3d(xf+0.1, yf*0.7, zf*0.8);

            // inside
            //1
            glTexCoord2f(0.0,0.0);
            glVertex3d(+0.1, yf*0.7, zf*0.2);

            glTexCoord2f(0.0,1.0);
            glVertex3d(+0.1, yf*0.2, zf*0.2);

            glTexCoord2f(1.0,1.0);
            glVertex3d(+0.1, yf*0.2,  zf*0.3);

            glTexCoord2f(1.0,0.0);
            glVertex3d(+0.1, yf*0.7, zf*0.3);

            //2
            glTexCoord2f(0.0,0.0);
            glVertex3d(+0.1, yf*0.7, zf*0.7);

            glTexCoord2f(0.0,1.0);
            glVertex3d(+0.1, yf*0.2, zf*0.7);

            glTexCoord2f(1.0,1.0);
            glVertex3d(+0.1, yf*0.2,  zf*0.8);

            glTexCoord2f(1.0,0.0);
            glVertex3d(+0.1, yf*0.7, zf*0.8);
            //3
            glTexCoord2f(0.0,0.0);
            glVertex3d(xf-0.1, yf*0.7, zf*0.2);

            glTexCoord2f(0.0,1.0);
            glVertex3d(xf-0.1, yf*0.2, zf*0.2);

            glTexCoord2f(1.0,1.0);
            glVertex3d(xf-0.1, yf*0.2,  zf*0.3);

            glTexCoord2f(1.0,0.0);
            glVertex3d(xf-0.1, yf*0.7, zf*0.3);

            //4
            glTexCoord2f(0.0,0.0);
            glVertex3d(xf-0.1, yf*0.7, zf*0.7);

            glTexCoord2f(0.0,1.0);
            glVertex3d(xf-0.1, yf*0.2, zf*0.7);

            glTexCoord2f(1.0,1.0);
            glVertex3d(xf-0.1, yf*0.2,  zf*0.8);

            glTexCoord2f(1.0,0.0);
            glVertex3d(xf-0.1, yf*0.7, zf*0.8);

        glEnd();
    glPopMatrix();
}

double animationstep;

void drawPool()
{
    glPushMatrix();
        // rotate to has right texture position
        double xf, xi, yf, yi, zi, zf, repeatedTextureY, repeatedTextureX;
        xi = -25.0;
        xf = 20.0;
        yi = 0.0;
        yf = 0.2;
        zi = -60.0;
        zf = -30.0;
        repeatedTextureX = 15.0;
        repeatedTextureY = 3.0;
        glTranslated(0.0,yf+yi,0.0);
        glRotated(180.0,0.0,0.0,1.0); // rotate around x axis of texture
        // no other trasform cause rotate 180 degrees
        glTranslated(xi,yi,zi);
        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, border.textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  0.0);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, 0.0);

        glEnd();

        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(0.0, yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(0.0, 0.0, zf);

        glEnd();

        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(xf, 0.0, 0.0);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(xf, yf, 0.0);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, zf);

        glEnd();

        glBegin(GL_QUADS);

            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, zf);

            glTexCoord2f(0.0,repeatedTextureY);
            glVertex3d(0.0, yf, zf);

            glTexCoord2f(repeatedTextureX,repeatedTextureY);
            glVertex3d(xf, yf,  zf);

            glTexCoord2f(repeatedTextureX,0.0);
            glVertex3d(xf, 0.0, zf);

        glEnd();

        // water
        // water animation
        animationstep = animationstep + 0.001;
        if(animationstep >= 1.0 || animationstep <= 0.0) animationstep = 0.0;
        glBindTexture(GL_TEXTURE_2D, water.textureId);
        glBegin(GL_QUADS);

            glTexCoord2f(0.0+animationstep,0.0+animationstep);
            glVertex3d(0.0, 0.0+0.1, 0.0);

            glTexCoord2f(0.0+animationstep,repeatedTextureY+animationstep);
            glVertex3d(0.0, 0.0+0.1, zf);

            glTexCoord2f(repeatedTextureX+animationstep,repeatedTextureY+animationstep);
            glVertex3d(xf, 0.0+0.1,  zf);

            glTexCoord2f(repeatedTextureX+animationstep,0.0+animationstep);
            glVertex3d(xf, 0.0+0.1, 0.0);

        glEnd();
    glPopMatrix();
}

void drawTree(TexturedTree* tree)
{
    glPushMatrix();
        //(x,y,z) center of figure
    glTranslated(tree->x-(tree->dimX/2), tree->y, tree->z+(tree->dimZ/2));
        glRotated(180.0,1.0,0.0,0.0); // rotate around x axis of texture
        //glTranslated(0.0,(tree->y/2)-(tree->dimY/2),0.0);
        glTranslated(0.0,-(tree->dimY/2),0.0);
        glColor4d(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, tree->textureId);
        glBegin(GL_QUADS);
            // a tree
            glTexCoord2f(0.0,0.0);
            glVertex3d(0.0, 0.0, (tree->dimZ/2));

            glTexCoord2f(0.0,1.0);
            glVertex3d(0.0, tree->dimY, (tree->dimZ/2));

            glTexCoord2f(1.0,1.0);
            glVertex3d(tree->dimX, tree->dimY, (tree->dimZ/2));

            glTexCoord2f(1.0,0.0);
            glVertex3d(tree->dimX, 0.0, (tree->dimZ/2));

            //a tree perpendicular
            glTexCoord2f(0.0,0.0);
            glVertex3d((tree->dimX/2), 0.0, 0.0);

            glTexCoord2f(0.0,1.0);
            glVertex3d((tree->dimX/2), tree->dimY, 0.0);

            glTexCoord2f(1.0,1.0);
            glVertex3d((tree->dimX/2), tree->dimY,  tree->dimZ);

            glTexCoord2f(1.0,0.0);
            glVertex3d((tree->dimX/2), 0.0, tree->dimZ);
        glEnd();
        glPopMatrix();
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();

    // look at using camera class
    gluLookAt(	camera.getCameraX(), camera.getCameraY(), camera.getCameraZ(),
                camera.getLookAtX(), camera.getLookAtY(), camera.getLookAtZ(),
                camera.getUpX(), camera.getUpY(), camera.getUpZ());

    glDisable(GL_LIGHTING); // until we define the lights
    // draw model
    drawGroundAndSky();

    drawTree(&alberellobello1);
    drawTree(&alberellobello2);
    drawTree(&alberellobello3);
    drawTree(&alberellobello4);

    drawHouse();
    drawPool();

    glEnable(GL_LIGHTING); // until we define the lights
    // draw fps
    drawTimer();

    // draw legend
    drawLegend();

    glFlush();
    glutSwapBuffers();
}

// draw a text in 2D (same position of text, camera position not affect changes)
void draw2DText(double x, double y, string text)
{
    // prepare to use a ortho2d modality
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, GLUT_SCREEN_WIDTH, 0, GLUT_SCREEN_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING); //disable lighting during text drawing
    glDisable(GL_TEXTURE_2D); //disable texture during text drawing

    //draw text
    glColor3d(DEFAULT_FONT_R, DEFAULT_FONT_G, DEFAULT_FONT_B);
    void *font= DEFAULT_FONT;
    char c;
    unsigned int i;
    glRasterPos2d(x, y);

    for(i=0; i<text.length(); i++)
    {
        c = text.at(i);
        glutBitmapCharacter(font, c);
    }
    //end draw text

    // exit from ortho 2d modality
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); //enable lighting after text drawing
    glEnable(GL_TEXTURE_2D); //enable texture after text drawing
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// same as 2d text but the position of text depends on camera position
void draw3DText(double x, double y, double z, string text)
{
    glPushMatrix();
    glDisable(GL_LIGHTING); //disable lighting during text drawing
    glDisable(GL_TEXTURE_2D); //disable texture during text drawing
    glColor3d(DEFAULT_FONT_R, DEFAULT_FONT_G, DEFAULT_FONT_B);
    void *font= DEFAULT_FONT;
    char c;
    unsigned int i;
    glRasterPos3d(x, y, z);

    for(i=0; i<text.length(); i++)
    {
        c = text.at(i);
        glutBitmapCharacter(font, c);
    }
    glEnable(GL_LIGHTING); //enable lighting after text drawing
    glEnable(GL_TEXTURE_2D); //enable texture after text drawing
    glPopMatrix();
}

// function that init model
void initModel()
{
    //new camera function
    // i'm in 0 0 50 and i look straight forward to 0 0 0l
    Angle tetaAngleRange;
    tetaAngleRange.setAngleLimit(-89.0, 89.0);
    camera = Camera(0.0, 0.0, 0.0, 100.0, tetaAngleRange, 0.0, -1.0);
    // remember in owr system of coordinates, z is negative
    camera.setCameraBound(-1000.0, 1000.0, 2.0, 200.0, 1000.0, -1000.0);

    // load texture

    initTextureId(&grass.textureId, "../SVDRV_as03/Image/grass.jpg", GL_RGB);
    initTextureId(&sky_line.textureId, "../SVDRV_as03/Image/sky_line.jpg", GL_RGB);
    initTextureId(&sky_plane.textureId, "../SVDRV_as03/Image/sky.jpg", GL_RGB);

    initTextureId(&window.textureId, "../SVDRV_as03/Image/window.jpg", GL_RGB);
    initTextureId(&door.textureId, "../SVDRV_as03/Image/door.jpg", GL_RGB);
    initTextureId(&water.textureId, "../SVDRV_as03/Image/water.jpg", GL_RGB);
    initTextureId(&border.textureId, "../SVDRV_as03/Image/marble.jpg", GL_RGB);
    initTextureId(&roof.textureId, "../SVDRV_as03/Image/roof.jpg", GL_RGB);
    initTextureId(&floorTexture.textureId, "../SVDRV_as03/Image/floor.jpg", GL_RGB);
    initTextureId(&wall.textureId, "../SVDRV_as03/Image/brick.jpg", GL_RGB);

    // first tree
    initTextureId(&alberellobello1.textureId, "../SVDRV_as03/Image/beech.png", GL_RGBA);
    alberellobello1.dimX = 70.0;
    alberellobello1.dimY = 60.0;
    alberellobello1.dimZ = 70.0;
    alberellobello1.x = 40.0;
    alberellobello1.y = (alberellobello1.dimY/2);
    alberellobello1.z = -80.0;
    // end first tree

    // second tree
    initTextureId(&alberellobello2.textureId, "../SVDRV_as03/Image/beech.png", GL_RGBA);
    alberellobello2.dimX = 60.0;
    alberellobello2.dimY = 80.0;
    alberellobello2.dimZ = 60.0;
    alberellobello2.x = -80.0;
    alberellobello2.y = (alberellobello2.dimY/2);
    alberellobello2.z = -90.0;
    // end second tree

    // third tree
    initTextureId(&alberellobello3.textureId, "../SVDRV_as03/Image/oak.png", GL_RGBA);
    alberellobello3.dimX = 120.0;
    alberellobello3.dimY = 100.0;
    alberellobello3.dimZ = 120.0;
    alberellobello3.x = -25.0;
    alberellobello3.y = (alberellobello3.dimY/2);
    alberellobello3.z = -140.0;
    // end fourth tree

    // fourth tree
    initTextureId(&alberellobello4.textureId, "../SVDRV_as03/Image/pine.png", GL_RGBA);
    alberellobello4.dimX = 2.0;
    alberellobello4.dimY = 7.0;
    alberellobello4.dimZ = 2.0;
    alberellobello4.x = -20.0;
    alberellobello4.y = (alberellobello4.dimY/2)+0.2;
    alberellobello4.z = -75.0;
    // end fourth tree

    showLabels = true;
    isRunning = true; //true
    isChanged = true;
}

void idleScene()
{
    // if the execution is running, update hours parameter
    //if(isRunning) hours += TIME_STEP;

    // draw the scene only if is changed
    if(isChanged)
    {
        drawScene();
        //isChanged = false;
    }
}
