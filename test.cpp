/*
Написать программу, которая находит все простые числа в заданном пользователем диапазоне. 
Программа - консольное приложение, в качестве параметров при вызове принимает одно или 
два целых числа, если число одно, то диапазон в котором ищутся простые числа от 1 до этого значения, 
если параметров два, то это границы диапазона. Программа однопоточная. 
Нужно постараться минимизировать время поиска простых чисел.
В качестве требований к оформлению можно посмотреть - https://google.github.io/styleguide/cppguide.html
*/


/*
строчки для компилятора

g++ -Wall test.cpp -o test

*/

/*
Время работы программы

v1
При ./test 622337203 ~ 18876 мс

v2
right + 1 -> right1
При ./test 622337203 ~ 18530 мс


v3
p*p < right1 -> p < sqrt_right1
float sqrt_right1 = sqrt(right1) -> long sqrt_right1 = sqrt(right1)
./test 622337203 ~ 18224 мс

v4
замена типа решета с long на bool 
./test 622337203 ~ 11284 мс

v5
с использование битовых полей (8 бит)
./test 622337203 ~ 7183 мс

v6
с использованием битовых масок(64 бит)
./test 622337203 ~ 6848 мс
*/


/*
Описание работы алгоритма Функции "решето Эратосфена". 

Входные данные: указатель на незаполненный массив(решето) типа char*;  число типа long, до которого нужно найти все простые числа.(условно обозначим N) 

Возвращаемое значение функции: Решето заполняется внутри функции по указателю, сама функция ничего не возвращает. 

Алгоритм:     

    1. Каждому элементу массива присвоить значение '1', индексы при этом от 0 до N включительно, 
        каждый индекс - и есть проверяемое число, а значение по индексу: 0 - не простое, 1 - простое.

    2. Сначала берется первое простое число (это число 2) и из ряда натуральных чисел "вычеркиваются" 
        (присваивается '0' элементу массива по индексу) все кратные ему.         

    2.1 Если вычеркиваются все числа, кратные некоторому простому числу p, то вычеркивать следует все числа, 
        начиная с p*p, т.к. это будет первое невычеркнутое число, кратное p (все предшествующие кратные p на момент проверки уже вычеркнуты).

    3. Затем берется следующее простое и "вычеркиваются" все кратные ему и так далее до квадратного корня из N, 
        т.к. этого достаточно, чтобы в решете остались только простые числа.     

    4. Значения индексов ненулевых элементов массива - и есть простые числа, исключая 0 и 1.

*/


#include<iostream>
#include<string>
#include<ctime>
#include<locale>
#include<cmath>
#include <thread>
#include <chrono>


#define ll long long

using namespace std;

// Решето Эратосфена(первая версия)
void SearchSimple_v1(bool *sieve, ll right){

    long right1 = right+1;
    long sqrt_right1 = sqrt(right1);

    //Заполнение решета Эратосфена
    for (ll i = 0; i < right1; i++){
        sieve[i] = 1;
    }
    
    //Вычеркивание всех составных чисел
    for (ll p = 2; p < sqrt_right1; p++){
        if (sieve[p] != 0){
            //cout << p << endl;
            for (ll j = p*p; j < right1; j += p){
                sieve[j] = 0;
            }      
        }
    } 

    cout << "Поиск завершен" << endl;
}


//Решето Эратосфена с использованием битовых масок
void SearchSimple(unsigned ll *sieve, unsigned ll right){ 


    unsigned ll right1 = right+1;
    unsigned ll sqrt_right1 = sqrt(right1)+1;
    unsigned ll num_bloks = right/64+1;
    unsigned ll* ps = sieve; 


    for(size_t i = 0; i < num_bloks; i++, ps++){
        *ps = 0xffffffffffffffff;
    }

    ps = sieve;
    *ps = 0xfffffffffffffffc;

    for (size_t p = 2; p < sqrt_right1; p++){
        if(sieve[p/64] & ((unsigned ll)1 << p)){
            for (size_t j = p*p; j < right1; j += p){
                sieve[j/64] = sieve[j/64] & ~((unsigned ll)1 << j);
            }
        }
    }
}

//Преобразует введенные мользователем данные из string в long long и проверяет корректность ввода.
void CheckInput(string str, ll &border){
    size_t sz_res = 0, sz_inp = str.size();

    border= stoll(str,&sz_res,0);
    
    if (sz_res != sz_inp){
        throw invalid_argument("Неверно введенные данные");
    }
}


//Вывод простых чисел
void OutputSimple(unsigned ll *sieve, unsigned ll left_border, unsigned ll right_border){
    string ans;

    cout << "Желаете увидеть все найденные числа?(y - Да, n - Нет)" << endl;
    cin >> ans;
    
    if (ans == "y"){
        unsigned ll right1 = right_border+1;

        for(size_t i = 0; i < right1; i++){
            if((sieve[i/64] & ((unsigned ll)1 << i % 64)) && (i >= left_border) && (i <= right_border) ){
                cout << i << ", ";
            }
        }

        cout << endl;
        cout << "Завершение программы..." << endl;  
    }
    else if(ans != "n"){
        cout << "Некорректный ввод, завершение программы..." << endl;
    }
    else{
        cout << "Завершение программы..." << endl;
    }
}


//вывод для пользователя и установка корректных значений диапазона для работы программы
void Swap(ll &left_border, ll &right_border){

    if(left_border > right_border){
        ll tmp = left_border;
        left_border = right_border;
        right_border = tmp;
    }

    cout << "Левая граница == " << left_border << endl;
    cout << "Правая граница == " << right_border << endl;

    if(left_border < 0){
        left_border = 0;
    }
    if(right_border < 0){
        right_border = 0;
    }

}


int main(int argc, char* argv[]){

    // установка времени начала работы программы
    auto start = chrono::high_resolution_clock::now();

    setlocale(LC_ALL, "Russian");

    try{

        ll left_border = 1, right_border = 1;

        //если параметры не введены 
        if(argc == 1){
            cout << "Вы не ввели данные" << endl;
            cout << "Завершение программы..." << endl;
        }

        //если введен 1 параметр
        else if(argc == 2){
            string buff= argv[1];

            //проверка введенных данных
            CheckInput(buff, right_border);
            //установка границ диапазона для работы программы и вывод для пользователя
            Swap(left_border, right_border);

            //поиск простых чисел
            //bool *sieve = new bool[right_border+1];
            size_t num_bloks = right_border/64+1;
            unsigned ll *sieve = new unsigned ll[num_bloks];
            SearchSimple(sieve, right_border);

            // установка конца и вывод итогового времени работы алгоритма
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<float> duration = end-start;
            cout << "Время работы программы " << duration.count() << " s" << endl;

            //вывод простых чисел по запросу пользователя
            OutputSimple(sieve, left_border,right_border);
            
            //очистка памяти
            delete[] sieve;
            sieve = nullptr;
            
        }

        //если введено 2 параметра
        else if(argc == 3){
            string buff = argv[1];

            //проверка введенных данных
            CheckInput(buff, left_border);

            buff = argv[2];

            CheckInput(buff, right_border);

            //установка границ диапазона
            Swap(left_border, right_border);
            
            //поиск простых чисел
            //bool *sieve = new bool[right_border+1];
            size_t num_bloks = right_border/64+1;
            unsigned ll *sieve = new unsigned ll[num_bloks];
            SearchSimple(sieve, right_border);

            // установка конца и вывод времени итогового работы алгоритма
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<float> duration = end-start;
            cout << "Время работы программы " << duration.count() << " s" << endl;

            //вывод простых чисел по запросу пользователя
            OutputSimple(sieve, left_border,right_border);
            
            //очистка памяти
            delete[] sieve;
            sieve = nullptr;
          
        }
        //если введено 3 и более параметров
        else{
            cout << "Вы ввели слишком много параметров, макс кол-во - 2" << endl;
            cout << "Завершение программы..." << endl;
        }
    }
    //при неверном формате ввода
    catch (invalid_argument& e)
    {
        cout << e.what() << endl;
    }
    //при нелостатке оперативной памяти
    catch (bad_alloc& e)
    {
        cout << "Недостаточно оперативной памяти" << endl;
    }
    //при вводе слишком большого числа(диапазона)
    catch (out_of_range& e)
    {
        cout << "Вы ввели слишком большое число" << endl;
    }
    catch(...){
        cout << "Непредвиденная ошибка" << endl;
    }
   
    cout << "Программа завершена" << endl;

    return 0;
}