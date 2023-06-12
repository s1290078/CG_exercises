#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>


#define _PI 3.14159f

// Number of subdivisions
#define N 256

// Coordinates and function values
static GLfloat g_x_coords[N];
static GLfloat g_sin_values[N];
static GLfloat g_cos_values[N];
static GLfloat g_x2_values[N];

// Set x coordinates to go through -pi to pi by step of 2*pi/N
static void setXCoords() {
    GLfloat x_start = -_PI;
    GLfloat delta_x = 2.0f*_PI / (N - 1);
    int i;
    for (i = 0; i < N; ++i) {
        g_x_coords[i] = x_start + i * delta_x;
    }
}

// Complete: 
// Compute sin(x), cos(x), x^2
static void setYValues() {
    int i;
    for (i = 0; i < N; ++i) {
        // Set g_sin_values[i] to the sin of g_x_coords[i]
        GLfloat x = g_x_coords[i];
        g_sin_values[i] = sin(x);
        // Set g_cos_values[i] to the cos of g_x_coords[i]
        g_cos_values[i] = cos(x);
        // Set g_x2_values[i] to x^2
        g_x2_values[i] = x*x;
    }
}

static void display(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    int i;
    glColor3f(0.f, 0.f, 0.f);
    glLineWidth(2.f);
    glScalef(1.f / _PI, 1.f / _PI, 1.f);

    // Complete:
    glBegin(GL_LINES);
    // Plot in red color the sine curve by connecting the points (g_x_coords[i], g_sin_values[i])
    // with line segments.
    for (int i = 0; i < N-1; i++){
        glColor3f(1,0,0);
        glVertex2f(g_x_coords[i], g_sin_values[i]);
        glVertex2f(g_x_coords[i+1], g_sin_values[i+1]);
    }
    // Same thing in green color for cos(x)
    for (int i = 0; i < N-1; i++){
        glColor3f(0,1,0);
        glVertex2f(g_x_coords[i], g_cos_values[i]);
        glVertex2f(g_x_coords[i+1], g_cos_values[i+1]);
    }
    // Same thing in blue color for x^2
    for (int i = 0; i < N-1; i++){
        glColor3f(0,0,1);
        glVertex2f(g_x_coords[i], g_x2_values[i]);
        glVertex2f(g_x_coords[i+1], g_x2_values[i+1]);
    }
    // End of complete
    glEnd();
    glutSwapBuffers();
}

// No need to modify the functions below
//

static void init(void) {
    // set the color for painting the background
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // set an orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, -1.0);

    // prepare the curve coordinates
    setXCoords();
    setYValues();
}

static void keyHandle(unsigned char key, int x, int y) {
    // exit when ESC is pressed
    if (key == 27) exit(0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Plot curves");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyHandle);
    glutMainLoop();
    return 0;
}