#pragma once
#include <GL\glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>

using namespace std;
using namespace Eigen;

struct Vector3
{
    float x;
    float y;
    float z;
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vector3& normalize()
    {
        float invLength = 1 / (sqrt(x*x + y*y + z*z));
        x *= invLength;
        y *= invLength;
        z *= invLength;
        return *this;
    }

    Vector3 operator-(const Vector3& rhs) const {
        return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    Vector3 cross(const Vector3& rhs) const {
        return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
    }
};

//RxRyRz scheme
void Angles_To_Axes(const Vector3& angles, Vector3& left, Vector3& up, Vector3& forward)
{
    const float DEG2RAD = 3.141593f / 180;

    float sx, sy, sz, cx, cy, cz, theta;

    theta = angles.x * DEG2RAD;
    sx = sinf(theta);
    cx = cosf(theta);

    theta = angles.y * DEG2RAD;
    sy = sinf(theta);
    cy = cosf(theta);

    theta = angles.z * DEG2RAD;
    sz = sinf(theta);
    cz = cosf(theta);

    //determine left axis (new X)
    left.x = cy*cz;
    left.y = sx*sy*cz + cx*sz;
    left.z = -cx*sy*cz + sx*sz;

    //determine up axis (new Y)
    up.x = -cy*sz;
    up.y = -sx*sy*sz + cx*cz;
    up.z = cx*sy*sz + sx*cz;

    //determine forward axis (new Z)
    forward.x = sy;
    forward.y = -sx*cy;
    forward.z = cx*cy;
}

void LookAtAxes(const Vector3& position, const Vector3& target, const Vector3& upDir, Vector3& left, Vector3& up, Vector3& forward)
{
    forward = target - position;
    forward.normalize();

    left = upDir.cross(forward);
    left.normalize();

    up = forward.cross(left);
    up.normalize();
}

Matrix3f GetRotationMatrix(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis)
{
    Matrix3f rot_matrix;
    rot_matrix << x_axis.x, y_axis.x, z_axis.x,
        x_axis.y, y_axis.y, z_axis.y,
        x_axis.z, y_axis.z, z_axis.z;
    return rot_matrix;
}

//TODO vaibhavt: Not working unless in GL context :-(
void TestGluLookAt()
{
    cout << "Test Glu Look at" << endl;
    GLfloat mvMtx[16] = { 0.0f };

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMtx);

    cout << Map<Matrix4f>(mvMtx, 4, 4) << endl;
}

void Test2()
{
    Vector3 x_axis, y_axis, z_axis;
    cout << "Rotate around X (-),Y (-),Z (-)" << endl;
    Angles_To_Axes(Vector3(0, -15, 0), x_axis, y_axis, z_axis);

    Matrix3f rot_matrix = GetRotationMatrix(x_axis, y_axis, z_axis);
    cout << "Matrix = " << endl << rot_matrix << endl;

    cout << "Inverse = " << endl << rot_matrix.inverse() << endl;

    //cout << "Using Look at to look at -Z axis (typical OpenGL setup)" << endl;
    //LookAtAxes(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0), x_axis, y_axis, z_axis);
    //Matrix3f rot_180y_matrix = GetRotationMatrix(x_axis, y_axis, z_axis);
    //cout << "Inverse (rotated around Z) = " << (rot_180y_matrix * rot_matrix.inverse()) << endl;
}

void Rotation_And_Translation()
{
    Vector3 x_axis, y_axis, z_axis;
    Angles_To_Axes(Vector3(10, -15, 0), x_axis, y_axis, z_axis);

    Matrix4f modelViewMatrix = Matrix4f::Identity();
    modelViewMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

    Vector3f translation;
    translation << 1, 3, 2;

    modelViewMatrix.block<3, 1>(0, 3) = translation;

    cout << modelViewMatrix;

    cout << "Inverse = " << endl;
    cout << modelViewMatrix.inverse() << endl;
}

//---Tests---
int main_func(int argc, char* argv[])
{
    Vector3 x_axis, y_axis, z_axis;
    Vector3 angles;

    cout << "Rotate around Y by 30 degrees" << endl;
    angles.x = 0;
    angles.y = 30;
    angles.z = 0;
    Angles_To_Axes(angles, x_axis, y_axis, z_axis);

    Matrix3f rot_matrix = GetRotationMatrix(x_axis, y_axis, z_axis);
    cout << rot_matrix << endl;

    Vector3f vec(1, 0, 0);
    cout << (rot_matrix * vec) << endl;

    //Rotation matrix after rotating around Y by 90 degrees
    cout << "Rotate around around Y by 90 degrees" << endl;
    angles.x = 0;
    angles.y = 90;
    angles.z = 0;
    Angles_To_Axes(angles, x_axis, y_axis, z_axis);

    rot_matrix = GetRotationMatrix(x_axis, y_axis, z_axis);
    cout << rot_matrix << endl;

    cout << "Using Look at to look at -Z axis (typical OpenGL setup)" << endl;
    LookAtAxes(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0), x_axis, y_axis, z_axis);
    rot_matrix = GetRotationMatrix(x_axis, y_axis, z_axis);
    cout << rot_matrix << endl;

    cout << "Rotate around Y axis 180 degrees to mimic the above" << endl;
    Angles_To_Axes(Vector3(0, 180, 0), x_axis, y_axis, z_axis);
    rot_matrix = GetRotationMatrix(x_axis, y_axis, z_axis);
    cout << rot_matrix << endl;

    TestGluLookAt();

    Test2();

    Rotation_And_Translation();
    return 0;
}
