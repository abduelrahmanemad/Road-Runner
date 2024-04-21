#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "getBMP.h"

#define ROWS 3  // Number of rows of asteroids.
#define COLUMNS 3 // Number of columns of asteroids.
#define FILL_PROBABILITY 50 // Percentage probability that a particular row-column slot will be
// filled with an asteroid. It should be an integer between 0 and 100.

// Globals.
static long font = (long long)GLUT_BITMAP_8_BY_13; // Font selection.
static int width, height; // Size of the OpenGL window.
static float angle = 12.0; // Angle of the spacecraft.
static float xVal = 0, zVal = 0; // Co-ordinates of the spacecraft.
static int isCollision = 0; // Is there collision between the spacecraft and an asteroid?
static int isTargetReached = 0; // Is the spacecraft reached the target?
static unsigned int spacecraft; // Display lists base index.
static int frameCount = 0; // Number of frames
static int numCubes = 4;
int finishX , finishZ = 0;
///////////////////

static unsigned int texture[4]; // Array of texture indices.
//static float angle = 0.0; // Angle of rotation of texture coordinates.

static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

static unsigned char chessboard[64][64][4]; // Storage for chessboard image.
static int ground_id = 0;
static int sky_id = 1;

////// car spotlight

static float xSpot1 = -9, zSpot1 = 5; // Co-ordinates of the first headlight.
static float xSpot2 = -9, zSpot2 = 1; // Co-ordinates of the second headlight.


// Globals.
float lightPos[] = { 0.0, 5.0, -2, 1.0 }; // Spotlight position.
static float spotAngle = 10.0; // Spotlight cone half-angle.
//float spotDirection[] = { 0.0, -1.0, 0.0 }; // Spotlight direction.
float spotDirection[] = { xSpot1, 0.0, 0.0 }; // Spotlight direction.
static float spotExponent = 2; // Spotlight attenuation exponent.


////////////
void loadTextures()
{
    // Local storage for bmp image data.
    imageFile *image[3];

    // Load the images.
    image[0] = getBMP("grass.bmp");
    image[1] = getBMP("sky.bmp");
    image[2] = getBMP("launch.bmp");
    // Bind grass image to texture object texture[0].
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Bind sky image to texture object texture[1]
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Bind 2nd image to texture object texture[2].
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void createChessboard(void)
{
    int i, j;
    for (i = 0; i < 64; i++)
        for (j = 0; j < 64; j++)
            if ((((i / 8) % 2) && ((j / 8) % 2)) || (!((i / 8) % 2) && !((j / 8) % 2)))
            {
                chessboard[i][j][0] = 0x00;
                chessboard[i][j][1] = 0x00;
                chessboard[i][j][2] = 0x00;
                chessboard[i][j][3] = 0xFF;
            }
            else
            {
                chessboard[i][j][0] = 0xDD;
                chessboard[i][j][1] = 0xDD;
                chessboard[i][j][2] = 0xDD;
                chessboard[i][j][3] = 0xFF;
            }
}

// Routine to load a program-generated image as a texture.
void loadChessboardTexture()
{
    // Generate internal texture.
    createChessboard();

    // Create texture object texture[1].
    glBindTexture(GL_TEXTURE_2D, texture[3]);

    // Specify image data for currently active texture object.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, chessboard);

    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void top_menu(int id)
{
    if (id == 1) exit(0);
}
void ground_menu(int id){
    if (id == 0) // grass
    {
        ground_id = 0;
    }
    if (id == 2)
    {
        ground_id = 2;
    }
    if (id == 3) // chess
    {
        ground_id = 3;
    }
    glutPostRedisplay();
}
void sky_menu(int id){
    if (id == 1)
    {
        sky_id = 1;
    }
    if (id == 3)
    {
        sky_id = 3;
    }
    glutPostRedisplay();
}
// Routine to make the menu.
void makeMenu(void)
{
    // The sub-menu is created first (because it should be visible when the top
    // menu is created): its callback function is registered and menu entries added.
    int grass_sub_menu;
    int sky_sub_menu;
    grass_sub_menu = glutCreateMenu(ground_menu);
    glutAddMenuEntry("Grass", 0);
    glutAddMenuEntry("Nasa/ELON_MUSK", 2);
    glutAddMenuEntry("Chess", 3);
    sky_sub_menu = glutCreateMenu(sky_menu);
    glutAddMenuEntry("Sky", 1);
    glutAddMenuEntry("Chess", 3);
    // The top menu is created: its callback function is registered and menu entries,
    // including a submenu, added.
    glutCreateMenu(top_menu);
    glutAddSubMenu("Ground Sub Menu", grass_sub_menu);
    glutAddSubMenu("Sky Sub Menu", sky_sub_menu);
    glutAddMenuEntry("Quit", 1);

    // The menu is attached to a mouse button.
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


//////////////

void Reset(int value);
// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
    char *c;

    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Obtscale class.
class Obtscale
{
public:
    Obtscale();
    Obtscale(float x, float y, float z, float r, unsigned char colorR,
             unsigned char colorG, unsigned char colorB);
    float getCenterX() { return centerX; }
    float getCenterY() { return centerY; }
    float getCenterZ() { return centerZ; }
    float getRadius() { return radius; }
    void draw();

private:
    float centerX, centerY, centerZ, radius;
    unsigned char color[3];
};
Obtscale target;
// Obtscale default constructor.
Obtscale::Obtscale()
{
    centerX = 0.0;
    centerY = 0.0;
    centerZ = 0.0;
    radius = 0.0; // Indicates no asteroid exists in the position.
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
}

// Obtscale constructor.
Obtscale::Obtscale(float x, float y, float z, float s, unsigned char colorR,
                   unsigned char colorG, unsigned char colorB)
{
    centerX = x;
    centerY = y;
    centerZ = z;
    radius =  sqrt(3*(s/2)*(s/2));
    color[0] = colorR;
    color[1] = colorG;
    color[2] = colorB;
}

// Function to draw asteroid.
void Obtscale::draw()
{
    if (radius > 0.0) // If asteroid exists.
    {
        glPushMatrix();
        glTranslatef(centerX, centerY, centerZ);
        glColor3ubv(color);
        glutSolidCube(5);
        glPopMatrix();
    }
}

Obtscale arrayAsteroids[ROWS][COLUMNS]; // Global array of asteroids.

// Routine to count the number of frames drawn every second.
void frameCounter(int value)
{
    if (value != 0) // No output the first time frameCounter() is called (from main()).
        std::cout << "FPS = " << frameCount << std::endl;
    frameCount = 0;
    glutTimerFunc(1000, frameCounter, 1);
}
void drawCircle(float x, float y, float z, float radius, int segments) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(1.0, 1.0, 0.0); // Yellow color for the circle
    glBegin(GL_TRIANGLE_FAN);

    for (int i = 0; i <= segments; i++) {
        float theta = (2.0f * 3.1415926f * float(i)) / float(segments);
        float xPos = radius * cosf(theta);
        float yPos = radius * sinf(theta);

        glVertex3f(xPos, yPos, 0.0);
    }

    glEnd();
    glPopMatrix();
}
// Initialization routine.
void setup(void)
{
    int i, j;

    spacecraft = glGenLists(1);
    glNewList(spacecraft, GL_COMPILE);

    glRotatef(180.0, 0.0, 1.0, 0.0); // To make the spacecraft point down the $z$-axis initially.
    glPushMatrix();
    glScalef(1,0.4,2);
    glColor3f(1.0, 1.0, 1.0);
    glutSolidCube(5.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,2,0);
    glColor3f(0.0, 0.0, 1.0);
    glScalef(1,0.4,2);
    glutSolidCube(3);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(3,-1,2.5);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(90,0,1,0);
    glutSolidTorus(0.5, 1, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-3,-1,2.5);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(90,0,1,0);
    glutSolidTorus(0.5, 1, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(3,-1,-2.5);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(90,0,1,0);
    glutSolidTorus(0.5, 1, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-3,-1,-2.5);
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(90,0,1,0);
    glutSolidTorus(0.5, 1, 10, 10);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(xSpot1 ,1.0 ,zSpot1 ); // Move the spotlight.
    glRotatef(angle, 0.0, 1.0, 1.0);
    // Draw the spotlight cone in wireframe after disabling lighting
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.7);
    glutSolidCone(3.0 * tan(spotAngle / 180.0 * M_PI), 2.0, 20, 20);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    // Spotlight position.
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Spotlight properties.
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
    glPopMatrix();
//////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(xSpot2 ,1.0 ,zSpot2 ); // Move the spotlight.
    glRotatef(angle, 0.0, 1.0, 1.0);
    // Draw the spotlight cone in wireframe after disabling lighting
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.7);
    glutSolidCone(3.0 * tan(spotAngle / 180.0 * M_PI), 2.0, 20, 20);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    // Spotlight position.
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Spotlight properties.
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);

    glPopMatrix();
//////////////////////////////////


    glPopMatrix();

    glEndList();

    glDisable(GL_LIGHTING);
    for (int cube = 0; cube < numCubes; cube++) {
        int randomColumn = rand() % COLUMNS;
        int randomRow = rand() % ROWS;

        // Check if the position is not occupied
        if (arrayAsteroids[randomRow][randomColumn].getRadius() == 0.0) {
            // Position the asteroid
            arrayAsteroids[randomRow][randomColumn] = Obtscale(
                    30.0 * (-COLUMNS / 2 + randomColumn), 0.0, -40.0 - 30.0 * randomRow,
                    5.0, rand() % 256, rand() % 256, rand() % 256
            );
        } else {
            // If the position is occupied, decrement the loop counter to repeat the iteration
            cube--;
        }
    }
    // create the target
    target =  Obtscale(30.0 * (-COLUMNS / 2 + 1 ), 0.0, -80.0 - 30.0 * 2, 20.0,
                       233, 233, 233);


    makeMenu();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glutTimerFunc(0, frameCounter, 0); // Initial call of frameCounter().

    //////////////////
    glGenTextures(4, texture);

    // Load external textures.
    loadTextures();
    loadChessboardTexture();

    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Turn on OpenGL texturing.
    glEnable(GL_TEXTURE_2D);
    /////////////////
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float globAmb[] = { 0.05, 0.05, 0.05, 1.0 };

    // Light properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);

    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.

    // Material property vectors.
    float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float matShine[] = { 50.0 };

    // Material properties shared by all the spheres.
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);


    // Enable color material mode:
    // The ambient and diffuse color of the front faces will track the color set by glColor().
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
/////////////////////////////////////////////////////////////////////   the general Light of sun
    float starDirection[] = { 0.1, -0.9, -0.9, 0.1 };
    float starAmbient[] = { 0.9, 173.0 / 255.0 , 67.0 / 255.0, 1.0 };
    float starDiffuse[] = { 0.9, 173.0 / 255.0 , 67.0 / 255.0, 1.0 };
    glClearColor(0.1, 0.1, 0.1, 0.1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT4);
    glLightfv(GL_LIGHT4, GL_AMBIENT, starAmbient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, starDiffuse);
    glLightfv(GL_LIGHT4, GL_POSITION, starDirection);
    float surfaceMaterialAmbientAndDiffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, surfaceMaterialAmbientAndDiffuse);
/////////////////////////////////////////////////////////////////////


}

// Function to check if two spheres centered at (x1,y1,z1) and (x2,y2,z2) with
// radius r1 and r2 intersect.
int checkSpheresIntersection(float x1, float y1, float z1, float r1,
                             float x2, float y2, float z2, float r2)
{
//    std::cout << "x1: " << x1 << " y1: " << y1 << " z1: " << z1 << " r1: " << r1 << std::endl;
    return ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2) <= (r1 + r2)*(r1 + r2));
}

// Function to check if the spacecraft collides with an asteroid when the center of the base
// of the craft is at (x, 0, z) and it is aligned at an angle a to to the -z direction.
// Collision detection is approximate as instead of the spacecraft we use a bounding sphere.
int asteroidCraftCollision(float x, float z, float a)
{
    int i, j;

    // Check for collision with each asteroid.
    for (j = 0; j<COLUMNS; j++)
        for (i = 0; i<ROWS; i++)
            if (arrayAsteroids[i][j].getRadius() > 0) // If asteroid exists.
                if (checkSpheresIntersection(x - 5 * sin((M_PI / 180.0) * a), 0.0,
                                             z - 5 * cos((M_PI / 180.0) * a), 1.5,
                                             arrayAsteroids[i][j].getCenterX(), arrayAsteroids[i][j].getCenterY(),
                                             arrayAsteroids[i][j].getCenterZ(), arrayAsteroids[i][j].getRadius()))
                    return 1;
    return 0;
}
int targetCraftCollision(float x, float z, float a)
{
    if( x>=30.0*(-COLUMNS / 2+1 )-10 and  x<=30.0*(-COLUMNS / 2+1 )+10  and z <=( -80.0 - 30.0*2)+10  and   z >=( -80.0 - 30.0*2)-10 )
        return 1;
    return 0;
}

// Drawing routine.
void drawScene(void)
{
    frameCount++; // Increment number of frames every redraw.

    int i, j;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Begin left viewport.
    glViewport(0, 0, width / 2.0, height);
    glLoadIdentity();

    // Write text in isolated (i.e., before gluLookAt) translate block.
    glPushMatrix();

    if (isCollision) {
        glColor3f(1.0, 0.0, 0.0);
        glRasterPos3f(-28.0, 25.0, -30.0);
        writeBitmapString((void*)font, "You lose! collision");
        glutTimerFunc(3000, Reset, 1);
    }
    else if (isTargetReached) {
        glColor3f(0.0, 1.0, 0.0);
        glRasterPos3f(-28.0, 25.0, -30.0);
        writeBitmapString((void*)font, "You win! target reached");
        glutTimerFunc(3000, Reset, 1);
    }

    glPopMatrix();

    // Fixed camera.
    gluLookAt(xVal, 10.0, zVal+20, xVal, 0.0, zVal, 0.0, 1.0, 0.0);
    ///////////
    glEnable(GL_TEXTURE_2D);
    // Map the grass texture onto a rectangle along the xz-plane.
    glBindTexture(GL_TEXTURE_2D, texture[ground_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -3.0, 90.0);
    glTexCoord2f(8.0, 0.0); glVertex3f(100.0, -3.0, 90.0);
    glTexCoord2f(8.0, 8.0); glVertex3f(100.0, -3.0, -325.0);
    glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, -3.0, -325.0);
    glEnd();

    // Apply translation to texture coordinates in a circular manner.
    glTranslatef(0.1 * cos(angle), 0.1 * sin(angle), 0.0);
    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

//////////
    ///////////

    glEnable(GL_TEXTURE_2D);
    // Map the sky texture onto a rectangle parallel to the xy-plane.  wesh
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-100.0, -3.0, -325.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(100.0, -3.0, -325.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(100.0, 150.0, -325.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-100.0, 150.0, -325.0);
    glEnd();

    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

//////////
    ///////////

    glEnable(GL_TEXTURE_2D);
    // Map the sky texture onto a rectangle parallel to the xy-plane.  dahr
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-100.0, -3.0, 90.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(100.0, -3.0, 90.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(100.0, 150.0, 90.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-100.0, 150.0, 90.0);
    glEnd();

    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

//////////
///////////

    glEnable(GL_TEXTURE_2D);
// Map the sky texture onto a rectangle parallel to the yz-plane.   ganb ymeen
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(90.0, -3.0, -325.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(90.0, 200.0, -325.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(90.0, 200.0, 90.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(90.0, -3.0, 90.0);
    glEnd();

// Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

///////////
///////////

    glEnable(GL_TEXTURE_2D);
// Map the sky texture onto a rectangle parallel to the yz-plane.   ganb shemal
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-90.0, -3.0, -325.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(-90.0, 200.0, -325.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(-90.0, 200.0, 90.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-90.0, -3.0, 90.0);
    glEnd();

// Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

///////////
    ///////////

    glEnable(GL_TEXTURE_2D);
    // Map the sky texture onto a rectangle parallel to the xy-plane.  sa2f
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-100.0, 50.0, 0.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(100.0, 50.0, 0.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(100.0, 50.0, -325.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-100.0, 50.0, -325.0);
    glEnd();

    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

//////////
//    gluLookAt(0.0, 90.0, -80.0, 0.0, 0.0, -90.0, 0.0, 1.0, 0.0);
    // Draw all the asteroids in arrayAsteroids.
    for (j = 0; j<COLUMNS; j++)
        for (i = 0; i<ROWS; i++)
            arrayAsteroids[i][j].draw();
    target.draw();
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); // Yellow color for the point

// Calculate the center of the top face of the cube.
    float topFaceCenterX = target.getCenterX();
    float topFaceCenterY = target.getCenterY() + target.getRadius() - 17.0; // Adjust the offset as needed.
    float topFaceCenterZ = target.getCenterZ()+ target.getRadius() - 15.0;

    glTranslatef(topFaceCenterX, topFaceCenterY, topFaceCenterZ);
    glutSolidSphere(1.50, 10, 10); // Adjust the parameters as needed for the desired appearance.
    glPopMatrix();

    // Draw spacecraft.
    glPushMatrix();
    glTranslatef(xVal, 0.0, zVal);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glCallList(spacecraft);
    glPopMatrix();
    // End left viewport.

    // Begin right viewport.
    glViewport(width / 2.0, 0, width / 2.0, height);
    glLoadIdentity();

    // Write text in isolated (i.e., before gluLookAt) translate block.
    glPushMatrix();

    if (isCollision) {
        glColor3f(1.0, 0.0, 0.0);
        glRasterPos3f(-28.0, 25.0, -30.0);
        writeBitmapString((void*)font, "You lose! collision");
        glutTimerFunc(3000, Reset, 1);
    }
    else if (isTargetReached) {
        glColor3f(0.0, 1.0, 0.0);
        glRasterPos3f(-28.0, 25.0, -30.0);
        writeBitmapString((void*)font, "You win! target reached");
        glutTimerFunc(3000, Reset, 1);
    }
    glPopMatrix();


    // Draw a vertical line on the left of the viewport to separate the two viewports
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex3f(-5.0, -5.0, -5.0);
    glVertex3f(-5.0, 5.0, -5.0);
    glEnd();
    glLineWidth(1.0);

    // Locate the camera at the tip of the cone and pointing in the direction of the cone.
    gluLookAt(xVal - 10 * sin((M_PI / 180.0) * angle),
              2.4,
              zVal - 10 * cos((M_PI / 180.0) * angle),
              xVal - 11 * sin((M_PI / 180.0) * angle),
              2.4,
              zVal - 11 * cos((M_PI / 180.0) * angle),
              0.0,
              1.0,
              0.0);

    // Draw all the asteroids in arrayAsteroids.
    for (j = 0; j<COLUMNS; j++)
        for (i = 0; i<ROWS; i++)
            arrayAsteroids[i][j].draw();
    target.draw();
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); // Yellow color for the point

// Calculate the center of the top face of the cube in the right viewport.
    float topFaceCenterXRight = target.getCenterX(); // Adjust for the right viewport.
    float topFaceCenterYRight = target.getCenterY() + target.getRadius() - 17; // Adjust the offset as needed.
    float topFaceCenterZRight = target.getCenterZ()+ target.getRadius() - 15;

    glTranslatef(topFaceCenterXRight, topFaceCenterYRight, topFaceCenterZRight);
    glutSolidSphere(1.5, 10, 10); // Adjust the parameters as needed for the desired appearance.


///////////

    ///////////

    glEnable(GL_TEXTURE_2D);
    // Map the sky texture onto a rectangle parallel to the xy-plane.   sa2f
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-200.0, 60.0, -200.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(200.0, 60.0, -200.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(200.0, 60.0, 400.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-200.0, 60.0, 400.0);
    glEnd();

    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);



//////////
///////////

    glEnable(GL_TEXTURE_2D);
// Map the sky texture onto a rectangle parallel to the yz-plane.   ganb ymeen
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(100.0, -3.0, -200.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(100.0, 200.0, -200.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(100.0, 200.0, 200.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(100.0, -3.0, 200.0);
    glEnd();

// Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

///////////
///////////

    glEnable(GL_TEXTURE_2D);
// Map the sky texture onto a rectangle parallel to the yz-plane.   ganb shemal
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-100.0, -3.0, -200.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(-100.0, 200.0, -200.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(-100.0, 200.0, 200.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-100.0, -3.0, 200.0);
    glEnd();

// Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

///////////
///////////
    glEnable(GL_TEXTURE_2D);
    // Map the sky texture onto a rectangle parallel to the xy-plane.   fel dahr
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-500.0, -3.0, 200.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(500.0, -3.0, 200.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(500.0, 150.0, 200.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-500.0, 150.0, 200.0);
    glEnd();

    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

///////////


    glEnable(GL_TEXTURE_2D);
    // Map the sky texture onto a rectangle parallel to the xy-plane.   fel wesh
    glBindTexture(GL_TEXTURE_2D, texture[sky_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-500.0, -3.0, -200.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(500.0, -3.0, -200.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(500.0, 150.0, -200.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-500.0, 150.0, -200.0);
    glEnd();

    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);
    // Map the grass texture onto a rectangle along the xz-plane.  ardya
    glBindTexture(GL_TEXTURE_2D, texture[ground_id]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -3.0, 200.0);
    glTexCoord2f(8.0, 0.0); glVertex3f(100.0, -3.0, 200.0);
    glTexCoord2f(8.0, 8.0); glVertex3f(100.0, -3.0, -200.0);
    glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, -3.0, -200.0);
    glEnd();

    // Apply translation to texture coordinates in a circular manner.
    glTranslatef(0.1 * cos(angle), 0.1 * sin(angle), 0.0);
    // Reenter modelview mode.
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);

//////////
    glPopMatrix();

    // End right viewport.

    glutSwapBuffers();



}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 500.0);
    glMatrixMode(GL_MODELVIEW);

    // Pass the size of the OpenGL window.
    width = w;
    height = h;
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;
        case 't':
            if (spotExponent > 0.0) spotExponent -= 0.1;
            glutPostRedisplay();
            break;
        case 'r':
            spotExponent += 0.1;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    float tempxVal = xVal, tempzVal = zVal, tempAngle = angle;
    float tempxSpot1 = xSpot1, tempzSpot1 = zSpot1 , tempAngle1 = angle;
    float tempxSpot2 = xSpot2, tempzSpot2 = zSpot2 ,  tempAngle2 = angle;;
    // Compute next position.
    if (key == GLUT_KEY_PAGE_DOWN)
    {
        if (spotAngle > 0.0) spotAngle -= 1.0;
    }
    if (key == GLUT_KEY_PAGE_UP)
    {
        if (spotAngle < 90.0) spotAngle += 1.0;
    }
    if (key == GLUT_KEY_LEFT) tempAngle = angle + 5.0;
    if (key == GLUT_KEY_RIGHT) tempAngle = angle - 5.0;
    if (key == GLUT_KEY_UP)
    {
        tempxVal = xVal - sin(angle * M_PI / 180.0);
        tempzVal = zVal - cos(angle * M_PI / 180.0);
        tempxSpot1 = xSpot1 -  sin(angle * M_PI / 180.0);
        tempzSpot1 = zSpot1 -  cos(angle * M_PI / 180.0);
        tempxSpot2 = xSpot2 -  sin(angle * M_PI / 180.0);
        tempzSpot2 = zSpot2 -  cos(angle * M_PI / 180.0);
    }
    if (key == GLUT_KEY_DOWN)
    {
        tempxVal = xVal + sin(angle * M_PI / 180.0);
        tempzVal = zVal + cos(angle * M_PI / 180.0);
        tempxSpot1 = xSpot1 +  sin(angle * M_PI / 180.0);
        tempzSpot1 = zSpot1 +  cos(angle * M_PI / 180.0);
        tempxSpot2 = xSpot2 +  sin(angle * M_PI / 180.0);
        tempzSpot2 = zSpot2 +  cos(angle * M_PI / 180.0);
    }

    // Angle correction.
    if (tempAngle > 360.0) tempAngle -= 360.0;
    if (tempAngle < 0.0) tempAngle += 360.0;
    if (tempAngle1 > 360.0) tempAngle1 -= 360.0;
    if (tempAngle1 < 0.0) tempAngle1 += 360.0;
    if (tempAngle2 > 360.0) tempAngle1 -= 360.0;
    if (tempAngle2 < 0.0) tempAngle1 += 360.0;

    // Move spacecraft to next position only if there will not be collision with an asteroid.
    if (!asteroidCraftCollision(tempxVal, tempzVal, tempAngle))
    {
        isCollision = 0;
        xVal = tempxVal;
        zVal = tempzVal;
        angle = tempAngle;
        xSpot1 = tempxSpot1;
        zSpot1  = tempzSpot1;
        xSpot2  = tempxSpot2;
        zSpot2  = tempzSpot2 ;
    }
    else isCollision = 1;
    if (!targetCraftCollision(tempxVal, tempzVal, tempAngle) && !isCollision)
    {
        isTargetReached = 0;
        xVal = tempxVal;
        zVal = tempzVal;
        angle = tempAngle;
        xSpot1 = tempxSpot1;
        zSpot1  = tempzSpot1;
        xSpot2  = tempxSpot2;
        zSpot2  = tempzSpot2 ;
    }
    else isTargetReached = 1;

    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    std::cout << "Interaction:" << std::endl;
    std::cout << "Press the left/right arrow keys to turn the craft." << std::endl
              << "Press the up/down arrow keys to move the craft." << std::endl;
}

void Reset(int value)
{
    if (value != 0){
        angle = 0.0; // Angle of the spacecraft.
        xVal = 0, zVal = 0; // Co-ordinates of the spacecraft.
        xSpot1 = -9, zSpot1 = 5; // Co-ordinates of the first headlight.
        xSpot2 = -9, zSpot2 = 1;
        isCollision = 0; // Is there collision between the spacecraft and an asteroid?
        isTargetReached = 0; // Is the spacecraft reached the target?
        frameCount = 0; // Number of frames
        numCubes = 4;
        for (int j = 0; j<COLUMNS; j++)
            for (int i = 0; i<ROWS; i++)
                arrayAsteroids[i][j] = Obtscale();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        setup();
        drawScene();
    }

    glutTimerFunc(1000, frameCounter, 0);
}

// Main routine.
int main(int argc, char **argv) {
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("ashfGamedTenen.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}