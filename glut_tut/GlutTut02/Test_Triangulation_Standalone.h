#pragma once
#include <GL/glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include <chrono>
#include <random>
#include "ProjectionUsingEigen.h"
#include "RotationAroundAxes.h"
#include "Triangulation.h"

using namespace std;
using namespace Eigen;

const int NUM_CAMERAS = 4;

Matrix4f g_world_to_cam_matrix[NUM_CAMERAS];
Matrix4f g_persp_intrinsic_matrix[NUM_CAMERAS];
Matrix3f g_persp_intrinsic_33_matrix[NUM_CAMERAS];

GLfloat g_modelView[NUM_CAMERAS][16] = { 0.0f };
GLfloat g_projection[NUM_CAMERAS][16] = { 0.0f };

float g_camera_rot_degrees[NUM_CAMERAS][3] = { 0.0f };
float g_camera_pos[NUM_CAMERAS][3] = { 0.0f };

const float g_noise_pixels_sigma = 1;
std::normal_distribution<float> g_pixel_noise_distribution;
std::default_random_engine g_generator;

void SetupExtrinsics(int camera)
{
    Vector3 x_axis, y_axis, z_axis;
    Angles_To_Axes(Vector3(g_camera_rot_degrees[camera][0], g_camera_rot_degrees[camera][1], g_camera_rot_degrees[camera][2]), x_axis, y_axis, z_axis);

    Matrix4f camToWorldMatrix = Matrix4f::Identity();
    camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

    Vector3f camTranslation;
    camTranslation << g_camera_pos[camera][0], g_camera_pos[camera][1], g_camera_pos[camera][2];

    camToWorldMatrix.block<3, 1>(0, 3) = camTranslation;
    g_world_to_cam_matrix[camera] = camToWorldMatrix.inverse();
}

void SetupIntrinsics(int camera)
{
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
    g_persp_intrinsic_33_matrix[camera] = Matrix3f::Zero();
    g_persp_intrinsic_33_matrix[camera].block<2, 2>(0, 0) = g_persp_intrinsic_matrix[camera].block<2, 2>(0, 0);
    g_persp_intrinsic_33_matrix[camera].block<2, 1>(0, 2) = g_persp_intrinsic_matrix[camera].block<2, 1>(0, 2);
    g_persp_intrinsic_33_matrix[camera](2, 2) = -1;
}

//---Tests---
int main_func(int argc, char* argv[])
{
    long long seed = std::chrono::system_clock::now().time_since_epoch().count();
    g_generator = std::default_random_engine((unsigned int)seed);

    g_pixel_noise_distribution = std::normal_distribution<float>(0.0f, g_noise_pixels_sigma);
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

    for (int camera = 0; camera < NUM_CAMERAS; ++camera)
    {
        SetupExtrinsics(camera);
        SetupIntrinsics(camera);
    }

    Vector4f world_points[10];
    Vector2f image_points[NUM_CAMERAS][10];

    for (int i = 0; i < 10; ++i)
    {
        world_points[i] = MatrixXf::Random(4, 1);
        world_points[i] = world_points[i] / world_points[i][3];

        cout << "Projections for world point " << i << endl << world_points[i] << endl;

        TriangulationData data[NUM_CAMERAS];
        TriangulationResults results[NUM_CAMERAS];

        for (int camera = 0; camera < NUM_CAMERAS; ++camera)
        {
            Vector4f cam_point = g_world_to_cam_matrix[camera] * world_points[i];
            cam_point = cam_point / cam_point(3);

            Vector3f cam_point_xyz = Vector3f(cam_point(0), cam_point(1), cam_point(2));
            Vector3f image_point = g_persp_intrinsic_33_matrix[camera] * cam_point_xyz;

            image_points[camera][i] = Vector2f(image_point(0) / image_point(2), image_point(1) / image_point(2));
            cout << "camera: " << camera << endl << image_points[camera][i] << endl;

            image_points[camera][i] += Vector2f(g_pixel_noise_distribution(g_generator), g_pixel_noise_distribution(g_generator));
            cout << "camera (noise added): " << camera << endl << image_points[camera][i] << endl;

            data[camera].use_for_triangulation = true;
            data[camera].x = image_points[camera][i](0);
            data[camera].y = image_points[camera][i](1);
        }

        Vector3f xyz;
        float triangulationError = -1.0f;
        Triangulate_LinearLeastSquares(g_world_to_cam_matrix, g_persp_intrinsic_33_matrix, data, xyz, triangulationError, results);

        cout << "Result (XYZ): " << endl << xyz << endl;
        cout << "GT: " << endl << world_points[i] << endl;
    }

    return 0;
}


