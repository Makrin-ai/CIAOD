#include <cstddef>
#include <cstring>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//1.1.	Разработать структуру записи двоичного файла согласно варианту задания.
struct BankAcc{
    int accNumber; //4 байта
    char fullName[100]; //100 байт
    char address[100]; //100 байт
    //Итого: 204 байта на запись
};

//1.2.	Подготовить тестовые данные в текстовом файле с кодировкой ASCII, 
// в соответствии со структурой записи варианта. 
// При открытии файла выполнить контроль его существования и открытия.
//1.3.	Имя файла вводит пользователь.
//1.4.	При открытии файла обеспечить контроль существования и открытия файла.

void readBankFile(string filename) {
    ifstream file_bank(filename); // БЕЗ ios::binary!
    if (!file_bank.is_open()) {
        cout << "File opening error" << endl;
        return;
    } 
    string line;
    int recordCount = 0;
    
    while(getline(file_bank, line)) {
        cout << line << endl;
        recordCount++;
    }

    if(recordCount == 0) {
        cout << "File is empty" << endl;
    }
    file_bank.close();
}


//1.5.	Р1.5.	При применении механизма прямого доступа к записи файла выполнить 
// контроль присутствия записи с заданным номером в файле.
bool findBankByKey(const string& filename, int accNumber){ 
    BankAcc acc;
    ifstream file_bank(filename); 
    if(!file_bank){
        //cout << "Error opening binary file." << endl;
        return false;
    }
    
    string line;
    bool found = false;
    int lineCount = 0;

    while(getline(file_bank, line)){
        lineCount++;

        if(line.empty()){
            continue;
        }

        bool isNumber = true;
        for(char c : line){
            if(!isdigit(c)){
                isNumber = false;
                break;
            }
        }

        if(isNumber){
            int currentNumber = stoi(line);
            if(currentNumber == accNumber){
                string fullname, address;

                if(getline(file_bank, fullname)){
                    if(getline(file_bank, address)){
                        // cout<<"Account Number: " << line << endl;
                        // cout<<"Full name: " << fullname << endl;
                        // cout<<"Address: " << address<< endl;
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    // if(!found){
    //     cout << "Account with number " << accNumber << "not found" << endl;
    // }
    file_bank.close();
    return found;
}


//1.6.	Разработать функции для выполнения операций:
//1.6.1.	функция преобразования тестовых данных из текстового файла в двоичный файл;
int convertTextToBinary(const string& textFilename, const string& binaryFilename){
    ifstream in(textFilename);
    ofstream out (binaryFilename, ios::binary);

    if(!in || !out){
        //cout << "Error" << endl;
        return -1;
    }

    BankAcc acc;
    string line;
    int count = 0;

    while(true){
        if(!getline(in, line)) {
            break;
        }
        if(line.empty()) {
            continue;
        }

        acc.accNumber = stoi(line);

        if (!getline(in, line)) break;
        strncpy(acc.fullName, line.c_str(),100);
        if (!getline(in, line)) break;
        strncpy(acc.address, line.c_str(), 100);
        out.write((char*) &acc, sizeof(BankAcc));
        count++;

        getline(in, line);
    }

    in.close();
    out.close();
    return count;
}

//1.6.2.функция преобразования данных из двоичного файла в текстовый
int convertBinaryToText(const string& textFilename, const string& binaryFilename){
    ifstream bin(binaryFilename, ios::binary);
    if(!bin){
        //cout << "Error opening text file: ";
        return 1;
    }

    ofstream out(textFilename);
    if(!out){
        //cout << "Error creating binary file: ";
        return -1;
    }

    BankAcc acc;
    //string line;
    int count = 0;
    
    while(bin.read((char*)&acc, sizeof(acc))) {
        out << acc.accNumber << "\n";
        out << acc.fullName << "\n";
        out << acc.address << "\n\n";   // пустая строка между записями
        count++;
    }
        
    // bin.close();
    // out.close();
    return count;
        
    }  


//1.6.3.функция вывода всех записей двоичного файла
void displayAllRecords(const string& filename){
    BankAcc acc;
    ifstream binFile(filename, ios::binary);
    if(!binFile){
        //cout << "File opening error" << endl;
        return;
    }

    int count = 0;
    while(binFile.read((char*)&acc, sizeof(acc))){
        // cout << "Account Number: " << acc.accNumber << endl;
        // cout << "Full Name: " << acc.fullName << endl;
        // cout << "Address: " << acc.address << endl;
        count++;
    }
    // if (count == 0){
    //     cout << "File is empty" << endl;
    // }
    
    //binFile.close();
}

//1.6.4.функция доступа к записи по ее порядковому номеру в файле, 
// используя механизм прямого доступа к записи в двоичном файле

//const size_t REC_SIZE = sizeof(BankAcc);
bool accessRecordByIndex(const string& filename, int index){
    BankAcc acc;
    ifstream file_bank(filename, ios::binary);
    if(!file_bank){
        //cout << "File opening error" << endl;
        return false;
    }

    file_bank.seekg(0, ios::end);
    int totalRecords = file_bank.tellg() / sizeof(BankAcc);
    if(index < 0 || index >= totalRecords){
        //cout << "Error: index out of bounds. Total records: " << totalRecords << endl;
        //file_bank.close();
        return false;
    }   
    file_bank.seekg(index * sizeof(BankAcc), ios::beg); // Move to the record at index
    if (file_bank.read((char*)&acc, sizeof(BankAcc))){
        return true;
    }
    return false;
    // cout << "Account Number: " << acc.accNumber << endl;
    // cout << "Full Name: " << acc.fullName << endl;
    // cout << "Address: " << acc.address << endl;
    //file_bank.close();
 }

 //1.6.5.функция удаления записи с заданным значением ключа, 
 // выполнить путем замены на последнюю запись.

bool deleteRecordByKey(const string& filename, int key) {
    fstream file(filename, ios::binary | ios::in | ios::out);
    if (!file) {
        cout << "Cannot open file\n";
        return false;
    }

    BankAcc acc;
    int index = -1;
    int pos = 0;

    // 1. Ищем запись с ключом
    while (file.read((char*)&acc, sizeof(acc))) {
        if (acc.accNumber == key) {
            index = pos;
            break;
        }
        pos++;
    }

    if (index == -1) {
        //cout << "Record not found\n";
        return false;
    }

    // 2. cмотрим размер файла
    file.seekg(0, ios::end);
    int totalRecords = file.tellg() / sizeof(BankAcc);

    BankAcc last;


    // 3. Читаем последнюю запись
    file.seekg((totalRecords - 1) * sizeof(BankAcc), ios::beg);
    file.read((char*)&last, sizeof(last));

    // 4. Записываем её на место удаляемой
    file.seekp(index * sizeof(BankAcc), ios::beg);
    file.write((char*)&last, sizeof(last));

    file.close();

    // 5. Пересоздаём файл без последней записи (самый простой способ)
    ifstream in(filename, ios::binary);
    ofstream out("temp.bin", ios::binary);

    for (int i = 0; i < totalRecords - 1; i++) {
        in.read((char*)&acc, sizeof(acc));
        out.write((char*)&acc, sizeof(acc));
    }

    in.close();
    out.close();
    remove(filename.c_str());
    rename("temp.bin", filename.c_str());

    return true;
}

//1.6.6.функция манипулирования записями в двоичном файле согласно дополнительным операциям, 
// определенным в варианте: 
// Доп. операция	1.	Поиск записи с заданным значением ключа. 2.	Обновить значение одного поля.

bool updateRecordByKey(const string& filename, int key, int fieldToUpdate, BankAcc& updatedRecord){
    fstream file_bank(filename, ios::in | ios::out | ios::binary);
    if(!file_bank){
        return false;
    }

    BankAcc acc;
    streampos pos = -1;
    bool found = false;
    int recordIndex = 0;
    
    // Поиск записи с сохранением индекса
    while (file_bank.read((char*)&acc, sizeof(BankAcc))) {
        if (acc.accNumber == key) {
            found = true;
            updatedRecord = acc;
            break;
        }
        recordIndex++;
    }
    
    if(!found){
        file_bank.close();
        return false;
    }

    // Вычисляем позицию через индекс (избегаем арифметики streampos)
    streampos writePos = streampos(recordIndex * sizeof(BankAcc));
    file_bank.seekp(writePos);
    file_bank.write((char*)&updatedRecord, sizeof(BankAcc));
    
    bool success = file_bank.good();
    file_bank.close();
    
    return success;
}

    
int main() {
    setlocale(LC_ALL, "Russian");

    string textFile, binFile;
    int choice;

    while (true) {
        cout << "\n=====MENU=====\n";
        cout << "1. Вывести текстовый файл\n";
        cout << "2. Найти запись в текстовом файле\n";
        cout << "3. Конвертировать текст → бинарный\n";
        cout << "4. Конвертировать бинарный → текст\n";
        cout << "5. Вывести все бинарные записи\n";
        cout << "6. Доступ к записи по индексу\n";
        cout << "7. Удалить запись по ключу\n";
        cout << "8. Найти запись (доп. операция) и обновить поле\n";
        cout << "9. Выйти\n";
        cout << "Выбор: ";
        cin >> choice;

        if (choice == 9) break;

        switch (choice) {

        case 1: {
            cout << "Введите имя текстового файла: ";
            cin >> textFile;
            // ОШИБКА: неправильный вызов функции
            readBankFile(textFile); // Просто вызываем функцию, она сама выводит
            break;
        }

        case 2: {
            cout << "Введите имя текстового файла: ";
            cin >> textFile;
            int key;
            cout << "Введите номер аккаунта: ";
            cin >> key;
            
            // ОШИБКА: функция findBankByKey не возвращает имя и адрес
            bool found = findBankByKey(textFile, key); // Вызываем как есть
            if(found){
                cout << "Account with number " << key << " found" << endl;
                // Если нужно вывести данные, нужно изменить функцию findBankByKey
            } else {
                cout << "Account with number " << key << " not found" << endl;
            }
            break;
        }

        case 3: {
            cout << "Введите текстовый файл: ";
            cin >> textFile;
            cout << "Введите бинарный файл: ";
            cin >> binFile;

            int result = convertTextToBinary(textFile, binFile);
            if(result == -1){
                cout << "Error opening files for conversion" << endl;
            } else {
                cout << "Конвертировано записей: " << result << endl;
            }
            break;
        }

        case 4: {
            cout << "Введите бинарный файл: ";
            cin >> binFile;
            cout << "Введите текстовый файл: ";
            cin >> textFile;

            int result = convertBinaryToText(textFile, binFile);
            if (result == -1) {
                cout << "Error opening files for conversion" << endl;
            } else {
                cout << "Конвертировано записей: " << result << endl;
            }
            break;
        }

        case 5: {
            cout << "Введите бинарный файл: ";
            cin >> binFile;

            // ОШИБКА: функция displayAllRecords не возвращает количество
            // и не принимает параметры для записей
            BankAcc acc;
            ifstream binFileStream(binFile, ios::binary);
            if (!binFileStream) {
                cout << "Error opening binary file." << endl;
            } else {
                int count = 0;
                while(binFileStream.read((char*)&acc, sizeof(acc))){
                    cout << "Account Number: " << acc.accNumber << endl;
                    cout << "Full Name: " << acc.fullName << endl;
                    cout << "Address: " << acc.address << endl;
                    cout << "-------------------" << endl;
                    count++;
                }
                if (count == 0) {
                    cout << "File is empty" << endl;
                }
                binFileStream.close();
            }
            break;
        }

        case 6: {
            cout << "Введите бинарный файл: ";
            cin >> binFile;
            int index;
            cout << "Введите индекс: ";
            cin >> index;

            // ОШИБКА: функция accessRecordByIndex не принимает параметр result
            bool success = accessRecordByIndex(binFile, index);
            if (success) {
                // Если нужно вывести данные, нужно изменить функцию accessRecordByIndex
                cout << "Record found at index " << index << endl;
            } else {
                cout << "Error: record not found or file error" << endl;
            }
            break;
        }

        case 7: {
            cout << "Введите бинарный файл: ";
            cin >> binFile;
            int key;
            cout << "Введите ключ записи для удаления: ";
            cin >> key;

            // ОШИБКА: функция deleteRecordByKey возвращает bool, а не int
            bool success = deleteRecordByKey(binFile, key);
            if (!success) {
                cout << "Record not found or error" << endl;
            } else {
                cout << "Record deleted successfully" << endl;
            }
            break;
        }

        case 8: {
    cout << "Введите бинарный файл: ";
    cin >> binFile;
    int key;
    cout << "Введите ключ записи: ";
    cin >> key;
    
    int updateChoice;
    cout << "Выберите что обновить:\n";
    cout << "1 - Обновить ФИО и адрес\n";
    cout << "2 - Ничего не менять\n";
    cout << "Выбор: ";
    cin >> updateChoice;
    cin.ignore();
    
    if (updateChoice == 1) {
        BankAcc currentRecord;
        
        // Сначала находим запись (fieldToUpdate = 0 - только поиск)
        bool found = updateRecordByKey(binFile, key, 0, currentRecord);
        if (!found) {
            cout << "Record with key " << key << " not found" << endl;
            break;
        }
        
        // Показываем текущие данные
        cout << "\nТекущие данные записи " << key << ":" << endl;
        cout << "ФИО: " << currentRecord.fullName << endl;
        cout << "Адрес: " << currentRecord.address << endl;
        cout << "-------------------" << endl;
        
        // Запрашиваем новые данные
        cout << "Введите новое ФИО: ";
        cin.getline(currentRecord.fullName, 100);
        cout << "Введите новый адрес: ";
        cin.getline(currentRecord.address, 100);
        
        // Обновляем запись (fieldToUpdate = 1 - обновить данные)
        bool success = updateRecordByKey(binFile, key, 1, currentRecord);
        if (success) {
            cout << "Record updated successfully!" << endl;
        } else {
            cout << "Error updating record" << endl;
        }
    } else if (updateChoice == 2) {
        cout << "Изменения отменены." << endl;
    } else {
        cout << "Неверный выбор!" << endl;
    }
    break;
}

        default:
            cout << "Неверный выбор!\n";
        }
    }

    return 0; 
}