#include <iostream>
#include "HashTable.h"
#include "BinFile.h"
#include "HashFile.h"

using namespace std;

int main() {
    // Запускаем ВСЕ тесты из условия
    runAllTests();
    
    // Дополнительно: демонстрация интерактивного поиска
    //агрузка данных в хеш-таблицу при интерактивном поиске
    //создаем пустую хэш табл
    //читаем 600 записей
    //для каждой записи вычисляем хэш;если нужно, то ищем свободную ячейку(вывод индексов);запись добавляем в табл
    cout << "\n\n=== работа ===\n";
    
    const char* txtFilename = "test.txt";
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
    cout << "\nВведите номер счета для поиска (1001-1600): ";
    cin >> key;
    
    cout << "\nРезультат поиска:\n";
    findInFile(ht, binFilename, key);
    
    return 0;
}