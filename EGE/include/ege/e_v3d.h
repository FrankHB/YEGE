#pragma once

#include "e_def.h"

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
	VECTOR3D& operator = (const VECTOR3D& _fp)
	{
		x = _fp.x;
		y = _fp.y;
		z = _fp.z;
		return *this;
	}
	VECTOR3D& operator += (const VECTOR3D& _fp);
	VECTOR3D& operator -= (const VECTOR3D& _fp);
	VECTOR3D operator + (const VECTOR3D& _fp) const;
	VECTOR3D operator - (const VECTOR3D& _fp) const;
	VECTOR3D& operator *= (float f); //����
	VECTOR3D operator * (float f) const; //����
	float operator * (const VECTOR3D& _fp) const; //���
	VECTOR3D operator & (const VECTOR3D& _fp) const; //���
	VECTOR3D& operator &= (const VECTOR3D& _fp); //���
	float GetModule() const;
	float GetSqrModule() const
	{
		return float(x * x + y * y + z * z);
	}
	VECTOR3D& SetModule(float m)
	{
		float t = m / GetModule();
		*this *= t;
		return *this;
	}
	VECTOR3D& Rotate(float rad, const VECTOR3D& v); //����������ת�����ֶ���radΪ����
	VECTOR3D& Rotate(float rad, float x, float y, float z)
	{
		VECTOR3D v(x, y, z);
		return Rotate(rad, v);
	}
	VECTOR3D& Rotate(const VECTOR3D& e, const VECTOR3D& s = VECTOR3D(0.0f, 0.0f, 1.0f)); //��s��e֮��ļн�ȷ����ת
	static float GetAngel(const VECTOR3D& e, const VECTOR3D& s = VECTOR3D(0.0f, 0.0f, 1.0f));
};

// 3d ���㸨������
void EGEAPI rotate_point3d_x(VECTOR3D*   pt, float r); //���ȣ����ֶ���
void EGEAPI rotate_point3d_y(VECTOR3D*   pt, float r);
void EGEAPI rotate_point3d_z(VECTOR3D*   pt, float r);

}

