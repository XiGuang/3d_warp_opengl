//
// Created by Admin on 2023/11/23.
//

#ifndef INC_3D_WARP_OPENGL_MATH_H
#define INC_3D_WARP_OPENGL_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Eigen/Dense>

namespace gl_math{
    // 将glm::mat4转换为GLdouble[16]
    void mat4ToGLdoubleArray(const glm::mat4& matrix, GLdouble* array) {
        const GLfloat* glmArray = glm::value_ptr(matrix);

        for (int i = 0; i < 16; ++i) {
            array[i] = static_cast<GLdouble>(glmArray[i]);
        }
    }

    // 将glm::mat4转换为GLfloat[16]
    void mat4ToGLfloatArray(const glm::mat4& matrix, GLfloat* array) {
        const GLfloat* glmArray = glm::value_ptr(matrix);

        for (int i = 0; i < 16; ++i) {
            array[i] = static_cast<GLfloat>(glmArray[i]);
        }
    }

    // 将glm::mat4转换为Eigen::Matrix4d
    Eigen::Matrix4d mat4ToEigenMatrix4d(const glm::mat4& matrix) {
        Eigen::Matrix4d result;
        for (int i = 0; i < 16; ++i) {
            result(i) = matrix[i / 4][i % 4];
        }
        return result;
    }

    // 将Eigen::Matrix4d转换为glm::mat4
    glm::mat4 eigenMatrix4dToMat4(const Eigen::Matrix4d& matrix) {
        glm::mat4 result;
        for (int i = 0; i < 16; ++i) {
            result[i / 4][i % 4] = matrix(i);
        }
        return result;
    }

    /// 将世界坐标系下的点投影到屏幕坐标系
    /// \param point 世界坐标系下的点
    /// \param projection 投影矩阵
    /// \param model 模型矩阵
    /// \param view 视图矩阵
    /// \param viewport 视口
    /// \return 屏幕坐标系下的点
    Eigen::Vector3d project(const Eigen::Vector3d& point, const Eigen::Matrix4d& projection, const Eigen::Matrix4d& model,const Eigen::Matrix4d& view, const Eigen::Vector4d& viewport) {
        Eigen::Vector4d point_4d(point(0), point(1), point(2), 1.0);
        Eigen::Vector4d projected_point = projection * view * model * point_4d;
        projected_point /= projected_point(3);
        Eigen::Vector3d result;
        result(0) = (projected_point(0) + 1.0) * viewport(2) / 2.0 + viewport(0);
        result(1) = (projected_point(1) + 1.0) * viewport(3) / 2.0 + viewport(1);
        result(2) = (projected_point(2) + 1.0) / 2.0;
        return result;
    }

    /// 将屏幕坐标系下的点反投影到世界坐标系
    /// \param point 屏幕坐标系下的点
    /// \param projection 投影矩阵
    /// \param model 模型矩阵
    /// \param view 视图矩阵
    /// \param viewport 视口
    /// \return 世界坐标系下的点
    Eigen::Vector3d unProject(const Eigen::Vector3d& point, const Eigen::Matrix4d& projection, const Eigen::Matrix4d& model,const Eigen::Matrix4d& view, const Eigen::Vector4d& viewport) {
        Eigen::Vector4d point_4d;
        point_4d(0) = (point(0) - viewport(0)) * 2.0 / viewport(2) - 1.0;
        point_4d(1) = (point(1) - viewport(1)) * 2.0 / viewport(3) - 1.0;
        point_4d(2) = point(2) * 2.0 - 1.0;
        point_4d(3) = 1.0;
        Eigen::Matrix4d inverse = (projection * view * model).inverse();
        Eigen::Vector4d unprojected_point = inverse * point_4d;
        unprojected_point /= unprojected_point(3);
        return {unprojected_point.x(), unprojected_point.y(), unprojected_point.z()};
    }

    /// 得到绕某一旋转轴旋转一定角度的旋转矩阵
    Eigen::Matrix3f Rotation(Eigen::Vector3f axis, float radian) {
        // Use Rodriguez rotation formula
        Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
        Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
        Eigen::Matrix3f M;
        Eigen::Matrix3f Rk;
        Rk << 0, -axis[2], axis[1],
                axis[2], 0, -axis[0],
                -axis[1], axis[0], 0;

        M = I + (1 - cos(radian)) * Rk * Rk + sin(radian) * Rk;

        return M;
    }
}

#endif //INC_3D_WARP_OPENGL_MATH_H
