#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _USE_MATH_DEFINES
using namespace std;

static GLfloat spin1 = 0.0;
static GLfloat spin2 = 0.0;
const int radius = 5;
GLfloat twicePi = 2.0f * M_PI;

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void car(void)
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

void wheelFront(void)
{
    glPushMatrix();
    int x = 15;
    int y = 0;
    glTranslatef(x , 0 , 0);
    glRotatef(spin2, 0.0, 0.0, 1.0);
    glTranslatef(-x , 0 , 0);
    glColor3f(1.0, 0.0, 0.0);
    int i;
    int triangleAmount = 20;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(i = 0; i <= triangleAmount;i++) { 
            glVertex2f(
                    x + (radius * cos(i *  twicePi / triangleAmount)), 
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
    glPopMatrix();
}

void wheelBack(void)
{
    glPushMatrix();
    int x = -10;
    int y = 0;
    glTranslatef(x , 0 , 0);
    glRotatef(spin1, 0.0, 0.0, 1.0);
    glTranslatef(-x , 0 , 0);
    glColor3f(1.0, 0.0, 0.0);
    int i;
    int triangleAmount = 20;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(i = 0; i <= triangleAmount;i++) { 
            glVertex2f(
                    x + (radius * cos(i *  twicePi / triangleAmount)), 
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
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

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    car();
    // window();
    wheelFront();
    wheelBack();
    glPopMatrix();
    glutSwapBuffers();
}

void spinDisplay(void)
{
    spin1 += 2.0;
    if (spin1 > 360.0)
        spin1 -= 360.0;
    spin2 += 2.0;
    if (spin2 > 360.0)
        spin2 -= 360.0;
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

void mouse(int button, int state, int x, int y) 
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            glutIdleFunc(spinDisplay);
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN)
            glutIdleFunc(NULL);
            break;
        default:
            break;
    }
}

/* 
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}