#include <GL/freeglut.h>
#include <GL/gl.h>

void InitializeAWindowPlease() {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("OpenGL - First window demo");
}

void UpdateTheWindowAndCheckForEvents() {
    glutMainLoop();
}

void renderFunction() {
    glClearColor(0.0, 0.0, 0.0, 0.0);           // establishes what color the window will be cleared to
    glClear(GL_COLOR_BUFFER_BIT);               // actually clears the window
    glColor3f(1.0, 1.0, 1.0);                   // establishes what color to use for drawing objects
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);   // specifies the coordinate system OpenGL assumes as it draws the final image and how the image gets mapped to the screen
    glBegin(GL_POLYGON);                        // define the object to be drawn
        glVertex2f(-0.5, -0.5);                 // defined corner  -- glVertex2f (x, y) : for 2D
        glVertex2f(-0.5, 0.5);                  // defined corner  -- glVertex3f (x, y, z) : for 3D (z = 0 -> plane)
        glVertex2f(0.5, 0.5);                   // defined corner  -- Command Suffixes and Argument Data Types : b (byte), s (small), i (int), f (float), d (double), ub, us, ui
        glVertex2f(0.5, -0.5);                  // defined corner  -- Command Suffixed for glVertex2[command]
    glEnd();                                    // end of drawing
    glFlush();                                  // ensures that the drawing commands are actually executed rather than stored in a buffer awaiting additional OpenGL commands
}

int main(int argc, char** argv) {
    InitializeAWindowPlease();                  // initializes a window on the screen
    glutDisplayFunc(renderFunction);            // function to display object
    UpdateTheWindowAndCheckForEvents();         // manages the contents of the window and begins event processing
    return 0;
}
