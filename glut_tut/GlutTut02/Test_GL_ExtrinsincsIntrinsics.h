#pragma once

#include <GL\glut.h>
#include "ProjectionUsingEigen.h"
#include "RotationAroundAxes.h"
#include <iostream>
#include <sstream>

using namespace std;

int m_w = 642;
int m_h = 240;

int last_clicked_viewport = -1;
int last_clicked_x = -1;
int last_clicked_y = -1;

int camera_chosen = -1;

float camera_pos[2][3] = { 0.0f };

static void render();
static void keyboard(unsigned char key, int x, int y);
static void reshape(int, int);
static void mouseFunc(int, int, int, int);

int main_func(int argc, char* argv[])
{
    camera_pos[0][0] = 0.0f;
    camera_pos[0][1] = 0.0f;
    camera_pos[0][2] = 5.0f;

    camera_pos[1][0] = 0.0f;
    camera_pos[1][1] = 0.0f;
    camera_pos[1][2] = 3.0f;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(m_w, m_h);
    glutCreateWindow("OpenGL_Tut");
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseFunc);
    glutReshapeFunc(reshape);
    glutDisplayFunc(render);

    glutMainLoop();
    return 0;
}

void mouseFunc(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (x >= 322)
        {
            last_clicked_viewport = 1;
            last_clicked_x = x - 322;
            last_clicked_y = y;
        }
        else
        {
            last_clicked_viewport = 0;
            last_clicked_x = x;
            last_clicked_y = y;
        }
        cout << "Viewport = " << last_clicked_viewport << endl << 
            "x = " << last_clicked_x << endl << 
            "y = " << last_clicked_y << endl;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    //cout << key << endl;
    if (key == 27)
    {
        exit(0);
    }

    if (camera_chosen == -1)
    {
        cout << "click on a camera" << endl;
        return;
    }

    if (key == 'r')
    {
        camera_pos[camera_chosen][0] += 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'l')
    {
        camera_pos[camera_chosen][0] -= 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'u')
    {
        camera_pos[camera_chosen][1] += 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'd')
    {
        camera_pos[camera_chosen][1] -= 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'n')
    {
        camera_pos[camera_chosen][2] -= 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'f')
    {
        camera_pos[camera_chosen][2] += 0.05f;
        glutPostRedisplay();
    }
}

void reshape(int w, int h)
{
    //snap back to original window width and height
    glutReshapeWindow(m_w, m_h);
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
    GLfloat mvMtx[16] = { 0.0f };
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        glViewport(0, 0, 320, m_h);
        glMatrixMode(GL_MODELVIEW);

        Vector3 x_axis, y_axis, z_axis;
        Angles_To_Axes(Vector3(0, 30, 0), x_axis, y_axis, z_axis);

        Matrix4f camToWorldMatrix = Matrix4f::Identity();
        camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

        Vector3f camTranslation;
        camTranslation << camera_pos[0][0], camera_pos[0][1], camera_pos[0][2];

        camToWorldMatrix.block<3, 1>(0, 3) = camTranslation;
        Matrix4f worldToCamMatrix = camToWorldMatrix.inverse();

        glLoadMatrixf(worldToCamMatrix.data());  // already in column-major order

        if (camera_chosen == 0)
        {
            printModelViewMatrix();
        }

        glMatrixMode(GL_PROJECTION);

        //Create a projection matrix from Calibration Model
        FakeCalibration_PinholeModel pinHoleModel;
        pinHoleModel.fx = 500.0f;
        pinHoleModel.fy = 500.0f;
        pinHoleModel.s = 0.0f;
        pinHoleModel.w = 1000.0f;
        pinHoleModel.h = 1000.0f;
        pinHoleModel.x0 = 500.0f;
        pinHoleModel.y0 = 500.0f;
        Matrix4f glProj, perspIntrinsicMatrix, orthoNdcMatrix;

        glProj = GetProjectionMatrix(pinHoleModel, 2.0f, 6.0f, perspIntrinsicMatrix, orthoNdcMatrix);

        //Equivalent to the following:
        //glProj = setGlFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 2.0, 6.0);
        glLoadMatrixf(glProj.data());

        if (camera_chosen == 0)
        {
            printProjectionMatrix();

            Vector4f world_location(-0.5f, 0.0f, 0.0f, 1.0f);
            Vector4f cam_location = worldToCamMatrix * world_location;
            cout << "Location in cam = " << cam_location / cam_location(3) << endl;
            Vector4f img_location = perspIntrinsicMatrix * cam_location;
            cout << "Location in img = " << img_location / img_location(3) << endl;
        }
        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
            drawTriangleVertices();
        glEnd();
    }

    {
        //Viewport Separator - nicer solution? how do I make this 1 pixel thick?
        glViewport(320, 0, 2, m_h);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-0.5, 0.5, -0.5, 0.5, 1.0, 10.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glVertex3f(-0.5f, -0.5f, 0.0f);
            glVertex3f(0.5f, -0.5f, 0.0f);
            glVertex3f(0.5f, 0.5f, 0.0f);
            glVertex3f(-0.5f, 0.5f, 0.0f);
        glEnd();
    }

    {
        glViewport(322, 0, 320, m_h);
        glMatrixMode(GL_MODELVIEW);

        Vector3 x_axis, y_axis, z_axis;
        Angles_To_Axes(Vector3(0, 0, 0), x_axis, y_axis, z_axis);

        Matrix4f camToWorldMatrix = Matrix4f::Identity();
        camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

        Vector3f camTranslation;
        camTranslation << camera_pos[1][0], camera_pos[1][1], camera_pos[1][2];

        camToWorldMatrix.block<3, 1>(0, 3) = camTranslation;
        Matrix4f worldToCamMatrix = camToWorldMatrix.inverse();

        glLoadMatrixf(worldToCamMatrix.data());  // already in column-major order

        if (camera_chosen == 1)
        {
            printModelViewMatrix();
        }

        glMatrixMode(GL_PROJECTION);

        //Create a projection matrix from Calibration Model
        FakeCalibration_PinholeModel pinHoleModel;
        pinHoleModel.fx = 500.0f;
        pinHoleModel.fy = 500.0f;
        pinHoleModel.s = 0.0f;
        pinHoleModel.w = 1000.0f;
        pinHoleModel.h = 1000.0f;
        pinHoleModel.x0 = 500.0f;
        pinHoleModel.y0 = 500.0f;
        Matrix4f glProj, perspIntrinsicMatrix, orthoNdcMatrix;

        glProj = GetProjectionMatrix(pinHoleModel, 2.0f, 6.0f, perspIntrinsicMatrix, orthoNdcMatrix);

        //Equivalent to the following:
        glProj = setGlFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 2.0, 6.0);
        glLoadMatrixf(glProj.data());

        if (camera_chosen == 1)
        {
            printProjectionMatrix();

            Vector4f world_location(-0.5f, 0.0f, 0.0f, 1.0f);
            Vector4f cam_location = worldToCamMatrix * world_location;
            cout << "Location in cam = " << cam_location / cam_location(3) << endl;
            Vector4f img_location = perspIntrinsicMatrix * cam_location;
            cout << "Location in img = " << img_location / img_location(3) << endl;
        }

        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
            drawTriangleVertices();
        glEnd();
    }

    glutSwapBuffers();
}
