#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono> // для анализа времени
#include <iomanip>  // для setw()
#include <ctime>
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
        //bool found = false;
        while (input.read((char*)&record, sizeof(Record))) {
            if (record.id == id) {
                // cout << "Найдена запись: ID = " << record.id 
                //         << ", Name = " << record.name 
                //         << ", Address = " << record.address << endl;    
                result = record;
                input.close();
                return true; // Возвращаем найденную запись
                //found = true;
            }
        }
        // if (!found) {
        //     cout << "Запись с ID = " << id << " не найдена." << endl;
        // }
        input.close();
        return false; // Запись не найдена
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

        //input.seekg(address * sizeof(Record), ios::beg);
        if (!input.read((char*)&result, sizeof(Record))) {
            input.close();
            return false; //если не удалось прочитать запись, значит адрес вне диапазона
        }

        //     cout << "Запись по адресу " << address << ": ID = "
        //             << record.id << ", Name = " << record.name 
        //             << ", Address = " << record.address << endl;    
        // } else {
        //     cout << "Нет записи по адресу " << address << "." << endl;
        // }
        input.close();
        return true; //успешно прочитана запись
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
        // cout << "Запись добавлена: ID = " << record.id
        //         << ", Name = " << record.name 
        //         << ", Address = " << record.address << endl;    
    }
    //Получить размер файла (количество записей)
    //нужно для 5 пункта задания
    /*
    Провести анализ времени выполнения поиска записей, находящихся в начале, середине и конце большого файла тремя способами.
    */
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
//поиск BST
class BinarySearchTree {
private:
    TreeNode* root;
    // Вспомогательные функции для работы с деревом
    //добавление элемента в дерево
    void insert(TreeNode*& node, int key, long offset) {
        if (node == nullptr) {
            node = new TreeNode(key, offset);//если нет узла, то создаем новый
        } else if (key < node->key) { //если ключ меньше
            insert(node->left, key, offset); //рекурсивно вставляем в левое поддерево
        } else if (key > node->key) {
            insert(node->right, key, offset); //рекурсивно вставляем в правое поддерево
        }
        //если же key == node->key — ничего не делаем (дубликаты игнорируем)
    }
    //поиск по ключу в дереве
    // Если ключ найден, возвращает смещение, иначе -1  
    long search(TreeNode* node, int key) {
        if (node == nullptr) {
            return -1; // Не найдено
        }
        if (key == node->key) {
            return node->offset; // Найдено
        } 
        if (key < node->key) {
            return search(node->left, key);
        }
        return search(node->right, key);
    }
    // Поиск минимального узла в поддереве
    // Используется для удаления узла
    // Возвращает указатель на узел с минимальным ключом
    // (самый левый узел в поддереве)
    // Возвращает указатель на узел с минимальным ключом
    TreeNode* findMin(TreeNode* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }
    //реализация удаления узла из BST
    TreeNode* removeNode(TreeNode* node, int key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left = removeNode(node->left, key);
        }
        else if (key > node->key) {
            node->right = removeNode(node->right, key);
        }
        else {
            //узел найден
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
                // 2 ребёнка → ищем минимальный справа
                TreeNode* minNode = findMin(node->right);
                node->key = minNode->key;
                node->offset = minNode->offset;
                node->right = removeNode(node->right, minNode->key);
            }
        }
        return node;
    }
    //удаление всего дерева
    void clear(TreeNode* node) {
        if (node != nullptr) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }
    //вывод дерева в форме дерева
    void inorder(TreeNode* node, int depth = 0) {
        if (node == nullptr) return;
    
        // Сначала правое поддерево
        if (node->right) {
            inorder(node->right, depth + 4);
        }
        
        // Отступ для текущего узла
        if (depth) {
            cout << setw(depth) << ' ';
        }
        
        // Рисуем ветвь к правому поддереву
        if (node->right) {
            cout << " /\n" << setw(depth) << ' ';
        }
        
        // Выводим сам узел
        cout << node->key << " (Offset: " << node->offset << ")" << endl;
        
        // Рисуем ветвь к левому поддереву
        if (node->left) {
            cout << setw(depth) << ' ' << " \\" << endl;
            inorder(node->left, depth + 4);
        }


        
        // cout << string(depth*4, ' ') 
        //     << node->key << " (Offset: " << node->offset << ")" << endl;
        // inorder(node->left, depth + 1);
}

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { clear(root); }

    
    void insert(int key, long offset) { insert(root, key, offset); }//метод вставки
    long search(int key) { return search(root, key); }//метод поиска
    void inorder() { inorder(root); }//метод для вывода дерева

    // Удаление узла по ключу
    void remove(int key) {
        root = removeNode(root, key); //удаляем узел с ключом key
    }

    // Построение дерева из бинарного файла
    void buildFromFile(const string& filename) {
        ifstream input(filename, ios::binary);
        if (!input.is_open()) return;
        Record rec;
        long offset = 0;
        while (input.read((char*)&rec, sizeof(Record))) {
            insert(rec.id, offset);// Вставляем каждый id в дерево
            offset += sizeof(Record);//смещение для каждой записи
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

    //для 5 аункта задания
    int rotationCount = 0; //счетчик поворотов

    //высота узла
    int height(AVLNode* n) {
        return n ? n->height : 0;
    }

    int balanceFactor(AVLNode* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }

    void updateHeight(AVLNode* n) {
        n->height = 1 + max(height(n->left), height(n->right));
    }

    //повороты для балансировки дерева
    AVLNode* rightRotate(AVLNode* y) {
        rotationCount++; // Увеличиваем счетчик поворотов
        // Поворот вправо
        // y - левый узел, x - правый узел
        // y становится правым узлом x, а x левым узлом y
        // x - левый узел y
        // y - правый узел x
        AVLNode* x = y->left;
        AVLNode* t = x->right;
        //y->left = x->right;
        x->right = y;
        y ->left = t; //правый узел y теперь t
        // Обновляем высоты узлов
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        rotationCount++;
        AVLNode* y = x->right;
        AVLNode* t = y->left;
        //x->right = y->left;
        y->left = x;
        x->right = t; //левый узел x теперь t
        // Обновляем высоты узлов
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    //балансировка узла
    // если балансировка нарушена, выполняем повороты
    // и возвращаем новый корень поддерева
    // если балансировка в норме, возвращаем узел как есть
    // обновляем высоту узла перед проверкой баланса
    AVLNode* balance(AVLNode* node) {
        updateHeight(node); //обновляем высоту тек узла
        int bf = balanceFactor(node);//считаем разность высот левого и правого поддеревьев

        if (bf > 1) { //если леввое выше
            if (balanceFactor(node->left) < 0) //если неправильный вид
                node->left = leftRotate(node->left);// выполняем левый поворот на левом поддереве
            return rightRotate(node); //правый поворот на текущ узле
        }
        //теперь же аналогичная проверка с правым поддеревом
        if (bf < -1) {
            if (balanceFactor(node->right) > 0)
                node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node; //если баланс высот в норме, то возвращаем узел как есть
    }

    //добавление элемента в дерево
    AVLNode* insertNode(AVLNode* node, int key, long offset) {
        if (!node) 
            return new AVLNode(key, offset);

        if (key < node->key)
            node->left = insertNode(node->left, key, offset);
        else if (key > node->key)
            node->right = insertNode(node->right, key, offset);
        else
            return node; //если ключ уже есть, ничего не делаем

        // Балансируем узел после вставки
        return balance(node);
    }

    //поиск по ключу в дереве
    long search(AVLNode* node, int key) {
        if (!node) return -1;
        if (key == node->key) return node->offset;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }
    //поиск минимума
    AVLNode* findMin(AVLNode* node) {
        while (node->left)
            node = node->left;
        return node;
    }
    //удаление узла из дерева
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
    //удаление всего дерева
    void clear(AVLNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    //вывод дерева в форме дерева
    void inorder(AVLNode* node, int depth=0) {
        if (node == nullptr) return; //если узел пуст, ничего не делаем
        if(node->right){
            inorder(node->right, depth+4);
        }
        //отступы для вывода дерева
        if(depth){
            cout << setw(depth) << ' ';
        }

        if(node->right){
            cout << " /\n" << setw(depth) << ' ';
        }
        cout << node->key << " (Offset: " << node->offset << ")" << endl;
        
        if(node->left){
            cout << setw(depth) << ' ' << " \\\n";
            inorder(node->left, depth+4);
        }
        // inorder(node->left, depth+1);
        // cout << string(depth*4, ' ') << node->key << " (Offset: " << node->offset << ")" << endl;
        // inorder(node->right, depth+1);
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

    void display() {
        inorder(root);
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
    // bool isBalanced(AVLNode* node) {
    // if (!node) return true;
    
    // int bf = balanceFactor(node);
    // if (bf < -1 || bf > 1) {
    //     cout << "НЕСБАЛАНСИРОВАН узел " << node->key 
    //          << " (bf=" << bf << ")" << endl;
    //     return false;
    // }
    
    // return isBalanced(node->left) && isBalanced(node->right);
    // }

    // bool checkAVL() {
    //     return isBalanced(root);
    // }

    // int getRotationCount() { 
    //     return rotationCount; 
    // }


    // Метод для получения среднего количества поворотов на одну запись
    // Используется для 6 пункта задания
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

    // === Генерация текстового файла из 1000 записей ===
    srand(time(nullptr));
    const int RECORDS = 1000; // количество записей

    vector<string> firstNames = {
        "Иван", "Петр", "Сидор", "Алексей", "Анна",
        "Дмитрий", "Ольга", "Сергей", "Екатерина", "Николай"
    };

    vector<string> lastNames = {
        "Иванов", "Петров", "Сидоров", "Кузнецов", "Смирнов",
        "Морозов", "Федоров", "Соколов", "Попов", "Лебедев"
    };

    vector<string> cities = {
        "Москва", "Санкт-Петербург", "Казань", "Новосибирск",
        "Екатеринбург", "Самара", "Уфа", "Ростов-на-Дону",
        "Краснодар", "Владивосток"
    };

    vector<string> streets = {
        "Ленина", "Советская", "Мира", "Куйбышева",
        "Баумана", "Победы", "Октябрьская", "Центральная"
    };

    ofstream out(textFileName);
    if (!out.is_open()) {
        cerr << "Ошибка создания data.txt\n";
        return 1;
    }

    for (int i = 0; i < RECORDS; i++) {
        int id = 1000 + i;

        string fullName =
            lastNames[rand() % lastNames.size()] + " " +
            firstNames[rand() % firstNames.size()] + " " +
            firstNames[rand() % firstNames.size()];

        string address =
            cities[rand() % cities.size()] + ", ул. " +
            streets[rand() % streets.size()] +
            ", д." + to_string(rand() % 200 + 1);

        out << id << "\n";
        out << fullName << "\n";
        out << address << "\n\n";
    }
    out.close();

    cout << "Файл data.txt успешно создан (" << RECORDS << " записей).\n";

    // 1. Создание двоичного файла из текстового файла
    fileManager.createBinaryFile(textFileName);
    cout << "Бинарный файл '" << binaryFileName
         << "' создан из текстового файла '" << textFileName << "'.\n";

    // 2. Построение деревьев
    cout << "\n=== Построение деревьев по бинарному файлу ===\n";
    BinarySearchTree bst;
    AVLTree avl;

    ifstream input(binaryFileName, ios::binary);
    if (!input.is_open()) {
        cerr << "Ошибка открытия бинарного файла.\n";
        return 1;
    }

    Record rec;
    long offset = 0;
    int totalKeys = 0; // количество вставленных ключей для подсчета поворотов

    while (input.read((char*)&rec, sizeof(Record))) {
        bst.insert(rec.id, offset);
        avl.insert(rec.id, offset);
        offset += sizeof(Record);
        totalKeys++;
    }
    input.close();
    cout << "Деревья построены!\n\n";

    // 3. Среднее число поворотов в AVL
    double avgRotations = avl.getAverageRotations(totalKeys);
    cout << "Среднее число поворотов AVL при формировании: "
         << avgRotations << endl << endl;

    // 4. Анализ времени поиска в начале, середине и конце файла
    int fileSize = fileManager.getSize();
    if (fileSize == 0) {
        cerr << "Файл пустой, поиск невозможен.\n";
        return 1;
    }

    int keysToSearch[3];
    Record tmp;

    fileManager.printRecordAtAddress(0, tmp);              // начало
    keysToSearch[0] = tmp.id;

    fileManager.printRecordAtAddress(fileSize / 2, tmp);  // середина
    keysToSearch[1] = tmp.id;

    fileManager.printRecordAtAddress(fileSize - 1, tmp);  // конец
    keysToSearch[2] = tmp.id;

    cout << "=== Анализ времени поиска ===\n";

    long long linearTimes[3], bstTimes[3], avlTimes[3];

    for (int i = 0; i < 3; i++) {
        int key = keysToSearch[i];

        // Линейный поиск
        auto start = chrono::high_resolution_clock::now();
        fileManager.linearSearch(key, tmp);
        auto end = chrono::high_resolution_clock::now();
        linearTimes[i] =
            chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // BST поиск
        start = chrono::high_resolution_clock::now();
        bst.search(key);
        end = chrono::high_resolution_clock::now();
        bstTimes[i] =
            chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // AVL поиск
        start = chrono::high_resolution_clock::now();
        avl.search(key);
        end = chrono::high_resolution_clock::now();
        avlTimes[i] =
            chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    }

    cout << "Начало файла (ключ " << keysToSearch[0] << "):\n";
    cout << "  Линейный: " << linearTimes[0] << " нс\n";
    cout << "  BST:      " << bstTimes[0] << " нс\n";
    cout << "  AVL:      " << avlTimes[0] << " нс\n\n";

    cout << "Середина файла (ключ " << keysToSearch[1] << "):\n";
    cout << "  Линейный: " << linearTimes[1] << " нс\n";
    cout << "  BST:      " << bstTimes[1] << " нс\n";
    cout << "  AVL:      " << avlTimes[1] << " нс\n\n";

    cout << "Конец файла (ключ " << keysToSearch[2] << "):\n";
    cout << "  Линейный: " << linearTimes[2] << " нс\n";
    cout << "  BST:      " << bstTimes[2] << " нс\n";
    cout << "  AVL:      " << avlTimes[2] << " нс\n\n";

    // 5. Демонстрация удаления элемента
    cout << "\n---Удаление элемента---\n";
    int keyToRemove;
    cout << "Введите ID для удаления: ";
    cin >> keyToRemove;

    bst.remove(keyToRemove);
    avl.remove(keyToRemove);

    cout << "Деревья после удаления ключа " << keyToRemove << ":\n";
    cout << "---BST---\n";
    bst.inorder();
    cout << "---AVL---\n";
    avl.display();

    // 6. Добавление новой записи
    cout << "\n---Добавление новой записи---\n";
    Record newRec;

    cout << "Введите ID: ";
    cin >> newRec.id;
    cin.ignore();

    cout << "Введите Name: ";
    cin.getline(newRec.name, 50);

    cout << "Введите Address: ";
    cin.getline(newRec.address, 100);

    fileManager.addRecord(newRec);

    long newOffset = offset; // смещение новой записи
    bst.insert(newRec.id, newOffset);
    avl.insert(newRec.id, newOffset);

    cout << "Деревья после добавления новой записи:\n";
    cout << "---BST---\n";
    bst.inorder();
    cout << "---AVL---\n";
    avl.display();

    cout << "\nРабота завершена. Ура!\n";
    return 0;
}
