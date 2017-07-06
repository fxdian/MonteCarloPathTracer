#ifndef __MODEL_H
#define __MODEL_H

#include <vector>
#include <string>

#include "Vec.h"
#include "Material.h"
#include "AABB.h"
#include "Geometry.h"
#include "KdTree.h"
#include "Light.h"

/*
class Vertex
{
public:
    Point3f point;
    Vec3f normal;//���㷨����,��Model��normals�н��м����õ�
};*/
class Model;
class Light;

//������ηֽ��������Ƭ
class Triangle:public Geometry
{
public:
/*
    std::vector<int> vertex_idx;//��ɶ��������
    std::vector<int> vertex_normal_idx;//���㷨����index
*/
    //��Ҫʹ��vector����triangle��ָ���ʾʱ�����⣬�����Ǳ���ǰ�ͷ���
    int vertex_idx[3];
    int vertex_normal_idx[3];

    Model* pmodel;//������Ӧ��model

    Point3f origin;//������Ƭ��һ����
    Vec3f edge1, edge2;//������Ƭ��2����
    //Mat4 barycentric;//����

    Vec3f normal;//3���������õ�����Ƭ������

    Mat4 barycentric;

    Triangle(Model* _pmodel, int _vertex_idx[3], int _vertex_normal_idx[3]);

    void init();

    //�̳�Geometry
    AABB getAABB();
    bool intersect(Ray& ray, Intersection& intersection);

    //std::vector<int> vertex_texture_idx;
    Vec3f Triangle::getNormal(const Vec3f& point);
};

//���ڱ�ʾobj�ļ���ģ�ͣ�û�м�¼vt��Ϣ��������texture
class Model:public Geometry
{
public:
    //std::vector<Vertex> vertices;
    std::vector<Point3f> vertices;
    std::vector<Geometry*> pTriangles;
    std::vector<Light> lights;

    std::vector<Vec3f> normals;//����/�淨����

    //Point3f center;//Model����vertex�������С�����ƽ��
    AABB bbox;//Model�İ�Χ�У����ڼ���Camera�ӵ㡢�������ꡢ����Ȳ���
    KdTree kdTree;

    Model(){};
    ~Model();
    Model(const std::string obj_path);

    bool load(const std::string obj_path);
    void init();//��Triangle��֯��kdTree

    //�̳�Geometry
    AABB getAABB();
    bool intersect(Ray& ray, Intersection& intersection);
    bool shadowRayIntersect(Ray& ray, Intersection& intersection);
};

#endif