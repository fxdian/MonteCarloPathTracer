#include "Model.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include "utils.h"

using namespace std;

static const Vec3f BLACK = Vec3f(0.0f, 0.0f, 0.0f);

//static int call_count = 0;

Triangle::Triangle(Model* _pmodel, int _vertex_idx[3], int _vertex_normal_idx[3])
{
    pmodel = _pmodel;
    memcpy(vertex_idx, _vertex_idx, sizeof(_vertex_idx) * 3);
    memcpy(vertex_normal_idx, _vertex_normal_idx, sizeof(_vertex_normal_idx) * 3);
}


void Triangle::init()
{
    Point3f& pt0 = pmodel->vertices[vertex_idx[0]],
        pt1 = pmodel->vertices[vertex_idx[1]],
        pt2 = pmodel->vertices[vertex_idx[2]];

    origin = pt0;
    edge1 = pt1 - pt0;
    edge2 = pt2 - pt0;
    normal = normalize(cross(edge1, edge2));

    barycentric = Mat4(pt0, pt1, pt2);
    barycentric.inverse();
}

AABB Triangle::getAABB()
{
    Point3f low, high;
    Point3f& pt0 = pmodel->vertices[vertex_idx[0]],
        pt1 = pmodel->vertices[vertex_idx[1]],
        pt2 = pmodel->vertices[vertex_idx[2]];

    low.x = min(pt0.x, min(pt1.x, pt2.x));
    low.y = min(pt0.y, min(pt1.y, pt2.y));
    low.z = min(pt0.z, min(pt1.z, pt2.z));

    high.x = max(pt0.x, max(pt1.x, pt2.x));
    high.y = max(pt0.y, max(pt1.y, pt2.y));
    high.z = max(pt0.z, max(pt1.z, pt2.z));

    return AABB(low, high);
}

Vec3f Triangle::getNormal(const Vec3f& point)//������Ƭ�ϵļ�Ȩƽ������ƽ����������
{
    //call_count++;
    if (vertex_normal_idx[0] == -1 || vertex_normal_idx[1] == -1 || vertex_normal_idx[2] == -1)
    {
        //cout << "����������!" << endl;
        //�������������sportsCar.obj�ļ�vertex˳�������⣨���·��߷��򲻶ԣ��żӵ�
        if (material.name == "WindowGlassMat" && normal.y < 0)
        {
            normal = -normal;
        }
        return normal;
    }

    Vec3f abg = barycentric * point;

    //��������������ϵ��abg�в��ܳ��ָ���ֵ������������������棩����֪��������bug
    abg.x = abs(abg.x);
    abg.y = abs(abg.y);
    abg.z = abs(abg.z);

    /*return normalize(
        abg.x*(pmodel->normals[vertex_normal_idx[0]]) +
        abg.y*(pmodel->normals[vertex_normal_idx[1]]) +
        abg.z*(pmodel->normals[vertex_normal_idx[2]]));*/
    /*return normalize(
        pmodel->normals[vertex_normal_idx[0]] +
        pmodel->normals[vertex_normal_idx[1]] +
        pmodel->normals[vertex_normal_idx[2]]);*/
    Vec3f return_normal = normalize(
        abg.x*(pmodel->normals[vertex_normal_idx[0]]) +
        abg.y*(pmodel->normals[vertex_normal_idx[1]]) +
        abg.z*(pmodel->normals[vertex_normal_idx[2]]));

    return return_normal;
}


//����������Ƭ�Ľ���
bool Triangle::intersect(Ray& ray, Intersection& intersection)
{
    //ray��������淨������ֱ������Ϊ���ཻ
    if (abs(dot(normal,ray.direction))<EPS)
    {
        return false;
    }

    Vec3f v = origin - ray.origin;

    //����������Ƭ�е�����(alpha=1-beta-gamma, beta, gamma)
    Vec3f tmp1 = cross(edge2, ray.direction);
    float beta = -dot(tmp1, v) / dot(tmp1, edge1);

    Vec3f tmp2 = cross(edge1, ray.direction);
    float gamma = -dot(tmp2, v) / dot(tmp2, edge2);

    float t = dot(normal, v) / dot(normal, ray.direction);

    //������Ƭ�ϵ�������������Ƭ�ڲ������н���
    if (beta > 0 && gamma > 0 && beta + gamma < 1 && ray.isWithinBounds(t))
    {
        //if (t < 1e-3)t = 0.0f;
        intersection.point = ray.getPoint(t);
        //intersection.normal = -normal;//�򻯣���Ϊ��ƽ��
        intersection.normal = getNormal(intersection.point);
        intersection.pGeometry = this;
        ray.tmax = t;
        return true;
    }

    return false;
}

static bool load_mtl(string path, map<string, Material>& material_table)
{
    ifstream file(path);
    if (!file.is_open())
    {
        cerr << "mtl�ļ�" + path + "�޷���" << endl;
        return false;
    }

    bool flag = false;
    string material_name;
    Material material;

    string type;
    int illumination_model;


    while (file >> type)
    {
        if (type == "newmtl")
        {
            if (flag)
            {
                material_table.insert(make_pair(material_name, material));
                material = Material();
            }
            else flag = true;

            file >> material_name;
            material.name = material_name;
        }
        else if (type == "Kd")//duffuse ����
        {
            file >> material.ref.kd.x >> material.ref.kd.y >> material.ref.kd.z;
        }
        else if (type == "Ka")//ambient ����
        {
            file >> material.ref.ka.x >> material.ref.ka.y >> material.ref.ka.z;
        }
        else if (type == "Ks")//specular ����
        {
            file >> material.ref.ks.x >> material.ref.ks.y >> material.ref.ks.z;
        }
        else if (type == "Ke")//emissive ����
        {
            file >> material.emission.r >> material.emission.g >> material.emission.b;
        }
        else if (type == "Ns")// shininess �����
        {
            file >> material.shiness;
        }
        else if (type == "Ni")//����ϵ�������ʵĹ��ܶȣ�optical density��
        {
            file >> material.refractiveIndex;
        }
        else if (type == "Tf")//͸���˲���transmission filter������Ӧ����Ϊr��g��b��a��
        {
            file >> material.tf.x >> material.tf.y >> material.tf.z;
        }
        else if (type == "illum")//�����ȣ�illumination��
        {
            file >> illumination_model;
            material.bUseFresnel = true;
/*
            if (illumination_model == 5 || illumination_model == 7)
                material.bUseFresnel = true;*/
        }
        else file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }

    if (flag) material_table.insert(make_pair(material_name, material));//�������һ�ֲ���

    return true;
}

bool Model::load(const string obj_path)
{
    ifstream file(obj_path);
    if (!file.is_open())
    {
        cerr << "Obj�ļ�" + obj_path << "�޷���" << endl;
        return false;
    }


    bool return_value = true;
    map<string, Material> material_table;

    string type;
    Point3f v;
    //Vertex v;
    Vec3f vn;
    string material_name;
    Material material;
    int vertex_idx[3];
    int vertex_normal_idx[3];

    while (file >> type)
    {
        if (type == "mtllib")//��mtl�ļ����ص�material_table��
        {
            string mtl_file_name;
            file >> mtl_file_name;

            int pos = obj_path.find_last_of('/');
            string mtl_path = obj_path.substr(0, pos + 1) + mtl_file_name;

            if (!load_mtl(mtl_path, material_table))
            {
                return_value = false;
                cerr << "mtl�ļ�" + mtl_path + "����ʧ��" << endl;
                break;
            }
        }
        else if (type == "usemtl")//ȡ��material_table�ж�Ӧ��material����Ӧ��Face
        {
            file >> material_name;

            map<string, Material>::const_iterator it = material_table.find(material_name);

            if (it != material_table.end())//���ҵõ���Ӧ��Material
            {
                material = it->second;
            }
            else
            {
                cerr << "����" + material_name + "���ڶ�Ӧ��mtl�ļ���" << endl;
                return_value = false;
                break;
            }
        }
        else if (type == "v")
        {
            file >> v.x >> v.y >> v.z;
            //��bbox
            if (bbox.low.x > v.x)bbox.low.x = v.x;
            if (bbox.low.y > v.y)bbox.low.y = v.y;
            if (bbox.low.z > v.z)bbox.low.z = v.z;
            if (bbox.high.x < v.x)bbox.high.x = v.x;
            if (bbox.high.y < v.y)bbox.high.y = v.y;
            if (bbox.high.z < v.z)bbox.high.z = v.z;

            vertices.push_back(v);
        }
        else if (type == "vn")
        {
            file >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        }
        //������-��������/��������/��������
        else if (type == "f")
        {
            int index = 0;
            memset(vertex_idx, -1, sizeof(vertex_idx));
            memset(vertex_normal_idx, -1, sizeof(vertex_normal_idx));

            while (true)
            {
                char ch = file.get();
                if (ch == ' ') continue;
                else if (ch == '\n' || ch == EOF) break;
                else file.putback(ch);

                file >> vertex_idx[index];//���������
                --vertex_idx[index];

                char splitter = file.get();

                if (splitter == '/')
                {
                    splitter = file.get();
                    if (splitter == '/')
                    {
                        file >> vertex_normal_idx[index];//�淨������index
                        --vertex_normal_idx[index];
                    }
                    else
                    {
                        int tIndex;//����
                        file.putback(splitter);
                        file >> tIndex;//����(texture)����
                        splitter = file.get();
                        if (splitter == '/')
                        {
                            file >> vertex_normal_idx[index];
                            --vertex_normal_idx[index];
                        }
                        else file.putback(splitter);
                    }
                }
                else file.putback(splitter);

                ++index;
                if (index >= 3)//������Σ��ֽ��������
                {
                    Triangle* tri = new Triangle(this, vertex_idx, vertex_normal_idx);
                    tri->material = material;
                    tri->init();
                    pTriangles.push_back(tri);

                    //�ҳ����еĹ�Դ
                    if (material.emission != BLACK)
                    {
                        lights.push_back(Light(tri->origin, tri->edge1, tri->edge2, material.emission));
                    }

                    vertex_idx[1] = vertex_idx[2];
                    vertex_normal_idx[1] = vertex_normal_idx[2];
                    index = 2;
                }
            }
           
        }
    }
    file.close();
    return return_value;
}

Model::Model(const std::string obj_path)
{
    bbox = AABB(Point3f(FLT_MAX, FLT_MAX, FLT_MAX), Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    if (load(obj_path))
    {
        cout << "ģ��" + obj_path + "���سɹ�" << endl;
    }
    else
    {
        cerr << "ģ��" + obj_path + "����ʧ��" << endl;
    }

    init();
}

Model::~Model()
{
    for (int i = 0, len = pTriangles.size(); i < len; ++i)
    {
        if (pTriangles[i] != NULL)
        {
            delete pTriangles[i];
        }
    }
}

void Model::init()
{
    kdTree.build(pTriangles);
}

AABB Model::getAABB()
{
    return kdTree.getAABB();
}

bool Model::intersect(Ray& ray, Intersection& intersection)
{
    return kdTree.intersect(ray, intersection);
}

bool Model::shadowRayIntersect(Ray& ray, Intersection& intersection)
{
    return kdTree.shadowRayIntersect(ray, intersection);
}

