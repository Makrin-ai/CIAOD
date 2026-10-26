#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono> // для анализа времени
#include <iomanip> // для setw
using namespace std;

/*
Разработать класс управления файлом. 
Включить методы: создание двоичного файла записей фиксированной длины из заранее подготовленных данных в текстовом файле;
 поиск и вывод записи в файле с помощью линейного поиска; 
 вывод записи по известному адресу в файле, 
 добавление записи в файл. 
 Структура записи файла определена индивидуальным вариантом задания.

*/

struct Record {
    int id;
    char name[50];
    char address[100];
};

class FileManager{
private:
    string filename;
public:
    FileManager(const string& fname) : filename(fname) {}
    // Создание двоичного файла из текстового файла
    void createBinaryFile(const string& textFile) {
        ifstream input(textFile);
        if (!input.is_open()) {
            cerr << "Ошибка открытия текстового файла!" << endl;
            return;
        }
        ofstream output(filename, ios::binary);
        if (!output.is_open()) {
            cerr << "Ошибка создания двоичного файла!" << endl;
            return;
    }
        Record record;
        string line;
        //int count = 0;
        
        // Читаем файл построчно
        while (getline(input, line)) {
            if (line.empty()) continue; // Пропускаем пустые строки
            
            // Пытаемся прочитать ID из строки
            try {
                record.id = stoi(line);
            } catch (...) {
                continue; // Если не число, пропускаем
            }
            
            // Читаем имя
            if (!getline(input, line)) break;
            strncpy(record.name, line.c_str(), sizeof(record.name) - 1);
            record.name[sizeof(record.name) - 1] = '\0';
            
            // Читаем адрес
            if (!getline(input, line)) break;
            strncpy(record.address, line.c_str(), sizeof(record.address) - 1);
            record.address[sizeof(record.address) - 1] = '\0';
            
            output.write((char*)&record, sizeof(Record));
            //count++;
            
            // Пропускаем пустую строку между записями (если есть)
            getline(input, line); 
        }
        
        input.close();
        output.close();
    }
    // Линейный поиск записи по ID
    bool linearSearch(int id, Record &result) {
        ifstream input(filename, ios::binary);
        if (!input.is_open()) {
            cerr << "Ошибка открытия двоичного файла!" << endl;
            return false; 
        }
        Record record;
        while (input.read((char*)&record, sizeof(Record))) {
            if (record.id == id) {
                result = record;
                input.close();
                return true;
            }
        }
        input.close();
        return false;
    }
    // Вывод записи по известному адресу
    bool printRecordAtAddress(int address, Record &result) {
        ifstream input(filename, ios::binary);
        if (!input.is_open()) {
            cerr << "Ошибка открытия двоичного файла!" << endl;
            return false;
        }
        long offset = address * sizeof(Record);
        input.seekg(offset, ios::beg);

        if (!input.read((char*)&result, sizeof(Record))) {
            input.close();
            return false;
        }

        input.close();
        return true;
    }
    
    // Вывод записи по известному адресу (для пользователя)
    void displayRecordAtAddress(int address) {
        Record result;
        if (printRecordAtAddress(address, result)) {
            cout << "Запись по адресу " << address << ": ID = "
                 << result.id << ", Name = " << result.name 
                 << ", Address = " << result.address << endl;
        } else {
            cout << "Нет записи по адресу " << address << "." << endl;
        }
    }
    
    // Добавление записи в файл
    void addRecord(const Record& record) {
        ofstream output(filename, ios::binary | ios::app);
        if (!output.is_open()) {
            cerr << "Ошибка открытия двоичного файла для добавления!" << endl;
            return;
        }
        output.write((char*)&record, sizeof(Record));
        output.close();
    }
    //Получить размер файла (количество записей)
    int getSize() {
        ifstream input(filename, ios::binary);
        input.seekg(0, ios::end);
        int size = input.tellg() / sizeof(Record);
        input.close();
        return size;
    }
};

/*
Разработать класс «Бинарное дерево поиска». 
Структура информационной части узла дерева включает ключ и ссылку на запись в файле. 
Разработать основные методы: 
построение дерева по существующему файлу; 
добавление элемента в дерево; 
поиск по ключу в дереве; 
удаление элемента из дерева; 
вывод дерева в форме дерева.
*/
struct TreeNode{
    int key; // ключ узла
    long offset; // смещение в файле, ссылка на запись в файле;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int k, long off) : key(k), offset(off), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    TreeNode* root;
    
    // Вспомогательные функции для работы с деревом
    void insert(TreeNode*& node, int key, long offset) {
        if (node == nullptr) {
            node = new TreeNode(key, offset);
        } else if (key < node->key) {
            insert(node->left, key, offset);
        } else if (key > node->key) {
            insert(node->right, key, offset);
        }
    }
    
    long search(TreeNode* node, int key) {
        if (node == nullptr) {
            return -1;
        }
        if (key == node->key) {
            return node->offset;
        } 
        if (key < node->key) {
            return search(node->left, key);
        }
        return search(node->right, key);
    }
    
    TreeNode* findMin(TreeNode* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }
    
    TreeNode* removeNode(TreeNode* node, int key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left = removeNode(node->left, key);
        }
        else if (key > node->key) {
            node->right = removeNode(node->right, key);
        }
        else {
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            }
            else if (!node->left) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            }
            else if (!node->right) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            }
            else {
                TreeNode* minNode = findMin(node->right);
                node->key = minNode->key;
                node->offset = minNode->offset;
                node->right = removeNode(node->right, minNode->key);
            }
        }
        return node;
    }
    
    void clear(TreeNode* node) {
        if (node != nullptr) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }
    
    // Вывод дерева в форме дерева (как у преподавателя)
    void printTree(TreeNode* p, int indent = 0) {
        if (p != nullptr) {
            if (p->right) {
                printTree(p->right, indent + 4);
            }
            if (indent) {
                cout << setw(indent) << ' ';
            }
            if (p->right) cout << " /\n" << setw(indent) << ' ';
            cout << p->key << " (Offset: " << p->offset << ")\n ";
            if (p->left) {
                cout << setw(indent) << ' ' << " \\\n";
                printTree(p->left, indent + 4);
            }
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { clear(root); }

    void insert(int key, long offset) { insert(root, key, offset); }
    long search(int key) { return search(root, key); }
    
    void remove(int key) {
        root = removeNode(root, key);
    }

    // Метод для вывода дерева в форме дерева
    void display() {
        cout << "Бинарное дерево поиска (BST):" << endl;
        printTree(root);
        cout << endl;
    }

    // Построение дерева из бинарного файла
    void buildFromFile(const string& filename) {
        ifstream input(filename, ios::binary);
        if (!input.is_open()) return;
        Record rec;
        long offset = 0;
        while (input.read((char*)&rec, sizeof(Record))) {
            insert(rec.id, offset);
            offset += sizeof(Record);
        }
        input.close();
    }
};

/*
AVL дерево
Разработать класс «Сбалансированное дерево поиска». 
Структура информационной части узла дерева включает ключ и ссылку на запись в файле. 
Разработать основные методы: 
построение дерева по существующему файлу; 
добавление элемента в дерево; 
поиск по ключу в дереве; 
удаление элемента из дерева; 
вывод дерева в форме дерева.

*/
struct AVLNode {
    int key;
    long offset;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(int k, long off): key(k), offset(off), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;
    int rotationCount = 0;

    int height(AVLNode* n) {
        return n ? n->height : 0;
    }

    int balanceFactor(AVLNode* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }

    void updateHeight(AVLNode* n) {
        n->height = 1 + max(height(n->left), height(n->right));
    }

    AVLNode* rightRotate(AVLNode* y) {
        rotationCount++;
        AVLNode* x = y->left;
        AVLNode* t = x->right;
        x->right = y;
        y->left = t;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        rotationCount++;
        AVLNode* y = x->right;
        AVLNode* t = y->left;
        y->left = x;
        x->right = t;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode* balance(AVLNode* node) {
        updateHeight(node);
        int bf = balanceFactor(node);

        if (bf > 1) {
            if (balanceFactor(node->left) < 0)
                node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        if (bf < -1) {
            if (balanceFactor(node->right) > 0)
                node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    AVLNode* insertNode(AVLNode* node, int key, long offset) {
        if (!node) 
            return new AVLNode(key, offset);

        if (key < node->key)
            node->left = insertNode(node->left, key, offset);
        else if (key > node->key)
            node->right = insertNode(node->right, key, offset);
        else
            return node;

        return balance(node);
    }

    long search(AVLNode* node, int key) {
        if (!node) return -1;
        if (key == node->key) return node->offset;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }
    
    AVLNode* findMin(AVLNode* node) {
        while (node->left)
            node = node->left;
        return node;
    }
    
    AVLNode* removeNode(AVLNode* node, int key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left = removeNode(node->left, key);
        }
        else if (key > node->key) {
            node->right = removeNode(node->right, key);
        }
        else {
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }

            AVLNode* minNode = findMin(node->right);
            node->key = minNode->key;
            node->offset = minNode->offset;
            node->right = removeNode(node->right, minNode->key);
        }

        return balance(node);
    }
    
    void clear(AVLNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }
    
    // Вывод дерева в форме дерева (как у преподавателя)
    void printTree(AVLNode* p, int indent = 0) {
        if (p != nullptr) {
            if (p->right) {
                printTree(p->right, indent + 4);
            }
            if (indent) {
                cout << setw(indent) << ' ';
            }
            if (p->right) cout << " /\n" << setw(indent) << ' ';
            cout << p->key << " (Offset: " << p->offset << ")\n ";
            if (p->left) {
                cout << setw(indent) << ' ' << " \\\n";
                printTree(p->left, indent + 4);
            }
        }
    }

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { clear(root); }

    void insert(int key, long offset) { 
        root = insertNode(root, key, offset); 
    }
    
    long search(int key) {
        return search(root, key);
    }

    void remove(int key) {
        root = removeNode(root, key);
    }

    // Метод для вывода дерева в форме дерева
    void display() {
        cout << "AVL дерево (сбалансированное):" << endl;
        printTree(root);
        cout << endl;
    }

    void buildFromFile(const string& filename) {
        ifstream input(filename, ios::binary);
        if (!input.is_open()) return;
        Record rec;
        long offset = 0;
        while (input.read((char*)&rec, sizeof(Record))) {
            insert(rec.id, offset);
            offset += sizeof(Record);
        }
        input.close();
    }

    double getAverageRotations(int totalKeys) {
        if (totalKeys == 0) return 0.0;
        return static_cast<double>(rotationCount) / totalKeys;
    }
};

/*
Разработать приложение, демонстрирующее выполнение всех методов и поиск записей в файле с помощью БДП, СДП и линейного поиска.
*/

int main() {
    string binaryFileName = "data.bin";
    string textFileName = "data.txt";
    FileManager fileManager(binaryFileName);

    cout << "ПРАКТИЧЕСКАЯ РАБОТА №7.1" << endl;
    cout << "Нелинейные структуры: балансировка деревьев поиска" << endl;
    cout << "===================================================\n" << endl;

    // 1. Создание двоичного файла из текстового файла
    cout << "1. Создание двоичного файла из текстового файла" << endl;
    fileManager.createBinaryFile(textFileName);
    cout << "   Бинарный файл '" << binaryFileName << "' создан из текстового файла '" << textFileName << "'.\n" << endl;

    // 2. Построение деревьев
    cout << "2. Построение деревьев по бинарному файлу" << endl;
    BinarySearchTree bst;
    AVLTree avl;

    ifstream input(binaryFileName, ios::binary);
    if (!input.is_open()) {
        cerr << "Ошибка открытия бинарного файла.\n";
        return 1;
    }

    Record rec;
    long offset = 0;
    int totalKeys = 0;

    while (input.read((char*)&rec, sizeof(Record))) {
        bst.insert(rec.id, offset);
        avl.insert(rec.id, offset);
        offset += sizeof(Record);
        totalKeys++;
    }
    input.close();
    
    int fileSize = fileManager.getSize();
    cout << "   Записей в файле: " << fileSize << endl;
    cout << "   Деревья построены успешно!\n" << endl;

    // 3. Вывод деревьев
    cout << "3. Вывод структуры деревьев" << endl;
    cout << "----------------------------------------" << endl;
    bst.display();
    cout << "----------------------------------------" << endl;
    avl.display();

    // 4. Среднее число поворотов в AVL
    cout << "4. Анализ поворотов AVL дерева" << endl;
    double avgRotations = avl.getAverageRotations(totalKeys);
    cout << "   Количество вставленных ключей: " << totalKeys << endl;
    cout << "   Общее число поворотов: " << avl.getAverageRotations(totalKeys) * totalKeys << endl;
    cout << "   Среднее число поворотов на ключ: " << avgRotations << "\n" << endl;

    // 5. Анализ времени поиска
    if (fileSize == 0) {
        cerr << "Файл пустой, поиск невозможен.\n";
        return 1;
    }

    int keysToSearch[3];
    Record tmp;
    
    // Определяем ключи для поиска (начало, середина, конец)
    cout << "5. Анализ времени поиска записей" << endl;
    cout << "=================================" << endl;
    
    // Ключ в начале файла
    if (fileManager.printRecordAtAddress(0, tmp)) {
        keysToSearch[0] = tmp.id;
        cout << "   Ключ в начале файла: " << keysToSearch[0] << endl;
    }
    
    // Ключ в середине файла
    int middlePos = fileSize / 2;
    if (fileManager.printRecordAtAddress(middlePos, tmp)) {
        keysToSearch[1] = tmp.id;
        cout << "   Ключ в середине файла: " << keysToSearch[1] << " (позиция " << middlePos << ")" << endl;
    }
    
    // Ключ в конце файла
    if (fileManager.printRecordAtAddress(fileSize - 1, tmp)) {
        keysToSearch[2] = tmp.id;
        cout << "   Ключ в конце файла: " << keysToSearch[2] << endl;
    }
    
    cout << endl;

    for (int i = 0; i < 3; i++) {
        int key = keysToSearch[i];
        string position;
        switch(i) {
            case 0: position = "в начале"; break;
            case 1: position = "в середине"; break;
            case 2: position = "в конце"; break;
        }
        
        cout << "   Поиск ключа " << key << " (" << position << " файла):" << endl;

        // Линейный поиск
        auto start = chrono::high_resolution_clock::now();
        bool foundLinear = fileManager.linearSearch(key, tmp);
        auto end = chrono::high_resolution_clock::now();
        auto linearTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        // BST поиск
        start = chrono::high_resolution_clock::now();
        long bstOffset = bst.search(key);
        end = chrono::high_resolution_clock::now();
        auto bstTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        // AVL поиск
        start = chrono::high_resolution_clock::now();
        long avlOffset = avl.search(key);
        end = chrono::high_resolution_clock::now();
        auto avlTime = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        // Вывод результатов
        cout << "     Линейный поиск: " << (foundLinear ? "найдено" : "не найдено") 
             << ", время = " << linearTime << " мкс" << endl;
        cout << "     BST поиск: " << (bstOffset != -1 ? "найдено" : "не найдено")
             << ", время = " << bstTime << " мкс" << endl;
        cout << "     AVL поиск: " << (avlOffset != -1 ? "найдено" : "не найдено")
             << ", время = " << avlTime << " мкс" << endl;
             
        // Вывод найденной записи
        if (foundLinear) {
            cout << "     Данные записи: ID=" << tmp.id 
                 << ", Имя=" << tmp.name 
                 << ", Адрес=" << tmp.address << endl;
        }
        cout << endl;
    }

    // 6. Демонстрация удаления элемента
    cout << "6. Демонстрация удаления элемента из деревьев" << endl;
    cout << "---------------------------------------------" << endl;
    
    // Показываем текущие деревья
    cout << "   Текущие деревья:" << endl;
    bst.display();
    avl.display();
    
    // Запрос ключа для удаления
    int keyToRemove;
    cout << "   Введите ID для удаления: ";
    cin >> keyToRemove;
    
    // Удаляем из обоих деревьев
    bst.remove(keyToRemove);
    avl.remove(keyToRemove);
    
    cout << "\n   Деревья после удаления ключа " << keyToRemove << ":" << endl;
    cout << "   BST после удаления:" << endl;
    bst.display();
    cout << "   AVL после удаления:" << endl;
    avl.display();

    // 7. Добавление новой записи
    cout << "7. Добавление новой записи" << endl;
    cout << "--------------------------" << endl;
    
    Record newRec;
    cout << "   Введите ID новой записи: ";
    cin >> newRec.id;
    cin.ignore();
    
    cout << "   Введите Name: ";
    cin.getline(newRec.name, 50);
    
    cout << "   Введите Address: ";
    cin.getline(newRec.address, 100);
    
    // Добавляем запись в файл
    fileManager.addRecord(newRec);
    
    // Добавляем в деревья
    long newOffset = fileSize * sizeof(Record); // Смещение для новой записи
    bst.insert(newRec.id, newOffset);
    avl.insert(newRec.id, newOffset);
    
    cout << "\n   Деревья после добавления новой записи:" << endl;
    cout << "   BST после добавления:" << endl;
    bst.display();
    cout << "   AVL после добавления:" << endl;
    avl.display();
    
    // 8. Дополнительная проверка поиска
    cout << "8. Проверка поиска добавленной записи" << endl;
    cout << "-------------------------------------" << endl;
    
    cout << "   Поиск только что добавленной записи (ID=" << newRec.id << "):" << endl;
    
    // BST поиск
    long offsetFound = bst.search(newRec.id);
    if (offsetFound != -1) {
        Record foundRec;
        if (fileManager.printRecordAtAddress(offsetFound / sizeof(Record), foundRec)) {
            cout << "   BST: найдено - ID=" << foundRec.id 
                 << ", Имя=" << foundRec.name << endl;
        }
    }
    
    // AVL поиск
    offsetFound = avl.search(newRec.id);
    if (offsetFound != -1) {
        Record foundRec;
        if (fileManager.printRecordAtAddress(offsetFound / sizeof(Record), foundRec)) {
            cout << "   AVL: найдено - ID=" << foundRec.id 
                 << ", Имя=" << foundRec.name << endl;
        }
    }

    cout << "\n===================================================" << endl;
    cout << "Работа программы завершена успешно!" << endl;
    cout << "===================================================" << endl;
    
    return 0;
}