#pragma once

#include <GL\glut.h>
#include <iostream>
#include <sstream>

using namespace std;

int m_w = 640;
int m_h = 480;

int camera_chosen = 0;

float camera_pos[2][3] = { 0.0f };

static void Initialize();
static void render();
static void keyboard(unsigned char key, int x, int y);
static void reshape(int, int);

int main_func(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(m_w, m_h);
    glutCreateWindow("OpenGL_Tut");
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    Initialize();
    glutDisplayFunc(render);

    glutMainLoop();
    return 0;
}

void keyboard(unsigned char key, int x, int y)
{
    //cout << key << endl;
    if (key == 27)
    {
        exit(0);
    }

    if (key == 't')
    {
        cout << "Camera toggled. Old camera: " << camera_chosen;
        camera_chosen = (camera_chosen + 1) % 2;
        cout << "New camera: " << camera_chosen << endl;
    }

    if (key == 'a')
    {
        camera_pos[camera_chosen][0] -= 0.1;
        glutPostRedisplay();
    }
    else if (key == 'd')
    {
        camera_pos[camera_chosen][0] += 0.1;
        glutPostRedisplay();
    }
    else if (key == 'w')
    {
        camera_pos[camera_chosen][1] += 0.1;
        glutPostRedisplay();
    }
    else if (key == 'z')
    {
        camera_pos[camera_chosen][1] -= 0.1;
        glutPostRedisplay();
    }

    //if (key == 'f') {
    //    glutFullScreen();
    //}
}

void reshape(int w, int h)
{
    m_w = w;
    m_h = h;
    glViewport(0, 0, m_w, m_h);
    cout << h << " " << w << endl;
}

void Initialize()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glTranslatef(-0.1, -0.1, -5);
    //GLfloat mvMtx[16] = { 0.0f };
    //glGetFloatv(GL_MODELVIEW_MATRIX, mvMtx);
    //cout << endl <<
    //    mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
    //    mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
    //    mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
    //    mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;
}

// This prints a string to the screen
void screen_print(int x, int y, const char *st)
{
    int l, i;

    l = strlen(st); // see how many characters are in text string.
    glRasterPos2i(x, y); // location to start printing text
    for (i = 0; i < l; i++) // loop until i is greater then l
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st[i]); // Print a character on the screen
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat mvMtx[16] = { 0.0f };

    glViewport(0, m_h/2.0, m_w/2.0, m_h/2.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-camera_pos[0][0], -camera_pos[0][1], -5);
    //NOTE vaibhavt: Works! -> Produces same as above
    //gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMtx);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 10.0);
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.2f, 0.2f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.8f, 0.2f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.5f, 0.5f, 0.0f);
    glEnd();

    cout << "Camera 0" << endl <<
        mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
        mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
        mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
        mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;

    glViewport(0, 0, m_w / 2.0, m_h / 2.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-camera_pos[1][0], -camera_pos[1][1], -5);
    //NOTE vaibhavt: Works! -> Produces same as above
    //gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMtx);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 10.0);
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.2f, 0.2f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.8f, 0.2f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glEnd();

    cout << "Camera 1" << endl <<
        mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
        mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
        mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
        mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;

    glutSwapBuffers();
}
