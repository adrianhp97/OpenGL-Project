#include <GL/freeglut.h>
#include <GL/gl.h>

void InitializeAWindowPlease() {
    glutInitWindowSize(500,500);                    // specifies the screen location for the upper-left corner of your window.
    glutInitWindowPosition(100,100);                // specifies the size, in pixels, of your window.
    glutCreateWindow("OpenGL - First window demo"); // creates a window with an OpenGL context. It returns a unique identifier for the new window. Be warned: Until glutMainLoop() is called (see next section), the window is not yet displayed.
}

void UpdateTheWindowAndCheckForEvents() {
    glutMainLoop();                             // All windows that have been created are now shown, and rendering to those windows is now effective. Event processing begins, and the registered display callback is triggered. Once this loop is entered, it is never exited!
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
    glutInit(&argc, argv);                      // initializes GLUT and processes any command line arguments (for X, this would be options like -display and -geometry)
    glutInitDisplayMode(GLUT_SINGLE);           // specifies whether to use an RGBA or color-index color model. You can also specify whether you want a single- or double-buffered window. you can use this routine to indicate that you want the window to have an associated depth, stencil, and/or accumulation buffer. glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH).

    InitializeAWindowPlease();                  // initializes a window on the screen
    glutDisplayFunc(renderFunction);            // you should put all the routines you need to redraw the scene in the display callback function
    UpdateTheWindowAndCheckForEvents();         // manages the contents of the window and begins event processing
    return 0;
}
