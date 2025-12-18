/*
Наумова Снежана
Пи-41 дизайн
Лабораторная работа 5
Шифр RSA
*/
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// Константы
const int MaxPrimeNum = 100;    // Max простое число

// Глобальные переменные
string message = "";             // Исходное сообщение
long long p = 0, q = 0;          // Простые числа
long long n = 0, phi = 0;        // n = p*q, значения чисел взаимно простых с n = phi = (p-1)*(q-1)
long long e = 0, d = 0;          // e = публичный ключ - закрывает, d = приватный ключ - открывает
vector<long long> encryptedValues; // Зашифрованное сообщение
bool isEncrypted = false;        // Флаг: зашифровано ли сообщение

// Функция: проверка на простое число
bool isPrime(long long num) {
    if (num <= 1) return false; // 1 и отрицательные
    if (num <= 3) return true; // 2 и 3
    if (num % 2 == 0 || num % 3 == 0) return false; // Четное или делится на 3

    // Попарная проверка чисел >= 5
    for (long long i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0)
            return false;
    }
    return true;
}

// Функция: преобразование символа в число
int charToNum(char c) {
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 1; // A=1,...,Z=26
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 27; // a=27,...,z=52
    else if (c >= '0' && c <= '9')
        return c - '0' + 53; // 0=53,...,9=62
    else if (c == ' ')
        return 63;
    else
        return 64; // Резерв
}

// Функция: преобразование числа в символ
char numToChar(int n) {
    if (n >= 1 && n <= 26)
        return 'A' + (n - 1);
    else if (n >= 27 && n <= 52)
        return 'a' + (n - 27);
    else if (n >= 53 && n <= 62)
        return '0' + (n - 53);
    else if (n == 63)
        return ' ';
    else
        return '?';
}

// Функция: преобразование любого числа в читаемый символ (для зашифрованного текста)
char anyNumToChar(long long num) {
    // Приводим число к диапазону 1-64
    int safeNum = (num % 64) + 1;
    return numToChar(safeNum);
}

// Функция:возведение в степень по модулю (a^b mod m)
long long modPow(long long a, long long b, long long m) {
    long long result = 1;
    a = a % m;

    while (b > 0) {
        // b нечетное => результат * a
        if (b % 2 == 1) {
            result = (result * a) % m;
        }

        // b - четное
        b = b / 2;
        a = (a * a) % m;
    }

    return result;
}

// Функция: алгоритм Евклида (нахождение обратного элемента)
long long modInverse(long long a, long long m) {
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        // q - частное
        q = a / m;
        t = m;

        // m - остаток от деления
        m = a % m;
        a = t;
        t = x0;

        // Обновление x0 и x1
        x0 = x1 - q * x0;
        x1 = t;
    }

    // => x1 положительнный
    if (x1 < 0) x1 += m0;

    return x1;
}

// Функция: ввод сообщения
void inputMessage() {
    bool valid = false;

    while (!valid) {
        cout << "Введите сообщение (только английские буквы, цифры, пробел): ";
        getline(cin, message);

        if (message.empty()) {
            cout << "Ошибка: Сообщение не может быть пустым!" << endl;
            continue;
        }

        valid = true;
        for (char c : message) {
            if (!((c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                (c == ' '))) {
                valid = false;
                cout << "Ошибка: Сообщение содержит недопустимые символы!" << endl;
                break;
            }
        }
    }

    isEncrypted = false;
}

// Функция: ввод простых чисел
void inputPrimes() {

    // Ввод p
    bool validP = false;
    while (!validP) {
        cout << "Введите первое простое число p < 100: ";
        cin >> p;

        if (isPrime(p) && p < MaxPrimeNum) {
            validP = true;
            cout << "p = " << p << " - простое число" << endl;
        }
        else {
            cout << "Ошибка! Число должно быть простым и < " << MaxPrimeNum << endl;
        }
    }

    // Ввод q
    bool validQ = false;
    while (!validQ) {
        cout << "Введите второе простое число q < 100: ";
        cin >> q;

        if (isPrime(q) && q < MaxPrimeNum && q != p) {
            validQ = true;
            cout << "q = " << q << " - простое число" << endl;
        }
        else {
            cout << "Ошибка! Число должно быть простым, < " << MaxPrimeNum << " и не равно p" << endl;
        }
    }

    cin.ignore();
}

// Функция: генерация ключей RSA
void generateKeys() {
    // n и phi(n)
    n = p * q;
    phi = (p - 1) * (q - 1);

    cout << "\nГенерация ключей RSA:" << endl;
    cout << "n = p * q = " << p << " * " << q << " = " << n << endl;
    cout << "phi(n) = (p-1)*(q-1) = " << (p - 1) << " * " << (q - 1) << " = " << phi << endl;

    e = 17;  // Простое число, взаимно простое с phi(n)

    // e взаимно простое с phi(n)
    if (phi % e == 0) {
        // e не подходит => другое
        for (e = 3; e < phi; e += 2) {
            if (isPrime(e) && phi % e != 0) {
                break;
            }
        }
    }

    // Находим d (обратный элемент к e по модулю phi(n))
    d = modInverse(e, phi);

    cout << "\nПубличный ключ: (e=" << e << ", n=" << n << ")" << endl;
    cout << "Приватный ключ: (d=" << d << ", n=" << n << ")" << endl;
    cout << "\nПроверка: e * d mod φ(n) = " << e << " * " << d << " mod " << phi
        << " = " << (e * d) % phi << " (должно быть 1)" << endl;
}

// Функция: ввод сообщения и ключей заново
void inputMessageAndKeys() {

    inputMessage();

    inputPrimes();

    generateKeys();
}

// Функция: шифрование RSA
void rsaEncrypt() {
    if (message.empty() || n == 0) {
        cout << "Ошибка: Сначала введите сообщение и простые числа!" << endl;
        return;
    }

    cout << "\nШифрование RSA" << endl;

    // Преобразование сообщения в числа
    vector<long long> messageValues;
    cout << "\nПреобразование символов в числа:" << endl;
    for (char c : message) {
        long long num = charToNum(c);
        messageValues.push_back(num);
        cout << "  '" << c << "' -> " << num << endl;
    }

    // Шифр: C = M^e mod n
    encryptedValues.clear();
    cout << "\nШифрование (C = M^" << e << " mod " << n << "):" << endl;

    for (long long m : messageValues) {
        // Проверка: m < n
        if (m >= n) {
            cout << "Ошибка: Число " << m << " >= n=" << n << "!" << endl;
            cout << "Используйте большие простые числа или более короткое сообщение." << endl;
            return;
        }

        long long c = modPow(m, e, n);
        encryptedValues.push_back(c);

        cout << "  " << m << "^" << e << " mod " << n << " = " << c << endl;
    }

    // Вывод 
    cout << "\nЗашифрованное сообщение (числа): ";
    for (long long val : encryptedValues) {
        cout << val << " ";
    }

    cout << "\nЗашифрованное сообщение: ";
    for (long long val : encryptedValues) {
        cout << anyNumToChar(val);
    }
    cout << endl;

    isEncrypted = true;
}

// Функция: расшифрование RSA
void rsaDecrypt() {
    if (!isEncrypted) {
        cout << "Ошибка: Сначала нужно зашифровать сообщение!" << endl;
        return;
    }

    if (encryptedValues.empty()) {
        cout << "Ошибка: Нет зашифрованного сообщения!" << endl;
        return;
    }

    cout << "\nРасшифрование RSA" << endl;
    cout << "Расшифрование (M = C^" << d << " mod " << n << "):" << endl;

    vector<long long> decryptedValues;
    string decryptedMessage = "";

    for (long long c : encryptedValues) {
        // Расшифрование: M = C^d mod n
        long long m = modPow(c, d, n);
        decryptedValues.push_back(m);

        // Преобразование числа в символ
        char symbol = numToChar(m);
        decryptedMessage += symbol;

        cout << "  " << c << "^" << d << " mod " << n << " = " << m
            << " -> '" << symbol << "'" << endl;
    }

    cout << "\nРасшифрованное сообщение: " << decryptedMessage << endl;
}

// Функция: отображение меню
void showMenu() {
    cout << "\nТекущие данные:" << endl;
    cout << "  Сообщение: " << (message.empty() ? "не введено" : "\"" + message + "\"") << endl;
    cout << "  p: " << (p == 0 ? "не введено" : to_string(p)) << endl;
    cout << "  q: " << (q == 0 ? "не введено" : to_string(q)) << endl;
    cout << "  n: " << (n == 0 ? "не вычислено" : to_string(n)) << endl;
    cout << "  e: " << (e == 0 ? "не вычислено" : to_string(e)) << endl;
    cout << "  d: " << (d == 0 ? "не вычислено" : to_string(d)) << endl;
    cout << "  Статус: " << (isEncrypted ? "зашифровано" : "не зашифровано") << endl;

    cout << "\nГлавное меню:" << endl;
    cout << "1. Зашифровать сообщение" << endl;
    cout << "2. Расшифровать сообщение" << endl;
    cout << "3. Ввести сообщение и ключи заново" << endl;
    cout << "4. Выход" << endl;

    cout << "\nВаш выбор: ";
}

// Основная функция
int main() {
    setlocale(LC_ALL, "Russian");

    // Ввод сообщения
    inputMessage();
    inputPrimes();

    // Генерация ключей
    generateKeys();

    int choice = 0;

    while (true) {
        showMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите число от 1 до 4." << endl;
            continue;
        }

        cin.ignore(10000, '\n');  // Очистка буфера

        switch (choice) {
        case 1:
            rsaEncrypt();
            break;

        case 2:
            rsaDecrypt();
            break;

        case 3:
            inputMessageAndKeys();
            break;

        case 4:
            cout << "\nПрограмма завершена." << endl;
            return 0;

        default:
            cout << "Неверный выбор! Введите число от 1 до 4." << endl;
            break;
        }
    }

    return 0;
}
