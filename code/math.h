#ifndef MATH_H
#define MATH_H

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

struct mat4
{
    r32 m[4][4];
};

r32 ToRad(r32 Angle);
r32 ToDeg(r32 Angle);
r32 NormalizeAngle(r32 Angle);

v2 operator+(v2 A, v3 B);
v2 operator-(v2 A, v3 B);
v2 operator+(v2 A, r32 S);
v2 operator-(v2 A, r32 S);
v2 operator*(v2 A, r32 S);

v3 operator+(v3 A, v3 B);
v3 operator-(v3 A, v3 B);
v3 operator+(v3 A, r32 S);
v3 operator-(v3 A, r32 S);
v3 operator*(v3 A, r32 S);

r32 LengthV3(v3 V);
v3 NormalizeV3(v3 V);
r32 DotV3(v3 A, v3 B);
v3 CrossV3(v3 A, v3 B);

mat4 IdentityMat4();
mat4 TranslationMat4(v3 V);
mat4 ScaleMat4(v3 V);
mat4 RotationXMat(r32 Angle);
mat4 RotationYMat(r32 Angle);
mat4 RotationZMat(r32 Angle);
mat4 ViewMat4(v3 Eye, v3 Target, v3 Up);
mat4 PerspectiveProjMat4(r32 YFov, r32 Aspect, r32 ZNear, r32 ZFar);
mat4 OrthogonalProjMat4(i32 Width, i32 Height, r32 ZNear, r32 ZFar);

#endif
