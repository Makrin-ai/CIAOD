#include <fstream>
#include <iostream>
#include <string>
using namespace std;

//создание текстового файла,содержащего десятичные числа по одному числу на строке
void createMyFile(const string& filename){
    ofstream myfile(filename);
    if (!myfile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    cout << "Please, enter some text (type 'exit' to finish):" << endl;
    int number;
    while(true) {
        cout << "Enter a number: ";
        cin >> number;
        if (cin.fail()) {
            cin.clear(); // clear the error flag
            string input;
            cin >> input; // read the invalid input
            if (input == "exit") {
                break; // exit if user types 'exit'
            }
            cerr << "Invalid input, please enter a number or 'exit'." << endl;
            continue; // prompt again
        }
        myfile << number << endl; // write the number to the file
    }
    myfile.close();
    cout << "File created successfully: " << filename << endl;
}

// вывод содержимого текстового файла и 

void transformToBinary(int num){
    for(int i = 31; i >= 0; --i) {
        cout << ((num >> i) & 1);
        if (i % 8 == 0){
            cout << " "; // add space every 8 bits
        }
    }
    cout << endl;
}


void readMyFile(const string& filename) {
    ifstream myfile(filename);
    if (!myfile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    cout << "Contents of the file " << filename << ":" << endl;
    int number;
    while (myfile >> number) 
    {
        cout << number << endl; // read and print each number
        cout << "Binary representation: ";
        transformToBinary(number); // transform and print binary representation
    }
    myfile.close();
}

//Добавление в файл новых чисел
void addNumbersToFile(const string& filename) {
    //ios::app is used to open the file in append mode
    //so that new numbers are added to the end of the file
    //without overwriting the existing content
    ofstream myfile(filename, ios::app); // open file in append mode
    if (!myfile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    cout << "Please, enter some text (type 'exit' to finish):" << endl;
    int number;
    while(true) {
        cout << "Enter a number: ";
        cin >> number;
        if (cin.fail()) {
            cin.clear(); // clear the error flag
            string input;
            cin >> input; // read the invalid input
            if (input == "exit") {
                break; // exit if user types 'exit'
            }
            cerr << "Invalid input, please enter a number or 'exit'." << endl;
            continue; // prompt again
        }
        myfile << number << endl; // write the number to the file
    }
    myfile.close();
    cout << "Numbers added successfully to: " << filename << endl;
}

//Получение числа из файла по порядковому номеру
int getNumberByIndex(const string& filename, int index) {
    ifstream myfile(filename);
    if (!myfile.is_open()) {
        cerr << "Error opening file: " <<  endl;
        return 0;
    }
    int number;
    int currentIndex = 1;
    while (myfile >> number) {
        if (currentIndex == index) {
            myfile.close();
            return number;
        }
        currentIndex++;
    }
    myfile.close();
    cerr << "Index " << index << " is out of bounds." << endl;
    return 0;
}

//подстчет количества чисел в файле
int countNumbersInFile(const string& filename) {
    ifstream myfile(filename);
    if (!myfile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 0;
    }
    int count = 0;
    int number;
    while (myfile >> number) {
        count++;
    }
    myfile.close();
    cout << "Total numbers in file " << filename << ": " << count << endl;
    return count;
}

//установить три старших бита в 1
int setThreeMostSignificantBits(int num) {
    // Create a mask with the three most significant bits set to 1
    int mask = 0xE0000000; // 1110 0000 0000 0000 0000 0000 0000 0000
    return num | mask; // Set the three most significant bits to 1
}

//обнулить 12-ый, 14-ый, 3-ий биты исходного значения числа, 
// используя соответствующую универсальную константную маску и поразрядную операцию.
int clearSpecificBits(int num) {
    // Create a mask with the 12th, 14th, and 3rd bits set to 0
    int mask = ~( (1 << 11) | (1 << 13) | (1 << 2) ); // Bits are 0-indexed
    return num & mask; // Clear the specific bits
}

//умножить на 4, используя соответствующую поразрядную операцию.
int multiplyByFour(int num) {
    return num << 2; // Shift left by 2 bits to multiply by 4
}

//разделить на 4, используя соответствующую поразрядную операцию.
int divideByFour(int num) {
    return num >> 2; // Shift right by 2 bits to divide by 4
}

//реализовать задание, в которой используются только поразрядные операции. 
// В выражении используется маска – переменная. Маска инициализируется единицей в младшем разряде (маска 1) 
// или единицей в старшем разряде (маска 2). Параметр n вводится с клавиатуры.
//Установить n-ый бит в 1, используя маску 2

int setNthBit(int num, int n) {
    int mask = 1 << (n - 1); // Create a mask with the n-th bit set to 1
    return num | mask; // Set the n-th bit to 1
}

//Дополнительные операции должны выполняться над всеми числами в файле. 
// Результат выполнения операций должен быть записан в результирующий файл.
//  В консоль в наглядном виде выводить исходные и полученные числа в десятичном и двоичном виде (своим способом).
void processFileNumbers(const string& filename) {
    ifstream myfile(filename);
    if (!myfile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    ofstream resultFile("result.txt");
    if (!resultFile.is_open()) {
        cerr << "Error opening result file." << endl;
        return;
    }
    cout << "Enter n for mask operation 2: ";
    int n;
    cin >> n;

    int number;
    cout << "Original and transformed numbers from file: " << endl;

    while(myfile >> number){
        int result = number;
        result = setThreeMostSignificantBits(result);
        result = clearSpecificBits(result);
        result = multiplyByFour(result);
        result = divideByFour(result);
        result = setNthBit(result, n);
        cout << "Original: " << number << " (Binary: ";
        transformToBinary(number);
        cout << "Transformed: " << result << " (Binary: ";
        transformToBinary(result);
        resultFile << result << endl; // write the result to the result file

    }

    if (myfile.fail()){
        cerr << "Error reading number from file." << endl;
    }

    
    myfile.close();
    resultFile.close();
    cout << "Processing complete. Results written to result.txt" << endl;
    
    


}

int main() {
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;

    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Create a new file\n";
        cout << "2. Read and display file contents\n";
        cout << "3. Add numbers to the file\n";
        cout << "4. Get number by index\n";
        cout << "5. Count numbers in the file\n";
        cout << "6. Process file numbers with bitwise operations\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                createMyFile(filename);
                break;
            case 2:
                readMyFile(filename);
                break;
            case 3:
                addNumbersToFile(filename);
                break;
            case 4: {
                int index;
                cout << "Enter the index of the number to retrieve: ";
                cin >> index;
                int number = getNumberByIndex(filename, index);
                if (number != 0) {
                    cout << "Number at index " << index << ": " << number << endl;
                }
                break;
            }
            case 5:
                countNumbersInFile(filename);
                break;
            case 6:
                processFileNumbers(filename);
                break;
            case 7:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 7); 
    return 0;
}
