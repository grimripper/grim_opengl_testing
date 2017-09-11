#pragma once
#include <GL/glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include "RotationAroundAxes.h"
#include "ProjectionUsingEigen.h"

using namespace std;
using namespace Eigen;

//---Tests---
int main_func(int argc, char* argv[])
{
    //Look at matrixModelView.exe and matrixProjection.exe

    // Now, if camera is at (0,0,0) and Pitch(X)=0, Heading(Y)=0, Roll(Z)=0
    // M<cam->world> = Translation of camera * Rotation of camera

    Vector3 x_axis, y_axis, z_axis;
    Angles_To_Axes(Vector3(0, 0, 0), x_axis, y_axis, z_axis);

    Matrix4f camToWorldMatrix = Matrix4f::Identity();
    camToWorldMatrix.block<3, 3>(0, 0) = GetRotationMatrix(x_axis, y_axis, z_axis);

    Vector3f translation;
    translation << 0, 0, 7;

    camToWorldMatrix.block<3, 1>(0, 3) = translation;

    cout << "Only Extrinsics: (World->Cam matrix = View matrix) = " << endl;
    cout << "--------------------------------------------" << endl;
    Matrix4f worldToCamMatrix = camToWorldMatrix.inverse();
    cout << worldToCamMatrix << endl;

    //What does this mean?

    cout << "Test1: World origin (0,0,0) in camera: " << endl << worldToCamMatrix * Vector4f(0, 0, 0, 1) << endl;
    cout << "Test2: 1 meter in front of origin (0,0,1) in camera: " << endl << worldToCamMatrix * Vector4f(0, 0, 1, 1) << endl;
    cout << "Test3: 1 meter to the right of origin (1,0,0) in camera: " << endl << worldToCamMatrix * Vector4f(1, 0, 0, 1) << endl;

    //Create a projection matrix from Calibration Model
    FakeCalibration_PinholeModel pinHoleModel;
    pinHoleModel.fx = 1.0f;
    pinHoleModel.fy = 1.0f;
    pinHoleModel.s = 0.0f;
    pinHoleModel.w = 1.0f;
    pinHoleModel.h = 1.0f;
    pinHoleModel.x0 = 0.5f;
    pinHoleModel.y0 = 0.5f;
    Matrix4f projMatrix_to_ndc, perspIntrinsicMatrix, orthoNdcMatrix;

    cout << "Intrinsics + Extrinsics" << endl;
    cout << "-----------------------" << endl;

    projMatrix_to_ndc = GetProjectionMatrix(pinHoleModel, 1.0f, 10.0f, perspIntrinsicMatrix, orthoNdcMatrix);

    cout << "Test1: World origin (0,0,0) in ndc: " << endl << projMatrix_to_ndc * worldToCamMatrix * Vector4f(0, 0, 0, 1) << endl;
    cout << "Test2: 1 meter in front of origin (0,0,1) in ndc: " << endl << projMatrix_to_ndc * worldToCamMatrix * Vector4f(0, 0, 1, 1) << endl;
    cout << "Test3: 1 meter to the right of origin (1,0,0) in ndc: " << endl << projMatrix_to_ndc * worldToCamMatrix * Vector4f(1, 0, 0, 1) << endl;

    //Final mapping to image coordinates
    //-1,-1 should map to 0,0
    //0, 0 should map to 320, 240
    //1, 1 should map to 640, 480

    return 0;
}
