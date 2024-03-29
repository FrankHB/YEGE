﻿#ifndef Inc_ege_v3d_h_
#define Inc_ege_v3d_h_

#include "ege/def.h"

namespace ege
{

struct VECTOR3D
{
	float x, y, z;

	VECTOR3D()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	VECTOR3D(float _x, float _y)
	{
		x = _x;
		y = _y;
		z = 0;
	}
	VECTOR3D(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	VECTOR3D(const VECTOR3D&) = default;

	VECTOR3D&
	operator=(const VECTOR3D& _fp)
	{
		x = _fp.x;
		y = _fp.y;
		z = _fp.z;
		return *this;
	}

	VECTOR3D&
	operator+=(const VECTOR3D& _fp);
	VECTOR3D&
	operator-=(const VECTOR3D& _fp);
	VECTOR3D
	operator+(const VECTOR3D& _fp) const;
	VECTOR3D
	operator-(const VECTOR3D& _fp) const;
	VECTOR3D&
	operator*=(float f); //缩放
	VECTOR3D
	operator*(float f) const; //缩放

	float
	operator*(const VECTOR3D& _fp) const; //点乘

	VECTOR3D
	operator&(const VECTOR3D& _fp) const; //叉乘

	VECTOR3D&
	operator&=(const VECTOR3D& _fp); //叉乘
	float
	GetModule() const;
	float
	GetSqrModule() const
	{
		return float(x * x + y * y + z * z);
	}

	VECTOR3D&
	SetModule(float m)
	{
		float t = m / GetModule();
		*this *= t;
		return *this;
	}

	VECTOR3D&
	Rotate(float rad, const VECTOR3D& v); //绕任意轴旋转，右手定则，rad为弧度
	VECTOR3D&
	Rotate(float rad, float _x, float _y, float _z)
	{
		VECTOR3D v(_x, _y, _z);

		return Rotate(rad, v);
	}
	VECTOR3D&
	Rotate(const VECTOR3D& e, const VECTOR3D& s = VECTOR3D(0.0f, 0.0f, 1.0f)); //从s到e之间的夹角确定旋转

	static float
	GetAngel(const VECTOR3D& e, const VECTOR3D& s = VECTOR3D(0.0f, 0.0f, 1.0f));
};


// 3d 计算辅助函数
EGEAPI void
rotate_point3d_x(VECTOR3D* pt, float r); //弧度，右手定则

EGEAPI void
rotate_point3d_y(VECTOR3D* pt, float r);

EGEAPI void
rotate_point3d_z(VECTOR3D* pt, float r);

}

#endif

