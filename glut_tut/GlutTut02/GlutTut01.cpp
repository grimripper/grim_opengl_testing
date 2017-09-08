// GlutTut01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL\glut.h>
#include <iostream>

using namespace std;

void Initialize();
void render();
void keyboard(unsigned char key, int x, int y);
void reshape(int, int);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
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
    if (key == 27)
    {
        exit(0);
    }

    if (key == 'f') {
        glutFullScreen();
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    cout << h << " " << w << endl;
}

void Initialize()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPointSize(1.5f);
    //glBegin(GL_POINTS);
    //    glColor3f(1.0f, 0.0f, 0.0f);
    //    glVertex3f(0.2, 0.2, 0.0);
    //    glColor3f(0.0f, 1.0f, 0.0f);
    //    glVertex3f(0.8, 0.2, 0.0);
    //glEnd();

    //glLineWidth(3.0f);
    //glBegin(GL_LINES);
    //    glColor3f(1.0f, 0.0f, 0.0f);
    //    glVertex3f(0.2, 0.2, 0.0);
    //    glColor3f(0.0f, 1.0f, 0.0f);
    //    glVertex3f(0.8, 0.2, 0.0);
    //glEnd();

    //glBegin(GL_TRIANGLES);
    //    glColor3f(1.0f, 0.0f, 0.0f);
    //    glVertex3f(0.0f, 0.0f, 0.0f);
    //    glColor3f(0.0f, 1.0f, 0.0f);
    //    glVertex3f(1.0f, 0.0f, 0.0f);
    //    glColor3f(0.0f, 0.0f, 1.0f);
    //    glVertex3f(1.0f, 0.5f, 0.0f);
    //glEnd();

    //glBegin(GL_LINE_STRIP);
    //    glColor3f(1.0f, 0.0f, 0.0f);
    //    glVertex3f(0.0f, 0.0f, 0.0f);
    //    glColor3f(0.0f, 1.0f, 0.0f);
    //    glVertex3f(1.0f, 0.0f, 0.0f);
    //    glColor3f(0.0f, 0.0f, 1.0f);
    //    glVertex3f(1.0f, 0.5f, 0.0f);
    //glEnd();

    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.5f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(0.5f, 0.5f, 0.0f);
    glEnd();

    glutSwapBuffers();
}
