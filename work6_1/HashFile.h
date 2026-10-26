#ifndef HASHFILE_H
#define HASHFILE_H

#include "HashTable.h"
#include "BinFile.h"
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>
using namespace std;
using namespace std::chrono;

// Загружаем все записи из файла в хеш-таблицу
void loadFileToHash(HashTable& ht, const char* filename) {
    int count = getRecordCount(filename);
    BankAcc acc;

    for (int i = 0; i < count; ++i) {
        if (accessRecordByIndex(filename, i, acc)) {
            insert(ht, acc.accNumber, i); // вставляем ключ и индекс записи
        }
    }
}

// Поиск записи по ключу через хеш-таблицу
bool findInFile(HashTable& ht, const char* filename, int key) {
    int index;
    if (!find(ht, key, index)) {
        cout << "NOT FOUND\n";  // <-- УБЕРИ КОММЕНТАРИЙ!
        return false;
    }

    BankAcc acc;
    if (!accessRecordByIndex(filename, index, acc)) {
        cout << "Ошибка чтения записи по индексу " << index << endl;
        return false;
    }

    cout << acc.accNumber << " | " << acc.fullName << " | " << acc.address << endl;
    return true;
}

// Добавление записи и её вставка в хеш-таблицу
bool addToFileAndHash(HashTable& ht, const char* filename, const BankAcc& acc) {
    if (!addRecord(filename, acc)) return false;
    int index = getRecordCount(filename) - 1; // индекс новой записи
    insert(ht, acc.accNumber, index);
    return true;
}

// Удалить запись из таблицы при заданном значении ключа и соответственно из файла.
bool deleteFromFileAndHash(HashTable& ht, const char* filename, int key) {
    int index;
    if (!find(ht, key, index)) {
        cout << "Ключ " << key << " не найден в таблице\n";
        return false;
    }

    if (!deleteRecordByKey(filename, key)) {
        cout << "Ошибка удаления записи из файла\n";
        return false;
    }

    remove(ht, key);

    // учитываем флаг deleted
    for (int i = 0; i < ht.size; ++i) {
        if (ht.table[i].occupied && !ht.table[i].deleted && ht.table[i].recordIndex > index) {
            ht.table[i].recordIndex--;
        }
    }

    cout << "Запись с ключом " << key << " успешно удалена\n";
    return true;
}

// Линейный поиск в файле (ДЛЯ СРАВНЕНИЯ ПО УСЛОВИЮ)
bool linearSearchInFile(const char* filename, int key) {
    int count = getRecordCount(filename);
    BankAcc acc;
    for (int i = 0; i < count; ++i) {
        if (accessRecordByIndex(filename, i, acc)) {
            if (acc.accNumber == key) {
                return true;
            }
        }
    }
    return false;
}



// Тест 1: Базовые операции
void testBasicOperations() {
    cout << "\n=== Тест 1: Базовые операции (3 записи) ===\n";
    
    const char* filename = "test_basic.dat";
    HashTable ht;
    initTable(ht);
    
    // Очистка файла
    ofstream clear(filename, ios::binary | ios::trunc);
    clear.close();

    BankAcc a = {1001, "Ivan Ivanov", "Moscow"};
    BankAcc b = {1002, "Petr Petrov", "SPb"};
    BankAcc c = {1003, "Anna Sidorova", "Kazan"};

    addToFileAndHash(ht, filename, a);
    addToFileAndHash(ht, filename, b);
    addToFileAndHash(ht, filename, c);

    cout << "\nПоиск через хеш-таблицу:\n";
    findInFile(ht, filename, 1001);
    findInFile(ht, filename, 1002);
    findInFile(ht, filename, 1003);

    cout << "\nУдаляем запись с ключом 1002\n";
    deleteFromFileAndHash(ht, filename, 1002);

    cout << "\nПосле удаления:\n";
    findInFile(ht, filename, 1001);
    findInFile(ht, filename, 1002); // должно показать NOT FOUND
    findInFile(ht, filename, 1003);
}

// Тест 2: Коллизии
void testWithCollisions() {
    cout << "\n=== Тест 2: Записи с коллизиями ===\n";
    
    const char* filename = "test_collisions.dat";
    ofstream clear(filename, ios::binary | ios::trunc);
    clear.close();
    
    HashTable ht;
    initTable(ht, 7); // маленький размер для демонстрации коллизий
    
    cout << "Размер таблицы: " << ht.size << endl;
    cout << "\nДобавляем записи:\n";
    
    // Записи БЕЗ коллизий
    BankAcc acc1 = {1, "Client 1", "Address 1"};    // hash = 1 % 7 = 1
    BankAcc acc2 = {2, "Client 2", "Address 2"};    // hash = 2 % 7 = 2
    BankAcc acc3 = {3, "Client 3", "Address 3"};    // hash = 3 % 7 = 3
    
    // Записи С коллизиями (одинаковые хеши с acc1)
    BankAcc acc4 = {8, "Client 8", "Address 8"};    // hash = 8 % 7 = 1 (коллизия с acc1)
    BankAcc acc5 = {15, "Client 15", "Address 15"}; // hash = 15 % 7 = 1 (коллизия с acc1 и acc4)
    
    addToFileAndHash(ht, filename, acc1);
    cout << "Добавлен ключ 1 (хеш: 1)\n";
    
    addToFileAndHash(ht, filename, acc2);
    cout << "Добавлен ключ 2 (хеш: 2)\n";
    
    addToFileAndHash(ht, filename, acc3);
    cout << "Добавлен ключ 3 (хеш: 3)\n";
    
    addToFileAndHash(ht, filename, acc4);
    cout << "Добавлен ключ 8 (хеш: 1 - КОЛЛИЗИЯ!)\n";
    
    addToFileAndHash(ht, filename, acc5);
    cout << "Добавлен ключ 15 (хеш: 1 - КОЛЛИЗИЯ!)\n";
    
    cout << "\nПроверяем поиск всех записей:\n";
    cout << "Без коллизий:\n";
    findInFile(ht, filename, 2);
    findInFile(ht, filename, 3);
    
    cout << "\nС коллизиями (все три дают хеш 1):\n";
    findInFile(ht, filename, 1);
    findInFile(ht, filename, 8);
    findInFile(ht, filename, 15);
}

// Тест 3: Рехеширование
void testRehashing() {
    cout << "\n=== Тест 3: Рехеширование ===\n";
    
    const char* filename = "test_rehash.dat";
    ofstream clear(filename, ios::binary | ios::trunc);
    clear.close();
    
    HashTable ht;
    initTable(ht, 5); // Очень маленький начальный размер
    
    cout << "Начальный размер таблицы: " << ht.size << endl;
    cout << "Коэффициент заполнения для рехеширования: 0.5\n";
    cout << "Рехеширование произойдет после добавления " << (ht.size / 2) + 1 << " записей\n\n";
    
    // Добавляем записи до рехеширования и после
    for (int i = 0; i < 8; i++) {
        BankAcc acc;
        acc.accNumber = 1000 + i;
        sprintf(acc.fullName, "Client %d", i);
        sprintf(acc.address, "Address %d", i);
        
        addToFileAndHash(ht, filename, acc);
        cout << "Добавлен ключ " << (1000 + i) 
             << ", занято/размер: " << ht.count << "/" << ht.size;
        
        if (ht.count * 2 >= ht.size) {
            cout << " -> ВЫЗОВЕТ РЕХЕШИРОВАНИЕ при следующей вставке";
        }
        cout << endl;
    }
    
    cout << "\nФинальный размер таблицы после рехеширования: " << ht.size << endl;
    cout << "Количество записей в таблице: " << ht.count << endl;
    
    // Проверяем, что все записи доступны после рехеширования
    cout << "\nПроверка доступности записей после рехеширования:\n";
    for (int i = 0; i < 8; i++) {
        if (findInFile(ht, filename, 1000 + i)) {
            cout << "Ключ " << (1000 + i) << " - OK\n";
        } else {
            cout << "Ключ " << (1000 + i) << " - ERROR: не найден!\n";
        }
    }
}

// Тест 4: 500+ записей и замер времени
void testPerformance() {
    cout << "\n=== Тест 4: Производительность (500+ записей) ===\n";
    
    const char* txtFilename = "test.txt";
    const char* binFilename = "test_performance.dat";
    
    // Создаем бинарный файл из текстового
    cout << "Создание файла из " << txtFilename << " ...\n";
    if (!createBinFromText(txtFilename, binFilename)) {
        cout << "Ошибка создания файла!\n";
        return;
    }
    
    int recordCount = getRecordCount(binFilename);
    cout << "Создан файл с " << recordCount << " записями\n";
    
    // Загружаем в хеш-таблицу
    HashTable ht;
    initTable(ht);
    loadFileToHash(ht, binFilename);
    cout << "Хеш-таблица создана, размер: " << ht.size << "\n\n";
    
    // Получаем ключи первой, средней и последней записи
    BankAcc firstAcc, middleAcc, lastAcc;
    accessRecordByIndex(binFilename, 0, firstAcc);
    accessRecordByIndex(binFilename, recordCount/2, middleAcc);
    accessRecordByIndex(binFilename, recordCount-1, lastAcc);
    
    int firstKey = firstAcc.accNumber;
    int middleKey = middleAcc.accNumber;
    int lastKey = lastAcc.accNumber;
    
    cout << "Тестовые ключи:\n";
    cout << "• Первая запись: " << firstKey << " (" << firstAcc.fullName << ")\n";
    cout << "• Средняя запись: " << middleKey << " (" << middleAcc.fullName << ")\n";
    cout << "• Последняя запись: " << lastKey << " (" << lastAcc.fullName << ")\n\n";
    
    // Функция для замера времени
    auto measureTime = [](auto func) {
        auto start = high_resolution_clock::now();
        bool result = func();
        auto end = high_resolution_clock::now();
        return make_pair(duration_cast<nanoseconds>(end - start).count(), result);
    };
    
    // Результаты
    vector<long long> hashTimes;
    vector<long long> linearTimes;
    
    cout << "--- Замер времени доступа ---\n";
    cout << fixed << setprecision(2);
    
    // Тестируем первую запись
    cout << "\n1. Первая запись (ключ: " << firstKey << "):\n";
    long long hashTime1 = measureTime([&]() { return findInFile(ht, binFilename, firstKey); }).first;
    hashTimes.push_back(hashTime1);
    cout << "   Хеш-поиск: " << hashTime1 << " нс";
    
    long long linearTime1 = measureTime([&]() { return linearSearchInFile(binFilename, firstKey); }).first;
    linearTimes.push_back(linearTime1);
    cout << "   | Линейный поиск: " << linearTime1 << " нс";
    cout << "   | Ускорение: " << (double)linearTime1 / hashTime1 << "x\n";
    
    // Тестируем среднюю запись
    cout << "\n2. Средняя запись (ключ: " << middleKey << "):\n";
    long long hashTime2 = measureTime([&]() { return findInFile(ht, binFilename, middleKey); }).first;
    hashTimes.push_back(hashTime2);
    cout << "   Хеш-поиск: " << hashTime2 << " нс";
    
    long long linearTime2 = measureTime([&]() { return linearSearchInFile(binFilename, middleKey); }).first;
    linearTimes.push_back(linearTime2);
    cout << "   | Линейный поиск: " << linearTime2 << " нс";
    cout << "   | Ускорение: " << (double)linearTime2 / hashTime2 << "x\n";
    
    // Тестируем последнюю запись
    cout << "\n3. Последняя запись (ключ: " << lastKey << "):\n";
    long long hashTime3 = measureTime([&]() { return findInFile(ht, binFilename, lastKey); }).first;
    hashTimes.push_back(hashTime3);
    cout << "   Хеш-поиск: " << hashTime3 << " нс";
    
    long long linearTime3 = measureTime([&]() { return linearSearchInFile(binFilename, lastKey); }).first;
    linearTimes.push_back(linearTime3);
    cout << "   | Линейный поиск: " << linearTime3 << " нс";
    cout << "   | Ускорение: " << (double)linearTime3 / hashTime3 << "x\n";
    
    // Анализ результатов
    cout << "\n--- Анализ результатов ---\n";
    
    // Проверяем равномерность времени доступа через хеш
    double avgHash = (hashTime1 + hashTime2 + hashTime3) / 3.0;
    double maxDiff = 0;
    
    cout << "Среднее время хеш-доступа: " << avgHash << " нс\n";
    cout << "Отклонения от среднего:\n";
    
    vector<double> deviations = {
        abs(hashTime1 - avgHash) / avgHash * 100,
        abs(hashTime2 - avgHash) / avgHash * 100,
        abs(hashTime3 - avgHash) / avgHash * 100
    };
    
    cout << "  • Первая запись: " << deviations[0] << "%\n";
    cout << "  • Средняя запись: " << deviations[1] << "%\n";
    cout << "  • Последняя запись: " << deviations[2] << "%\n";
    
    maxDiff = *max_element(deviations.begin(), deviations.end());
    
    if (maxDiff < 20) {
        cout << " Время доступа ко всем записям примерно одинаково (макс. отклонение < 20%)\n";
    } else {
        cout << " Время доступа различается значительно (макс. отклонение: " << maxDiff << "%)\n";
    }
    
    // Сравнение с линейным поиском
    cout << "\nСравнение производительности:\n";
    double avgLinear = (linearTime1 + linearTime2 + linearTime3) / 3.0;
    double speedup = avgLinear / avgHash;
    
    cout << "Среднее время линейного поиска: " << avgLinear << " нс\n";
    cout << "Среднее ускорение хеш-поиска: " << speedup << "x\n";
    
    if (speedup > 10) {
        cout << " Хеш-поиск значительно быстрее линейного\n";
    } else {
        cout << " Хеш-поиск показывает умеренное ускорение\n";
    }
}

// Главная функция для запуска всех тестов
void runAllTests() {
    cout << "========== ТЕСТИРОВАНИЕ ПРИЛОЖЕНИЯ ==========\n";
    
    // Тест хеш-таблицы
    testHeshT();
    
    // Тесты из условия
    testBasicOperations();
    testWithCollisions();
    testRehashing();
    testPerformance();
    
    cout << "\n========== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ==========\n";
}

#endif