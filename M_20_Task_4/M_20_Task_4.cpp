/*Задание 4. Реализация симуляции банкомата

Что нужно сделать
Требуется реализовать упрощённую симуляцию работы банкомата. В банкомате
могут храниться только бумажные купюры номиналом от 100 до 5000 рублей.
Максимально в банкомате может храниться только 1000 купюр. Все они
записываются в отдельный файл. У банкомата, как у устройства, две основных
операции — снятие денег пользователем и наполнение деньгами инкассаторами
банка.
Наполнение банкомата происходит, если при старте программы в консоль
вводится символ плюс “+”.  Количество купюр рассчитывается так, чтобы
банкомат был заполнен полностью. Все купюры при этом выбираются случайным
образом.
Если на старте программы в консоль вводится минус (“-”), то симулируется
снятие пользователем денег. Пользователь указывает сумму с точностью до 100
рублей. Мы будем считать, что каждый клиент обладает неограниченным балансом
в системе и теоретически может снять любую сумму. На практике, если данная
сумма не может быть снята из-за отсутствия подходящих денег в машине,
показывается сообщение, что эта операция невозможна.
После выполнения любой из операций программа завершает выполнение. Состояние
банкомата должно храниться в отдельном бинарном файле, автономно.*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>

//#define _CRT_SECURE_NO_WARNINGS

namespace constants{

    const int MAX_CAPACITY = 1000;
    const int INTEGER = 4;
    const int REC_SIZE = 7;
    const std::string PATCH = "atm.bank";
}

enum Money
{
    HUNDRED       = 100,
    TWO_HUNDRED   = 200,
    FIVE_HUNDRED  = 500,
    THOUSAND      = 1000,
    TWO_THOUSAND  = 2000,
    FIVE_HOUSAND  = 5000
};

//Функция возвращает номинал банкноты в зависимости от положения в массиве
int banknote(int i) {

    int arr[]{ FIVE_HOUSAND, TWO_THOUSAND, THOUSAND, FIVE_HUNDRED, TWO_HUNDRED, HUNDRED };
    return arr[i];
}

//Валидация введения суммы для снятия из банкомата
int inAmount() {

    std::string amount{};

    std::cout << "Enter the amount with an accuracy of 100: ";
    std::getline(std::cin, amount);

    while (amount.find_first_of("0123456789") == -1) {

        std::cout << "Incorrect input. Try again.";
        std::getline(std::cin, amount);
    }
    return std::stoi(amount);
}

//Валидация команды (пополнить банкомат или снять наличные)
char validChar() {

    int count = 0;
    char tmp;
    std::cout << "Select an action (+ or -): ";

    while (true) {

        std::cin >> tmp;
        while (std::cin.get() != '\n') count++;
        if ((tmp == '+' || tmp == '-') && count == 0)
            return tmp;
        else {

            count = 0;
            std::cout << "Incorrect input. Try again.";
        }
    }
}

//Функция вносит изменения в массивы arr_in и arr_out при снятии наличных
bool getMoney(int* arr_in, int* arr_out, int sum) {

    int tmpBank{};
    int count_banknote{};
    for (int i = 0; i < constants::REC_SIZE - 1; i++) {

        tmpBank = sum / banknote(i);
        if (arr_in[i + 1] >= tmpBank && tmpBank != 0) {

            *(arr_in + i + 1) -= tmpBank;
            *(arr_out + i) = tmpBank;
            sum -= tmpBank * banknote(i);
            count_banknote += tmpBank;
        }
        if (*(arr_in + i + 1) < tmpBank && *(arr_in + i + 1) != 0 && tmpBank != 0) {

            *(arr_out + i) = *(arr_in + i + 1);
            *(arr_in + i + 1) = 0;
            sum -= *(arr_out + i) * banknote(i);
            count_banknote += tmpBank;
        }
    }  
    if (sum == 0) {

        *arr_in -= count_banknote;
        return true;
    }        
    else
        return false;
}

bool openFile(std::fstream& fs) {

    fs.open(constants::PATCH, std::ios::binary | std::ios::in | std::ios::out);
    return fs.is_open() ? true : false;
}

void closeFile(std::fstream& fs) {    

    if (fs.is_open())
        fs.close();
}

//Функция заполняет банкомат до 1000 купюр случайным количеством
void loadMoney(std::fstream& fs) {

    if (openFile(fs)) {

        int quantity = 0;
        fs.seekg(0, std::ios::beg);
        fs.read((char*)&quantity, constants::INTEGER);
        int range = constants::MAX_CAPACITY - quantity;
        if (range == 0) {

            closeFile(fs);
            return;
        }            

        int tmp[constants::REC_SIZE];
        int temp = 0;
        fs.seekg(0, std::ios::beg);
        for (int i = 0; i < constants::REC_SIZE; i++) {
            
            fs.read((char*)&temp, constants::INTEGER);
            tmp[i] = temp;            
        }        

        int pos;         
        for (int i = 0; i < range; i++) {

            pos = 1 + (std::rand() % 6);
            tmp[pos]++;
            tmp[0]++;
        }       
        fs.seekp(0, std::ios::beg);
        for (int i = 0; i < constants::REC_SIZE; i++) {
            
            fs.write((char*)&tmp[i], constants::INTEGER);
        }
        closeFile(fs);
    }
    else {

        std::cout << "File opening error!";
        return;
    }
}

//Функция чтения из файла
void readingBank(std::fstream& fs, int* arr) {

    if (!openFile(fs)) {

        std::cout << "File opening error!";
        return;
    }
    for (int i = 0; i < constants::REC_SIZE; i++) {

        fs.read((char*)(arr + i), constants::INTEGER);
    }
    closeFile(fs);
}

//Функция записи в файл
void recordBank(std::fstream& fs, int* arr) {

    if (!openFile(fs)) {

        std::cout << "File opening error!";
        return;
    }
    for (int i = 0; i < constants::REC_SIZE; i++) {

        fs.write((char*)(arr + i), constants::INTEGER);
    }
    closeFile(fs);
}

//Функция демонстрации состояния банкомата
void showBank(std::fstream& fs) {

    if (!openFile(fs)) {

        std::cout << "File opening error!";
        return;
    }
    int temp{};
    for (int i = 0; i < constants::REC_SIZE; i++) {

        fs.read((char*)&temp, constants::INTEGER);
        switch (i)
        {
        case 0:
            std::cout << std::setw(35) << std::left << "Total number of bills: "
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        case 1:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 5000:"
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        case 2:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 2000:"
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        case 3:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 1000:"
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        case 4:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 500:"
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        case 5:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 200:"
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        case 6:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 100:"
                      << std::setw(6) << std::right << temp << std::endl;
            break;
        }
    }
    std::cout << std::endl;
    closeFile(fs);
}

//Функция демонстрации выдаваемых денежных средств
void showDebiting(int* arr) {
    
    for (int i = 0; i < constants::REC_SIZE - 1; i++) {
        
        switch (i)
        {        
        case 0:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 5000:"
                << std::setw(6) << std::right << *(arr + i) << std::endl;
            break;
        case 1:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 2000:"
                << std::setw(6) << std::right << *(arr + i) << std::endl;
            break;
        case 2:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 1000:"
                << std::setw(6) << std::right << *(arr + i) << std::endl;
            break;
        case 3:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 500:"
                << std::setw(6) << std::right << *(arr + i) << std::endl;
            break;
        case 4:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 200:"
                << std::setw(6) << std::right << *(arr + i) << std::endl;
            break;
        case 5:
            std::cout << std::setw(35) << std::left << "The number of banknotes is 100:"
                << std::setw(6) << std::right << *(arr + i) << std::endl;
            break;
        }
    }
    std::cout << std::endl;
}

//Итоговая функция
void result(std::fstream& fs) {    

    char sign = validChar();    
    switch (sign) {
    case '+': 
        showBank(fs);
        loadMoney(fs);
        showBank(fs);        
        break;
    case '-':
        int arr_in[constants::REC_SIZE]{};
        int arr_out[constants::REC_SIZE - 1]{};
        int amount = inAmount();     
        showBank(fs);
        readingBank(fs, arr_in);
        if (getMoney(arr_in, arr_out, amount)) {
            recordBank(fs, arr_in);
            showDebiting(arr_out);
            showBank(fs);
        }
        else
            std::cout << "Not enough money!" << std::endl;        
    }
}

int main() {  

    std::srand(0);
    std::fstream fs;    

    showBank(fs);
    result(fs);    


    /*int arr[]{ 1,2,3,4,5,6,7,8,9 };

    std::ofstream ofs("test.txt", std::ios::binary | std::ios::out);
    for (int i = 0; i < 9; i++) {

        ofs.write((char*)(arr + i), 4);
    }
    ofs.close();

    std::ifstream ifs("test.txt", std::ios::binary | std::ios::in);
    int temp{};
    for (int i = 0; i < 9; i++) {

        ifs.read((char*)&temp, 4);
        std::cout << temp << " ";
    }
    std::cout << std::endl;
    ifs.close();

    std::ofstream ofs1("test.txt", std::ios::binary | std::ios::out);
    ofs1.seekp(5, std::ios::beg);
    int a = 20;
    ofs1.write((char*)&a, 4);
    ofs1.close();

    std::ifstream ifs1("test.txt", std::ios::binary | std::ios::in);    
    for (int i = 0; i < 9; i++) {

        ifs1.read((char*)&temp, 4);
        std::cout << temp << " ";
    }
    std::cout << std::endl;
    ifs1.close();*/
}

