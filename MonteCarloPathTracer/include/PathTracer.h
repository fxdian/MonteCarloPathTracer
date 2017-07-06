#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "Vec.h"
#include "Ray.h"
#include "Scene.h"
#include "Intersection.h"

class PathTracer
{
private:
    Scene* pScene;
    int iterations;
    int px_sample_num;//ÿ�����صĲ�����Ŀ
    int mc_sample_num;//���ؿ��������Ŀ
    int max_rescursive_depth;//�������ݹ����
    bool use_direct_light;//�Ƿ�ʹ��ֱ�ӹ���
    Color3f ambient; //��������

    //·�����٣�����ray������color
    Color3f trace(Ray& ray, int cur_depth = 0);

    // Monte Carlo Sampling��������һ��Ray
    Ray monteCarloSample(Ray& ray, Intersection& intersection);

    // ����˹����
    bool russianRoulette(float probability, float& survivor);

    // cosine-weighted importance sampling��ʹ��Monte Carlo Integral�Ⱦ��Ȳ�����������һЩ
    // �����ϰ������ray����
    Vec3f importanceSample(Vec3f up, float n = -1);


public:

    PathTracer(Scene* _scene);
    float* render();
    Scene* getScene();
};

#endif