/*
Наумова Снежана
Пи-41 дизайн
Лабораторная работа 3
Шифр RC5 (на 6 битах по алгоритму из лекции)
*/

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

// Константы
const int bits = 6;              // 6 бит в слове
const int ModValue = 64;         // 2^6 = 64
const int BlockSize = 2;         // Блок из 2 слов (A и B)

// Глобальные переменные
string message = "";             // Исходное сообщение
string key = "";                 // Ключ
int R = 0;                       // Количество раундов
vector<unsigned int> Q;          // Массив подключей (Q0,...,Q(2R+2))
vector<unsigned int> encryptedValues; // Зашифрованное сообщение
bool isEncrypted = false;        // Флаг: зашифровано ли сообщение

// Функция: циклический сдвиг влево
unsigned int rotateLeft(unsigned int value, unsigned int shift) {
    shift = shift % bits;  // 6 бит для сдвига
    unsigned int mask = (1 << bits) - 1;  // Маска 6 бит = 0x3F

    shift = shift % bits;
    unsigned int result = ((value << shift) | (value >> (bits - shift)));
    return result & mask;
}

// Функция: циклический сдвиг вправо
unsigned int rotateRight(unsigned int value, unsigned int shift) {
    shift = shift % bits;  // 6 бит для сдвига
    unsigned int mask = (1 << bits) - 1;  // Маска 6 бит = 0x3F

    shift = shift % bits;
    unsigned int result = ((value >> shift) | (value << (bits - shift)));
    return result & mask;
}

// Преобразование символа в 6-битное число (0-63)
int charTo6bit(char c) {
    if (c >= 'A' && c <= 'Z')
        return c - 'A';                    // A=0,..., Z=25
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;               // a=26,..., z=51
    else if (c >= '0' && c <= '9')
        return c - '0' + 52;               // 0=52,..., 9=61
    else if (c == ' ')
        return 62;                         // Пробел = 62
    else
        return 63;                         // Резерв
}

// Преобразование 6-битного числа в символ
char sixbitToChar(int n) {
    n = n & 0x3F; // 6 бит

    if (n >= 0 && n <= 25)
        return 'A' + n;
    else if (n >= 26 && n <= 51)
        return 'a' + (n - 26);
    else if (n >= 52 && n <= 61)
        return '0' + (n - 52);
    else if (n == 62)
        return ' ';
    else
        return '?'; // Если что-то пошло не так
}

// Функция: проверка корректности символа сообщения и ключа
bool isValidChar(char c) {
    // Английские буквы (A-Z, a-z), цифры (0-9) или пробел
    return (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') ||
        (c == ' ');
}

// Проверка всего сообщения
bool checkMessage(const string& message) {
    // Пустое сообщение
    if (message.empty()) {
        cout << "Ошибка: Сообщение не может быть пустым!" << endl;
        return false;
    }

    // Каждый символ сообщения
    for (int i = 0; i < message.length(); i++) {
        char symbol = message[i];
        if (!isValidChar(symbol)) {
            cout << "Ошибка: Недопустимый символ в сообщении! Разрешены только: английские буквы, цифры и пробел." << endl;
            return false;
        }
    }

    return true;
}

// Проверка всего ключа
bool checkKey(const string& key) {
    // Пустой ключ
    if (key.empty()) {
        cout << "Ошибка: Ключ не может быть пустым!" << endl;
        return false;
    }

    // Каждый символ ключа
    for (int i = 0; i < key.length(); i++) {
        char symbol = key[i];
        if (!isValidChar(symbol)) {
            cout << "Ошибка: Недопустимый символ в ключе! Разрешены только: английские буквы, цифры и пробел." << endl;
            return false;
        }
    }

    return true;
}

// Проверка количества раундов
bool checkRounds(int R) {
    if (R < 1 || R > 5) {
        cout << "Ошибка: Количество раундов должно быть от 1 до 5!" << endl;
        return false;
    }
    return true;
}

// Функция: ввод сообщения с проверками
void inputMessage() {
    bool valid = false;

    while (!valid) {
        cout << "Введите сообщение: ";
        getline(cin, message);

        // Проверка сообщения
        valid = checkMessage(message);

        if (!valid) {
            cout << "\nПопробуйте снова." << endl;
        }
    }

    cout << "Сообщение успешно введено!" << endl;
    isEncrypted = false;
}

// Функция: ввод ключа с проверками
void inputKey() {
    bool valid = false;

    while (!valid) {
        cout << "\nВведите ключ: ";
        getline(cin, key);

        // Проверка ключа
        valid = checkKey(key);

        if (!valid) {
            cout << "\nПопробуйте снова." << endl;
        }
    }

    cout << "Ключ успешно введен!" << endl;
    isEncrypted = false;
}

// Функция: ввод количества раундов с проверками
void inputRounds() {
    bool valid = false;
    string input;

    while (!valid) {
        cout << "\nВведите количество раундов (от 1 до 5): ";
        getline(cin, input);

        try {
            // Преобразование строки в число
            R = stoi(input);

            // Проверка диапазона
            if (checkRounds(R)) {
                valid = true;
            }
        }
        catch (const invalid_argument& e) { // Исключение - некорректный аргумент
            cout << "Ошибка: Введено не число!" << endl;
        }
        catch (const out_of_range& e) { // Исключение - выход из диапозона
            cout << "Ошибка: Слишком большое число!" << endl;
        }

        if (!valid) {
            cout << "\nПопробуйте снова." << endl;
        }
    }

    cout << "Количество раундов успешно введено!" << endl;
    isEncrypted = false;
}

// Функция: генерация подключей
void generateSubKeys() {
    int totalKeys = 2 * R + 2;
    Q.resize(totalKeys);

    // Генерация подключей из ключа
    for (int i = 0; i < totalKeys; i++) {
        if (i < key.length()) {
            Q[i] = charTo6bit(key[i]);
        }
        else {
            int pos = i % key.length();
            Q[i] = charTo6bit(key[pos]);
        }
        // + индекс для разнообразия
        Q[i] = (Q[i] + i) & 0x3F;
    }
}

// Шифрование RC5 по алгоритму 1 из лекции
void cipherRC5(unsigned int& A, unsigned int& B) {
    // 1 Шаг: i = 1
    int i = 1;

    // A=(A+Q0)mod 2^6
    A = (A + Q[0]) & 0x3F;

    // B=(B+Q1)mod 2^6
    B = (B + Q[1]) & 0x3F;

    // 2 Шаг: Основной цикл
    while (i <= R) {
        // A = {[(A XOR B)<<<B]+Q от 2i}mod2^6
        unsigned int tempA = A;
        unsigned int xorAB = A ^ B;
        unsigned int rotatedA = rotateLeft(xorAB, B & 0x3F); // 6 бит
        A = (rotatedA + Q[2 * i]) & 0x3F;

        // B = {[(B XOR A)<<<A]+Q от (2i+1)}mod2^6
        unsigned int xorBA = B ^ A;
        unsigned int rotatedB = rotateLeft(xorBA, A & 0x3F); // 6 бит
        B = (rotatedB + Q[2 * i + 1]) & 0x3F;

        // 3 Шаг: Проверка условия
        if (i != R) {
            i++;
        }
        else {
            break;
        }
    }
}

// Расшифровка RC5 по алгоритму 2 из лекции
void decryptRC5(unsigned int& A, unsigned int& B) {
    // 1 Шаг: i = R
    int i = R;

    // 2 Шаг: Основной цикл
    while (i >= 1) {
        // B = {[(B - Q от (2i+1))mod2^6]>>>A} XOR A
        unsigned int subB = (B + 64 - Q[2 * i + 1]) & 0x3F;
        unsigned int rotatedB = rotateRight(subB, A & 0x3F);
        B = rotatedB ^ A;

        // A = {[(A - Q от (2i))mod2^6]>>>B} XOR B
        unsigned int subA = (A + 64 - Q[2 * i]) & 0x3F;
        unsigned int rotatedA = rotateRight(subA, B & 0x3F);
        A = rotatedA ^ B;

        // 3 Шаг: Проверка условия
        if (i != 1) {
            i--;
        }
        else {
            break;
        }
    }

    // 4 Шаг: B=(B-Q1)mod 2^6
    B = (B + 64 - Q[1]) & 0x3F;

    // A=(A-Q0)mod 2^6
    A = (A + 64 - Q[0]) & 0x3F;
}

// Функция: шифрование сообщения
void encryptMessage() {
    if (message.empty() || key.empty() || R == 0) {
        cout << "Ошибка: Сначала введите сообщение, ключ и количество раундов!" << endl;
        return;
    }

    // Генерация подключей
    generateSubKeys();

    // Преобразование сообщения в 6-битные числа
    vector<unsigned int> sixBitValues;
    for (char c : message) {
        int val = charTo6bit(c);
        sixBitValues.push_back(val);
    }

    // Разбивка на блоки по 2 слова (A и B)
    int originalLength = sixBitValues.size();
    if (sixBitValues.size() % 2 != 0) {
        sixBitValues.push_back(63); // Добавляем '?' для выравнивания
    }

    // Шифр каждого блока
    encryptedValues.clear();

    for (int i = 0; i < sixBitValues.size(); i += 2) {
        unsigned int A = sixBitValues[i];
        unsigned int B = sixBitValues[i + 1];

        cipherRC5(A, B);

        encryptedValues.push_back(A);
        encryptedValues.push_back(B);
    }

    // Вывод результата
    cout << "\nЗашифрованное сообщение: ";
    for (unsigned int val : encryptedValues) {
        cout << sixbitToChar(val);
    }
    cout << endl;

    isEncrypted = true;
}

// Функция: расшифрование сообщения
void decryptMessage() {
    if (!isEncrypted) {
        cout << "Ошибка: Сначала нужно зашифровать сообщение!" << endl;
        return;
    }

    if (encryptedValues.empty()) {
        cout << "Ошибка: Нет зашифрованного сообщения для расшифровки!" << endl;
        return;
    }

    // Генерация подключей
    generateSubKeys();

    // Расшифровка каждого блока
    vector<unsigned int> decryptedValues;

    for (int i = 0; i < encryptedValues.size(); i += 2) {
        unsigned int A = encryptedValues[i];
        unsigned int B = encryptedValues[i + 1];

        decryptRC5(A, B);

        decryptedValues.push_back(A);
        decryptedValues.push_back(B);
    }

    // Сборка расшифрованного сообщения
    string decryptedMessage = "";
    for (int i = 0; i < message.length(); i++) {
        decryptedMessage += sixbitToChar(decryptedValues[i]);
    }

    // Вывод результата
    cout << "\nРасшифрованное сообщение: " << decryptedMessage << endl;
}

// Функция: ввод новых данных
void inputNewData() {
    inputMessage();
    inputKey();
    inputRounds();
}

// Функция: отображение меню
void showMenu() {
    cout << "\nТекущие данные:" << endl;
    cout << "  Сообщение: " << (message.empty() ? "не введено" : "\"" + message + "\"") << endl;
    cout << "  Ключ: " << (key.empty() ? "не введен" : "\"" + key + "\"") << endl;
    cout << "  Раунды: " << (R == 0 ? "не введены" : to_string(R)) << endl;
    cout << "  Статус: " << (isEncrypted ? "зашифровано" : "не зашифровано") << endl;
    cout << "\nВыберите действие:" << endl;
    cout << "1. Зашифровать" << endl;
    cout << "2. Расшифровать" << endl;
    cout << "3. Ввести новые данные" << endl;
    cout << "4. Выход" << endl;
    cout << "Введите номер действия: ";
}

// Основная функция
int main() {
    setlocale(LC_ALL, "Russian");

    // Ввод данных
    inputNewData();

    int choice = 0;

    // Основной цикл меню
    while (true) {
        showMenu();

        // Проверка корректности ввода
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка: Введите число от 1 до 4!" << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        switch (choice) {
        case 1:
            encryptMessage();
            break;
        case 2:
            decryptMessage();
            break;
        case 3:
            inputNewData();
            break;
        case 4:
            cout << "\nВыход из программы." << endl;
            return 0;
        default:
            cout << "Ошибка: Неверный выбор! Введите число от 1 до 4." << endl;
            break;
        }
    }

    return 0;
}
