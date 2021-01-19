//
// Created by YoUr_IcOn on 06.01.2021.
//

#ifndef LABS_LLTCALCULATOR_H
#define LABS_LLTCALCULATOR_H

#include "matrix.h"

class LLTCalculator{
private:
    Matrix* matrix;
    double* vector;
    double* vector_x;
    double* vector_y;
    Matrix* matrix_L;
public:
    LLTCalculator(Matrix* , double* );
    void factorization();
    void resolveY();
    void resolveX();
    double* solve();
    ~LLTCalculator();
};

#endif //LABS_LLTCALCULATOR_H
