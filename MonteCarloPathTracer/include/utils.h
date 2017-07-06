#ifndef __UTILS_H
#define __UTILS_H

#include<cmath>
#include <io.h>
#include <string>
#include <vector>
#include <iostream>
#include <climits>

//����������ڿ���2������֮����С�ľ��룬����������С��EPS����ô��Ϊ��2����Ϊͬһ���㡣
//��������㲻�ټ�������ݹ飨EPS�������õ�̫С�����򣬻���ֺܶవ��/���㣬
//����EPS = 1e-7f�ͻ������������
//ԭ��������ǲ���ȷ�ģ������㼯����һ����С�������ڣ�
//����Ϊʲô��䰵(����ԭ��)
const float EPS = 1e-5f;//ʮ����Ҫ������

const float PI = 3.1415926f;

bool isEqualf(float a, float b);

std::vector<std::string> getListFiles(std::string path, std::string suffix);


#endif