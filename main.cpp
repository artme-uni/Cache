#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>

static inline unsigned long long rdtsc(void)  //подсчет времени в тактах
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long) lo) | (((unsigned long long) hi) << 32);
}

volatile unsigned *direct(int N) //12345670
{
    volatile unsigned* arr = new unsigned[N]();
    for (int i = 0; i < N; i++)
    {
        arr[i] = (i + 1) % N;
    }

    return arr;
}

volatile unsigned *reverse(int N) //70123456
{
    volatile unsigned* arr = new unsigned[N]();
    arr[0] = N - 1;
    for (int i = 1; i < N; i++)
    {
        arr[i] = i - 1;
    }

    return arr;
}

volatile unsigned *permutation(int N) //Алгоритм Саттоло
{
    srand(time(NULL));

    volatile unsigned* arr = new unsigned[N]();

    for (int i = 0; i < N; i++)
    {
        arr[i] = i;
    }

    for (int i = N - 1; i > 0; i--)
    {
        int j = rand() % i;

        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    return arr;
}

unsigned long long pass(volatile unsigned *arr, int N, int repeat, int check)
{
    int next = 0;
    unsigned long long volatile t1, t2;
    unsigned long long min_time = ULLONG_MAX;

    for (int i = 0; i < N; i++) //"прогреваем" кэш память
    {
        next = arr[next];
    }

    for (int k = 0; k < check; k++) //ищем наймиешьшее значений среди cheсk попыток
    {
        t1 = rdtsc();
        for (int i = 0; i < N * repeat; i++)
        {
            next = arr[next];
        }
        t2 = rdtsc();

        if (min_time > t2 - t1)
        {
            min_time = t2 - t1;
        }
    }

    return min_time;
}

bool power_of_two(int &Value)
{
    int InitValue = 1;
    while (InitValue < Value)
        InitValue *= 2;
    if (InitValue == Value)
        return true;
    return false;
}

int to_mb(int kb)
{
    if(power_of_two(kb))
    {
        if(kb >= 1024)
            kb /= 1024;
    } else
        kb =0;

    return kb;
}


int main()
{
    int min_N = 256 * 1;
    int max_N = 256 * 32 * 1024 * 2; //32 мб = 32768 кб
    int cur_N = min_N;

    int repeat = 10;

    std::ofstream out;
    out.open("/Users/twiggy/Desktop/output.txt");

    out << "КБ" << "\t" << "МБ" << "\t" << "direct" << "\t" << "reverse" << "\t" << "random" << std::endl;

    for (int i = 1; cur_N <= max_N; i++)
    {
        int temp_N = cur_N;

        while (temp_N < cur_N * 2)
        {

            out << temp_N / 256 << "\t" << to_mb(temp_N / 256) << "\t";

            volatile unsigned *arr_d = direct(temp_N);
            int time = pass(arr_d, temp_N, repeat, 10) / repeat / temp_N;

            out << time << "\t";


            volatile unsigned *arr_r = reverse(temp_N);
            time = pass(arr_r, temp_N, repeat, 10) / repeat / temp_N;

            out << time << "\t";


            volatile unsigned *arr_p = permutation(temp_N);
            time = pass(arr_p, temp_N, repeat, 5) / repeat / temp_N;

            out << time << std::endl;

            delete[] arr_d;
            delete[] arr_p;
            delete[] arr_r;

            temp_N += cur_N / 4;
        }

        cur_N *= 2;
    }

    out.close();

    return 0;
}

//hw.l1icachesize: 32768