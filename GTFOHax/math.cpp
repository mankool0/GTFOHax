#include "math.h"

namespace Math
{
    float DotProduct(app::Vector3 first, app::Vector3 second)
    {
        return (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
    }
    
    app::Vector3 Vector3Sub(app::Vector3 lhs, app::Vector3 rhs)
    {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }

    bool Vector3Eq(app::Vector3 lhs, app::Vector3 rhs)
    {
        return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z));
    }

    app::Matrix4x4 MatrixMult(app::Matrix4x4 lhs, app::Matrix4x4 rhs)
    {
        app::Matrix4x4 matrix4x4;
        matrix4x4.m00 = (lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30);
        matrix4x4.m01 = (lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31);
        matrix4x4.m02 = (lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32);
        matrix4x4.m03 = (lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33);
        matrix4x4.m10 = (lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30);
        matrix4x4.m11 = (lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31);
        matrix4x4.m12 = (lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32);
        matrix4x4.m13 = (lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33);
        matrix4x4.m20 = (lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30);
        matrix4x4.m21 = (lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31);
        matrix4x4.m22 = (lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32);
        matrix4x4.m23 = (lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33);
        matrix4x4.m30 = (lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30);
        matrix4x4.m31 = (lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31);
        matrix4x4.m32 = (lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32);
        matrix4x4.m33 = (lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33);
        return matrix4x4;
    }

    bool WorldToScreen(app::Vector3& worldPos, ImVec2& screenPos)
    {
        auto matrix = G::viewMatrix;
        app::Vector3 screenPosTemp;

        screenPosTemp.z = (matrix.m20 * worldPos.x + matrix.m21 * worldPos.y + matrix.m22 * worldPos.z) + matrix.m23;
        if (screenPosTemp.z < 0.001f)
            return false;


        float num = 1.0f / ((matrix.m30 * worldPos.x + matrix.m31 * worldPos.y + matrix.m32 * worldPos.z) + matrix.m33);
        screenPosTemp.x = (matrix.m00 * worldPos.x + matrix.m01 * worldPos.y + matrix.m02 * worldPos.z) + matrix.m03;
        screenPosTemp.y = (matrix.m10 * worldPos.x + matrix.m11 * worldPos.y + matrix.m12 * worldPos.z) + matrix.m13;
        screenPosTemp.x *= num;
        screenPosTemp.y *= num;

        screenPosTemp.x = ((screenPosTemp.x + 1.0f) * 0.5f) * G::screenWidth;
        screenPosTemp.y = ((screenPosTemp.y + 1.0f) * 0.5f) * G::screenHeight;


        screenPos.x = screenPosTemp.x;
        screenPos.y = G::screenHeight - screenPosTemp.y;

        //if (screenPos.x < 0 || screenPos.y < 0 || screenPos.x > G::screenWidth || screenPos.y > G::screenHeight)
        //    return false;

        return true;
    }

}

