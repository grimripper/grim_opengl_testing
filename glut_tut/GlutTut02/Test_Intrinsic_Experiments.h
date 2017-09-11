#pragma once
#include <GL/glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include "ProjectionUsingEigen.h"

using namespace std;
using namespace Eigen;

//---Tests---
int main_func(int argc, char* argv[])
{
    FakeCalibration_PinholeModel pinHoleModel;
    pinHoleModel.fx = 1.0f;
    pinHoleModel.fy = 1.0f;
    pinHoleModel.s = 0.0f;
    pinHoleModel.w = 1.0f;
    pinHoleModel.h = 1.0f;
    pinHoleModel.x0 = 0.5f;
    pinHoleModel.y0 = 0.5f;
    Matrix4f projMatrix, perspMatrix, orthoMatrix;

    projMatrix = GetProjectionMatrix(pinHoleModel, 2.0f, 10.0f, perspMatrix, orthoMatrix);

    cout << "Perspective matrix = " << endl << perspMatrix << endl;

    cout << "Ortho matrix = " << endl << orthoMatrix << endl;

    Vector4f output;

    cout << "Testing perspective (intrinsic) matrix only" << endl
        << "------------------------------------------" << endl;
        
    cout << "Test1: (0,0, -2)" << endl;
    output = perspMatrix * Vector4f(0.0f, 0.0f, -2.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -2.1)" << endl;
    output = perspMatrix * Vector4f(0.0f, 0.0f, -2.1f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -6)" << endl;
    output = perspMatrix * Vector4f(0.0f, 0.0f, -6.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -9.9)" << endl;
    output = perspMatrix * Vector4f(0.0f, 0.0f, -9.9f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -10)" << endl;
    output = perspMatrix * Vector4f(0.0f, 0.0f, -10.0f, 1.0f);
    cout << output / output(3) << endl;

    cout << "Test1: (1,1, -2)" << endl;
    output = perspMatrix * Vector4f(1.0f, 1.0f, -2.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -2.1)" << endl;
    output = perspMatrix * Vector4f(1.0f, 1.0f, -2.1f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -6)" << endl;
    output = perspMatrix * Vector4f(1.0f, 1.0f, -6.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -9.9)" << endl;
    output = perspMatrix * Vector4f(1.0f, 1.0f, -9.9f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -10)" << endl;
    output = perspMatrix * Vector4f(1.0f, 1.0f, -10.0f, 1.0f);
    cout << output / output(3) << endl;

    cout << "Test1: (-1,-1, -2)" << endl;
    output = perspMatrix * Vector4f(-1.0f, -1.0f, -2.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -2.1)" << endl;
    output = perspMatrix * Vector4f(-1.0f, -1.0f, -2.1f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -6)" << endl;
    output = perspMatrix * Vector4f(-1.0f, -1.0f, -6.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -9.9)" << endl;
    output = perspMatrix * Vector4f(-1.0f, -1.0f, -9.9f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -10)" << endl;
    output = perspMatrix * Vector4f(-1.0f, -1.0f, -10.0f, 1.0f);
    cout << output / output(3) << endl;

    cout << "Testing ortho (NDC) * perspective (intrinsic) matrix only" << endl
        << "------------------------------------------" << endl;

    cout << "Test1: (0,0, -2)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(0.0f, 0.0f, -2.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -2.1)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(0.0f, 0.0f, -2.1f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -6)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(0.0f, 0.0f, -6.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -9.9)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(0.0f, 0.0f, -9.9f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (0,0, -10)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(0.0f, 0.0f, -10.0f, 1.0f);
    cout << output / output(3) << endl;

    cout << "Test1: (1,1, -2)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(1.0f, 1.0f, -2.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -2.1)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(1.0f, 1.0f, -2.1f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -6)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(1.0f, 1.0f, -6.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -9.9)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(1.0f, 1.0f, -9.9f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (1,1, -10)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(1.0f, 1.0f, -10.0f, 1.0f);
    cout << output / output(3) << endl;

    cout << "Test1: (-1,-1, -2)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(-1.0f, -1.0f, -2.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -2.1)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(-1.0f, -1.0f, -2.1f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -6)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(-1.0f, -1.0f, -6.0f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -9.9)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(-1.0f, -1.0f, -9.9f, 1.0f);
    cout << output / output(3) << endl;
    cout << "Test1: (-1,-1, -10)" << endl;
    output = orthoMatrix * perspMatrix * Vector4f(-1.0f, -1.0f, -10.0f, 1.0f);
    cout << output / output(3) << endl;

    return 0;
}
