#ifndef __GEOMETRY_H
#define __GEOMETRY_H

#include "Intersection.h"
#include "AABB.h"
#include "Ray.h"
#include "Material.h"

class Intersection;
class AABB;

//���ּ�����״�Ļ��࣬��������ת��
class Geometry
{
public:
    Material material;

    //�õ���Χ��
    virtual AABB getAABB() = 0;

    //������Ray�Ľ���
    virtual bool intersect(Ray& ray, Intersection& intersection) = 0;

    //����ray�ͽ���֮���Ƿ����ڵ�
    virtual bool shadowRayIntersect(Ray& ray, Intersection& intersection)
    {
        return intersect(ray, intersection);
    }

};

#endif