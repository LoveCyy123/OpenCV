#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

namespace OpenCV{

class Matrix{
    https://blog.csdn.net/xholes/article/details/79966211

    https://blog.csdn.net/nameofcsdn/article/details/120968236?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-120968236-blog-79966211.pc_relevant_3mothn_strategy_and_data_recovery&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-120968236-blog-79966211.pc_relevant_3mothn_strategy_and_data_recovery&utm_relevant_index=2
public:
    Matrix::Matrix();

    //指定类型的构造函数
    Matrix::Matrix(int rows, int cols, int type);

    //指定类型和初始化值的构造函数
    Matrix::Matrix(int rows, int cols, int type, const Scalar&s);   

    //指定类型的二维数组，并且将具体的数据储存其中
    Matrix::Matrix(int rows, int cols, int type, void* data, size_t step = AUTO_STEP);


private:




};

}

#endif