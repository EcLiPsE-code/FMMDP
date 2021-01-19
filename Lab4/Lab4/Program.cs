using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading;
using System.Management;
using Matrix;

namespace Lab4
{
    internal static class Program
    {
        private const string DataFolder = @"C:\Users\YoUr_IcOn\Desktop\Lab4\Lab4\data\";
        private const string ResultFolder = @"C:\Users\YoUr_IcOn\Desktop\Lab4\Lab4\result\";

        private static readonly Semaphore solveSemaphore = new Semaphore(1, 1);
        private static readonly Semaphore saveSemaphore = new Semaphore(1, 1);

        private static string[] values = new string[10];
        private static int i = 0;
        private static int index = 0;
        
        public static StreamReader file;
        
        private static bool _finish;
        private static int _index = 1;
        private static int count_thread = countCores();
        private static double[,] A { set; get; } //матрица коэфф.
        private static double[] B { set; get; } //вектор свободных членов
        
        private static Stopwatch trackStopwatch = new Stopwatch();
        
        public static void Main()
        {
            trackStopwatch.Start();
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
            //SingleThreading();
            MultiThreading();
            stopwatch.Stop();
            Console.WriteLine("Time elapsed (ms): {0}", stopwatch.Elapsed.TotalMilliseconds);

            Console.ReadKey();
        }

        private static void SingleThreading()
        {
            using (StreamReader file = new StreamReader(DataFolder + "tasks.csv"))
            {
                while (!file.EndOfStream)
                {
                    string[] values = file.ReadLine()?.Split(';').ToArray();
                    if (values != null)
                    {
                        double[,] matrix = ReadA(values[0]);
                        double[] coeffs = ReadB(values[1]);
                        CalculateLLT calculateLLT = new CalculateLLT(matrix, coeffs);
                        double[] result = calculateLLT.solve();
                        saveAnswer($"x{_index}", result);
                    }

                    _index++;
                }
            }
        }

        private static void ReadTasks()
        {
            using (StreamReader file = new StreamReader(DataFolder + "tasks.csv"))
            {
                int i = 0;
                while (!file.EndOfStream)
                {
                    string valuesOne = file.ReadLine();
                    values[i] = valuesOne;
                    i++;
                }
            }
        }
        
        private static void MultiThreading()
        {
            ReadTasks();
            Thread[] threads = new Thread[2]
            {
                new Thread(solve),
                new Thread(solve)
            };
            int i = 0;
            foreach(var thread in threads)
            {
                thread.Name = $"Thread{i}";
                thread.Start();
                i++;
                Console.WriteLine($"Thread {thread.Name} started");
            }

            foreach (var thread in threads)
            {
                thread.Join();
            }
        }

        public static void solve()
        {
            bool flag = true;
            while(flag)
            {
                solveSemaphore.WaitOne();

                Console.WriteLine(Thread.CurrentThread.Name + " начал решение");

                string[] val = values[i].Split(';').ToArray();
                double[,] matrix = ReadA(val[0]);
                double[] coeffs = ReadB(val[1]);
                CalculateLLT calculateLLT = new CalculateLLT(matrix, coeffs);
                double[] result = calculateLLT.solve();
                
                index++;
                Console.WriteLine(Thread.CurrentThread.Name + " закончил решение");
                
                Thread.Sleep(1000);
                solveSemaphore.Release();
                saveSemaphore.WaitOne();
                Console.WriteLine(Thread.CurrentThread.Name + " начал сохранение");
                saveAnswer($"x{index}", result);
                Console.WriteLine(Thread.CurrentThread.Name + " закончил сохранение");
                Thread.Sleep(1000);
                saveSemaphore.Release();
                
                if (index == 10)
                {
                    flag = false;
                }
            }
        }

        //количество ядер компьютера
        private static int countCores() 
        {
            int coreCount = 0;
            foreach (var item in new ManagementObjectSearcher("Select * from Win32_Processor").Get())
            {
                coreCount += int.Parse(item["NumberOfCores"].ToString());
            }
            return coreCount;
        }

        //сохранение вектора решений в файле
        private static void saveAnswer(string filename, double[] vector_answers)
        {
            using (FileStream fileStream = new FileStream($"{ResultFolder}" + $"\\{filename}.csv", FileMode.OpenOrCreate))
            {
                for (int i = 0; i < vector_answers.Length; i++)
                {
                    byte[] array_bytes = System.Text.Encoding.Default.GetBytes((vector_answers[i] + "\t").ToString());
                    fileStream.Write(array_bytes, 0, array_bytes.Length);
                }
            }
        }

        //чтение из файла матрицы коэффициентов
        private static double[,] ReadA(string filename)
        {
            using (StreamReader file = new StreamReader(DataFolder + filename))
            {
                double[][] doubles = file.ReadToEnd()
                    .Split('\n')
                    .Select(line => line.TrimEnd().Split(' ').Select(double.Parse).ToArray())
                    .ToArray();
                double[,] matrix = new double[doubles.Length, doubles[0].Length];
                for (int i = 0; i < matrix.GetLength(0); i++)
                {
                    for (int j = 0; j < matrix.GetLength(1); j++)
                    {
                        matrix[i, j] = doubles[i][j];
                    }
                }

                return matrix;
            }
        }

        //чтение вектора свободных членов
        private static double[] ReadB(string filename)
        {
            using (StreamReader file = new StreamReader(DataFolder + filename))
            {
                return file.ReadToEnd().TrimEnd().Split(' ').Select(double.Parse).ToArray();
            }
        }
    }
}