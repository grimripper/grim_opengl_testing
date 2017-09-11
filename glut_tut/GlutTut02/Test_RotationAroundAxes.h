#pragma once
#include "RotationAroundAxes.h"

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
