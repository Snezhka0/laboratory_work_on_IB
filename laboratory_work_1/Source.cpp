/*
Наумова Снежана
Пи-41 дизайн
Лабораторная работа 1
Шифр Цезаря
*/
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

// Функция для определения языка текста
char detectTextLanguage(const string& text) {
	bool hasRussian = false;
	bool hasEnglish = false;

	for (unsigned char symbol : text) {
		// Русские буквы (большие/маленькие)
		if ((symbol >= 192 && symbol <= 255)) {
			hasRussian = true;
		}
		// Английские буквы (большие/маленькие)
		else if ((symbol >= 65 && symbol <= 90) || (symbol >= 97 && symbol <= 122)) {
			hasEnglish = true;
		}
	}
	if (hasRussian && hasEnglish) return 'B'; // Смешанный
	if (hasRussian) return 'R'; // Русский
	if (hasEnglish) return 'E'; // Английский
	return 'N'; // Нет букв
}

// Функция для проверки ключа в зависимости от языка
bool isValidKey(int key, char language) {
	if (language == 'R') return (key >= -32 && key <= 32); // Русский
	if (language == 'E') return (key >= -26 && key <= 26); // Английский
	return true; // Нет букв - любой ключ
}

// Функция шифр Цезаря
string caesarCipher(string text, int key) {
	string rezultText = "";

	for (unsigned char symbol : text) {
		// русские маленькие буквы (224-255)
		if (symbol >= 224 && symbol <= 255) {
			int shifted = symbol + key;
			if (shifted > 255) {  // больше я
				shifted = 224 + (shifted - 256);  // возврат к а
			}
			else if (shifted < 224) {  // меньше а (отрицательный ключ)
				shifted = 255 - (223 - shifted);  // возврат к я
			}
			rezultText += static_cast<char>(shifted);  // Явное преобразование
		}
		// русские большие буквы (192-223)
		else if (symbol >= 192 && symbol <= 223) {
			int shifted = symbol + key;
			if (shifted > 223) {  // больше Я
				shifted = 192 + (shifted - 224);  // возврат к А
			}
			else if (shifted < 192) {  // меньше А (отрицательный ключ)
				shifted = 223 - (191 - shifted);  // возврат к Я
			}
			rezultText += static_cast<char>(shifted);  // Явное преобразование
		}
		// английские маленькие буквы (97-122)
		else if (symbol >= 97 && symbol <= 122) {
			int shifted = symbol + key;
			if (shifted > 122) {
				shifted = 97 + (shifted - 122 - 1); // возврат к a
			}
			else if (shifted < 97) { // меньше a (отрицательный ключ)
				shifted = 122 - (97 - shifted - 1); // возврат к z
			}
			rezultText += static_cast<char>(shifted);  // Явное преобразование
		}
		// английские большие буквы (65-90)
		else if (symbol >= 65 && symbol <= 90) {
			int shifted = symbol + key;
			if (shifted > 90) {
				shifted = 65 + (shifted - 90 - 1); // возврат к A
			}
			else if (shifted < 65) { // меньше A (отрицательный ключ)
				shifted = 90 - (65 - shifted - 1);  // возврат к Z
			}
			rezultText += static_cast<char>(shifted);  // Явное преобразование
		}
		// Остальные символы
		else {
			rezultText += symbol;
		}
	}
	return rezultText;
}

// Функция для взлома шифра (частотный анализ)
void crackCipher(string encryptedText) {
	cout << "\nВзлом шифра (частотный анализ)" << endl;

	char language = detectTextLanguage(encryptedText);

	if (language != 'R' && language != 'E') {
		cout << "Частотный анализ не сработает: текст не содержит букв или смешанный." << endl;
		return;
	}

	// Массив для подсчета букв (256 элементов)
	int letterCount[256] = { 0 };
	int totalLetters = 0;

	// Подсчитываем буквы
	for (unsigned char c : encryptedText) {
		if ((c >= 192 && c <= 255) ||  // русские
			(c >= 65 && c <= 90) ||    // английские большие
			(c >= 97 && c <= 122)) {   // английские маленькие
			letterCount[c]++;
			totalLetters++;
		}
	}

	cout << "\n1. Всего букв в тексте: " << totalLetters << endl;

	// Самая частая буква
	char mostCommon = ' ';
	int maxCount = 0;

	for (int i = 0; i < 256; i++) {
		if (letterCount[i] > maxCount) {
			maxCount = letterCount[i];
			mostCommon = char(i);
		}
	}

	cout << "2. Самая частая буква: '" << mostCommon << "' (встречается "
		<< maxCount << " раз, "
		<< (totalLetters > 0 ? (maxCount * 100 / totalLetters) : 0)
		<< "% текста)" << endl;

	// Предположение, какая это буква в исходном тексте
	char normalLetter;
	int maxKey;

	if (language == 'R') {
		cout << "3. В русском языке самая частая буква - 'о'" << endl;
		normalLetter = 'о';  // русская 'о' 
		maxKey = 32;
	}
	else {
		cout << "3. В английском языке самая частая буква - 'e'" << endl;
		normalLetter = 'e';  // английская 'e'
		maxKey = 26;
	}

	// Предполагаемый ключ
	int guessedKey = mostCommon - normalLetter;

	// Корректируем ключа (выходит за пределы)
	if (guessedKey > maxKey) guessedKey -= (maxKey + 1);
	if (guessedKey < -maxKey) guessedKey += (maxKey + 1);

	cout << "4. Возможно, ключ = " << guessedKey << endl;
	cout << "   (потому что '" << mostCommon << "' - '" << normalLetter
		<< "' = " << guessedKey << ")" << endl;

	cout << "\n5. Взлом с этим ключом:" << endl;
	string decrypted = caesarCipher(encryptedText, -guessedKey);
	cout << "Ключ " << guessedKey << ": " << decrypted << endl;

	// Проверяем, похоже ли на нормальный текст
	cout << "\nПохоже на нормальный текст? (y(д) / n(н)): ";
	char answer;
	cin >> answer;
	cin.ignore(1000, '\n');

	if (answer == 'y' || answer == 'Y' || answer == 196 || answer == 228) {
		cout << "\nШифр взломан с помощью частотного анализа." << endl;
		cout << "Найденный ключ: " << guessedKey << endl;
		return;
	}

	// Не получилось => вычисления с другими частыми буквами
	cout << "\n6. Другие варианты:" << endl;

	// 3 самые частые буквы (перебор)
	char top3[3] = { ' ', ' ', ' ' };
	int counts[3] = { 0, 0, 0 };

	for (int i = 0; i < 256; i++) {
		if (letterCount[i] > counts[0]) {
			counts[2] = counts[1];
			top3[2] = top3[1];
			counts[1] = counts[0];
			top3[1] = top3[0];
			counts[0] = letterCount[i];
			top3[0] = char(i);
		}
		else if (letterCount[i] > counts[1]) {
			counts[2] = counts[1];
			top3[2] = top3[1];
			counts[1] = letterCount[i];
			top3[1] = char(i);
		}
		else if (letterCount[i] > counts[2]) {
			counts[2] = letterCount[i];
			top3[2] = char(i);
		}
	}

	cout << "3 самые частые буквы: ";
	for (int i = 0; i < 3; i++) {
		if (counts[i] > 0) {
			cout << "'" << top3[i] << "'(" << counts[i] << " раз) ";
		}
	}
	cout << endl;

	// Подбор для каждой из 3 частых
	for (int i = 0; i < 3; i++) {
		if (counts[i] == 0) continue;

		int key = top3[i] - normalLetter;
		if (key > maxKey) key -= (maxKey + 1);
		if (key < -maxKey) key += (maxKey + 1);

		if (key != guessedKey) {  // Был первый вариант
			cout << "\n   Пробую, если '" << top3[i] << "' это '" << normalLetter << "':" << endl;
			cout << "   Ключ " << key << ": " << caesarCipher(encryptedText, -key) << endl;
		}
	}
}

// Функция для проверки корректности ввода текста
string textCorrectness() {
	string text;
	const int MAX_TEXT_LENGTH = 1000;

	while (true) {
		cout << "Введите текст для шифрования: ";

		// Ошибка входного потока
		if (!getline(cin, text)) {
			cout << "Ошибка ввода! Программа завершена" << endl;
			return "";
		}

		// Пустая строка
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
		for (char c : text) {
			if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
				onlySpaces = false;
				break;
			}
		}
		if (onlySpaces) {
			cout << "Ошибка! Текст не может состоять только из пробелов." << endl;
			continue;
		}

		// Определение языка текста
		char language = detectTextLanguage(text);

		// Запрет на смешанный текст!
		if (language == 'B') {
			cout << "Ошибка! Текст содержит буквы на разных языках!" << endl;
			continue;
		}

		// Нет букв
		if (language == 'N') {
			cout << "Текст не содержит букв. Шифрование не изменит текст." << endl;
			continue;
		}

		// Проверки пройдены
		return text;
	}
}

// Функция для проверки корректности ввода ключа
int keyCorrectness(char language) {
	int key;
	bool keyAccepted = false;

	while (!keyAccepted) {
		cout << "Введите ключ (целое число): ";

		if (cin >> key) {
			// После числа только пробелы/конец строки
			char nextChar = cin.peek();
			if (nextChar == '\n' || nextChar == '\t' || nextChar == '\r' || nextChar == ' ') {
				// Проверка ключа по языку
				if (isValidKey(key, language)) {
					keyAccepted = true;
				}
				else {
					cout << "Ошибка! Ключ не соответствует возможному сдвигу." << endl;

					if (language == 'R') {
						cout << "Для русского текста ключ должен быть от -32 до 32." << endl;
					}
					else if (language == 'E') {
						cout << "Для английского текста ключ должен быть от -26 до 26." << endl;
					}
					cin.clear();
					cin.ignore(1000, '\n');
				}
			}
			else {
				// Дробная часть/символы/буквы после числа
				cout << "Ошибка! Введите число без других символов." << endl;

				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
		else {
			// Не число
			cout << "Ошибка! Вы ввели не число." << endl;

			cin.clear();
			cin.ignore(1000, '\n');
		}
	}

	// Очистка буфер после успешного ввода ключа
	cin.ignore(1000, '\n');
	return key;
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);


	int choice;
	string originalText, encryptedText, decryptedText;
	int key;
	char language;

	originalText = textCorrectness();  // ввод текста с проверками
	language = detectTextLanguage(originalText);  // определение языка
	key = keyCorrectness(language);  // ввод ключа с проверками

	do {
		cout << "\nГЛАВНОЕ МЕНЮ" << endl;
		cout << "1. Зашифровать текст" << endl;
		cout << "2. Расшифровать текст (с ключом)" << endl;
		cout << "3. Взломать шифр (частотный анализ)" << endl;
		cout << "4. Выход" << endl;
		cout << "Выберите действие (1-4): ";

		if (!(cin >> choice)) {
			cout << "Ошибка ввода!" << endl;
			cin.clear();
			cin.ignore(1000, '\n');
			continue;
		}

		cin.ignore(1000, '\n');

		switch (choice) {
		case 1: { // Зашифровать
			if (originalText.empty()) break;

			encryptedText = caesarCipher(originalText, key);
			cout << "\nИсходный текст: " << originalText << endl;
			cout << "Ключ: " << key << endl;
			cout << "Зашифрованный текст: " << encryptedText << endl;
			break;
		}

		case 2: { // Расшифровать
			string textToDecrypt;
			if (encryptedText.empty()) {
				cout << "Ошибка! Зашифрованный текст еще не создан или неверный выбор." << endl;
				break;
			}
			else {
				textToDecrypt = encryptedText;
				cout << "Расшифровка текста с ключом " << key << ":" << endl;
				decryptedText = caesarCipher(textToDecrypt, -key);
				cout << "\nРезультат расшифровки: " << decryptedText << endl;
				break;
			}
		}

		case 3: { // Взломать

			if (encryptedText.empty()) {
				cout << "Сначала создайте зашифрованный текст (пункт 1)!" << endl;
				break;
			}
			cout << "Взламываем зашифрованный текст: " << encryptedText << endl;
			crackCipher(encryptedText);
			break;
		}

		case 4: { // Выход
			cout << "\nВыход из программы." << endl;
			break;
		}

		default: {
			cout << "Неверный выбор. Пожалуйста, выберите от 1 до 4." << endl;
			break;
		}
		}

		if (choice == 4) break;

	} while (true);

	return 0;
}
