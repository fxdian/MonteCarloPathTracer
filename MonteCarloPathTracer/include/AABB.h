#ifndef __AABB_H
#define __AABB_H

#include "Ray.h"
#include "Intersection.h"
#include "Vec.h"

class Intersection;//ΪʲôҪ�����󣬱������Ų�����(Error 47 error C2061: syntax error : identifier Intersection)

// Aix-Align Bounding Box
class AABB
{
public:
    Point3f low, high;//AABB��2���Խ�����㣬�Ա�ʾAABB

    AABB(){};
    AABB(Point3f _low, Point3f _high) :low(_low), high(_high){};
    
    //�ϲ�2��AABB
    static AABB merge(const AABB& bbox1, const AABB& bbox2);

    //�õ���Ӧ�������ϵ���С����
    inline float getMinCoord(int axis){ return low.num[axis]; }
    inline float getMaxCoord(int axis){ return high.num[axis]; }

    //����ray��AABB����
    bool intersect(Ray& ray, Intersection& intersection);


};

#endif