#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <string>
#include "Vec.h"

class Reflectance {
public:
    Vec3f ka;//������ϵ��
    Vec3f kd;//������ϵ��
    Vec3f ks;//���淴��ϵ��
};

class Material
{
public:

    Material() :shiness(60), refractiveIndex(1.0), bUseFresnel(false){};

    std::string name;//��������
    Color3f emission;//����

    double shiness;//�����Ns
    double refractiveIndex;//����ϵ��Ni
    Vec3f tf;//transmission filter

    bool bUseFresnel;//Ĭ��Ϊ����Fresnel
    Reflectance ref;//����ϵ��
};

#endif