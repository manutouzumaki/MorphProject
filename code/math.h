#ifndef MATH_H
#define MATH_H

#include <math.h>
#define PI 3.14159265
#define TAU 6.28318530

struct v2 
{
    r32 X, Y;    
};

struct v3
{

    r32 X, Y, Z;    
};

struct v4
{
    r32 X, Y, Z, W;
};

struct mat4
{
    r32 m[4][4];
};

r32 ToRad(r32 Angle)
{
    return Angle * (PI/180.0f);
}

r32 ToDeg(r32 Angle)
{
    return Angle * (180.0f/PI);
}

r32 NormalizeAngle(r32 Angle)
{
    Angle = fmodf(Angle, 2.0f*PI);
    if(Angle < 0)
    {
        Angle = (2 * PI) + Angle;
    }
    return Angle;
}


v2 operator+(v2 A, v2 B)
{
    v2 Result = {};
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return Result;
}

v2 operator-(v2 A, v2 B)
{
    v2 Result = {};
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return Result;
}

v2 operator*(v2 A, v2 B)
{
    v2 Result = {};
    Result.X = A.X * B.X;
    Result.Y = A.Y * B.Y;
    return Result;
}

v2 operator/(v2 A, v2 B)
{
    v2 Result = {};
    Result.X = A.X / B.X;
    Result.Y = A.Y / B.Y;
    return Result;
}

v2 operator+(v2 A, r32 S)
{
    v2 Result = {};
    Result.X = A.X + S;
    Result.Y = A.Y + S;
    return Result;
}

v2 operator-(v2 A, r32 S)
{    
    v2 Result = {};
    Result.X = A.X - S;
    Result.Y = A.Y - S;
    return Result;
}

v2 operator*(v2 A, r32 S)
{
    v2 Result = {};
    Result.X = A.X * S;
    Result.Y = A.Y * S;
    return Result;
}

v3 operator+(v3 A, v3 B)
{
    v3 Result = {};
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return Result;
}

v3 operator-(v3 A, v3 B)
{
    v3 Result = {};
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return Result;
}

v3 operator+(v3 A, r32 S)
{
    v3 Result = {};
    Result.X = A.X + S;
    Result.Y = A.Y + S;
    Result.Z = A.Z + S;
    return Result;
}

v3 operator-(v3 A, r32 S)
{    
    v3 Result = {};
    Result.X = A.X - S;
    Result.Y = A.Y - S;
    Result.Z = A.Z - S;
    return Result;
}

v3 operator*(v3 A, r32 S)
{
    v3 Result = {};
    Result.X = A.X * S;
    Result.Y = A.Y * S;
    Result.Z = A.Z * S;
    return Result;
}

v4 operator*(mat4& M, v4& V)
{
    v4 Result = {};
    Result.X = M.m[0][0] * V.X + M.m[0][1] * V.Y + M.m[0][2] * V.Z + M.m[0][3] * V.W;
    Result.Y = M.m[1][0] * V.X + M.m[1][1] * V.Y + M.m[1][2] * V.Z + M.m[1][3] * V.W;
    Result.Z = M.m[2][0] * V.X + M.m[2][1] * V.Y + M.m[2][2] * V.Z + M.m[2][3] * V.W;
    Result.W = M.m[3][0] * V.X + M.m[3][1] * V.Y + M.m[3][2] * V.Z + M.m[3][3] * V.W;
    return Result;
}

mat4 operator*(mat4& A, mat4& B)
{
    mat4 Result;
    for(int Y = 0;
        Y < 4;
        ++Y)
    {
        for(int X = 0;
            X < 4;
            ++X)
        {
                Result.m[Y][X] =
                A.m[Y][0] * B.m[0][X] +
                A.m[Y][1] * B.m[1][X] +
                A.m[Y][2] * B.m[2][X] +
                A.m[Y][3] * B.m[3][X];
        }
    }
    return Result;
}

r32 DotV3(v3 A, v3 B)
{
    r32 Result = A.X*B.X + A.Y*B.Y + A.Z*B.Z;
    return Result;
}

r32 LengthV2(v2 V)
{
    return sqrtf(V.X*V.X + V.Y*V.Y);
}

r32 LengthV3(v3 V)
{
    return sqrtf(DotV3(V, V));
}

v2 NormalizeV2(v2 V)
{
    v2 Result = {};
    r32 Length = LengthV2(V);
    Result.X = V.X / Length;
    Result.Y = V.Y / Length;
    return Result;
}

v3 NormalizeV3(v3 V)
{
    v3 Result = {};
    r32 Length = LengthV3(V);
    Result.X = V.X / Length;
    Result.Y = V.Y / Length;
    Result.Z = V.Z / Length;
    return Result;
}

v3 CrossV3(v3 A, v3 B)
{
    v3 Result = {};
    Result.X = A.Y*B.Z - A.Z*B.Y; 
    Result.Y = A.Z*B.X - A.X*B.Z;
    Result.Z = A.X*B.Y - A.Y*B.X;
    return Result;
}

mat4 IdentityMat4()
{
    mat4 Result = {{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return Result;
}

mat4 TranslationMat4(v3 V)
{
    mat4 Result = {{
        {1.0f, 0.0f, 0.0f, V.X},
        {0.0f, 1.0f, 0.0f, V.Y},
        {0.0f, 0.0f, 1.0f, V.Z},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return Result;
}

mat4 ScaleMat4(v3 V)
{
    mat4 Result = {{
        {V.X,  0.0f, 0.0f, 0.0f},
        {0.0f, V.Y,  0.0f, 0.0f},
        {0.0f, 0.0f, V.Z,  0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return Result;
}

mat4 RotationXMat(r32 Angle)
{
    mat4 Result = {{
        {1.0f,        0.0f,         0.0f, 0.0f},
        {0.0f, cosf(Angle), sinf(Angle), 0.0f},
        {0.0f, -sinf(Angle),  cosf(Angle), 0.0f},
        {0.0f,        0.0f,         0.0f, 1.0f}
    }};
    return Result;
}

mat4 RotationYMat(r32 Angle)
{

    mat4 Result = {{
        {cosf(Angle), 0.0f, -sinf(Angle), 0.0f},
        {       0.0f, 1.0f,         0.0f, 0.0f},
        {sinf(Angle), 0.0f,  cosf(Angle), 0.0f},
        {       0.0f, 0.0f,         0.0f, 1.0f}
    }};
    return Result;
}

mat4 RotationZMat(r32 Angle)
{
    mat4 Result = {{
        {cosf(Angle),   sinf(Angle), 0.0f, 0.0f},
        {-sinf(Angle),  cosf(Angle), 0.0f, 0.0f},
        {       0.0f,         0.0f, 1.0f, 0.0f},
        {       0.0f,         0.0f, 0.0f, 1.0f}
    }};
    return Result;
}

mat4 ViewMat4(v3 Eye, v3 Target, v3 Up)
{
    v3 Z = NormalizeV3(Target - Eye);
    v3 X = NormalizeV3(CrossV3(Up, Z));
    v3 Y = CrossV3(Z, X);
    mat4 Result = {{
        {X.X,  X.Y,  X.Z, -DotV3(X, Eye)},
        {Y.X,  Y.Y,  Y.Z, -DotV3(Y, Eye)},
        {Z.X,  Z.Y,  Z.Z, -DotV3(Z, Eye)},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    return Result; 
}

mat4 PerspectiveProjMat4(r32 YFov, r32 Aspect, r32 ZNear, r32 ZFar)
{
    r32 YScale = 1.0f/tanf(YFov*0.5);
    r32 XScale = YScale / Aspect;
    mat4 Result = {{
        {XScale, 0.0f, 0.0f, 0.0f},
        {0.0f, YScale, 0.0f, 0.0f},
        {0.0f, 0.0f, ZFar/(ZFar - ZNear), -ZNear*ZFar/(ZFar - ZNear)},
        {0.0f, 0.0f, 1.0f, 0.0f}
    }};
    return Result;
}

mat4 OrthogonalProjMat4(i32 Width, i32 Height, r32 ZNear, r32 ZFar)
{
    r32 W = (r32)Width;
    r32 H = (r32)Height;
    mat4 Result = {{
        {2.0f / W, 0.0f,     0.0f,                   0.0f},
        {0.0f,     2.0f / H, 0.0f,                   0.0f},
        {0.0f,     0.0f,     1.0f / (ZFar - ZNear),  0.0f},
        {0.0f,     0.0f,     ZNear / (ZNear - ZFar), 1.0f}
    }};
    return Result;
}

r32 Lerp(r32 A, r32 B, r32 T)
{
    r32 Result = (A + (B - A) * T);
    return Result;
}

v2 Lerp(v2 A, v2 B, r32 T)
{
    v2 Result = {};
    Result = A + ((B - A) * T);
    return Result;
}

r32 InvLerp(r32 A, r32 B, r32 T)
{
    r32 Result = (T - A)/(B - A);
    return Result;
}

#endif

