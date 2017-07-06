#ifndef __RAY_H
#define __RAY_H

#include "Vec.h"
#include "utils.h"
enum SOURCE{ NONE, DIRECT, DIFFUSE_REFLECT, SPECULAR_REFLECT, TRANSMISSON };

class Ray
{
public:
    Point3f origin;
    Vec3f direction;
    SOURCE source;
    Vec3f inverse_direction;//ȡ������ֻ����ֵ���壬û���������壬���������ʹ�õ����������ظ�����
    float tmin,tmax;

    float IOR;//������

    Ray(Point3f _origin = Point3f(0.0, 0.0, 0.0),
        Vec3f _direction = Vec3f(0.0, 0.0, 0.0),
        SOURCE _source = SOURCE::NONE,
        float _ior = 1.0)
    {
        origin = _origin;
        direction = normalize(_direction);
        source = _source;
        IOR = _ior;
        tmin = EPS;
        tmax = FLT_MAX;

        inverse_direction = 1.0 / direction;
    }

/*
    Point3f getPoint(float t){ return origin + direction*t; }
    inline Vec3f getInverseDirection() { return inverse_direction; }

    //���Լӿ��Ƿ��н�����ж�
    inline float getLowerBound() { return tmin; }
    inline float getUpperBound() { return tmax; }
    inline bool isWithinBounds(float tVal) { return tVal <= tmax && tVal >= tmin; }
    inline void setBounds(float min, float max) { tmin = min; tmax = max; }

    inline Vec3f reflect(const Vec3f& normal){ return direction - 2.0f * dot(normal, direction) * normal; }
    bool refract(const Vec3f& normal, float nit, Vec3f& refract_direction);
*/
    Point3f getPoint(float t);
    Vec3f getInverseDirection();

    //���Լӿ��Ƿ��н�����ж�
    float getLowerBound();
    float getUpperBound();
    bool isWithinBounds(float tVal);
    void setBounds(float min, float max);

    Vec3f reflect(const Vec3f& normal);
    bool refract(const Vec3f& normal, float nit, Vec3f& refract_direction);



};

#endif