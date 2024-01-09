#pragma once
#include "globals.h"

#define M_PI         3.14159265358979323846
#define K_ANGRAD     (M_PI / 180)
#define K_RADANG     (180 / M_PI)
#define RAD_ANG(rad) ((rad) * K_RADANG)
#define ANG_RAD(ang) ((ang) * K_ANGRAD)

namespace Math
{
    app::Vector3 Vector3Sub(app::Vector3 lhs, app::Vector3 rhs);
    bool Vector3Eq(app::Vector3 lhs, app::Vector3 rhs);
    app::Matrix4x4 MatrixMult(app::Matrix4x4 lhs, app::Matrix4x4 rhs);
    bool WorldToScreen(app::Vector3& worldPos, ImVec2& screenPos);
}