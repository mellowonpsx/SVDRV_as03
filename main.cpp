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
#define DEFAULT_WEEL_STEP 5;

void handleKeypress(unsigned char key,int x,int y);
void handleSpecialKeypress(int key,int x,int y);
void drawScene();
void idleScene();
void initRendering();
void changeSize(int w, int h);
void initModel();
void draw2DText(double x, double y, string text);
void draw3DText(double x, double y, double z, string text);
void initTextureId(GLuint *textureId, char* filename, GLenum rgbMode);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void pressKey(int key, int xx, int yy);
void releaseKey(int key, int x, int y);

// variabiles
bool isChanged, isRunning, showLabels;
Camera camera;

// load a texture and link to a identifier
void initTextureId(GLuint *textureId, char* filename, GLenum rgbMode)
{
    // load file & error check
    SDL_Surface* surface = IMG_Load(filename);
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
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
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
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
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

    return (void)(x+y);
}

void pressKey(int key, int x, int y)
{
/*    switch (key)
    {
        case GLUT_KEY_CTRL_L:
        case GLUT_KEY_ALT_L:
        case GLUT_KEY_ALT_R:
        case GLUT_KEY_CTRL_R:
        case GLUT_KEY_SHIFT_L:
        case GLUT_KEY_SHIFT_R: activeKeyModifiers = 1;
    }

    // used to not have warning (i hate warning!)
    return (void)(x+y);*/
}

void releaseKey(int key, int x, int y)
{
    /*switch (key)
    {
        case GLUT_KEY_CTRL_L:
        case GLUT_KEY_ALT_L:
        case GLUT_KEY_ALT_R:
        case GLUT_KEY_CTRL_R:
        case GLUT_KEY_SHIFT_L:
        case GLUT_KEY_SHIFT_R: activeKeyModifiers = 0;
    }

    // used to not have warning (i hate warning!)
    return (void)(x+y);*/
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
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_CULL_FACE); //if used i don't see the "inside" of sphere->trick used to fancy universe effect
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

int angle;

void drawGround()
{
    angle++;
    angle%=360;
    glDisable(GL_LIGHTING);
    //glColor4d(1.0, 1.0, 1.0, 1.0);
    //glRotated(angle, 1.0,1.0,1.0);
    glColor4d(0.2, 0.2, 0.2, 1.0);
    glBegin(GL_QUADS);
        glVertex3d(-100.0, 0.0, -100.0);
        glVertex3d(-100.0, 0.0,  100.0);
        glVertex3d( 100.0, 0.0,  100.0);
        glVertex3d( 100.0, 0.0, -100.0);
    glEnd();
    glColor4d(0.2, 0.0, 0.0, 0.2);
    glBegin(GL_QUADS);
        glVertex3d(-100.0, -100.0, -100.0);
        glVertex3d(-100.0, 100.0, -100.0);
        glVertex3d( 100.0, 100.0,-100.0);
        glVertex3d( 100.0, -100.0, -100.0);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();

    // look at using camera class
    /*cout << camera.getCameraX() << " " << camera.getCameraY() << " " << camera.getCameraZ() << " "
         << camera.getLookAtX() << " " << camera.getLookAtY() << " " << camera.getLookAtZ() << endl;*/
    gluLookAt(	camera.getCameraX(), camera.getCameraY(), camera.getCameraZ(),
                camera.getLookAtX(), camera.getLookAtY(), camera.getLookAtZ(),
                camera.getUpX(), camera.getUpY(), camera.getUpZ());

    // draw model
    drawGround();

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
    // i'm in 0 0 500 and i look straight forward to 0 0 0l
    Angle tetaAngleRange;
    tetaAngleRange.setAngleLimit(-80.0, 90.0);
    camera = Camera(0.0, 20.0, 500.0, 100.0, tetaAngleRange, 0.0, -1.0);
    camera.setCameraBound(-1000.0, 1000.0, 0.0, 1000.0, -1000.0, 1000.0);

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

void mouseMove(int x, int y)
{
    // this will only be true when the left button is down
    /*if (xOrigin >= 0)
    {
        // don't like this but is what i have
        if(activeKeyModifiers)
        {
             deltaZ = -(y - yOrigin)*DEFAULT_DELTA_FRACTION;
        }
        else
        {
            deltaX = (x - xOrigin)*DEFAULT_DELTA_FRACTION;
            deltaY = (y - yOrigin)*DEFAULT_DELTA_FRACTION;
        }
    }*/
}

void mouseButton(int button, int state, int x, int y)
{
    /*// only start motion if the left button is pressed
    if (button == GLUT_LEFT_BUTTON)
    {
        // when the button is released
        if (state == GLUT_UP)
        {
            xOrigin = -1;
            yOrigin = -1;
        }
        else
        {
            // state = GLUT_DOWN
            xOrigin = x;
            yOrigin = y;
        }
    }
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if (button == 3) // It's a wheel event
    {
        if(state == GLUT_DOWN)
            if(deltaZ <= deltaZMax)
            {
                deltaZ+= DEFAULT_WEEL_STEP;
            }
    }
    if (button == 4)
    {
        if(state == GLUT_DOWN)
            if(deltaZ >= deltaZMin)
            {
                deltaZ-= DEFAULT_WEEL_STEP;
            }
    }*/
}
