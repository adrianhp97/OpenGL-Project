#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _USE_MATH_DEFINES
using namespace std;

static GLfloat spinFront = 0.0;
static GLfloat spinBack = 0.0;
static GLfloat moveOffset = 0.0;
static float rgb[3] = {1.0f, 0, 0};
const int radius = 5;
GLfloat twicePi = 2.0f * M_PI;

void init(void);
void carBody(void);
void wheelFront(void);
void wheelBack(void);
void window(void);
void renderDisplay(void);
void moveForward(void);
void moveBackward(void);
void reshape(int w, int h);
void readMouse(int button, int state, int x, int y);
void readKeyboard(unsigned char key, int x, int y);
void initializeWindow(int x, int y, int posX, int posY);

/* 
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    initializeWindow(500,500,725,200);
    init ();
    glutDisplayFunc(renderDisplay); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(readKeyboard);
    glutMouseFunc(readMouse);
    glutMainLoop();
    return 0;
}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void carBody(void)
{
    glColor3f(0.8, 0, 0);
    glBegin(GL_POLYGON);
        glVertex2i(-19, 0);
        glVertex2i(-27, 1);
        glVertex2i(-27, 10);
        glVertex2i(-20, 12);
        glVertex2i(-10, 20);
        glVertex2i(20, 19);
        glVertex2i(23, 15);
        glVertex2i(23, 1);
        glVertex2i(22, 0);
        glTranslatef(moveOffset, 0, 0);
    glEnd();
}

void wheelFront(void)
{
    glPushMatrix();
    int x = 15;
    int y = 0;
    glTranslatef(x , 0 , 0);
    glRotatef(spinFront, 0.0, 0.0, 1.0);
    glTranslatef(-x , 0 , 0);
    int i;
    int triangleAmount = 20;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(i = 0; i <= triangleAmount;i++) { 
            switch (i%3) {
                case 0:
                    glColor3f(1.0f, 0, 0);
                    break;
                case 1:
                    glColor3f(0, 1.0f, 0);
                    break;
                case 2:
                    glColor3f(0, 0, 1.0f);
                    break;
            }
            glVertex2f(
                    x + (radius * cos(i *  twicePi / triangleAmount)), 
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glTranslatef(moveOffset, 0, 0);
    glEnd();
    glPopMatrix();
}

void wheelBack(void)
{
    glPushMatrix();
    int x = -10;
    int y = 0;
    glTranslatef(x , 0 , 0);
    glRotatef(spinBack, 0.0, 0.0, 1.0);
    glTranslatef(-x , 0 , 0);
    int i;
    int triangleAmount = 20;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(i = 0; i <= triangleAmount;i++) { 
            switch (i%3) {
                case 0:
                    glColor3f(1.0f, 0, 0);
                    break;
                case 1:
                    glColor3f(0, 1.0f, 0);
                    break;
                case 2:
                    glColor3f(0, 0, 1.0f);
                    break;
            }
            glVertex2f(
                    x + (radius * cos(i *  twicePi / triangleAmount)), 
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glTranslatef(moveOffset, 0, 0);
    glEnd();
    glPopMatrix();
}

void window(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex2i(-30, 0);
        glVertex2i(-30, 10);
        glVertex2i(-20, 10);
        glVertex2i(-10, 20);
        glVertex2i(20, 20);
        glVertex2i(30, 10);
        glVertex2i(30, 0);
    glEnd();
}

void renderDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    carBody();
    wheelFront();
    wheelBack();
    glPopMatrix();
    glutSwapBuffers();
}

void moveForward(void)
{
    spinBack += 2.0;
    if (spinBack > 360.0)
        spinBack -= 360.0;
    spinFront += 2.0;
    if (spinFront > 360.0)
        spinFront -= 360.0;
    if(moveOffset > -30.0)
        moveOffset -= 0.000001f;
    glTranslatef(moveOffset, 0, 0);
    glutPostRedisplay();
}

void moveBackward(void) {
    spinBack -= 2.0;
    if (spinBack < 0)
        spinBack += 360.0;
    spinFront -= 2.0;
    if (spinFront < 0)
        spinFront += 360.0;
    if(moveOffset < 30.0)
        moveOffset += 0.000001f;
    glTranslatef(moveOffset, 0, 0);
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void readMouse(int button, int state, int x, int y) 
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                spinFront = 0;
                spinBack = 0;
                moveOffset = 0;
                glutIdleFunc(moveForward);
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN) {
                glutIdleFunc(NULL);
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN) {
                spinFront = 0;
                spinBack = 0;
                moveOffset = 0;
                glutIdleFunc(moveBackward);
            }
            break;
        default:
            break;
    }
}

void readKeyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            int id = glutGetWindow();
            glutDestroyWindow(id);
            exit(0);
            break;
    }
}

void initializeWindow(int x, int y, int posX, int posY) {
    glutInitWindowSize(x,y);
    glutInitWindowPosition(posX,posY);
    glutCreateWindow ("Window");
    glutSetWindow(1);
}
