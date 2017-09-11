#pragma once

#pragma once
#include <GL/glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include "Util.h"

using namespace std;
using namespace Eigen;

//---Tests---
int main_func(int argc, char* argv[])
{
    //Look at matrixModelView.exe

    // Now, if camera is at (0,0,0) and Pitch(X)=0, Heading(Y)=0, Roll(Z)=0
    // M<cam->world> = Translation of camera * Rotation of camera

    Vector3 x_axis, y_axis, z_axis;
    Angles_To_Axes(Vector3(10, 0, 0), x_axis, y_axis, z_axis);

    Matrix4f camToWorldMatrix = Matrix4f::Identity();
    camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

    Vector3f translation;
    translation << 0, 0, 10;

    camToWorldMatrix.block<3, 1>(0, 3) = translation;

    cout << "Cam -> World matrix" << endl;
    cout << "-------------------" << endl;
    cout << camToWorldMatrix << endl;

    //What does this mean?
    //To know the origin of the camera in world, do the following:

    cout << "Test1: Camera origin (0,0,0) in world: " << endl << camToWorldMatrix * Vector4f(0, 0, 0, 1) << endl;
    cout << "Test2: 1 meter ahead of camera (0,0,-1) in world: " << endl << camToWorldMatrix * Vector4f(0, 0, -1, 1) << endl;
    cout << "Test3: 1 meter to the right of camera (1,0,0) in world: " << endl << camToWorldMatrix * Vector4f(1, 0, 0, 1) << endl;
    cout << "Test4: x-axis of camera (1,0,0,0) in the world: " << endl << camToWorldMatrix * Vector4f(1, 0, 0, 0) << endl;
    cout << "Test5: y-axis of camera (0,1,0,0) in the world: " << endl << camToWorldMatrix * Vector4f(0, 1, 0, 0) << endl;
    cout << "Test6: z-axis of camera (0,0,1,0) in the world: " << endl << camToWorldMatrix * Vector4f(0, 0, 1, 0) << endl;

    cout << "Inverse (World->Cam matrix = View matrix) = " << endl;
    cout << "--------------------------------------------" << endl;
    Matrix4f worldToCamMatrix = camToWorldMatrix.inverse();
    cout << worldToCamMatrix << endl;

    //What does this mean?

    cout << "Test1: World origin (0,0,0) in camera: " << endl << worldToCamMatrix * Vector4f(0, 0, 0, 1) << endl;
    cout << "Test2: 1 meter behind origin (0,0,1) in camera: " << endl << worldToCamMatrix * Vector4f(0, 0, 1, 1) << endl;
    cout << "Test3: 1 meter to the right of origin (1,0,0) in camera: " << endl << worldToCamMatrix * Vector4f(1, 0, 0, 1) << endl;
    cout << "Test4: x-axis of world (1,0,0,0) in the camera: " << endl << worldToCamMatrix * Vector4f(1, 0, 0, 0) << endl;
    cout << "Test5: y-axis of world (0,1,0,0) in the camera: " << endl << worldToCamMatrix * Vector4f(0, 1, 0, 0) << endl;
    cout << "Test6: z-axis of world (0,0,1,0) in the camera: " << endl << worldToCamMatrix * Vector4f(0, 0, 1, 0) << endl;

    return 0;
}
