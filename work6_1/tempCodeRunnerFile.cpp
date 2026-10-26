#include <iostream>
#include "HashTable.h"
#include "BinFile.h"
#include "HashFile.h"

using namespace std;

int main() {
    // Запускаем ВСЕ тесты из условия
    runAllTests();
    
    // Дополнительно: демонстрация интерактивного поиска
    cout << "\n\n=== работа ===\n";
    
    const char* txtFilename = ".txt";
    const char* binFilename = "bank.dat";
    
    // Создаем бинарный файл
    if (!createBinFromText(txtFilename, binFilename)) {
        cout << "Ошибка создания файла!\n";
        return 1;
    }
    
    // Загружаем в хеш-таблицу
    HashTable ht;
    initTable(ht);
    loadFileToHash(ht, binFilename);
    
    // Пользовательский поиск
    int key;
    cout << "\nВведите номер счета для поиска (1001-1300): ";
    cin >> key;
    
    cout << "\nРезультат поиска:\n";
    findInFile(ht, binFilename, key);
    
    return 0;
}