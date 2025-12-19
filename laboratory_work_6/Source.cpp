/*
Наумова Снежана
Пи-41 дизайн
Лабораторная работа 6
Хэширование (по алгоритму MD5)
*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace std;

// Глобальные переменные
string message = "";           // Исходное сообщение
string MD5Hash = "";          // Результат хеширования

// Функция: проверка допустимости символов в сообщении
bool isValidMessage(const string& message) {
    if (message.empty()) {
        cout << "Ошибка: Сообщение не может быть пустым!" << endl;
        return false;
    }

    for (char symbol : message) {
        if (!((symbol >= 'A' && symbol <= 'Z') || // A-Z
            (symbol >= 'a' && symbol <= 'z') || // a-z
            (symbol >= '0' && symbol <= '9') || // 0-9
            (symbol >= '!' && symbol <= '/') || // !"#$%&'()*+,-./
            (symbol >= ':' && symbol <= '@') || // :;<=>?@
            (symbol >= '[' && symbol <= '`') || // [\]^_`
            (symbol >= '{' && symbol <= '~'))) { // {|}~
            cout << "Ошибка! Можно вводить: английские буквы, цифры и символы." << endl;
            return false;
        }
    }

    return true;
}

// Функция: вычисление констант
vector<uint32_t> MD5Table() {
    vector<uint32_t> Table(64);

    // T[i] = floor(2^32 * |sin(i+1)|)
    for (int i = 0; i < 64; i++) {
        // Все в одной строке
        Table[i] = static_cast<uint32_t>(4294967296.0 * fabs(sin(i + 1.0)));
    }

    return Table;
}

// Функция: генерация сдвигов для каждого раунда
vector<uint32_t> tableSubstitution() {
    vector<uint32_t> Substitution(64);

    // 4 набора сдвигов для 4 функций
    uint32_t shift_sets[4][4] = {
        {7, 12, 17, 22},  // Для F
        {5,  9, 14, 20},  // Для G  
        {4, 11, 16, 23},  // Для H
        {6, 10, 15, 21}   // Для I
    };

    // Заполнение таблицы
    for (int i = 0; i < 64; i++) {
        // Определение набора
        int set_num = i / 16;      // 0-3 (0: раунды 0-15, 1: 16-31, 2: 32-47, 3: 48-63)
        int value_num = i % 4;     // 0-3 (значение из набора)

        Substitution[i] = shift_sets[set_num][value_num];
    }

    return Substitution;
}

// Функция: ввод сообщения
void inputMessage() {
    bool valid = false;

    while (!valid) {
        cout << "Введите сообщение (английские буквы, цифры, символы, без пробелов): ";
        getline(cin, message);

        valid = isValidMessage(message);
    }

    MD5Hash = "";  // Сбрасываем предыдущий хеш
}

// Функция: циклический сдвиг 32-битного числа влево
uint32_t rotateLeft(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

// Функция: преобразование в little-endian (32-битное число в 4 байта) Пр: 1234 = четыре тридцать двадцать тысяча
vector<uint8_t> toLittleEndian(uint32_t value) {
    vector<uint8_t> result(4);
    result[0] = static_cast<uint8_t>(value & 0xFF);
    result[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    result[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    result[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    return result;
}

// Функция: функции MD5 (F, G, H, I)
uint32_t F(uint32_t b, uint32_t c, uint32_t d) { // 0-16 раунды
    return (b & c) | (~b & d); // биты = 1 в b и c | биты = 1 в d и не = 1 в b
}

uint32_t G(uint32_t b, uint32_t c, uint32_t d) { // 16-31 раунды
    return (d & b) | (~d & c); // биты = 1 в d и b | биты = 1 в c и не = 1 в d
}

uint32_t H(uint32_t b, uint32_t c, uint32_t d) { // 32-47 раунды
    return b ^ c ^ d; // XOR (1 при нечетном кол-ве 1-иц)

}

uint32_t I(uint32_t b, uint32_t c, uint32_t d) { // 47-63 раунды
    return c ^ (b | ~d); // Комбинация всех операций
}

// Функция: дополнение сообщения по стандарту MD5
vector<uint8_t> additionMessage(const string& message) {
    vector<uint8_t> bytes(message.begin(), message.end());

    // + бит "1" (0x80)
    bytes.push_back(0x80);

    // + нули до длины, кратной 64 байтам - 8
    while ((bytes.size() + 8) % 64 != 0) {
        bytes.push_back(0x00);
    }

    // + исходная длина сообщения в битах (little-endian)
    uint64_t originalBits = message.length() * 8;

    for (int i = 0; i < 8; i++) {
        bytes.push_back(static_cast<uint8_t>(originalBits & 0xFF));
        originalBits >>= 8;
    }

    return bytes;
}

// Функция: обработка одного 512-битного блока
void processBlock(const vector<uint8_t>& block, uint32_t& a0, uint32_t& b0, uint32_t& c0, uint32_t& d0) {

    vector<uint32_t> T = MD5Table();
    vector<uint32_t> S = tableSubstitution();

    // Разбиение блока на 16 слов по 32 бита (little-endian)
    uint32_t messageBlock[16];

    for (int i = 0; i < 16; i++) {
        messageBlock[i] = (block[i * 4]) |
            (block[i * 4 + 1] << 8) |
            (block[i * 4 + 2] << 16) |
            (block[i * 4 + 3] << 24);
    }

    // Начальные значения
    uint32_t A = a0;
    uint32_t B = b0;
    uint32_t C = c0;
    uint32_t D = d0;

    // Раунды
    for (int i = 0; i < 64; i++) {
        uint32_t FResult, g;

        if (i < 16) {
            FResult = F(B, C, D);
            g = i;
        }
        else if (i < 32) {
            FResult = G(B, C, D);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48) {
            FResult = H(B, C, D);
            g = (3 * i + 5) % 16;
        }
        else {
            FResult = I(B, C, D);
            g = (7 * i) % 16;
        }

        uint32_t temp = D;
        D = C;
        C = B;

        uint32_t addition = A + FResult + T[i] + messageBlock[g];
        B = B + rotateLeft(addition, S[i]);

        A = temp;
    }

    // Добавляем к начальным значениям
    a0 += A;
    b0 += B;
    c0 += C;
    d0 += D;
}


// Функция: вычисление хеша
string computeHash(const string& message) {

    // Начальные значения
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    // Дополнение сообщения
    vector<uint8_t> extendedMessage = additionMessage(message);

    // Обработка блоков
    for (size_t i = 0; i < extendedMessage.size(); i += 64) {
        // Проверка: достаточно данных для блока
        if (i + 64 <= extendedMessage.size()) {
            vector<uint8_t> block(extendedMessage.begin() + i, extendedMessage.begin() + i + 64);
            processBlock(block, a0, b0, c0, d0);
        }
    }

    // 4. Формирование итоговой строки (little-endian)
    vector<uint8_t> resultBytes;

    // Преобразование значений в little-endian
    vector<uint8_t> aBytes = toLittleEndian(a0);
    vector<uint8_t> bBytes = toLittleEndian(b0);
    vector<uint8_t> cBytes = toLittleEndian(c0);
    vector<uint8_t> dBytes = toLittleEndian(d0);

    // Собирка байтов вместе
    for (uint8_t byte : aBytes) resultBytes.push_back(byte);
    for (uint8_t byte : bBytes) resultBytes.push_back(byte);
    for (uint8_t byte : cBytes) resultBytes.push_back(byte);
    for (uint8_t byte : dBytes) resultBytes.push_back(byte);

    // 5. Преобразование в 16-ричную строку
    stringstream strStream; // Потоковая строка
    strStream << hex << setfill('0');

    for (uint8_t byte : resultBytes) {
        strStream << setw(2) << static_cast<int>(byte);
    }

    return strStream.str();
}

// Функция: хеширование сообщения
void hashMessage() {
    if (message.empty()) {
        cout << "Ошибка: Сначала введите сообщение!" << endl;
        return;
    }

    cout << "Исходное сообщение: \"" << message << "\"" << endl;
    cout << "Длина сообщения: " << message.length() << " символов" << endl;

    // Вычисление хэша
    MD5Hash = computeHash(message);

    cout << "\nРезультат хеширования MD5:" << endl;
    cout << MD5Hash << endl;

    // Вывод
    cout << "\nРазбиение хеша (для проверки):" << endl;
    cout << "Часть 1: " << MD5Hash.substr(0, 8) << endl;
    cout << "Часть 2: " << MD5Hash.substr(8, 8) << endl;
    cout << "Часть 3: " << MD5Hash.substr(16, 8) << endl;
    cout << "Часть 4: " << MD5Hash.substr(24, 8) << endl;
    cout << "Всего 32 шестнадцатеричных символа (128 бит)" << endl;
}

// Функция: отображение меню
void showMenu() {
    cout << "\nТекущие данные:" << endl;
    cout << "  Сообщение: " << (message.empty() ? "не введено" : "\"" + message + "\"") << endl;
    cout << "  Длина сообщения: " << (message.empty() ? "0" : to_string(message.length())) << " символов" << endl;
    cout << "  MD5 хеш: " << (MD5Hash.empty() ? "не вычислен" : MD5Hash) << endl;

    cout << "\nГлавное меню:" << endl;
    cout << "1. Хэшировать сообщение" << endl;
    cout << "2. Ввести новое сообщение" << endl;
    cout << "3. Выход" << endl;

    cout << "\nВаш выбор: ";
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Ввод сообщения (при одинаковом вводе - одинаковый результат хэширования)
    inputMessage();

    int choice = 0;

    while (true) {
        showMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите число от 1 до 3." << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        switch (choice) {
        case 1:
            hashMessage();
            break;

        case 2:
            inputMessage();
            break;

        case 3:
            cout << "\nПрограмма завершена." << endl;
            return 0;

        default:
            cout << "Неверный выбор! Введите число от 1 до 3." << endl;
            break;
        }
    }

    return 0;
}
