#pragma once
#include <GL\glut.h>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include "ProjectionUsingEigen.h"

using namespace std;
using namespace Eigen;

struct TriangulationData
{
    bool use_for_triangulation; // supports a mode where we just get triangulation results reprojected
    float x;
    float y;
};

struct TriangulationResults
{
    float reproj_error;

    float x_reproj;
    float y_reproj;
};

//Minimize algebraic error
void Triangulate_LinearLeastSquares(
    Matrix4f all_extrinsics[4],
    Matrix3f all_intrinsics[3],
    TriangulationData points[4],
    Vector3f& xyz,
    float& triangulation_error,
    TriangulationResults results[4])
{
    //Unproject: Convert from image (x,y) -> w[u v 1]
    Vector3f unprojected_point[4];
    int num_points = 0;
    for (int point_idx = 0; point_idx < 4; ++point_idx)
    {
        if (!points[point_idx].use_for_triangulation)
        {
            continue;
        }

        const Matrix3f& intrinsics = all_intrinsics[point_idx];
        cout << "Intrinsics: " << endl << intrinsics << endl;

        Matrix3f intrinsics_inverse = all_intrinsics[point_idx].inverse();
        unprojected_point[point_idx] = intrinsics_inverse * Vector3f(points[point_idx].x, points[point_idx].y, 1.0f);

        unprojected_point[point_idx] = unprojected_point[point_idx] / unprojected_point[point_idx](2);

        cout << "x = " << points[point_idx].x << ", y = " <<  points[point_idx].y << " unprojects to " << endl << unprojected_point[point_idx] << endl;

        ++num_points;
    }
 
    MatrixXf A_matrix(2*num_points, 4);
    //For each point, add 2 rows to A
    // A = [u.p3' - p1']
    //     [v.p3' - p2']
    int row_id = 0;
    for (int point_idx = 0; point_idx < 4; ++point_idx)
    {
        if (!points[point_idx].use_for_triangulation)
        {
            continue;
        }

        const Matrix4f& extrinsics = all_extrinsics[point_idx];
        cout << "Extrinsics: " << endl << extrinsics << endl;

        A_matrix(2* row_id, 0) = unprojected_point[point_idx](0) * extrinsics(2, 0) - extrinsics(0, 0);
        A_matrix(2* row_id, 1) = unprojected_point[point_idx](0) * extrinsics(2, 1) - extrinsics(0, 1);
        A_matrix(2* row_id, 2) = unprojected_point[point_idx](0) * extrinsics(2, 2) - extrinsics(0, 2);
        A_matrix(2* row_id, 3) = unprojected_point[point_idx](0) * extrinsics(2, 3) - extrinsics(0, 3);

        A_matrix(2* row_id + 1, 0) = unprojected_point[point_idx](1) * extrinsics(2, 0) - extrinsics(1, 0);
        A_matrix(2* row_id + 1, 1) = unprojected_point[point_idx](1) * extrinsics(2, 1) - extrinsics(1, 1);
        A_matrix(2* row_id + 1, 2) = unprojected_point[point_idx](1) * extrinsics(2, 2) - extrinsics(1, 2);
        A_matrix(2* row_id + 1, 3) = unprojected_point[point_idx](1) * extrinsics(2, 3) - extrinsics(1, 3);

        ++row_id;
    }
    cout << "Final A: " << endl << A_matrix << endl;

    //Solve for AX = 0 using SVD decomposition of A and last eigen vector column (corresponding to smallest eigen value) is solution
    //or eigen decomposition of A'A and eigen vector column corresponding to smallest eigen value is solution
    JacobiSVD<MatrixXf> svd_A(A_matrix, ComputeThinU | ComputeThinV);

    cout << endl << svd_A.matrixV() << endl;
    Vector4f triangulated_pos = svd_A.matrixV().col(3);
    triangulated_pos = triangulated_pos / triangulated_pos(3);

    xyz(0) = triangulated_pos(0);
    xyz(1) = triangulated_pos(1);
    xyz(2) = triangulated_pos(2);

    triangulation_error = 0.0f;
}

//Minimize geometric 2d error
void Triangulate_NonLinearLeastSquares(
    const TriangulationData points[4],
    Vector3f& xyz,
    float& triangulation_error,
    TriangulationResults results[4])
{
    //Might do linear least squares first (or expect it's already done)
}
