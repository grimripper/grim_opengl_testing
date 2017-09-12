#pragma once

#include <GL\glut.h>
#include "ProjectionUsingEigen.h"
#include "RotationAroundAxes.h"
#include <iostream>
#include <sstream>

using namespace std;

int m_w = 640;
int m_h = 480;

int camera_chosen = 0;

float camera_pos[2][3] = { 0.0f };
float camera_focal_length[2] = { 0.0f };

static void Initialize();
static void render();
static void keyboard(unsigned char key, int x, int y);
static void reshape(int, int);

int main_func(int argc, char* argv[])
{
    camera_pos[0][2] = 5.0f;
    camera_pos[1][2] = 5.0f;

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

    if (key == 'r')
    {
        camera_pos[camera_chosen][0] -= 0.05;
        glutPostRedisplay();
    }
    else if (key == 'l')
    {
        camera_pos[camera_chosen][0] += 0.05;
        glutPostRedisplay();
    }
    else if (key == 'u')
    {
        camera_pos[camera_chosen][1] += 0.05;
        glutPostRedisplay();
    }
    else if (key == 'd')
    {
        camera_pos[camera_chosen][1] -= 0.05;
        glutPostRedisplay();
    }
    else if (key == 'n')
    {
        camera_pos[camera_chosen][2] -= 0.05;
        glutPostRedisplay();
    }
    else if (key == 'f')
    {
        camera_pos[camera_chosen][2] += 0.05;
        glutPostRedisplay();
    }
    else if (key == '+')
    {
        camera_focal_length[camera_chosen] += 0.01;
    }
    else if (key == '-')
    {
        camera_focal_length[camera_chosen] -= 0.01;
    }
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
}

void drawTriangleVertices()
{
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
}

void printProjectionMatrix()
{
    GLfloat mvMtx[16] = { 0.0f };
    glGetFloatv(GL_PROJECTION_MATRIX, mvMtx);
    cout << "Camera chosen: " << camera_chosen << endl <<
        mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
        mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
        mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
        mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;
}

void printModelViewMatrix()
{
    GLfloat mvMtx[16] = { 0.0f };
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMtx);
    cout << "Camera chosen: " << camera_chosen << endl <<
        mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
        mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
        mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
        mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat mvMtx[16] = { 0.0f };

    {
        glViewport(0, m_h / 2.0, m_w / 2.0, m_h / 2.0);
        glMatrixMode(GL_MODELVIEW);

        Vector3 x_axis, y_axis, z_axis;
        Angles_To_Axes(Vector3(0, 0, 0), x_axis, y_axis, z_axis);

        Matrix4f camToWorldMatrix = Matrix4f::Identity();
        camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

        Vector3f camTranslation;
        camTranslation << camera_pos[0][0], camera_pos[0][1], camera_pos[0][2];

        camToWorldMatrix.block<3, 1>(0, 3) = camTranslation;
        Matrix4f worldToCamMatrix = camToWorldMatrix.inverse();

        glLoadMatrixf(worldToCamMatrix.data());  // already in column-major order

        //glLoadIdentity();
        ////Moves the camera back by 5 units (meters) in Z
        ////glTranslatef(-camera_pos[0][0], -camera_pos[0][1], -camera_pos[0][2]);

        if (camera_chosen == 0)
        {
            printModelViewMatrix();
        }

        //NOTE vaibhavt: Works! -> Produces same as above
        //gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-3.0, 3.0, -3.0, 3.0, 0.0, 10.0);

        //TODO vaibhavt: Works! -> Produces same as above
        //Eigen::Matrix4f glProj = setGlFrustum(-3.0, 3.0, -3.0, 3.0, 0.0, 10.0);
        //glLoadMatrixf(glProj.data());

        if (camera_chosen == 0)
        {
            printProjectionMatrix();
            Eigen::Matrix4f eigenProj = setOrthoFrustum(-3.0, 3.0, -3.0, 3.0, 0.0, 10.0);
            cout << eigenProj << endl;
        }
        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
        drawTriangleVertices();
        glEnd();
    }

    {
        glViewport(0, 0, m_w / 2.0, m_h / 2.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //Moves the camera back by 5 units (meters) in Z
        glTranslatef(-camera_pos[1][0], -camera_pos[1][1], -camera_pos[1][2]);

        if (camera_chosen == 1)
        {
            printModelViewMatrix();
        }
        //NOTE vaibhavt: Works! -> Produces same as above
        //gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //Near = 0
        //Far = 3
        glFrustum(-3.0, 3.0, -3.0, 3.0, 2.0, 6.0);

        //TODO vaibhavt: Works! -> Produces same as above
        //Eigen::Matrix4f glProj = setGlFrustum(-3.0, 3.0, -3.0, 3.0, 2.0, 6.0);
        //glLoadMatrixf(glProj.data());

        //glOrtho(-3.0, 3.0, -3.0, 3.0, 0.0, 10.0);
        if (camera_chosen == 1)
        {
            printProjectionMatrix();
            Eigen::Matrix4f eigenProj = setGlFrustum(-3.0, 3.0, -3.0, 3.0, 2.0, 6.0);
            cout << eigenProj << endl;
        }

        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
        drawTriangleVertices();
        glEnd();
    }

    glutSwapBuffers();
}
