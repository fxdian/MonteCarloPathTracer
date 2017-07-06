#ifndef __SCENE_H
#define __SCENE_H

#include "Geometry.h"
#include "Light.h"
#include "Camera.h"
#include "KdTree.h"
#include "Model.h"

using std::vector;

class Light;

class Scene
{
public:
    vector<Geometry*> pGeometrys;//��Ҫ�ⲿ����model
    KdTree kdTree;
    vector<Light> lights;//��Դ��Ҫ������������,����ֱ�ӹ���
    Camera* pCamera;
    float* colors;//�������ڵ�color����RayTracer�м���
    int width, height;

    Scene();
    ~Scene();
    int getWidth(){ return width; }
    int getHeight(){ return height; }
    void setSize(int _width, int _height);

    void init();
    Ray* getRays(float x, float y, int pxSampleNum);
    bool intersect(Ray& ray, Intersection& intersection);
    bool isInShadow(Ray& ray);//�ж�ֱ�ӹ��գ��ڽ���͹�Դ��֮���Ƿ����ڵ�
    Color3f directIllumination(Intersection& intersection, Ray& ray);

};

#endif