#pragma once

#include <GL\glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>

using namespace std;
using namespace Eigen;

//FROM: http://www.songho.ca/opengl/gl_projectionmatrix.html
Eigen::Matrix4f setGlFrustum(float l, float r, float b, float t, float n, float f)
{
    Eigen::Matrix4f projectionMatrix = Matrix4f::Zero();
    projectionMatrix(0, 0) = 2 * n / (r - l);
    projectionMatrix(0, 2) = (r + l) / (r - l);
    projectionMatrix(1, 1) = 2 * n / (t - b);
    projectionMatrix(1, 2) = (t + b) / (t - b);
    projectionMatrix(2, 2) = -(f + n) / (f - n);
    projectionMatrix(2, 3) = -2 * f*n / (f - n);
    projectionMatrix(3, 2) = -1;

    return projectionMatrix;
}

Eigen::Matrix4f setOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
    Eigen::Matrix4f projectionMatrix = Matrix4f::Zero();
    projectionMatrix(0, 0) = 2 / (r - l);
    projectionMatrix(0, 3) = -(r + l) / (r - l);
    projectionMatrix(1, 1) = 2 / (t - b);
    projectionMatrix(1, 3) = -(t + b) / (t - b);
    projectionMatrix(2, 2) = -2 / (f - n);
    projectionMatrix(2, 3) = -(f + n) / (f - n);
    projectionMatrix(3, 3) = 1;

    return projectionMatrix;
}

Eigen::Matrix4f setGluPerspective(float fovY, float aspectRatio, float front, float back)
{
    const double DEG2RAD = 3.14159265 / 180;

    double tangent = tan((fovY / 2)* DEG2RAD);
    double height = front*tangent; //half height of near plane
    double width = height * aspectRatio; // half width of near plane

    return setGlFrustum(-width, width, -height, height, front, back);
}

//NOTE: A bit different from http://jamesgregson.blogspot.com/2011/11/matching-calibrated-cameras-with-opengl.html
struct FakeCalibration_PinholeModel
{
    float fx;
    float fy;
    float s;
    float x0;
    float y0;

    //assume TopLeft is (0,0)
    //what is pixel convention???
    float w;
    float h;
};

Matrix4f GetProjectionMatrix(
    const FakeCalibration_PinholeModel& calib,
    float near_plane,
    float far_plane,
    Matrix4f& perspMatrix,
    Matrix4f& orthoMatrix)
{
    perspMatrix = Matrix4f::Zero();
    perspMatrix(0, 0) = calib.fx;
    perspMatrix(0, 1) = calib.s;
    perspMatrix(0, 2) = -calib.x0; //different here
    perspMatrix(0, 3) = 0.0f;
    perspMatrix(1, 0) = 0.0f;
    perspMatrix(1, 1) = calib.fy;
    perspMatrix(1, 2) = -calib.y0; //different here
    perspMatrix(1, 3) = 0.0f;
    perspMatrix(2, 0) = 0.0f;
    perspMatrix(2, 1) = 0.0f;
    perspMatrix(2, 2) = (near_plane + far_plane);
    perspMatrix(2, 3) = near_plane*far_plane;
    perspMatrix(3, 0) = 0.0f;
    perspMatrix(3, 1) = 0.0f;
    perspMatrix(3, 2) = -1.0f;
    perspMatrix(3, 3) = 0.0f;

    //Construct an orthographic matrix which maps from projected coordinates to normalized device coordinates
    //in the range [-1,1]. OpenGL then maps coordinates in NDC to the current viewport. 

    //NOTE Different from kyle simek here. Doing 0->1
    orthoMatrix = setOrthoFrustum(0, calib.w, 0, calib.h, near_plane, far_plane);

    Matrix4f projMatrix = orthoMatrix * perspMatrix;
    return projMatrix;
}
