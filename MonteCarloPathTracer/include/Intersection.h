#ifndef __INTERSECTION_H
#define __INTERSECTION_H

#include "Geometry.h"
#include "Vec.h"
class Geometry;

class Intersection
{
public:
    Geometry* pGeometry;//���ڼ�¼�������ڼ����壬����������Ƭ
    Point3f point;//����
    Vec3f normal;//��������λ�õķ���

    Intersection() :pGeometry(NULL){};
};

#endif