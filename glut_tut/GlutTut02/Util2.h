#pragma once
#include <GL\glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>

using namespace std;
using namespace Eigen;

//TODO vaibhavt: http://www.songho.ca/opengl/gl_projectionmatrix.html
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

Eigen::Matrix4f setGluPerspective(double fovY, double aspectRatio, double front, double back)
{
    const double DEG2RAD = 3.14159265 / 180;

    double tangent = tan((fovY / 2)* DEG2RAD);
    double height = front*tangent; //half height of near plane
    double width = height * aspectRatio; // half width of near plane

    return setGlFrustum(-width, width, -height, height, front, back);
}

////---Tests---
//int main_func(int argc, char* argv[])
//{
//    return 0;
//}
