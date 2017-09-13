#pragma once

#include <GL\glut.h>
#include "ProjectionUsingEigen.h"
#include "RotationAroundAxes.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace Eigen;

Matrix4f g_world_to_cam_matrix[2];
Matrix4f g_persp_intrinsic_matrix[2];

GLfloat g_modelView0[16] = { 0.0f };
GLfloat g_modelView1[16] = { 0.0f };
GLfloat g_projection0[16] = { 0.0f };
GLfloat g_projection1[16] = { 0.0f };

int g_w = 642;
int g_h = 240;

int g_last_clicked_viewport = -1;
int g_last_clicked_x = -1;
int g_last_clicked_y = -1;

int g_camera_chosen = -1;

float g_camera_rot_degrees[2][3] = { 0.0f };
float g_camera_pos[2][3] = { 0.0f };

static void render();
static void keyboard(unsigned char key, int x, int y);
static void reshape(int, int);
static void mouseFunc(int, int, int, int);

int main_func(int argc, char* argv[])
{
    g_camera_rot_degrees[0][0] = 0.0f;
    g_camera_rot_degrees[0][1] = 30.0f;
    g_camera_rot_degrees[0][2] = 0.0f;

    g_camera_rot_degrees[1][0] = 0.0f;
    g_camera_rot_degrees[1][1] = 0.0f;
    g_camera_rot_degrees[1][2] = 0.0f;

    g_camera_pos[0][0] = 0.0f;
    g_camera_pos[0][1] = 0.0f;
    g_camera_pos[0][2] = 5.0f;

    g_camera_pos[1][0] = 0.0f;
    g_camera_pos[1][1] = 0.0f;
    g_camera_pos[1][2] = 3.0f;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(g_w, g_h);
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
            g_last_clicked_viewport = 1;
            g_last_clicked_x = x - 322;
            g_last_clicked_y = (240 - y);
        }
        else
        {
            g_last_clicked_viewport = 0;
            g_last_clicked_x = x;
            g_last_clicked_y = (240 - y);
        }
        g_camera_chosen = g_last_clicked_viewport;
        cout << "Viewport = " << g_last_clicked_viewport << endl << 
            "x = " << g_last_clicked_x << endl << 
            "y = " << g_last_clicked_y << endl;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    //cout << key << endl;
    if (key == 27)
    {
        exit(0);
    }

    if (g_camera_chosen == -1)
    {
        cout << "click on a camera" << endl;
        return;
    }

    if (key == 'r')
    {
        g_camera_pos[g_camera_chosen][0] += 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'l')
    {
        g_camera_pos[g_camera_chosen][0] -= 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'u')
    {
        g_camera_pos[g_camera_chosen][1] += 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'd')
    {
        g_camera_pos[g_camera_chosen][1] -= 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'n')
    {
        g_camera_pos[g_camera_chosen][2] -= 0.05f;
        glutPostRedisplay();
    }
    else if (key == 'f')
    {
        g_camera_pos[g_camera_chosen][2] += 0.05f;
        glutPostRedisplay();
    }
}

void reshape(int w, int h)
{
    //snap back to original window width and height
    glutReshapeWindow(g_w, g_h);
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

void SetupViewportandModelViewMatrix(int camera)
{
    if (camera == 0)
    {
        glViewport(0, 0, 320, g_h);
    }
    else if (camera == 1)
    {
        glViewport(322, 0, 320, g_h);
    }
    else
    {
        exit(1);
    }

    glMatrixMode(GL_MODELVIEW);

    Vector3 x_axis, y_axis, z_axis;
    Angles_To_Axes(Vector3(g_camera_rot_degrees[camera][0], g_camera_rot_degrees[camera][1], g_camera_rot_degrees[camera][2]), x_axis, y_axis, z_axis);

    Matrix4f camToWorldMatrix = Matrix4f::Identity();
    camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

    Vector3f camTranslation;
    camTranslation << g_camera_pos[camera][0], g_camera_pos[camera][1], g_camera_pos[camera][2];

    camToWorldMatrix.block<3, 1>(0, 3) = camTranslation;
    g_world_to_cam_matrix[camera] = camToWorldMatrix.inverse();

    glLoadMatrixf(g_world_to_cam_matrix[camera].data());  // already in column-major order

    GLfloat* modelView = (camera == 0) ? g_modelView0 : g_modelView1;
    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
}

void SetupProjectionMatrix(int camera)
{
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
    Matrix4f glProj, orthoNdcMatrix;

    glProj = GetProjectionMatrix(pinHoleModel, 2.0f, 6.0f, g_persp_intrinsic_matrix[camera], orthoNdcMatrix);

    //Equivalent to the following:
    //glProj = setGlFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 2.0, 6.0);
    glLoadMatrixf(glProj.data());

    GLfloat* projection = (camera == 0) ? g_projection0 : g_projection1;
    glGetFloatv(GL_PROJECTION_MATRIX, projection);
}

void drawEpipolarLineInOtherViewports()
{
    int otherViewPort = (g_camera_chosen + 1) % 2;

    Vector3f clicked_uv((float)g_last_clicked_x, (float)g_last_clicked_y, 1);

    Matrix4f perspIntrinsic = g_persp_intrinsic_matrix[g_camera_chosen];

    Matrix3f perspIntrinsic_33 = Matrix3f::Zero();
    perspIntrinsic_33.block<2, 2>(0, 0) = perspIntrinsic.block<2, 2>(0, 0);
    perspIntrinsic_33.block<2, 1>(0, 2) = perspIntrinsic.block<2, 1>(0, 2);
    perspIntrinsic_33(2, 2) = -1;
    cout << "Intrinsic--" << endl;
    cout << perspIntrinsic << endl;
    cout << perspIntrinsic_33 << endl;
    cout << perspIntrinsic_33.inverse() << endl;

    Vector3f projectedClickedPoint = Vector3f((1000.0f / 320.0f)*clicked_uv(0), (1000.0f / 240.0f)*clicked_uv(1), 1.0f);
    cout << "Projected point = " << projectedClickedPoint << endl;
    Vector3f unprojectedRay = perspIntrinsic_33.inverse() * projectedClickedPoint;
    cout << "Unprojected ray = " << unprojectedRay << endl;

    Matrix4f modelViewMatrix = g_world_to_cam_matrix[g_camera_chosen];

    SetupViewportandModelViewMatrix(otherViewPort);
    SetupProjectionMatrix(otherViewPort);
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (int multiplier = -100; multiplier < 100; ++multiplier)
    {
        Vector3f point_on_ray = unprojectedRay * multiplier;
        if (point_on_ray(2) < 0)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        else
        {
            glColor3f(0.0f, 0.0f, 1.0f);
        }
        Vector4f point_on_ray_world, temp;
        temp(0) = point_on_ray(0);
        temp(1) = point_on_ray(1);
        temp(2) = point_on_ray(2);
        temp(3) = 1.0f;

        point_on_ray_world = modelViewMatrix.inverse() * temp;

        glVertex3f(point_on_ray_world(0)/point_on_ray_world(3), point_on_ray_world(1)/ point_on_ray_world(3), point_on_ray_world(2)/ point_on_ray_world(3));
    }
    glEnd();
}

void printGLMatrix(GLfloat mvMtx[16])
{
    cout << "Camera chosen: " << g_camera_chosen << endl <<
        mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
        mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
        mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
        mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;
}

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        SetupViewportandModelViewMatrix(0);

        SetupProjectionMatrix(0);

        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
            drawTriangleVertices();
        glEnd();
    }

    {
        //Viewport Separator - nicer solution? how do I make this 1 pixel thick?
        glViewport(320, 0, 2, g_h);
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
        SetupViewportandModelViewMatrix(1);

        SetupProjectionMatrix(1);

        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
            drawTriangleVertices();
        glEnd();
    }

    if (g_camera_chosen != -1)  //if something was clicked
    {
        cout << "ModelView" << endl;
        printGLMatrix(g_camera_chosen == 0 ? g_modelView0 : g_modelView1);
        cout << "Projection" << endl;
        printGLMatrix(g_camera_chosen == 0 ? g_projection0 : g_projection1);

        const Matrix4f& worldToCamMatrix = g_world_to_cam_matrix[g_camera_chosen];
        const Matrix4f& perspIntrinsicMatrix = g_persp_intrinsic_matrix[g_camera_chosen];

        Vector4f world_location(-0.5f, 0.0f, 0.0f, 1.0f);
        Vector4f cam_location = worldToCamMatrix * world_location;
        cout << "Location in cam = " << cam_location / cam_location(3) << endl;
        Vector4f img_location = perspIntrinsicMatrix * cam_location;
        cout << "Location in img = " << img_location / img_location(3) << endl;
        Vector2f viewport_location = Vector2f((img_location(0) / img_location(3))*320.0f / 1000.0f, (img_location(1) / img_location(3))*240.0f / 1000.0f);
        cout << "Location in viewport should be = " << viewport_location << endl;

        drawEpipolarLineInOtherViewports();
    }

    glutSwapBuffers();
}
