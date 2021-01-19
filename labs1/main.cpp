#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <windows.h>
#include "src/headers/LLTCalculator.h"
#include "string"


const std::string DATA_PACKAGE = "../data/";
const std::string RESULT_PACKAGE = "../result/";

struct FileInfo {
    std::string slau_name;
    std::string answer_name;
    std::string result_name;
};

struct ProcessInfo {
    std::string slau_name;
    std::string vector_name;
    int cpu_burst;
};

Matrix* read_matrix(std::string fileName);
double* read_vector(std::string fileName);
void manage_threads_FCFS(ProcessInfo *processInfo, int process_count);
void manage_displacing_guaranteed_planning(ProcessInfo *processInfo, int process_count);

DWORD solveLLT(void *param);

int main(int argc, char **argv) {
    ProcessInfo processInfo[2] = {
            ProcessInfo{"matrix_A", "fb_A", 7},
            ProcessInfo{"matrix_B", "fb_B", 10},
    };
    manage_displacing_guaranteed_planning(processInfo, 2);
    manage_threads_FCFS(processInfo, 2);
}

void manage_displacing_guaranteed_planning(ProcessInfo *processes_info, int process_count) {
    int time_slice_ms = 1;
    HANDLE *threads = new HANDLE[process_count];
    for (int i = 0; i < process_count; i++) {
        std::string process_name = "P" + std::to_string(i) + "_DGP";
        threads[i] = CreateThread(
                nullptr,
                0,
                solveLLT,
                (LPVOID) new FileInfo{processes_info[i].slau_name, processes_info[i].vector_name, process_name},
                CREATE_SUSPENDED,
                nullptr
        );
    }
    // Засекаем время
    std::chrono::time_point<std::chrono::high_resolution_clock> time_start = std::chrono::high_resolution_clock::now();

    int active_thread = 1;

    int *thread_lifetime = new int[process_count];
    for (int i = 0; i < process_count; i++) thread_lifetime[i] = 0;

    int runned_count, executed_count = 0;
    do {
        int current_thread = 0;
        float min_cpu_burst = processes_info[0].cpu_burst * thread_lifetime[0] * 1.0f / (process_count - executed_count);
        for (int i = 0; i < process_count; i++) {
            float koeff_justice = processes_info[i].cpu_burst * thread_lifetime[i] * 1.0f / (process_count - executed_count);
            if (min_cpu_burst > koeff_justice && processes_info[i].cpu_burst > 0 || min_cpu_burst < 0) {
                min_cpu_burst = koeff_justice;
                current_thread = i;
            }
        }
        ResumeThread(threads[current_thread]);

        WaitForSingleObject(threads[current_thread], time_slice_ms);

        thread_lifetime[current_thread] += time_slice_ms;
        processes_info[current_thread].cpu_burst -= time_slice_ms;

        SuspendThread(threads[current_thread]);

        if (processes_info[current_thread].cpu_burst <= 0) {
            ResumeThread(threads[current_thread]);
            WaitForSingleObject(threads[current_thread], INFINITE);
            processes_info[current_thread].cpu_burst = -1;
            executed_count++;
        }
    } while (executed_count < process_count);

    // Смотрим сколько потрачено
    std::chrono::time_point<std::chrono::high_resolution_clock> time_end = std::chrono::high_resolution_clock::now();
    // Смотрим затраты времени
    std::chrono::milliseconds delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    std::cout << "Total spent time: " << delta_time.count()<< " ms" << std::endl;

    delete [] thread_lifetime;
    delete [] threads;
}


void manage_threads_FCFS(ProcessInfo *processes_info, int process_count) {
    HANDLE *threads = new HANDLE[process_count];
    for (int i = 0; i < process_count; i++) {
        std::string process_name = "P" + std::to_string(i) + "_FCFS";
        threads[i] = CreateThread(
                nullptr,
                0,
                &solveLLT,
                (LPVOID) new FileInfo{processes_info[i].slau_name, processes_info[i].vector_name, process_name},
                CREATE_SUSPENDED,
                nullptr
        );
    }
    // Засекаем время
    auto time_start = std::chrono::steady_clock::now();

    int active_thread = 1;
    HANDLE runned_threads[active_thread];
    int runned_count, executed_count = 0;
    do {
        runned_count = 0;
        do {
            runned_threads[runned_count] = threads[executed_count + runned_count];
            ResumeThread(runned_threads[runned_count]);
            runned_count++;
        } while (runned_count < active_thread && executed_count + runned_count < process_count);

        WaitForMultipleObjects(runned_count, runned_threads, true , INFINITE);

        executed_count += runned_count;
    } while (executed_count < process_count);

    // Смотрим сколько потрачено
    auto time_end = std::chrono::steady_clock::now();
    // Смотрим затраты времени
    auto delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_start);
    std::cout << "Total spent time: " << delta_time.count() * 0.001f * 0.001f<< " ms" << std::endl;

    delete [] threads;
}

double* read_vector(std::string fileName) {
    //Создаем файловый поток и связываем его с файлом
    std::ifstream in(fileName);

    if (in.is_open())
    {
        //Если открытие файла прошло успешно
        //Вначале посчитаем сколько чисел в файле
        int count = 0;// число чисел в файле
        int temp;//Временная переменная

        while (!in.eof())// пробегаем пока не встретим конец файла eof
        {
            in >> temp;//в пустоту считываем из файла числа
            count++;// увеличиваем счетчик числа чисел
        }

        //Опять переходим в потоке в начало файла
        in.seekg(0, std::ios::beg);
        in.clear();


        double *x = new double [count];

        //Считаем матрицу из файла
        for (int i = 0; i < count; i++)
            in >> x[i];

        in.close();//под конец закроем файла

        return x;
    } else {
        //Если открытие файла прошло не успешно
        std::cout << "File not found.\n";
    }
    return 0;
}

Matrix* read_matrix(std::string fileName) {
    //Создаем файловый поток и связываем его с файлом
    std::ifstream in(fileName);

    if (in.is_open())
    {
        //Если открытие файла прошло успешно

        //Вначале посчитаем сколько чисел в файле
        int count = 0;// число чисел в файле
        int temp;//Временная переменная

        while (!in.eof())// пробегаем пока не встретим конец файла eof
        {
            in >> temp;//в пустоту считываем из файла числа
            count++;// увеличиваем счетчик числа чисел
        }

        //Число чисел посчитано, теперь нам нужно понять сколько
        //чисел в одной строке
        //Для этого посчитаем число пробелов до знака перевода на новую строку

        //Вначале переведем каретку в потоке в начало файла
        in.seekg(0, std::ios::beg);
        in.clear();

        //Число пробелов в первой строчке вначале равно 0
        int count_space = 0;
        char symbol;
        while (!in.eof())//на всякий случай цикл ограничиваем концом файла
        {
            //теперь нам нужно считывать не числа, а посимвольно считывать данные
            in.get(symbol);//считали текущий символ
            if (symbol == ' ') count_space++;//Если это пробел, то число пробелов увеличиваем
            if (symbol == '\n') break;//Если дошли до конца строки, то выходим из цикла
        }
        //cout << count_space << endl;

        //Опять переходим в потоке в начало файла
        in.seekg(0, std::ios::beg);
        in.clear();

        //Теперь мы знаем сколько чисел в файле и сколько пробелов в первой строке.
        //Теперь можем считать матрицу.

        int n = count / (count_space + 1);//число строк
        int m = count_space + 1;//число столбцов на единицу больше числа пробелов
        double **x;
        x = new double *[n];
        for (int i = 0; i<n; i++) x[i] = new double [m];

        //Считаем матрицу из файла
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                in >> x[i][j];

        in.close();//под конец закроем файла
        return new Matrix(x, n, m);

    } else {
        //Если открытие файла прошло не успешно
        std::cout << "File not found.\n";
    }
    return 0;
}

int save_result(std::string fileName, double *answer, int size) {
    std::ofstream of(fileName);
    for (int i = 0; i < size; i++) {
        of << answer[i] << " ";
    }
    of << std::endl;
    of.close();
    return 0;
}

DWORD solveLLT(LPVOID arg) {
    // Засекаем время
    auto time_start = std::chrono::steady_clock::now();

    FileInfo *fileInfo = (FileInfo*)arg;

    std::string slauFileName = DATA_PACKAGE + "matrics/" + fileInfo->slau_name + ".txt";
    std::string answerFileName = DATA_PACKAGE + "free_members/" + fileInfo->answer_name + ".txt";
    std::string resultFileName = RESULT_PACKAGE + fileInfo->result_name + ".txt";
    std::string xResultFileName = RESULT_PACKAGE + fileInfo->result_name + "_x_values" + ".txt";

    auto *matrix = read_matrix(slauFileName);
    auto *vector = read_vector(answerFileName);

    auto calculator = new LLTCalculator(matrix, vector);
    auto x = calculator->solve();

    /**
    std::cout << "Find values X:" << std::endl;
    for (int i = 0; i < matrix->get_row(); i++){
        std::cout << x[i] << std::endl;
    }
    **/
    save_result(xResultFileName, x, matrix->get_row());
    std::ofstream of(resultFileName);
    // Смотрим сколько потрачено
    auto time_end = std::chrono::steady_clock::now();
    // Смотрим затраты времени
    auto delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_start);
    std::cout << fileInfo->result_name << " time: " << delta_time.count() * 0.001f * 0.001f  << " ms" << std::endl;
    std::string msg = fileInfo->result_name + ", execute time: " + std::to_string(delta_time.count()) + "ms";

    of << msg << std::endl;
    of.close();

    delete [] x;
    delete calculator;
    delete fileInfo;
}



