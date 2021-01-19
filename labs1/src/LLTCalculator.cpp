//
// Created by YoUr_IcOn on 06.01.2021.
//
#include "headers/LLTCalculator.h"
#include <cmath>

LLTCalculator::LLTCalculator(Matrix * _matrix, double * _vector_coeff) {
    this->matrix = new Matrix(_matrix);
    this->vector = _vector_coeff;
    this->vector_x = new double [_matrix->get_row()];
    this->vector_y = new double [_matrix->get_row()];
}
LLTCalculator::~LLTCalculator() {
    delete matrix;
    delete [] vector;
}

void LLTCalculator::factorization() {
    matrix_L = new Matrix(matrix->get_row(), matrix->get_column());

    for (int i = 0; i < matrix->get_row(); i++)
    {
        double s = matrix->get(i, i);
        for (int k = 0; k < i; k++)
        {
            s -= std::pow(matrix_L->get(i, k), 2);
        }
        matrix_L->get(i, i) = std::sqrt(s);
        for (int j = i + 1; j < matrix->get_row(); j++)
        {
            s = matrix->get(i, j);
            for (int k = 0; k < i; k++)
                s -= matrix_L->get(i, k) * matrix_L->get(j, k);
            matrix_L->get(j, i) = s / matrix_L->get(i, i);
        }
    }
    /**
    std::cout << "-----------------";
    std::cout << matrix->get_row();
    std::cout << "\n";
    for(int i = 0; i < matrix_L->get_row(); i++){
        for(int j = 0; j < matrix_L->get_column(); j++){
            std::cout << matrix_L->get(i, j);
            std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "-----------------";
    std::cout << "\n";
    **/
}

void LLTCalculator::resolveY() {
    double* result = new double[matrix_L->get_row()];
    for (int i = 0; i < matrix_L->get_row(); ++i)
    {
        result[i] = vector[i];
        for (int j = 0; j < i; ++j)
        {
            result[i] -= matrix_L->get(i, j) * result[j];
        }
        result[i] /= matrix_L->get(i ,i);
    }
    vector_y = result;
}

void LLTCalculator::resolveX() {
    double* result = new double[matrix->get_row()];
    for (int i = matrix->get_row() - 1; i >= 0; --i)
    {
        result[i] = vector_y[i];
        for (int j = matrix->get_row() - 1; j > i; --j)
        {
            result[i] -= matrix_L->get(j, i) * result[j];
        }
        result[i] /= matrix_L->get(i, i);
    }
    vector_x = result;
    /**
    std::cout << "-----------------";
    std::cout << "\n";
    for(int i = 0; i < matrix_L->get_row(); i++){
        std::cout << vector_x[i];
        std::cout << " ";
    }
    std::cout << "-----------------";
    std::cout << "\n";
    **/
}

double* LLTCalculator::solve() {
    factorization();
    resolveY();
    resolveX();
    return vector_x;
}