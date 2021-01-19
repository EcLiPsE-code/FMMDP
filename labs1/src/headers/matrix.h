//
// Created by YoUr_IcOn on 06.01.2021.
//

#ifndef LABS_MATRIX_H
#define LABS_MATRIX_H

#endif //LABS_MATRIX_H

#include <iostream>

class Matrix{
private:
    double **matrix;
    int row{};
    int col{};
public:
    Matrix(Matrix *matrix_copy){
        row = (*matrix_copy).row;
        col = (*matrix_copy).col;
        matrix = new double *[row];
        for(int i = 0; i < row; i++){
            matrix[i] = new double [col];
            for (int j = 0; j < col; j++){
                matrix[i][j] = (double ) (*matrix_copy).get(i, j);
            }
        }
    }

    Matrix(int row, int col){
        this->row = row;
        this->col = col;
        double** _matrix = new double* [row];
        for (int i = 0; i < row; i++){
            _matrix[i] = new double [col]();
        }
        this->matrix = _matrix;
    }

    Matrix(double **arr, int row, int col){
        this->matrix = arr;
        this->row = row;
        this->col = col;
    }

    ~Matrix(){
        for (int i = 0; i < row; i++) {
            delete[] matrix[i];
        }
        matrix = nullptr;
        col = 0;
        row = 0;
    }

    double& get(int i, int j){
        return matrix[i][j];
    }

    int get_size() {
        return row * col;
    }
    int get_column() {
        return col;
    }
    int get_row() {
        return row;
    }
    double get_element(int i, int j) {
        return matrix[i][j];
    }
    void set_element(int i, int j, double element) {
        matrix[i][j] = element;
    }

    Matrix* multiply(Matrix* multi_matrix){
        if (get_column() != (*multi_matrix).get_row()){
            std::cout << "The dimensions of the matrices are incorrect" << std::endl;
            return 0;
        }
        Matrix* new_matrix = new Matrix(get_row(), (*multi_matrix).get_column());
        for(int i = 0; i < (*new_matrix).row; i++){
            for(int j = 0; j < (*new_matrix).col; i++){
                for (int k = 0; k < get_column(); k++) {
                    (*new_matrix).matrix[i][j] += matrix[i][k] * (*multi_matrix).matrix[k][j];
                }
            }
        }
        return new_matrix;
    }

    Matrix* transponse(){
        Matrix* transp_matrix = new Matrix(row, col);
        for(int i = 0; i < row; i++){
            for (int j = 0; j < col; j++){
                (*transp_matrix).get(i, j) = matrix[j][i];
            }
        }
        return transp_matrix;
    }

    bool isConditionCheckMatrix(){ //проверка условий матрицы
        int count = 0;
        for (int i = 0; i < row; ++i) {
            for (int j = 1; j < col; ++j) {
                for (int k = 1; k < row; ++k) {
                    if (matrix[k][i] == matrix[i][j]) {
                        count++;
                    }
                }
            }
        }
        if (row * row - row == count) {
            return true;
        } else {
            return false;
        }
    }
    bool isCheckMainDiagonal(){//прверка главной диагонали
        for (int i = 0; i < row; ++i) {
            if (matrix[i][i] < 0)
                return false;
        }
        return true;
    }
};