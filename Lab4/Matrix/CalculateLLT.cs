using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Matrix
{
    public class CalculateLLT
    {
        public double[,] matrix { get; set; } //матрица коэфф.
        public double[] vector_koeff { get; set; } //вектор свободных членов
        public double[,] matrix_L { get; set; } //матрица коэфф, после факторизации
       
        private double[] vector_y { get; set; } 
        public double[] vector_x { get; set; }

        public CalculateLLT(double[,] matrix, double[] vector_koeff) {
            this.matrix = matrix;
            this.vector_koeff = vector_koeff;
        }

        //факторизация матрицы коэфф.
        private void factorization()
        {
            matrix_L = new double[matrix.GetLength(0), matrix.GetLength(1)];

            for (int i = 0; i < matrix.GetLength(0); i++)
            {
                double s = matrix[i, i];
                for (int k = 0; k < i; k++)
                {
                    s -= Math.Pow(matrix_L[i, k], 2);
                }
                matrix_L[i, i] = Math.Sqrt(s);
                for (int j = i + 1; j < matrix.GetLength(0); j++)
                {
                    s = matrix[j, i];
                    for (int k = 0; k < i; k++)
                        s -= matrix_L[i, k] * matrix_L[j, k];
                    matrix_L[j, i] = s / matrix_L[i, i];
                }
            }
        }

        private void resolveY()
        {
            double[] result = new double[matrix_L.GetLength(0)];
            for (int i = 0; i < matrix_L.GetLength(0); ++i)
            {
                result[i] = vector_koeff[i];
                for (int j = 0; j < i; ++j)
                {
                    result[i] -= matrix_L[i, j] * result[j];
                }
                result[i] /= matrix_L[i, i];
            }
            vector_y = result;
        }

        private void resolveX()
        {
            double[] result = new double[matrix.GetLength(0)];
            for (int i = matrix.GetLength(0) - 1; i >= 0; --i)
            {
                result[i] = vector_y[i];
                for (int j = matrix.GetLength(0) - 1; j > i; --j)
                {
                    result[i] -= matrix_L[j, i] * result[j];
                }
                result[i] /= matrix_L[i, i];
            }
            vector_x = result;
        }

        public double[] solve()
        {
            factorization();
            resolveY();
            resolveX();
            return vector_x;
        }
    }
}
