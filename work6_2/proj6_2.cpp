#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <chrono>

using namespace std;


//Дан текст. Найти количество вхождений заданной подстроки в текст. 
//1: Построение таблицы смещений для алгоритма
//Бойера–Мура (правило "плохих символов")
// vector<int> buildBadCharTable(const string &pattern) {
//     const int ALPHABET = 256;
//     vector<int> table(ALPHABET, -1);
//     for (int i = 0; i < pattern.size(); i++)
//         table[(unsigned char)pattern[i]] = i;
/*
Пусть шаблон — "abra" (длина \(m=4\)). 
Инициализируем таблицу смещений размером 256 (для ASCII-символов) со значениями, равными 4.
Заполняем таблицу:a: \(4-1-0=3\) (последнее вхождение a на позиции 3, 
т.е. a на позиции 0 -> \(4-1-0=3\))
r: \(4-1-2=1\) (последнее вхождение r на позиции 3, т.е. r на позиции 2 -> \(4-1-2=1\))
b: \(4-1-1=2\) (последнее вхождение b на позиции 3, т.е. b на позиции 1 -> \(4-1-1=2\))
a: \(4-1-3=0\) (последнее вхождение a на позиции 3 -> \(4-1-3=0\))
Для других символов, которые не входят в шаблон, значение в таблице остается равным 4.
Таким образом, таблица будет выглядеть так:
a: 3
b: 2
r: 1
Остальные символы: 4
*/

//Построение таблицы смещений для алгоритма Бойера–Мура
vector<int> buildBadTable(const string &pattern) {
    const int ALPHABET = 256; 
    int m = pattern.size(); //длина образца
    vector<int> table(ALPHABET, m); //инициализация таблицы

    // Заполняем таблицу смещений
    for (int i = 0; i < m - 1; i++) {
        table[(unsigned char)pattern[i]] = m - 1 - i; //сохраняем индекс последнего вхождения символа
    }

    return table;
}



//Эвристика хорошего суффикса
vector<int> boyerGoodTable(const string &pattern) {
    int m = pattern.size();
    vector<int> good(m, m);  // по умолчанию смещение = m

    // ишем самые длинные суффиксы, которые являются префиксами
    int lastPrefix = m;
    for (int i = m - 1; i >= 0; i--) {
        bool isPrefix = true;

        for (int j = 0; j < m - i; j++) {
            if (pattern[j] != pattern[i + j]) {
                isPrefix = false;
                break;
            }
        }

        if (isPrefix) {
            lastPrefix = i;
        }

        good[m - 1 - i] = lastPrefix - 1 + m;
    }

    // ищем внутренние совпадающие суффиксы
    for (int i = 0; i < m - 1; i++) {
        int suffixLength = 0;

        // Ищем максимальный суффикс, который совпадает
        while (suffixLength < m - 1 - i &&
               pattern[i - suffixLength] == pattern[m - 1 - suffixLength])
        {
            suffixLength++;
        }

        // Устанавливаем смещение
        good[suffixLength] = m - 1 - i + suffixLength;
    }

    return good;
}

//Эффективный поиск Бойера–Мура
int boyerMooreSearch(const string &text, const string &pattern, int &comparisons) {
    int n = text.size(); //длина текста
    int m = pattern.size(); //длина образца
    comparisons = 0; 

    if (m == 0 || n < m) return -1; // Проверка на пустой образец или текст     
    vector<int> badCharTable = buildBadTable(pattern); 
    vector<int> goodSuffixTable = boyerGoodTable(pattern); 
    // auto bad = buildBadTable(pattern); // Таблица плохих символов
    // auto good = boyerGoodTable(pattern); // Таблица хороших суффиксов   
   
    int i = 0;
    while (i <= n - m) {
        int j = m - 1;

        while (j >= 0) {
            comparisons++; // считаем каждое сравнение

            if (text[i + j] != pattern[j])
                break;

            j--;
        }

        if (j < 0)
            return i;
        else {
            int badShift = badCharTable[(unsigned char)text[i + j]];
            int goodShift = goodSuffixTable[j + 1];
            int shift = max(badShift, goodShift);
            if (shift < 1) {
                shift = 1; // минимальное смещение
            }
            i += shift; // сдвигаем индекс на максимальное смещение
        }
        
    }

    return -1;
}


// Подсчет количества вхождений образца в тексте
int countCurrectEntry(const string &text, const string &pattern, int &totalComparisons) {
    int count = 0; //cчетчик вхождений
    int pos = 0; //индекс текущего вхождения
    totalComparisons = 0; 
    int n = text.size(); //длина текста
    int m = pattern.size(); //длина образца
    if (m == 0 ) return count; // Проверка на пустой образец или текст

    while (pos <= n - m) {
        int comparisons = 0; //счетчик сравнений для текущего вхождения
        int index = boyerMooreSearch(text.substr(pos), pattern, comparisons); //поиск вхождения
        totalComparisons += comparisons; // добавляем к общему количеству сравнений

        if (index == -1) {
            break; // увеличиваем счетчик вхождений
        }
        count++;
        pos += index + 1;
    }
    return count; //возвращаем общее количество вхождений
}





//Дан текст. 
// Удалите из текста все значения времени из промежутка 00:00 до 02:00. 
// До и после этих значений может быть по одному пробелу.
// 2: Регулярное выражение для удаления времени 00:00–02:00

//string text = "Встреча в 00:00, а затем в 02:00.";
/*
Пояснение регулярного выражения (0\d|1\d|2[0-3]):[0-5][0-9]
( и ): Группирует части выражения.
0\d: Соответствует 00 до 09 (где \d — любая цифра).
|: Означает "или".
1\d: Соответствует 10 до 19.
2[0-3]: Соответствует 20 до 23.
:: Соответствует двоеточию в строке.
[0-5]: Соответствует одной цифре от 0 до 5 (для минут).
[0-9]: Соответствует одной цифре от 0 до 9. 
*/
string removeTime(const string &text) {
    // Формат времени HH:MM, где HH = 00, 01, 02
    regex timePattern(R"(\b(?:00:[0-5][0-9]|01:[0-5][0-9]|02:00)\b)");
    return regex_replace(text, timePattern, " ");
}


int main() {
    ifstream input("anna.txt");
    string text; 
    string line;
    if (!input.is_open()){
        cerr << "Ошибка чтения";
        return 1;
    }
    
    while(getline(input, line)){
        text += line + "\n";
    }
    input.close();
    cout << "Файл прочитан успешно!" << endl;
    
    // ========== ТЕСТ ДЛЯ ТАБЛИЦЫ ==========
    string pattern_for_table = "Karenina";
    int M = pattern_for_table.size();
    
    vector<int> textSizes = {100000, 200000, 300000, 400000, 500000};
    vector<long long> times;
    vector<int> comps;  
    
    cout << "\n=== тест для отчета ===\n";
    cout << "Образец: " << pattern_for_table << " (M=" << M << ")\n\n";
    
    for (int n : textSizes) {
        string subtext = text.substr(0, min(n, (int)text.size()));
        
        auto start = chrono::high_resolution_clock::now();
        int totalComparisons = 0;
        countCurrectEntry(subtext, pattern_for_table, totalComparisons);
        auto end = chrono::high_resolution_clock::now();
        
        long long timeMicro = chrono::duration_cast<chrono::microseconds>(end - start).count();
        times.push_back(timeMicro);
        comps.push_back(totalComparisons);
        
        cout << "n=" << n << ": T(n)=" << timeMicro << " мкс, C=" << totalComparisons << endl;
    }
    
    
    cout << "\nДанные для таблицы:\n";
    cout << left << setw(10) << "n" 
         << left << setw(15) << "T(n) мкс" 
         << left << setw(15) << "f(n*M)" 
         << left << setw(15) << "C" 
         << endl;
    cout << string(50, '-') << endl;
    
    for (size_t i = 0; i < textSizes.size(); i++) {
        long long f = static_cast<long long>(textSizes[i]) * M;
        cout << left << setw(10) << textSizes[i]
             << left << setw(15) << times[i]
             << left << setw(15) << f
             << left << setw(15) << comps[i]
             << endl;
    }
    
    // ========== первоначальные тесты ==========
    vector<string> patterns = {"Oblonskys", "Levin", "Arkadyevitch", "Karenina", "untrussing"};
    
    for (const string& pattern : patterns) {
        cout << "\n=== Образец: " << pattern << " ===\n";
        auto start = chrono::high_resolution_clock::now();

        int comparisons = 0;
        int index = boyerMooreSearch(text, pattern, comparisons);

        auto end = chrono::high_resolution_clock::now();
        long long timeMicro = chrono::duration_cast<chrono::microseconds>(end - start).count();

        if (index != -1)
            cout << "Первое вхождение найдено на позиции: " << index << endl;
        else
            cout << "Вхождение не найдено!!!\n";

        cout << "Сравнений для первого вхождения: " << comparisons << endl;
        cout << "Время поиска T(n): " << timeMicro << " мкс" << endl;

        int totalComparisons = 0;
        int totalEntry = countCurrectEntry(text, pattern, totalComparisons);

        cout << "Всего вхождений: " << totalEntry << endl;
        cout << "Общее количество сравнений(C): " << totalComparisons << endl;
    }

    // ========== ЗАДАНИЕ 2 ==========
    cout << "\n~~~ Удаление времени 00:00–02:00 ~~~\n";
    string test1 =  "Сегодня в 00:00 мы начали, потом в 00:45 , в 01:05  и в 02:59 закончили. Дальше уже было в 03:00.";
    string test2 = "Начало отчёта: 01:15\nПродолжение в 02:30, завершено в 03:10.\nНо была пауза в 00:05 и в 02:01 тоже.";
    string test3 = "История:\n"
                    "В 00:00 поступил сигнал, спустя минуты в 00:30 начались проверки. "
                    "Первый этап завершён в 01:10, второй — в 01:59. "
                    "Третий этап начался в 02:45, а уже в 04:00 работа завершилась.";
    string test4 = "Система показывала «01:00», потом (02:10), и даже: 02:58! Но вот время 03:40 уже не удаляем.";
    string test5 = "Коды: 1000, 0210, 0020 — не время. "
                "Настоящие времена: 00:10, 01:55, 02:00. "
                "Неверные форматы: 2:00, 003:00, 00-30, 25:00 должны остаться.";

    vector<string> tests = {test1, test2, test3, test4, test5};

    for (int i = 0; i < tests.size(); i++) {
        cout << "\n--- Тест " << (i+1) << " ---\n";
        cout << "Исходный текст:\n" << tests[i] << endl;
        cout << "После удаления:\n" << removeTime(tests[i]) << endl;
    }
    
    return 0;   
}