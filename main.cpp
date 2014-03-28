#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "camera.h"

// sphere proprieties
#define SPHERE_SLICE 100
#define SPHERE_STACKS 100

// time step
#define TIME_STEP 10
#define STEP_TIME_STEP 1

// font
#define DEFAULT_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DEFAULT_FONT_R 1.0
#define DEFAULT_FONT_G 1.0
#define DEFAULT_FONT_B 1.0
#define DEFAULT_FONT_HEIGHT 24.0
#define DEFAULT_DELTA_FRACTION 1;
#define DEFAULT_WEEL_STEP 5;

// struct that contain a rotating sphere
typedef struct
{
    double radius; // radius of the sphere
    int rotationTime; //rotationTime is "daylenght" in hours per one rotation
    double r; // is the color of the sphere, can be used if we not use texture (test or early version)
    double g; // is the color of the sphere, can be used if we not use texture (test or early version)
    double b; // is the color of the sphere, can be used if we not use texture (test or early version)
    double angle; //angle of rotation
    GLuint textureId; // textureId is used to blind texture
} RotatingTexturedSphere;

// struct that rapresent a star
typedef struct
{
    double x,y,z; // position
    double light_intensity; // parameter used in light function
    char* name; // labelname of the star
    RotatingTexturedSphere sphereModel; // sphere used to draw the star
} Star;

// struct that rapresent a planet
typedef struct
{
    Star *star; // pointer to the star of the planet
    char *name; // labelname for the planet
    int revolutionTime; // revolution time in days for years (day is the one in RotatingTextureSphere.rotationTime
    double x,y,z; //position
    double angle; //angle of revolution
    RotatingTexturedSphere sphereModel; // sphere used to draw the planet
} Planet;

// next 3 function sets star value, used during "init"
void initStar(Star *star, char* name, double radius, double x, double y, double z, int rotationTime) //add texture!
{
    star->name = name;
    star->sphereModel.radius = radius;
    star->x = x;
    star->y = y;
    star->z = z;
    star->sphereModel.rotationTime = rotationTime;
    star->sphereModel.angle = 0.0;
}

void setStarLight(Star *star, double light_intensity)
{
    star->light_intensity = light_intensity;
}

void setStarColor(Star *star, double r, double g, double b)
{
    star->sphereModel.r = r;
    star->sphereModel.g = g;
    star->sphereModel.b = b;
}

// next 2 function sets planet value, used during "init"
void setPlanetColor(Planet *planet, double r, double g, double b)
{
    planet->sphereModel.r = r;
    planet->sphereModel.g = g;
    planet->sphereModel.b = b;
}

void initPlanet(Planet *planet, char* name, Star *star, double radius, double x, double y, double z, int rotationTime, int revolutionTime)
{
    planet->name = name;
    planet->star = star;
    planet->sphereModel.radius = radius;
    planet->x = x;
    planet->y = y;
    planet->z = z;
    planet->sphereModel.rotationTime = rotationTime;
    planet->sphereModel.angle = 0.0;
    planet->revolutionTime = revolutionTime;
    planet->angle = 0.0;
}

// update the star position, using hours for calculate position and angle
void updateStarPosition(Star *star, int hours)
{
    int rotationHours = hours%star->sphereModel.rotationTime;

    double rotationAngle = 360.0*((double)rotationHours/(double)star->sphereModel.rotationTime);
    star->sphereModel.angle = rotationAngle;
}

// update the planet position, using hours for calculate angle
void updatePlanetPosition(Planet *planet, int hours)
{
    int rotationHours = hours%planet->sphereModel.rotationTime;
    double rotationAngle = 360.0*((double)rotationHours/(double)planet->sphereModel.rotationTime);

    int revolutionDays = (hours/planet->sphereModel.rotationTime)%planet->revolutionTime;
    double revolutionAngle = 360.0*((double)revolutionDays/(double)planet->revolutionTime);

    planet->angle = revolutionAngle;
    planet->sphereModel.angle = rotationAngle;
}

void handleKeypress(unsigned char key,int x,int y);
void handleSpecialKeypress(int key,int x,int y);
void drawScene();
void idleScene();
void initRendering();
void changeSize(int w, int h);
void drawStar(Star *star);
void drawPlanet(Planet *planet);
void initModel();
void draw2DText(double x, double y, char* text);
void draw3DText(double x, double y, double z, char* text);
void initRotatingTexturedSphere(RotatingTexturedSphere *sphere, char* filename, GLenum rgbMode);
void initTextureId(GLuint *textureId, char* filename, GLenum rgbMode);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void pressKey(int key, int xx, int yy);
void releaseKey(int key, int x, int y);

// variabiles
Star sun, universe; // 2 stars: one is the sun, the other is used to rappresent the universe (the background)
Planet p1,p2,p3,p4,p5,p6,p7,p8; // 8 planet
double hours; // simulation time
int showLabels, isRunning, activeKeyModifiers; // flags
double startX, startY, startZ; // starting position of the camera
double deltaX, deltaY, deltaZ; // camera position modifier
double deltaXMin, deltaXMax, deltaYMin, deltaYMax, deltaZMin, deltaZMax; // camera modifier bounds (not really used)
int xOrigin, yOrigin; //used for mouse movement
Camera camera;

// load a texture and link to a RotatingTexturedSphere
void initRotatingTexturedSphere(RotatingTexturedSphere *sphere, char* filename, GLenum rgbMode)
{
    initTextureId(&sphere->textureId, filename, rgbMode);
}

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
    // Initialize glut
    glutInit(&argc,argv);
    // Initialize display (RGBA has alpha channel
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    // -1 -1 automatic positioning
    glutInitWindowPosition(0, 0); // Window position (from top corner), and size (width and hieght)
    // i want 16:9 "cinema" like
    glutInitWindowSize( 1600, 900);
    // window name
    glutCreateWindow("SVDRV_as02");
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
            camera.moveCameraForward();
            break;
        case 's':
        case 'S':
            camera.moveCameraBackward();
            break;
        case 'a':
        case 'A':
            camera.moveCameraLeft();
            break;
        case 'd':
        case 'D':
            camera.moveCameraRight();
            break;
        case 'r':
        case 'R':
            camera.moveCameraUp();
            break;
        case 'f':
        case 'F':
            camera.moveCameraDown();
            break;
        case 'j':
        case 'J':
            camera.rotateCameraLeft();
            break;
        case 'l':
        case 'L':
            camera.rotateCameraRight();
            break;
        case 'i':
        case 'I':
            camera.rotateCameraUp();
            break;
        case 'k':
        case 'K':
            camera.rotateCameraDown();
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
            camera.moveForward();
            break;
        case GLUT_KEY_DOWN:
            camera.moveBackward();
            break;
        case GLUT_KEY_LEFT:
            camera.strafeLeft();
            break;
        case GLUT_KEY_RIGHT:
            camera.strafeRight();
            break;
    }

    return (void)(x+y);
}

void pressKey(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_CTRL_L:
        case GLUT_KEY_ALT_L:
        case GLUT_KEY_ALT_R:
        case GLUT_KEY_CTRL_R:
        case GLUT_KEY_SHIFT_L:
        case GLUT_KEY_SHIFT_R: activeKeyModifiers = 1;
    }

    // used to not have warning (i hate warning!)
    return (void)(x+y);
}

void releaseKey(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_CTRL_L:
        case GLUT_KEY_ALT_L:
        case GLUT_KEY_ALT_R:
        case GLUT_KEY_CTRL_R:
        case GLUT_KEY_SHIFT_L:
        case GLUT_KEY_SHIFT_R: activeKeyModifiers = 0;
    }

    // used to not have warning (i hate warning!)
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
    gluPerspective(60.0, ratio, 1, -1); // like "infinite"

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

// function that draw a star
void drawStar(Star *star)
{
    glPushMatrix();
        glTranslated(star->x,star->y,star->z);
        if(showLabels) draw3DText(star->x,star->y+1.10*star->sphereModel.radius,star->z, star->name);
        glRotated(star->sphereModel.angle,0.0,1.0,0.0); // rotate around y axis of star

        // case solid sphere
        //glColor3d(star->sphereModel.r, star->sphereModel.g, star->sphereModel.b);
        //glutSolidSphere(star->sphereModel.radius,SPHERE_SLICE,SPHERE_STACKS);

        // case textured sphere
        // when draw a star, it is not affected by the light!!!
        glDisable(GL_LIGHTING); // disable light
        GLUquadric *quad;
        quad = gluNewQuadric();
        glBindTexture(GL_TEXTURE_2D,star->sphereModel.textureId);
        gluQuadricTexture(quad,GL_TRUE);
        // rotate to has right texture position
        glRotated(90.0,1.0,0.0,0.0); // rotate around x axis of star
        gluSphere(quad,star->sphereModel.radius,SPHERE_SLICE,SPHERE_STACKS);
        glEnable(GL_LIGHTING); // renable light

        // light produced by the star
        glEnable(GL_LIGHT0);
        GLfloat light0_position[] = {0.0,0.0,0.0,star->light_intensity};
        GLfloat light0_ambient[] = {0.2,0.2,0.2,star->light_intensity};
        GLfloat light0_diffused[] = {1.0,1.0,1.0,star->light_intensity};
        GLfloat light0_specular[] = {1.0,1.0,1.0,star->light_intensity};
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffused);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glPopMatrix();
}

// function that draw a planet
void drawPlanet(Planet *planet)
{
    glPushMatrix();
        glTranslated(planet->star->x,planet->star->y,planet->star->z);
        glRotated(planet->angle,0.0,1.0,0.0); // rotate around y axis of planet's star
        glPushMatrix();
            //drawText(planet->x,planet->y+3*planet->sphereModel.radius,planet->z, planet->name);
            if(showLabels) draw3DText(planet->x,planet->y+1.10*planet->sphereModel.radius,planet->z, planet->name);
            glTranslated(planet->x,planet->y,planet->z);
            glRotated(planet->sphereModel.angle,0.0,1.0,0.0); // rotate around y axis of planet

            // case solid sphere
            //glColor3d(planet->sphereModel.r, planet->sphereModel.g, planet->sphereModel.b);
            //glutSolidSphere(planet->sphereModel.radius,SPHERE_SLICE,SPHERE_STACKS);

            // case textured sphere
            GLUquadric *quad;
            quad = gluNewQuadric();
            //glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,planet->sphereModel.textureId);
            gluQuadricTexture(quad,GL_TRUE);
            // rotate to has right texture position
            glRotated(90.0,1.0,0.0,0.0); // rotate around x axis of star
            gluSphere(quad,planet->sphereModel.radius,SPHERE_SLICE,SPHERE_STACKS);
            // red dot, check rotation time used during test
            //glColor3d(1.0,0.0,0.0);
            //glTranslated(planet->sphereModel.radius,0.0,0.0);
            //glutSolidSphere(0.1,SPHERE_SLICE,SPHERE_STACKS);
        glPopMatrix();
    glPopMatrix();
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();

    // look at the origin spot (0,0,0) from camera position, moved by mouse
    /*gluLookAt(	startX+deltaX, startY+deltaY, startZ+deltaZ, //camera pos
                0.0, 0.0,   0.0, //looking at
                0.0, 1.0,   0.0); //up orientation*/
    gluLookAt(	camera.getCameraX(), camera.getCameraY(), camera.getCameraZ(),
                camera.getLookAtX(), camera.getLookAtY(), camera.getLookAtZ(),
                camera.getUpX(), camera.getUpY(), camera.getUpZ());

    // draw model elements (stars and planet)
    drawStar(&universe);
    drawStar(&sun);
    drawPlanet(&p1);
    drawPlanet(&p2);
    drawPlanet(&p3);
    drawPlanet(&p4);
    drawPlanet(&p5);
    drawPlanet(&p6);
    drawPlanet(&p7);
    drawPlanet(&p8);

    // draw time
    char time[10000];
    int int_hours = (int)(hours);
    int int_days = int_hours/24;
    int int_years = int_days/365;
    sprintf(time, "Time:\tY-%d\tD-%d\tH-%d", int_years, int_days%365, int_hours%24);
    draw2DText(1.0,GLUT_WINDOW_HEIGHT*1.89, time);

    // draw legend
    char legend[10000];
    sprintf(legend, "\"ESC\" for exit, \"t\" for planet label, \"s\" for step animation, \"r\" for running animation, Mouse moves camera (wheel or ctrl | shift | alt for zoom) ");
    draw2DText(1.0, 1.0, legend);

    glFlush();
    glutSwapBuffers();
}

// draw a text in 2D (same position of text, camera position not affect changes)
void draw2DText(double x, double y, char* text)
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

    for(i=0; i<strlen(text); i++)
    {
        c = text[i];
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
void draw3DText(double x, double y, double z, char* text)
{
    glPushMatrix();
    glDisable(GL_LIGHTING); //disable lighting during text drawing
    glDisable(GL_TEXTURE_2D); //disable texture during text drawing
    glColor3d(DEFAULT_FONT_R, DEFAULT_FONT_G, DEFAULT_FONT_B);
    void *font= DEFAULT_FONT;
    char c;
    unsigned int i;
    glRasterPos3d(x, y, z);

    for(i=0; i<strlen(text); i++)
    {
        c = text[i];
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
    camera = Camera(0.0, 0.0, -400.0);

    // camera position
    startX=0;
    startY= 500.0;
    startZ= -500.0;
    // camera modifier
    deltaZMax = -(startZ/2.0);
    deltaZMin = -10000.0;
    // not used
    deltaXMax = -500.0;
    deltaXMin = +500.0;
    deltaYMax = -500.0;
    deltaYMin = +500.0;
    // end not used

    showLabels = 0; //false
    isRunning = 1; //true

    // it's for background stars effect
    initStar(&universe,"universe", 18000.0,0.0,0.0,0.0,1000000.0);
    setStarColor(&universe, 1.0, 1.0, 1.0);
    setStarLight(&universe, 0.0);
    initRotatingTexturedSphere(&universe.sphereModel, "../SVDRV_as02/image/universemap.jpg", GL_RGB);

    // add a star in this case sun
    // i make the sun really small cause i can't respect proportion
    // diam = 1391000km
    // tr = 27d 6h = ~650h (?)
    initStar(&sun,"SUN", 139.1000,0.0,0.0,0.0,650);
    setStarColor(&sun, 1.0, 1.0, 1.0);
    setStarLight(&sun, 1.0);
    initRotatingTexturedSphere(&sun.sphereModel, "../SVDRV_as02/image/sun.jpg", GL_RGB);

    // planet data from wikipedia, not really checked eventual error

    // add a planet in this case earth
    // diam 12756km
    // tr 24 h
    // tR 365 g
    // orbit 6.1 UA
    initPlanet(&p1, "Earth", &sun, 127.56, 610,0.0,0.0,24,365);
    setPlanetColor(&p1, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p1.sphereModel, "../SVDRV_as02/image/earthmap1k.jpg", GL_RGB);

    // add a planet in this case Mercury
    // diam 4879km
    // tr 58g
    // tR 88g
    // orbit 4.5 UA
    initPlanet(&p2, "Mercury", &sun, 48.79, 450,0.0,0.0,58*24,88);
    setPlanetColor(&p2, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p2.sphereModel, "../SVDRV_as02/image/mercurymap.jpg", GL_RGB);

    // add a planet in this case Venus
    // diam 12103km
    // tr 243g
    // tR 224g
    // orbit 2.4 UA
    initPlanet(&p3, "Venus", &sun, 12.103, 240,0.0,0.0,243*24,224);
    setPlanetColor(&p3, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p3.sphereModel, "../SVDRV_as02/image/venusmap.jpg", GL_RGB);

    // add a planet in this case Mars
    // diam 6804km
    // tr 24h
    // tR 687g
    // orbit 9.5 UA
    initPlanet(&p4, "Mars", &sun, 6.804, 950,0.0,0.0,24,687);
    setPlanetColor(&p4, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p4.sphereModel, "../SVDRV_as02/image/mars_1k_color.jpg", GL_RGB);

    // add a planet in this case Jupiter
    // diam 142984km
    // tr 10h
    // tR 4333g
    // orbit 32,67 UA
    initPlanet(&p5, "Jupiter", &sun, 142.984, 3267,0.0,0.0,9,4333);
    setPlanetColor(&p5, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p5.sphereModel, "../SVDRV_as02/image/jupitermap.jpg", GL_RGB);

    // add a planet in this case Saturn
    // diam 120536km
    // tr 11h
    // tR 10756g
    // orbit 59,9 UA
    initPlanet(&p6, "Saturn", &sun, 120.536, 5990,0.0,0.0,11,10756);
    setPlanetColor(&p6, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p6.sphereModel, "../SVDRV_as02/image/saturnmap.jpg", GL_RGB);

    // add a planet in this case Uranus
    // diam 51118km
    // tr 17h
    // tR 42718 d
    // orbit 120,5 UA
    initPlanet(&p7, "Uranus", &sun, 51.118, 12050,0.0,0.0,17,42718);
    setPlanetColor(&p7, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p7.sphereModel, "../SVDRV_as02/image/uranusmap.jpg", GL_RGB);

    // add a planet in this case Neptune
    // diam 49528km
    // tr 16h
    // tR 60233g
    // orbit 189 UA
    initPlanet(&p8, "Neptune", &sun, 49.528, 18900,0.0,0.0,16,60233);
    setPlanetColor(&p8, 0.0, 1.0, 1.0);
    initRotatingTexturedSphere(&p8.sphereModel, "../SVDRV_as02/image/neptunemap.jpg", GL_RGB);
}

void idleScene()
{
    // if the execution is running, update hours parameter
    if(isRunning) hours += TIME_STEP;

    // update all planet position
    updateStarPosition(&universe, hours);
    updateStarPosition(&sun, hours);
    updatePlanetPosition(&p1, hours);
    updatePlanetPosition(&p2, hours);
    updatePlanetPosition(&p3, hours);
    updatePlanetPosition(&p4, hours);
    updatePlanetPosition(&p5, hours);
    updatePlanetPosition(&p6, hours);
    updatePlanetPosition(&p7, hours);
    updatePlanetPosition(&p8, hours);

    // draw the scene
    drawScene();
}

void mouseMove(int x, int y)
{
    // this will only be true when the left button is down
    if (xOrigin >= 0)
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
    }
}

void mouseButton(int button, int state, int x, int y)
{
    // only start motion if the left button is pressed
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
    }
}
