#include "common.h"

double step = 0;
double minX, maxX, minY, maxY;
double *x;
double *y;
int n_count;

double function(double x) {
    return std::pow(x, 3) * std::exp(std::sin(x));
}

double derivative(double x) {
    return std::pow(x, 2) * std::exp(sin(x))*(x * std::cos(x) + 3);
}

double random(){
    return ((double)rand() / RAND_MAX);
}

double findMin(double* arr) {
    double minElem = arr[0];
    for (int i = 1; i < n_count; i++)
        if (minElem > arr[i])
            minElem = arr[i];
    return minElem;
}

double findMax(double* arr) {
    double maxElem = arr[0];
    for (int i = 1; i < n_count; i++)
        if (maxElem < arr[i])
            maxElem = arr[i];
    return maxElem;
}

double findMinByRange(double* arr, int start, int end) {
    double minElem = arr[start];
    for (int i = start + 1; i < end; i++)
        if (minElem > arr[i])
            minElem = arr[i];
    return minElem;
}

double findMaxByRange(double* arr, int start, int end) {
    double maxElem = arr[start];
    for (int i = start + 1; i < end; i++)
        if (maxElem < arr[i])
            maxElem = arr[i];
    return maxElem;
}


double methodSimpson(double a, double b, double eps, int n, int pointsCount) {
    step = (b - a) / (n - 1);
    x = new double[n];
    y = new double[n];
    n_count = n;
    for (int i = 0; i < n; i++) {
        x[i] = a + step * i;
        y[i] = function(x[i]);
    }
    double square;
    double maxX = b;
    double minX = a;
    double maxY = findMax(y);
    double minY = findMin(y);
    int counter = 0;
    for (int i = 0; i < pointsCount; i++) {
        double randomX = random() * (maxX - minX) + minX;
        double randomY = random() * (maxY - minY) + minY;
        if (randomY <= function(randomX)) // если попали в область, то испытание успешно
            counter++;
    }
    return (maxY - minY) * (maxX - minX) * counter / pointsCount; // Возврат площади для заданного участка.
}
