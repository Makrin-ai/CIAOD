//управление хеш-таблицей
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
using namespace std;
/*
Для обеспечения прямого доступа к записи в файле элемент хеш-таблицы
 должен включать обязательные поля: 
 ключ записи в файле, 
 номер записи с этим ключом в файле. 
 Элемент может содержать другие поля, 
 требующиеся методу (указанному в вашем варианте), 
 разрешающему коллизию.

 С открытой адресацией (смещение на номер выполняемого подбора)
*/
//структура элемента хеш-таблицы
struct HashItem {
    int key;//ключ записи в файле
    int recordIndex;//номер записи с этим ключом в файле
    bool occupied;//флаг занятости для открытой адресации
    bool deleted;//флаг удаления
};

//структура хеш-таблицы  в соответствии с методом разрешения коллизии,
//С открытой адресацией (смещение на номер выполняемого подбора)
struct HashTable {
    HashItem* table; //массив элементов хеш-таблицы
    int size;//размер таблицы
    int count;//число занятых элементов
};

//реализация простой хеш-функции
/*
Разработать хеш-функцию (метод определить самостоятельно), 
выполнить ее тестирование, убедиться, 
что хеш (индекс элемента таблицы) формируется верно.
*/
int hashFunc(int key, int size) {//простая хеш-функция
    return key % size;//хеш-значение
}
//инициализация хеш-таблицы
void initTable(HashTable& ht, int size = 11) {
    ht.size = size;//начальный размер таблицы
    ht.count = 0;//число занятых элементов
    ht.table = new HashItem[size];//выделение памяти под таблицу

    for (int i = 0; i < size; i++) {
        ht.table[i].occupied = false;//флаг занятости
        ht.table[i].deleted = false;//флаг удаления
    }
}

//реорганизация (увеличение) хеш-таблицы
//рехешировать таблицу
void rehash(HashTable& ht) {
    HashItem* oldTable = ht.table;
    int oldSize = ht.size;

    initTable(ht, oldSize * 2 + 1);

    for (int i = 0; i < oldSize; i++) {
        if (oldTable[i].occupied) {
            int h = hashFunc(oldTable[i].key, ht.size);
            for (int j = 0; j < ht.size; j++) {
                int idx = (h + j) % ht.size;
                if (!ht.table[idx].occupied) {
                    ht.table[idx] = oldTable[i];
                    ht.table[idx].occupied = true;
                    ht.count++;
                    break;
                }
            }
        }
    }
    delete[] oldTable;
}

//вставка ключа в хеш-таблицу
bool insert(HashTable& ht, int key, int recordIndex) {
    //Проверяем необходимость рехеширования: если занято более половины ячеек
    //(коэффициент заполнения > 0.5), увеличиваем таблицу
    if (ht.count * 2 >= ht.size)
        rehash(ht);

    //вычисляем начальный индекс с помощью хеш-функции
    int h = hashFunc(key, ht.size);

    // Линейное пробинование: последовательно проверяем ячейки таблицы
    // i - номер выполняемого подбора (по варику 2)
    for (int i = 0; i < ht.size; i++) {
        // Вычисляем индекс текущей проверяемой ячейки
        // (h + i) % size - формула линейного пробинования - смещение на один(метод коллизий)
        int idx = (h + i) % ht.size;
        cout << "Insert probe index: " << idx << endl; //

        // Если ячейка свободна или помечена как удаленная
        if (!ht.table[idx].occupied || ht.table[idx].deleted) {
            // Вставляем элемент: ключ, индекс записи в файле, 
            // устанавливаем флаги занятости и удаления
            ht.table[idx] = {key, recordIndex, true, false};
            ht.count++;// Увеличиваем счетчик занятых ячеек
            return true;
        }
    }
    return false;
}

//поиск ключа в хеш-таблице
bool find(HashTable& ht, int key, int& recordIndex) {
    int h = hashFunc(key, ht.size);

    for (int i = 0; i < ht.size; i++) {
        int idx = (h + i) % ht.size;

        if (!ht.table[idx].occupied && !ht.table[idx].deleted)
            return false;

        if (ht.table[idx].occupied && ht.table[idx].key == key) {
            recordIndex = ht.table[idx].recordIndex;
            return true;
        }
    }
    return false;
}

//удаление ключа из хеш-таблицы
bool remove(HashTable& ht, int key) {
    int h = hashFunc(key, ht.size);

    for (int i = 0; i < ht.size; i++) {
        int idx = (h + i) % ht.size;

        if (!ht.table[idx].occupied && !ht.table[idx].deleted)
            return false;

        if (ht.table[idx].occupied && ht.table[idx].key == key) {
            ht.table[idx].occupied = false;
            ht.table[idx].deleted = true;
            ht.count--;
            return true;
        }
    }
    return false;
}

//Подготовить тесты (последовательность значений ключей), обеспечивающие:
// вставку ключа без коллизии
// вставку ключа и разрешение коллизии
// вставку ключа с последующим рехешированием
// удаление ключа из таблицы
// поиск ключа в таблице. Для метода с открытым адресом подготовить тест для поиска ключа, который размещен в таблице после удаленного ключа, с одним значением хеша для этих ключей

void testHeshT() {
    cout << "\n=== testHeshT ===\n";
    HashTable ht;
    initTable(ht, 5); // маленький размер, чтобы показать рехеширование

    // Вставка без коллизии (ключи 1, 2)
    //Вставка с коллизией (ключи 1 и 6 → одинаковый хеш)
    insert(ht, 1, 0);
    insert(ht, 2, 1);

    // Вставка с коллизией (1 и 6 % 5 = 1)
    insert(ht, 6, 2);

    // Вставка с рехешированием (после 5 элементов)
    insert(ht, 3, 3);
    insert(ht, 4, 4);
    insert(ht, 5, 5); // рехеширование произойдет здесь

    int idx;

    // Поиск ключей
    if (find(ht, 6, idx)) cout << "Found key 6 at index " << idx << endl;
    if (!find(ht, 10, idx)) cout << "Key 10 not found\n";

    // Удаление ключа из табл
    remove(ht, 2);
    if (!find(ht, 2, idx)) cout << "Key 2 successfully removed\n";

    // Проверка поиска ключа после удаленного элемента
    if (find(ht, 6, idx)) cout << "Key 6 still found at index " << idx << endl;

    cout << "testHeshT completed\n";
}


#endif
