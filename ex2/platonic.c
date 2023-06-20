#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define _PI 3.14159f

enum { TRANS, ROT, SCALE, N };
static int g_op_mode = TRANS;

enum { PERSP, ORTHO };
static int g_proj_mode = PERSP;

// window dimension
static int g_width = 800;
static int g_height = 800;

// angle (in degree) to rotate around x, y, z
static GLfloat g_angle[3] = { 0.0f, 0.0f, 0.0f };

// amount to translate along x, y, z
static GLfloat g_trans[3] = { 0.0f, 0.0f, 0.0f };

// scaling factor along x, y, z
static GLfloat g_scale[3] = { 1.0f, 1.0f, 1.0f };

static int levels = 2;

static GLfloat m[] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

// Model-View transforms
static void myTranslatef(GLfloat tx, GLfloat ty, GLfloat tz) {
    // Complete
    m[12] = tx;
    m[13] = ty;
    m[14] = tz;

    glMultMatrixf(m);
}


static void myScalef(GLfloat sx, GLfloat sy, GLfloat sz) {
    // Complete
    m[0] = sx;
    m[5] = sy;
    m[10] = sz;
    m[15] = 1;

    glMultMatrixf(m);
}

static void myRotatef(GLfloat theta, GLfloat kx, GLfloat ky, GLfloat kz) {
    // Complete
    float cosTheta = cos(theta * _PI/180);
    float sinTheta = sin(theta * _PI/180);

  GLfloat m[] = {
    kx*kx*(1.0f-cosTheta)+cosTheta, ky*kx*(1.f-cosTheta)+kz*sinTheta, kz*kx*(1.f-cosTheta)-ky*sinTheta, 0.0f,
    kx*ky*(1.0f-cosTheta)-kz*sinTheta, ky*ky*(1.f-cosTheta)+cosTheta, kz*ky*(1.f-cosTheta)+kx*sinTheta, 0.0f,
    kx*kz*(1.0f-cosTheta)+ky*sinTheta, ky*kz*(1.f-cosTheta)-kx*sinTheta, kz*kz*(1.f-cosTheta)+cosTheta, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

    glMultMatrixf(m);
}

// Projection transforms
static void myOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat n, GLfloat f) {
    // Complete
    GLfloat m[] = {
    2.0f/(right-left), 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f/(top-bottom), 0.0f, 0.0f, 
    0.0f, 0.0f, 2.0f/(n-f), 0.0f, 
    -(right+left)/(right-left), -(top+bottom)/(top-bottom), (f+n)/(n-f), 1.0f
  };

    glMultMatrixf(m);
}

static void myPerspective(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f) {
    // Complete
    GLfloat left = -1 * tan(fovy/2.0f) * n * aspect;
    GLfloat right = tan(fovy/2.0f) * n * aspect;
    GLfloat bottom = -1 * tan(fovy/2.0f) * n;
    GLfloat top = tan(fovy/2.0f) * n;

    GLfloat m[] = {
        (2.0f*n)/(right-left), 0.0f, 0.0f, 0.0f,
        0.0f, (2.0f*n)/(top-bottom), 0.0f, 0.0f,
        (right+left)/(right-left), (top+bottom)/(top-bottom), -(f+n)/(f-n), -1.0f,
        0.0f, 0.0f, (-2.0f*f*n)/(f-n), 0.0f
        };

    glMultMatrixf(m);
}

static void drawIcosahedron(void) {
    GLfloat phi = (1.f + sqrtf(5.f)) * .5f;
    GLfloat a = 1.f;
    GLfloat b = 1.f / phi;

    GLfloat vertices[12][3] = { {0.f,b,-a}, {b,a,0.f}, {-b,a,0.f},
    {0.f,b,a}, {0.f,-b,a}, {-a,0.f,b},
    {0.f,-b,-a}, {a,0.f,-b}, {a,0.f,b},
    {-a,0.f,-b},{b,-a,0.f},{-b,-a,0.f}
    };

    GLfloat color[20][3] = { {0.0f,0.0f,0.6f}, {0.0f,0.0f,0.8f}, {0.0f,0.0f,1.0f},
    {0.f, 0.2f, 1.f}, {0.f,0.4f,1.f}, {0.f,0.6f,1.f}, {0.f,0.8f,1.f}, {0.f,1.f,1.f},
    {0.2f,1.f,0.8f}, {0.4f,1.f,0.6f}, {0.6f,1.f,0.4f}, {0.8f,1.f,0.2f},
    {1.f,1.f,0.f}, {1.f,0.8f,0.f}, {1.f,0.6f,0.f}, {1.f,0.4f,0.f}, {1.f,0.2f,0.f},
    {1.f,0.f,0.f}, {0.8f,0.f,0.f}, {0.6f,0.f,0.f}
    };

    int faces[20][3] = { {2,1,0}, {1,2,3}, {5,4,3}, {4,8,3},
    {7,6,0}, {6,9,0}, {11,10,4}, {10,11,6},
    {9,5,2}, {5,9,11}, {8,7,1}, {7,8,10},
    {2,5,3}, {8,1,3}, {9,2,0}, {1,7,0},
    {11,9,6}, {7,10,6}, {5,11,4}, {10,8,4}
    };

    int i;

    for (i = 0; i < 20; ++i) {
        GLfloat* c = color[i];
        int* f = faces[i];
        int v0 = f[0], v1 = f[1], v2 = f[2];
        glColor3f(c[0], c[1], c[2]);
        glBegin(GL_TRIANGLES);
        glVertex3f(vertices[v0][0], vertices[v0][1], vertices[v0][2]);
        glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);
        glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);
        glEnd();
    }
}

// recursive function
static void recursive (int n) {
    if (n == levels) {
        return;
    }

    drawIcosahedron();
    glPushMatrix();
    glScalef(0.5f, 0.5f, 0.5f);
    glTranslatef(-2.5f, -2.5f, 0.0f);
    recursive(n+1);
    glPopMatrix();

    glPushMatrix();
    glScalef(0.5f, 0.5f, 0.5f);
    glTranslatef(2.5f, -2.5f, 0.0f);
    recursive(n+1);
    glPopMatrix();
}

static void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (g_proj_mode == PERSP) {
        // Complete: 
        // After completing the code of myPerspective() above, 
        // replace the call to gluPerspective with a call to myPerspective 
        // or gluPerspective depending on the transformation mode
        myPerspective(45.0, (GLdouble)g_width / (GLdouble)g_height, 0.1, 20.0);
    }
    else {
        // Complete: 
        // After completing the code of myOrtho() above, 
        // replace the call to glOrtho with a call to myOrtho
        // or glOrtho depending on the transformation mode
        myOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
    }

    // Modelview transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    glPushMatrix();

    // Complete: 
    // After completing the code of myTranslatef, myScalef, 
    // and myRotatef, 
    // replace the call to glTranslatef, glRotatef, glScalef by calls to 
    // myTranslatef, myScalef, and myRotatef or 
    // glTranslatef, glRotatef, glScalef based on the transformation mode. 
    myTranslatef(g_trans[0], g_trans[1], g_trans[2]);
    myRotatef(g_angle[0], 1.f, 0.f, 0.f);
    myRotatef(g_angle[1], 0.f, 1.f, 0.f);
    myRotatef(g_angle[2], 0.f, 0.f, 1.f);
    myScalef(g_scale[0], g_scale[1], g_scale[2]);

    recursive(1);
    // drawIcosahedron();

    glPopMatrix();

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    g_width = w;
    g_height = h;
}

// Increase the rotation angle by amt around ax
static void rotate(int ax, GLfloat amt) {
    g_angle[ax] += amt;
}

// Increase the translation by amt along ax
static void translate(int ax, GLfloat amt) {
    g_trans[ax] += amt;
}

// Multiply the scaling factor by amt along ax
static void scale(int ax, GLfloat amt) {
    g_scale[ax] *= amt;
}

static void keyboard(unsigned char k, int x, int y) {
    switch (k) {
    case 27:
        exit(EXIT_SUCCESS);
        break;

        // Complete: 
        // Allow to switch between OpenGL transformations and your implementations

    case 'p':
        g_proj_mode = (1 - g_proj_mode);
        break;

    case 't':
    case 'T':
        g_op_mode = TRANS;
        break;

    case 'r':
    case 'R':
        g_op_mode = ROT;
        break;

    case 's':
    case 'S':
        g_op_mode = SCALE;
        break;

    case 'n':
    case 'N':
        g_op_mode = N;
        break;

    case 'x':
        if (g_op_mode == TRANS) translate(0, -0.5f);
        if (g_op_mode == ROT) rotate(0, -5.0f);
        if (g_op_mode == SCALE) scale(0, 0.9f);
        if (g_op_mode == N) {
            if (levels == 1) break;
            levels--;
        }
        break;

    case 'X':
        if (g_op_mode == TRANS) translate(0, 0.5f);
        if (g_op_mode == ROT) rotate(0, 5.0f);
        if (g_op_mode == SCALE) scale(0, 1.1f);
        if (g_op_mode == N) {
            if (levels == 5) break;
            levels++;
        }
        break;

    case 'y':
        if (g_op_mode == TRANS) translate(1, -0.5f);
        if (g_op_mode == ROT) rotate(1, -5.0f);
        if (g_op_mode == SCALE) scale(1, 0.9f);
        break;

    case 'Y':
        if (g_op_mode == TRANS) translate(1, 0.5f);
        if (g_op_mode == ROT) rotate(1, 5.0f);
        if (g_op_mode == SCALE) scale(1, 1.1f);
        break;

    case 'z':
        if (g_op_mode == TRANS) translate(2, -0.5f);
        if (g_op_mode == ROT) rotate(2, -5.0f);
        if (g_op_mode == SCALE) scale(2, 0.9f);
        break;

    case 'Z':
        if (g_op_mode == TRANS) translate(2, 0.5f);
        if (g_op_mode == ROT) rotate(2, 5.0f);
        if (g_op_mode == SCALE) scale(2, 1.1f);
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

static void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Icosahedron");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
