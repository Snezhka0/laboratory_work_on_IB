/*
Наумова Снежана
Пи-41 дизайн
Лабораторная работа 2
Шифр Виженера
*/
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

// Проверка: символ = русская буква
bool isRussianLetter(char symbol) {
	unsigned char ucSymbol = symbol;
	return (ucSymbol >= 192 && ucSymbol <= 255);
}

// Проверка: цифр
bool isDigit(char symbol) {
	return (symbol >= '0' && symbol <= '9');
}

// Проверка: символ = английская буква
bool isEnglishLetter(char symbol) {
	return ((symbol >= 65 && symbol <= 90) || (symbol >= 97 && symbol <= 122));
}

// Номер русской буквы в алфавите
int indexByLetter(char letter) {
	unsigned char ucLetter = letter;
	if (ucLetter >= 192 && ucLetter <= 223) {
		return ucLetter - 192;
	}
	else if (ucLetter >= 224 && ucLetter <= 255) {
		return ucLetter - 224;
	}
	return -1;
}

// Русская буква по номеру в алфавите
char letterByIndex(int index, char letter) {
	unsigned char ucLetter = letter;
	if (ucLetter >= 192 && ucLetter <= 223) {
		return 192 + (index % 32);
	}
	else if (ucLetter >= 224 && ucLetter <= 255) {
		return 224 + (index % 32);
	}
	return letter;
}

// Получение сдвига от любого символа в ключе
int getShiftFromKeyChar(char keyChar) {
	if (isRussianLetter(keyChar)) {
		return indexByLetter(keyChar);  // Русские буквы = сдвиг 0-31
	}
	//Остальные символы = сдвиг 0
	return 0;
}

// Шифр Виженера (квадрат Виженера ROT0)
string cipherVigener(string text, string key, bool encrypt = true) {
	string resultText = "";
	int keyPos = 0;
	int keyLen = key.length();

	for (int i = 0; i < text.length(); i++) {
		char symbol = text[i];

		// Русская буква
		int textNum = indexByLetter(symbol);

		if (textNum != -1) {
			char keySymbol;
			int shift;

			// Подходящий символ в ключе
			bool found = false;
			int tries = 0;

			while (!found && tries < keyLen * 2) {
				keySymbol = key[keyPos % keyLen];
				shift = getShiftFromKeyChar(keySymbol); // Сдвиг

				// Следующий символ ключа
				keyPos++;
				tries++;

				// Сдвиг не 0
				if (shift != 0) {
					found = true;
				}
			}

			int newNum;

			if (encrypt) {
				// Шифруем
				newNum = (textNum + shift) % 32;
			}
			else {
				// Расшифровываем
				newNum = (textNum - shift + 32) % 32;
			}

			// Новуа буква
			char newSymbol = letterByIndex(newNum, symbol);
			resultText += newSymbol;

		}
		else {
			// Не русская буква
			resultText += symbol;
		}
	}

	return resultText;
}

// Проверка: корректность ввода текста (только русский)
string textCorrectness() {
	string text;
	const int MAX_TEXT_LENGTH = 1000;

	while (true) {
		cout << "Введите текст на русском языке: ";

		// Ошибка входного потока
		if (!getline(cin, text)) {
			cout << "Ошибка ввода! Программа завершена" << endl;
			return "";
		}

		// Пустой текст
		if (text.empty()) {
			cout << "Ошибка! Текст не может быть пустым." << endl;
			continue;
		}

		// Слишком длинный текст
		if (text.length() > MAX_TEXT_LENGTH) {
			cout << "Внимание! Текст слишком длинный. Будут обработаны первые "
				<< MAX_TEXT_LENGTH << " символов." << endl;
			text = text.substr(0, MAX_TEXT_LENGTH);
		}

		// Текст из пробелов
		bool onlySpaces = true;
		bool hasRussianLetters = false;
		bool hasEnglishLetters = false; // Английские буквы

		for (char c : text) {
			if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
				onlySpaces = false;

				if (isRussianLetter(c)) {
					hasRussianLetters = true;  // Русские буквы
				}
				else if (isEnglishLetter(c)) {
					hasEnglishLetters = true; // Английские буквы
				}
			}
		}

		if (onlySpaces) {
			cout << "Ошибка! Текст не может состоять только из пробелов." << endl;
			continue;
		}

		// Английские буквы
		if (hasEnglishLetters) {
			cout << "Ошибка! Текст содержит английские буквы." << endl;
			continue;
		}

		// Нет русских букв
		if (!hasRussianLetters) {
			cout << "Текст не содержит русских букв. Шифрование не изменит текст." << endl;
			cout << "Продолжить? (1 - да, 0 - ввести другой текст): ";

			int choice;
			cin >> choice;
			cin.ignore(1000, '\n');

			if (choice != 1) {
				continue;
			}
		}

		return text;
	}
}

// Проверка корректность ввода ключа (любые символы)
string keyCorrectness(const string& currentText = "") {
	string key;
	const int MAX_KEY_LENGTH = 1000;

	while (true) {
		cout << "Введите ключевое слово (любые символы): ";

		// Ошибка входного потока
		if (!getline(cin, key)) {
			cout << "Ошибка ввода! Программа завершена" << endl;
			return "";
		}

		// Пустой ключ
		if (key.empty()) {
			cout << "Ошибка! Ключ не может быть пустым." << endl;
			continue;
		}

		// Слишком длинный ключ
		if (key.length() > MAX_KEY_LENGTH) {
			cout << "Внимание! Ключ слишком длинный. Будут использованы первые "
				<< MAX_KEY_LENGTH << " символов." << endl;
			key = key.substr(0, MAX_KEY_LENGTH);
		}

		// Ключ <= открытый текст
		if (!currentText.empty()) {
			int textRussianLetters = 0;
			int keyRussianLetters = 0;

			// Кол-во русских букв в тексте
			for (char c : currentText) {
				if (isRussianLetter(c)) {
					textRussianLetters++;
				}
			}

			// Только русские буквы в ключе
			for (char c : key) {
				if (isRussianLetter(c)) {
					keyRussianLetters++;
				}
			}

			// Проверяем условия
			if (keyRussianLetters > textRussianLetters) {
				cout << "Ошибка! В ключе больше русских букв (" << keyRussianLetters
					<< "), чем в тексте (" << textRussianLetters << ")." << endl;
				continue;
			}
		}

		// Ключ из пробелов
		bool onlySpaces = true;
		for (char c : key) {
			if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
				onlySpaces = false;
				break;
			}
		}

		if (onlySpaces) {
			cout << "Ошибка! Ключ не может состоять только из пробелов." << endl;
			continue;
		}

		return key;
	}
}

// Вывод меню
void showMenu() {
	cout << "\nМЕНЮ" << endl;
	cout << "1. Зашифровать текст" << endl;
	cout << "2. Расшифровать текст" << endl;
	cout << "3. Ввести новый текст и ключ" << endl;
	cout << "0. Выход" << endl;
	cout << "Выберите действие: ";
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	string text;
	string key;
	string encryptedText;
	bool hasEncryptedText = false;

	cout << "Ввод текста" << endl;
	text = textCorrectness();
	if (text.empty()) {
		return 0;
	}

	cout << "\nВвод ключа" << endl;
	key = keyCorrectness(text);
	if (key.empty()) {
		return 0;
	}

	int choice;
	do {
		showMenu();
		cin >> choice;
		cin.ignore(1000, '\n');

		switch (choice) {
		case 1:
			cout << "\nШифрование" << endl;
			cout << "Исходный текст: " << text << endl;
			cout << "Ключ: " << key << endl;
			encryptedText = cipherVigener(text, key, true);
			cout << "Зашифрованный текст: " << encryptedText << endl;
			hasEncryptedText = true;
			break;

		case 2:
			cout << "\nРасшифровка" << endl;
			if (hasEncryptedText) {
				cout << "Зашифрованный текст: " << encryptedText << endl;
				cout << "Ключ: " << key << endl;
				string decryptedText = cipherVigener(encryptedText, key, false);
				cout << "Расшифрованный текст: " << decryptedText << endl;
			}
			else {
				cout << "Ошибка! Сначала нужно зашифровать текст (пункт 1)." << endl;
			}
			break;

		case 3:
			cout << "\nВвод нового текста и ключа" << endl;
			text = textCorrectness();
			hasEncryptedText = false;

			// Проверка ключа
			key = keyCorrectness(text);
			if (key.empty()) {
				return 0;
			}
			break;

		case 0:
			cout << "\nВыход из программы..." << endl;
			break;

		default:
			cout << "Ошибка! Неверный выбор. Попробуйте еще раз." << endl;
			break;
		}

	} while (choice != 0);

	return 0;
}
