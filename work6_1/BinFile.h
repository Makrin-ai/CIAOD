#ifndef BINFILE_H
#define BINFILE_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;

struct BankAcc {
    int accNumber;
    char fullName[100];
    char address[100];
};

// Создание бинарного файла из текстового
bool createBinFromText(const char* textFilename, const char* binFilename) {
    ifstream in(textFilename);
    if (!in.is_open()) {
        cout << "Ошибка открытия текстового файла: " << textFilename << endl;
        return false;
    }

    ofstream out(binFilename, ios::binary | ios::trunc);
    if (!out.is_open()) {
        cout << "Ошибка создания бинарного файла: " << binFilename << endl;
        return false;
    }

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue; // пропускаем пустые строки

        BankAcc acc;
        acc.accNumber = stoi(line); // первая строка — номер счета

        // Читаем имя
        string fname, lname;
        if (!getline(in, fname) || fname.empty()) continue;
        size_t spacePos = fname.find(' ');
        if (spacePos != string::npos) {
            lname = fname.substr(spacePos + 1);
            fname = fname.substr(0, spacePos);
        } else {
            lname = "";
        }
        string fullName = fname + " " + lname;
        strncpy(acc.fullName, fullName.c_str(), sizeof(acc.fullName));
        acc.fullName[sizeof(acc.fullName)-1] = '\0';

        // Читаем адрес
        string addr;
        if (!getline(in, addr)) addr = "";
        strncpy(acc.address, addr.c_str(), sizeof(acc.address));
        acc.address[sizeof(acc.address)-1] = '\0';

        out.write(reinterpret_cast<char*>(&acc), sizeof(acc));
    }

    in.close();
    out.close();
    return true;
}

// Получить количество записей в бинарном файле
int getRecordCount(const char* filename) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) return 0;
    in.seekg(0, ios::end);
    int count = in.tellg() / sizeof(BankAcc);
    in.close();
    return count;
}

// Добавить запись в файл
bool addRecord(const char* filename, const BankAcc& acc) {
    ofstream out(filename, ios::binary | ios::app);
    if (!out.is_open()) return false;
    out.write(reinterpret_cast<const char*>(&acc), sizeof(acc));
    out.close();
    return true;
}

//  прочитать запись файла по заданному номеру записи
bool accessRecordByIndex(const char* filename, int index, BankAcc& acc) {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) return false;

    int recordCount = getRecordCount(filename);
    if (index < 0 || index >= recordCount) return false;

    in.seekg(index * sizeof(BankAcc), ios::beg);
    in.read(reinterpret_cast<char*>(&acc), sizeof(acc));
    return in.gcount() == sizeof(acc);
}

// Удалить запись по номеру счёта (accNumber)
bool deleteRecordByKey(const char* filename, int accNumber) {
    int recordCount = getRecordCount(filename);
    if (recordCount == 0) return false;

    ifstream in(filename, ios::binary);
    if (!in.is_open()) return false;

    ofstream temp("temp.bin", ios::binary);
    if (!temp.is_open()) {
        in.close();
        return false;
    }

    BankAcc acc;
    bool found = false;
    for (int i = 0; i < recordCount; ++i) {
        in.read(reinterpret_cast<char*>(&acc), sizeof(acc));
        if (acc.accNumber != accNumber) {
            temp.write(reinterpret_cast<char*>(&acc), sizeof(acc));
        } else {
            found = true;
        }
    }

    in.close();
    temp.close();

    if (found) {
        remove(filename);
        rename("temp.bin", filename);
    } else {
        remove("temp.bin");
    }

    return found;
}

// Функция для тестирования операций с бинарным файлом
void testBinF(const char* textFilename, const char* binFilename) {
    cout << "\n=== testBinF ===\n";

    // Создаём бинарный файл из текстового
    if (!createBinFromText(textFilename, binFilename)) {
        cout << "Не удалось создать бинарный файл\n";
        return;
    }

    // Вывод всех записей
    int n = getRecordCount(binFilename);
    cout << "Всего записей: " << n << endl;
    for (int i = 0; i < n; ++i) {
        BankAcc r;
        if (accessRecordByIndex(binFilename, i, r)) {
            cout << r.accNumber << " | " << r.fullName << " | " << r.address << endl;
        }
    }

    // Пример удаления записи
    cout << "\nУдаляем запись с accNumber = 1002\n";
    if (deleteRecordByKey(binFilename, 1002)) {
        cout << "Удаление выполнено\n";
    } else {
        cout << "Запись не найдена\n";
    }

    // Вывод после удаления
    n = getRecordCount(binFilename);
    cout << "Всего записей после удаления: " << n << endl;
    for (int i = 0; i < n; ++i) {
        BankAcc r;
        if (accessRecordByIndex(binFilename, i, r)) {
            cout << r.accNumber << " | " << r.fullName << " | " << r.address << endl;
        }
    }

    cout << "testBinF completed\n";
}

#endif
