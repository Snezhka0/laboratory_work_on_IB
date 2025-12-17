/*
Наумова Снежана
Пи-41 дизайн
Лабораторная работа 4
Поточный шифр на RC5 (на 6 битах)
*/

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Константы
const int bits = 6;              // 6 бит в слове

// Глобальные переменные
string message = "";             // Исходное сообщение
string key = "";                 // Ключ
int R = 0;                       // Количество раундов
vector<unsigned int> Q;          // Массив подключей (Q0,...,Q(2R+2))
vector<unsigned int> encryptedValues; // Зашифрованное сообщение
bool isEncrypted = false;        // Флаг: зашифровано ли сообщение
vector<unsigned int> keyStream;  // Ключевой поток
unsigned int counter = 0;        // Счетчик для генерации ключевого потока

// Функция: циклический сдвиг влево
unsigned int rotateLeft(unsigned int value, unsigned int shift) {
    shift = shift % bits;
    unsigned int mask = (1 << bits) - 1;
    return ((value << shift) | (value >> (bits - shift))) & mask;
}

// Преобразование символа в 6-битное число (0-63)
int charTo6bit(char c) {
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    else if (c >= '0' && c <= '9')
        return c - '0' + 52;
    else if (c == ' ')
        return 62;
    else
        return 63;
}

// Преобразование 6-битного числа в символ
char sixbitToChar(int n) {
    n = n & 0x3F;
    if (n >= 0 && n <= 25)
        return 'A' + n;
    else if (n >= 26 && n <= 51)
        return 'a' + (n - 26);
    else if (n >= 52 && n <= 61)
        return '0' + (n - 52);
    else if (n == 62)
        return ' ';
    else
        return '?';
}

// Функция: проверка корректности символа сообщения и ключа
bool isValidChar(char c) {
    return (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') ||
        (c == ' ');
}

// Проверка всего сообщения
bool checkMessage(const string& message) {
    if (message.empty()) {
        cout << "Ошибка: Сообщение не может быть пустым!" << endl;
        return false;
    }
    for (int i = 0; i < message.length(); i++) {
        if (!isValidChar(message[i])) {
            cout << "Ошибка: Недопустимый символ в сообщении!" << endl;
            return false;
        }
    }
    return true;
}

// Проверка всего ключа
bool checkKey(const string& key) {
    if (key.empty()) {
        cout << "Ошибка: Ключ не может быть пустым!" << endl;
        return false;
    }
    for (int i = 0; i < key.length(); i++) {
        if (!isValidChar(key[i])) {
            cout << "Ошибка: Недопустимый символ в ключе!" << endl;
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
        valid = checkMessage(message);
        if (!valid) cout << "\nПопробуйте снова." << endl;
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
        valid = checkKey(key);
        if (!valid) cout << "\nПопробуйте снова." << endl;
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
            R = stoi(input);
            if (checkRounds(R)) valid = true;
        }
        catch (const invalid_argument& e) {
            cout << "Ошибка: Введено не число!" << endl;
        }
        catch (const out_of_range& e) {
            cout << "Ошибка: Слишком большое число!" << endl;
        }
        if (!valid) cout << "\nПопробуйте снова." << endl;
    }
    cout << "Количество раундов успешно введено!" << endl;
    isEncrypted = false;
}

// Функция: генерация подключей
void generateSubKeys() {
    int totalKeys = 2 * R + 2;
    Q.resize(totalKeys);
    for (int i = 0; i < totalKeys; i++) {
        if (i < key.length()) {
            Q[i] = charTo6bit(key[i]);
        }
        else {
            int pos = i % key.length();
            Q[i] = charTo6bit(key[pos]);
        }
        Q[i] = (Q[i] + i) & 0x3F;
    }
}

// Шифрование RC5 по алгоритму 1 из лекции
void cipherRC5(unsigned int& A, unsigned int& B) {
    int i = 1;
    A = (A + Q[0]) & 0x3F;
    B = (B + Q[1]) & 0x3F;

    while (i <= R) {
        unsigned int xorAB = A ^ B;
        unsigned int rotatedA = rotateLeft(xorAB, B & 0x3F);
        A = (rotatedA + Q[2 * i]) & 0x3F;

        unsigned int xorBA = B ^ A;
        unsigned int rotatedB = rotateLeft(xorBA, A & 0x3F);
        B = (rotatedB + Q[2 * i + 1]) & 0x3F;

        if (i != R) i++;
        else break;
    }
}

// Функция: генерация ключевого потока
void generateKeyStream(int length) {
    keyStream.clear(); // Очистка предыдущего ключевого потока

    for (int i = 0; i < length; i++) {
        // Новое состояние (каждые 2 символа)
        if (i % 2 == 0) {
            // A и B на основе текущего счетчика
            unsigned int A = counter;
            unsigned int B = counter ^ 0x3F;

            // Шифр состояния
            cipherRC5(A, B);

            // + A в поток
            keyStream.push_back(A);

            // Не последний символ => + B в поток
            if (i + 1 < length) {
                keyStream.push_back(B);
                i++;
            }

            counter++;
        }
    }
}

// Функция: поточное шифрование
void streamEncrypt() {
    if (message.empty() || key.empty() || R == 0) {
        cout << "Ошибка: Сначала введите сообщение, ключ и количество раундов!" << endl;
        return;
    }

    generateSubKeys();

    vector<unsigned int> messageValues;
    for (char c : message) {
        messageValues.push_back(charTo6bit(c));
    }

    counter = 0;
    generateKeyStream(messageValues.size());

    // Шифр (каждый символ XOR ключевой поток)
    encryptedValues.clear();
    for (int i = 0; i < messageValues.size(); i++) {
        unsigned int encrypted = messageValues[i] ^ keyStream[i];
        encryptedValues.push_back(encrypted);
    }

    cout << "\nЗашифрованное сообщение: ";
    for (unsigned int val : encryptedValues) {
        cout << sixbitToChar(val);
    }
    cout << endl;

    isEncrypted = true;
}

// Функция: поточное расшифрование
void streamDecrypt() {
    if (!isEncrypted) {
        cout << "Ошибка: Сначала нужно зашифровать сообщение!" << endl;
        return;
    }
    if (encryptedValues.empty()) {
        cout << "Ошибка: Нет зашифрованного сообщения для расшифровки!" << endl;
        return;
    }

    generateSubKeys();

    counter = 0;
    generateKeyStream(encryptedValues.size());

    // Расшифровка: шифротекст XOR ключевой поток
    vector<unsigned int> decryptedValues;
    for (int i = 0; i < encryptedValues.size(); i++) {
        unsigned int decrypted = encryptedValues[i] ^ keyStream[i];
        decryptedValues.push_back(decrypted);
    }

    string decryptedMessage = "";
    for (unsigned int val : decryptedValues) {
        decryptedMessage += sixbitToChar(val);
    }
    cout << "\nРасшифрованное сообщение: " << decryptedMessage << endl;
}

// Функция: ввод новых данных
void inputNewData() {
    inputMessage();
    inputKey();
    inputRounds();
    counter = 0;
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

    inputNewData();

    int choice = 0;
    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка: Введите число от 1 до 4!" << endl;
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
        case 1:
            streamEncrypt();
            break;
        case 2:
            streamDecrypt();
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
