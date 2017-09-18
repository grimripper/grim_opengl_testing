#pragma once

#include <GL\glut.h>
#include "ProjectionUsingEigen.h"
#include "RotationAroundAxes.h"
#include "Triangulation.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace Eigen;


const int NUM_CAMERAS = 4;

const int WIDTH_VIEWPORT = 320;
const int HEIGHT_VIEWPORT = 240;

const int WIDTH_WINDOW = WIDTH_VIEWPORT*NUM_CAMERAS + (NUM_CAMERAS - 1) * 2;

Matrix4f g_world_to_cam_matrix[NUM_CAMERAS];
Matrix4f g_persp_intrinsic_matrix[NUM_CAMERAS];
Matrix3f g_persp_intrinsic_matrix_33[NUM_CAMERAS];

GLfloat g_modelView[NUM_CAMERAS][16] = { 0.0f };
GLfloat g_projection[NUM_CAMERAS][16] = { 0.0f };

int g_triangulation_clicked_xybool_location[NUM_CAMERAS][3] = { 0 };

int g_last_clicked_viewport = -1;
int g_last_clicked_x = -1;
int g_last_clicked_y = -1;

int g_camera_chosen = -1;

float g_camera_rot_degrees[NUM_CAMERAS][3] = { 0.0f };
float g_camera_pos[NUM_CAMERAS][3] = { 0.0f };

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
    g_camera_rot_degrees[1][1] = -15.0f;
    g_camera_rot_degrees[1][2] = 0.0f;

    g_camera_rot_degrees[2][0] = 15.0f;
    g_camera_rot_degrees[2][1] = 0.0f;
    g_camera_rot_degrees[2][2] = 0.0f;

    g_camera_rot_degrees[3][0] = 0.0f;
    g_camera_rot_degrees[3][1] = 0.0f;
    g_camera_rot_degrees[3][2] = -15.0f;

    g_camera_pos[0][0] = 0.0f;
    g_camera_pos[0][1] = 0.0f;
    g_camera_pos[0][2] = 5.0f;

    g_camera_pos[1][0] = 1.0f;
    g_camera_pos[1][1] = 0.0f;
    g_camera_pos[1][2] = 3.0f;

    g_camera_pos[2][0] = 0.0f;
    g_camera_pos[2][1] = 0.0f;
    g_camera_pos[2][2] = 4.0f;

    g_camera_pos[3][0] = 0.0f;
    g_camera_pos[3][1] = 3.0f;
    g_camera_pos[3][2] = 3.5f;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH_WINDOW, HEIGHT_VIEWPORT);
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
        //Reset on left-click
        g_triangulation_clicked_xybool_location[0][2] = 0;
        g_triangulation_clicked_xybool_location[1][2] = 0;
        g_triangulation_clicked_xybool_location[2][2] = 0;
        g_triangulation_clicked_xybool_location[3][2] = 0;

        if (x >= 966)
        {
            g_last_clicked_viewport = 3;
            g_last_clicked_x = x - 966;
            g_last_clicked_y = (240 - y);
        }
        else if (x >= 644 && x < 964)
        {
            g_last_clicked_viewport = 2;
            g_last_clicked_x = x - 644;
            g_last_clicked_y = (240 - y);
        }
        else if (x >= 322 && x < 642)
        {
            g_last_clicked_viewport = 1;
            g_last_clicked_x = x - 322;
            g_last_clicked_y = (240 - y);
        }
        else if (x < 320)
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
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //Reset on right-click
        g_camera_chosen = -1;

        //UGLY (copy of above code)
        if (x >= 966)
        {
            g_triangulation_clicked_xybool_location[3][2] = 1;
            g_triangulation_clicked_xybool_location[3][0] = x - 966;
            g_triangulation_clicked_xybool_location[3][1] = (240 - y);
        }
        else if (x >= 644 && x < 964)
        {
            g_triangulation_clicked_xybool_location[2][2] = 1;
            g_triangulation_clicked_xybool_location[2][0] = x - 644;
            g_triangulation_clicked_xybool_location[2][1] = (240 - y);
        }
        else if (x >= 322 && x < 642)
        {
            g_triangulation_clicked_xybool_location[1][2] = 1;
            g_triangulation_clicked_xybool_location[1][0] = x - 322;
            g_triangulation_clicked_xybool_location[1][1] = (240 - y);
        }
        else if (x < 320)
        {
            g_triangulation_clicked_xybool_location[0][2] = 1;
            g_triangulation_clicked_xybool_location[0][0] = x;
            g_triangulation_clicked_xybool_location[0][1] = (240 - y);
        }
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
    glutReshapeWindow(WIDTH_WINDOW, HEIGHT_VIEWPORT);
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

void SetupViewport(int camera)
{
    if (camera == 0)
    {
        glViewport(0, 0, 320, HEIGHT_VIEWPORT);
    }
    else if (camera == 1)
    {
        glViewport(322, 0, 320, HEIGHT_VIEWPORT);
    }
    else if (camera == 2)
    {
        glViewport(644, 0, 320, HEIGHT_VIEWPORT);
    }
    else if (camera == 3)
    {
        glViewport(966, 0, 320, HEIGHT_VIEWPORT);
    }
    else
    {
        exit(1);
    }
}

void SetupModelViewMatrix(int camera)
{
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

    GLfloat* modelView = g_modelView[g_camera_chosen];
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

    g_persp_intrinsic_matrix_33[camera] = Matrix3f::Zero();
    g_persp_intrinsic_matrix_33[camera].block<2, 2>(0, 0) = g_persp_intrinsic_matrix[camera].block<2, 2>(0, 0);
    g_persp_intrinsic_matrix_33[camera].block<2, 1>(0, 2) = g_persp_intrinsic_matrix[camera].block<2, 1>(0, 2);
    g_persp_intrinsic_matrix_33[camera](2, 2) = -1;

    //Equivalent to the following:
    //glProj = setGlFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 2.0, 6.0);
    glLoadMatrixf(glProj.data());

    GLfloat* projection = g_projection[g_camera_chosen];
    glGetFloatv(GL_PROJECTION_MATRIX, projection);
}

void drawEpipolarLineInOtherViewports()
{
    Vector3f clicked_uv((float)g_last_clicked_x, (float)g_last_clicked_y, 1);

    Matrix4f perspIntrinsic = g_persp_intrinsic_matrix[g_camera_chosen];

    Matrix3f perspIntrinsic_33 = g_persp_intrinsic_matrix_33[g_camera_chosen];

    cout << "Intrinsic--" << endl;
    cout << perspIntrinsic << endl;
    cout << perspIntrinsic_33 << endl;
    cout << perspIntrinsic_33.inverse() << endl;

    Vector3f projectedClickedPoint = Vector3f((1000.0f / 320.0f)*clicked_uv(0), (1000.0f / 240.0f)*clicked_uv(1), 1.0f);
    cout << "Projected point = " << projectedClickedPoint << endl;
    Vector3f unprojectedPoint = perspIntrinsic_33.inverse() * projectedClickedPoint;
    unprojectedPoint = unprojectedPoint / unprojectedPoint(2);
    cout << "Unprojected point = " << unprojectedPoint << endl;

    Matrix4f modelViewMatrix = g_world_to_cam_matrix[g_camera_chosen];

    for (int otherViewport = 0; otherViewport < NUM_CAMERAS; ++otherViewport)
    {
        if (otherViewport == g_camera_chosen)
        {
            continue;
        }

        SetupViewport(otherViewport);
        SetupModelViewMatrix(otherViewport);
        SetupProjectionMatrix(otherViewport);
        glPointSize(1.0f);
        glBegin(GL_POINTS);
        for (int multiplier = -100; multiplier < 100; ++multiplier)
        {
            Vector3f point_on_ray = unprojectedPoint * multiplier * 0.1;
            glColor3f(1.0f, 1.0f, 1.0f);
            Vector4f point_on_ray_world, temp;
            temp(0) = point_on_ray(0);
            temp(1) = point_on_ray(1);
            temp(2) = point_on_ray(2);
            temp(3) = 1.0f;

            point_on_ray_world = modelViewMatrix.inverse() * temp;
            point_on_ray_world = point_on_ray_world / point_on_ray_world(3);

            glVertex3f(point_on_ray_world(0), point_on_ray_world(1), point_on_ray_world(2));
        }
        glEnd();
    }
}

void drawReprojectedPointsInAllViewports(const Vector3f& world_location)
{
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        Matrix4f modelViewMatrix = g_world_to_cam_matrix[i];
        Matrix4f perspIntrinsic = g_persp_intrinsic_matrix[i];
        Matrix3f perspIntrinsic_33 = g_persp_intrinsic_matrix_33[i];

        cout << "Camera = " << i << endl;
        cout << "----------" << endl;
        Vector4f cam_location = modelViewMatrix * Vector4f(world_location(0), world_location(1), world_location(2), 1.0f);
        cout << "Location in cam = " << cam_location / cam_location(3) << endl;
        Vector4f img_location = perspIntrinsic * cam_location;
        cout << "Location in img = " << img_location / img_location(3) << endl;
        Vector2f viewport_location = Vector2f((img_location(0) / img_location(3))*320.0f / 1000.0f, (img_location(1) / img_location(3))*240.0f / 1000.0f);
        cout << "Location in viewport should be = " << viewport_location << endl;

        SetupViewport(i);
        glColor3f(1.0f, 1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1.0, 0, 1.0, 0, 1);
        glPointSize(10.0f);
        glBegin(GL_POINTS);
        glVertex2f(img_location(0) / (1000.0f*img_location(3)), img_location(1) / (1000.0f*img_location(3)));
        glEnd();
    }
}

void printGLMatrix(GLfloat mvMtx[16])
{
    cout << "Camera chosen: " << g_camera_chosen << endl <<
        mvMtx[0] << "\t" << mvMtx[4] << "\t" << mvMtx[8] << "\t" << mvMtx[12] << endl <<
        mvMtx[1] << "\t" << mvMtx[5] << "\t" << mvMtx[9] << "\t" << mvMtx[13] << endl <<
        mvMtx[2] << "\t" << mvMtx[6] << "\t" << mvMtx[10] << "\t" << mvMtx[14] << endl <<
        mvMtx[3] << "\t" << mvMtx[7] << "\t" << mvMtx[11] << "\t" << mvMtx[15] << endl;
}

//Viewport Separator - nicer solution? how do I make this 1 pixel thick?
void drawSeparator(int first_viewport, int second_viewport)
{
    if (first_viewport == 0)
    {
        glViewport(320, 0, 2, HEIGHT_VIEWPORT);
    }
    else if (first_viewport == 1)
    {
        glViewport(642, 0, 2, HEIGHT_VIEWPORT);
    }
    else if (first_viewport == 2)
    {
        glViewport(964, 0, 2, HEIGHT_VIEWPORT);
    }
    else
    {
        exit(1);
    }

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

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupViewport(0);
    SetupModelViewMatrix(0);
    SetupProjectionMatrix(0);

    glLineWidth(3.0f);
    glBegin(GL_TRIANGLES);
    drawTriangleVertices();
    glEnd();

    for (int viewport = 1; viewport < NUM_CAMERAS; ++viewport)
    {
        drawSeparator(viewport - 1, viewport);

        SetupViewport(viewport);
        SetupModelViewMatrix(viewport);
        SetupProjectionMatrix(viewport);

        glLineWidth(3.0f);
        glBegin(GL_TRIANGLES);
        drawTriangleVertices();
        glEnd();
    }

    if (g_camera_chosen != -1)  //if something was clicked
    {
        //cout << "ModelView" << endl;
        //printGLMatrix(g_modelView[g_camera_chosen]);
        //cout << "Projection" << endl;
        //printGLMatrix(g_projection[g_camera_chosen]);

        //const Matrix4f& worldToCamMatrix = g_world_to_cam_matrix[g_camera_chosen];
        //const Matrix4f& perspIntrinsicMatrix = g_persp_intrinsic_matrix[g_camera_chosen];

        //Vector4f world_location(-0.5f, 0.0f, 0.0f, 1.0f);
        //Vector4f cam_location = worldToCamMatrix * world_location;
        //cout << "Location in cam = " << cam_location / cam_location(3) << endl;
        //Vector4f img_location = perspIntrinsicMatrix * cam_location;
        //cout << "Location in img = " << img_location / img_location(3) << endl;
        //Vector2f viewport_location = Vector2f((img_location(0) / img_location(3))*320.0f / 1000.0f, (img_location(1) / img_location(3))*240.0f / 1000.0f);
        //cout << "Location in viewport should be = " << viewport_location << endl;
        drawEpipolarLineInOtherViewports();
    }

    //Check for at least 2 cameras that have x,y values set
    int countOfCamerasSet = 0;
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        countOfCamerasSet += g_triangulation_clicked_xybool_location[i][2];
    }
    cout << "Count of viewports clicked on = " << countOfCamerasSet << endl;
    if (countOfCamerasSet > 1)
    {
        TriangulationData data[NUM_CAMERAS];
        TriangulationResults results[NUM_CAMERAS];
        for (int i = 0; i < NUM_CAMERAS; ++i)
        {
            if (g_triangulation_clicked_xybool_location[i][2] == 0)
            {
                data[i].use_for_triangulation = false;
                continue;
            }

            Matrix3f perspIntrinsic_33 = g_persp_intrinsic_matrix_33[i];

            //Do triangulation every time
            Vector3f clickedPointInCalibImage = Vector3f((1000.0f / 320.0f)*g_triangulation_clicked_xybool_location[i][0], (1000.0f / 240.0f)*g_triangulation_clicked_xybool_location[i][1], 1.0f);
            cout << "Projected point = " << clickedPointInCalibImage << endl;

            data[i].use_for_triangulation = true;
            data[i].x = clickedPointInCalibImage(0);
            data[i].y = clickedPointInCalibImage(1);
        }

        Vector3f xyz;
        float triangulationError = -1.0f;
        Triangulate_LinearLeastSquares(g_world_to_cam_matrix, g_persp_intrinsic_matrix_33, data, xyz, triangulationError, results);

        cout << "Result (XYZ): " << endl << xyz << endl;

        drawReprojectedPointsInAllViewports(xyz);
    }

    glutSwapBuffers();
}
