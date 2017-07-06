#include "AABB.h"
#include <algorithm>

#include "Vec.h"

using namespace std;

AABB AABB::merge(const AABB& box1, const AABB& box2)
{
    Point3f pt1, pt2;

    pt1.x = min(box1.low.x, box2.low.x);
    pt1.y = min(box1.low.y, box2.low.y);
    pt1.z = min(box1.low.z, box2.low.z);

    pt2.x = max(box1.high.x, box2.high.x);
    pt2.y = max(box1.high.y, box2.high.y);
    pt2.z = max(box1.high.z, box2.high.z);

    return AABB(pt1, pt2);
}

//��Ҫע�⸡�������������ڵ�
bool AABB::intersect(Ray& ray, Intersection& result)
{
    Vec3f& inverseDirection = ray.getInverseDirection();

    float t1 = (low.x - ray.origin.x)*inverseDirection.x;//��box��yozƽ���ཻ��x��С
    float t2 = (high.x - ray.origin.x)*inverseDirection.x;

    float t3 = (low.y - ray.origin.y)*inverseDirection.y;//��box��xozƽ���ཻ��y��С
    float t4 = (high.y - ray.origin.y)*inverseDirection.y;

    float t5 = (low.z - ray.origin.z)*inverseDirection.z;//��box��xoyƽ���ཻ��z��С
    float t6 = (high.z - ray.origin.z)*inverseDirection.z;

    //������box��ǰ��2������ľ���t
    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
    if (tmax < 0) return false;

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) return false;

    //�������ܹؼ���������Ϊû�н����t̫��Ҳ������Ϊ���������Լ��ཻ��̫С
    return (tmin < ray.getUpperBound() && tmax > ray.getLowerBound());
    //return true;
}