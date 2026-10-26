#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include <memory>

using namespace std;

// ====================== RLE (Run-Length Encoding) ======================
string rle_encode(const string& text) {
    if (text.empty()) return "";
    
    string encoded;
    int count = 1;
    char current = text[0];
    
    for (size_t i = 1; i < text.length(); i++) {
        if (text[i] == current) {
            count++;
        } else {
            encoded += to_string(count) + current;
            current = text[i];
            count = 1;
        }
    }
    encoded += to_string(count) + current;
    
    return encoded;
}

string rle_decode(const string& encoded) {
    string decoded;
    string num;
    
    for (size_t i = 0; i < encoded.length(); i++) {
        if (isdigit(encoded[i])) {
            num += encoded[i];
        } else {
            int count = stoi(num);
            decoded += string(count, encoded[i]);
            num.clear();
        }
    }
    
    return decoded;
}

double compression_ratio(const string& original, const string& compressed) {
    if (original.empty()) return 0.0;
    return (1.0 - (double)compressed.size() / original.size()) * 100.0;
}

// ====================== LZ77 ======================
struct LZ77Token {
    int offset;
    int length;
    char next;
    
    LZ77Token(int o = 0, int l = 0, char n = '\0') : offset(o), length(l), next(n) {}
};

vector<LZ77Token> lz77_encode(const string& input, int window_size = 10, int buffer_size = 5) {
    vector<LZ77Token> tokens;
    size_t pos = 0;
    size_t input_len = input.length();
    
    while (pos < input_len) {
        int match_offset = 0;
        int match_length = 0;
        
        // Поиск самой длинной совпадающей подстроки в окне
        int search_start = max(0, (int)pos - window_size);
        for (int i = search_start; i < (int)pos; i++) {
            int len = 0;
            while (pos + len < input_len && 
                   i + len < (int)pos && 
                   len < buffer_size && 
                   input[i + len] == input[pos + len]) {
                len++;
            }
            if (len > match_length) {
                match_length = len;
                match_offset = (int)pos - i;
            }
        }
        
        char next_char = (pos + match_length < input_len) ? input[pos + match_length] : '\0';
        tokens.push_back(LZ77Token(match_offset, match_length, next_char));
        pos += match_length + 1;
    }
    
    return tokens;
}

string lz77_decode(const vector<LZ77Token>& tokens) {
    string decoded;
    
    for (const auto& token : tokens) {
        if (token.length > 0) {
            size_t start = decoded.length() - token.offset;
            for (int i = 0; i < token.length; i++) {
                decoded += decoded[start + i];
            }
        }
        if (token.next != '\0') {
            decoded += token.next;
        }
    }
    
    return decoded;
}

// ====================== LZ78 ======================
struct LZ78Token {
    int index;
    char next;
    
    LZ78Token(int i = 0, char n = '\0') : index(i), next(n) {}
};

vector<LZ78Token> lz78_encode(const string& input) {
    vector<LZ78Token> tokens;
    unordered_map<string, int> dictionary;
    string current;
    int dict_size = 1; // индекс 0 зарезервирован для пустой строки
    
    dictionary[""] = 0;
    
    for (char c : input) {
        string new_current = current + c;
        if (dictionary.find(new_current) == dictionary.end()) {
            // Находим самый длинный префикс, который есть в словаре
            int prefix_index = dictionary[current];
            tokens.push_back(LZ78Token(prefix_index, c));
            dictionary[new_current] = dict_size++;
            current.clear();
        } else {
            current = new_current;
        }
    }
    
    // Обработка оставшейся части
    if (!current.empty()) {
        tokens.push_back(LZ78Token(dictionary[current], '\0'));
    }
    
    return tokens;
}

string lz78_decode(const vector<LZ78Token>& tokens) {
    vector<string> dictionary;
    dictionary.push_back(""); // начинаем с пустой строки
    string decoded;
    
    for (const auto& token : tokens) {
        string entry = dictionary[token.index] + (token.next != '\0' ? string(1, token.next) : "");
        decoded += entry;
        dictionary.push_back(entry);
    }
    
    return decoded;
}

// ====================== Shannon-Fano ======================
struct SFNode {
    char symbol;
    double probability;
    string code;
    
    SFNode(char s, double p) : symbol(s), probability(p), code("") {}
    
    bool operator<(const SFNode& other) const {
        return probability > other.probability; // для сортировки по убыванию
    }
};

void shannon_fano(vector<SFNode>& nodes, int start, int end, double total_prob) {
    if (end - start <= 1) return;
    
    double half = 0.0;
    int split = start;
    
    // Находим точку разделения
    for (int i = start; i < end; i++) {
        if (half + nodes[i].probability <= total_prob / 2) {
            half += nodes[i].probability;
            split++;
        } else {
            break;
        }
    }
    
    // Присваиваем коды
    for (int i = start; i < split; i++) {
        nodes[i].code += "0";
    }
    for (int i = split; i < end; i++) {
        nodes[i].code += "1";
    }
    
    // Рекурсивно обрабатываем обе части
    shannon_fano(nodes, start, split, half);
    shannon_fano(nodes, split, end, total_prob - half);
}

string shannon_fano_encode(const string& text, vector<SFNode>& nodes) {
    // Подсчет частот
    map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }
    
    // Создание узлов
    for (auto& pair : freq) {
        nodes.push_back(SFNode(pair.first, (double)pair.second / text.length()));
    }
    
    // Сортировка по убыванию вероятности
    sort(nodes.begin(), nodes.end());
    
    // Кодирование Шеннона-Фано
    double total_prob = 1.0;
    shannon_fano(nodes, 0, nodes.size(), total_prob);
    
    // Создание таблицы кодирования
    map<char, string> code_table;
    for (const auto& node : nodes) {
        code_table[node.symbol] = node.code;
    }
    
    // Кодирование текста
    string encoded;
    for (char c : text) {
        encoded += code_table[c];
    }
    
    return encoded;
}

string shannon_fano_decode(const string& encoded, const vector<SFNode>& nodes) {
    // Создание таблицы декодирования
    map<string, char> decode_table;
    for (const auto& node : nodes) {
        decode_table[node.code] = node.symbol;
    }
    
    // Декодирование
    string decoded;
    string current_code;
    
    for (char bit : encoded) {
        current_code += bit;
        if (decode_table.find(current_code) != decode_table.end()) {
            decoded += decode_table[current_code];
            current_code.clear();
        }
    }
    
    return decoded;
}

// ====================== Huffman ======================
struct HuffmanNode {
    char symbol;
    int frequency;
    shared_ptr<HuffmanNode> left;
    shared_ptr<HuffmanNode> right;
    
    HuffmanNode(char s, int f) : symbol(s), frequency(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f, shared_ptr<HuffmanNode> l, shared_ptr<HuffmanNode> r) 
        : symbol('\0'), frequency(f), left(l), right(r) {}
    
    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

struct CompareHuffmanNode {
    bool operator()(const shared_ptr<HuffmanNode>& a, const shared_ptr<HuffmanNode>& b) {
        return a->frequency > b->frequency;
    }
};

void generate_huffman_codes(shared_ptr<HuffmanNode> node, string code, 
                           map<char, string>& codes, map<string, char>& reverse_codes) {
    if (!node) return;
    
    if (node->isLeaf()) {
        codes[node->symbol] = code;
        reverse_codes[code] = node->symbol;
    } else {
        generate_huffman_codes(node->left, code + "0", codes, reverse_codes);
        generate_huffman_codes(node->right, code + "1", codes, reverse_codes);
    }
}

pair<string, map<char, string>> huffman_encode(const string& text, vector<pair<char, double>>& stats) {
    // Подсчет частот
    map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }
    
    // Создание узлов
    priority_queue<shared_ptr<HuffmanNode>, vector<shared_ptr<HuffmanNode>>, CompareHuffmanNode> pq;
    
    for (auto& pair : freq) {
        stats.push_back({pair.first, (double)pair.second / text.length()});
        pq.push(make_shared<HuffmanNode>(pair.first, pair.second));
    }
    
    // Построение дерева Хаффмана
    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        
        auto parent = make_shared<HuffmanNode>(left->frequency + right->frequency, left, right);
        pq.push(parent);
    }
    
    auto root = pq.top();
    
    // Генерация кодов
    map<char, string> codes;
    map<string, char> reverse_codes;
    generate_huffman_codes(root, "", codes, reverse_codes);
    
    // Кодирование текста
    string encoded;
    for (char c : text) {
        encoded += codes[c];
    }
    
    return {encoded, codes};
}

string huffman_decode(const string& encoded, const map<string, char>& reverse_codes) {
    string decoded;
    string current_code;
    
    for (char bit : encoded) {
        current_code += bit;
        if (reverse_codes.find(current_code) != reverse_codes.end()) {
            decoded += reverse_codes.at(current_code);
            current_code.clear();
        }
    }
    
    return decoded;
}

// ====================== Вспомогательные функции ======================
void print_binary_string(const string& str) {
    for (char c : str) {
        if (c == '0' || c == '1') {
            cout << c;
        } else {
            bitset<8> bits(c);
            cout << bits;
        }
    }
    cout << endl;
}

void print_lz77_tokens(const vector<LZ77Token>& tokens) {
    cout << "LZ77 Tokens: (offset, length, next)" << endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        cout << "[" << i << "]: (" << tokens[i].offset << ", " 
             << tokens[i].length << ", ";
        if (tokens[i].next == '\0') cout << "NULL";
        else cout << "'" << tokens[i].next << "'";
        cout << ")" << endl;
    }
}

void print_lz78_tokens(const vector<LZ78Token>& tokens) {
    cout << "LZ78 Tokens: (index, next)" << endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        cout << "[" << i << "]: (" << tokens[i].index << ", ";
        if (tokens[i].next == '\0') cout << "NULL";
        else cout << "'" << tokens[i].next << "'";
        cout << ")" << endl;
    }
}

void print_shannon_fano_table(const vector<SFNode>& nodes) {
    cout << "\nShannon-Fano Table:" << endl;
    cout << "Symbol | Probability | Code" << endl;
    cout << "-------|-------------|------" << endl;
    for (const auto& node : nodes) {
        cout << "  '" << node.symbol << "'  | "
             << fixed << setprecision(4) << node.probability << "    | "
             << node.code << endl;
    }
}

void print_huffman_stats(const string& text, const map<char, string>& codes, 
                        const vector<pair<char, double>>& stats) {
    cout << "\nHuffman Coding Statistics:" << endl;
    cout << "Symbol | Frequency | Probability | Code" << endl;
    cout << "-------|-----------|-------------|------" << endl;
    
    map<char, int> freq;
    for (char c : text) freq[c]++;
    
    double avg_length = 0.0;
    double variance = 0.0;
    
    for (const auto& stat : stats) {
        char symbol = stat.first;
        double prob = stat.second;
        string code = codes.at(symbol);
        int frequency = freq[symbol];
        
        cout << "  '" << symbol << "'  | " 
             << setw(9) << frequency << " | "
             << fixed << setprecision(4) << setw(11) << prob << " | "
             << code << endl;
        
        avg_length += prob * code.length();
    }
    
    // Расчет дисперсии
    for (const auto& stat : stats) {
        char symbol = stat.first;
        double prob = stat.second;
        string code = codes.at(symbol);
        variance += prob * pow(code.length() - avg_length, 2);
    }
    
    cout << "\nAverage code length: " << avg_length << " bits" << endl;
    cout << "Variance: " << variance << endl;
}

// ====================== Основная программа ======================
int main() {
    cout << "========== Data Compression Algorithms (Variant 2) ==========\n" << endl;
    
    // Тестовые данные из варианта 2
    string binary_lz77 = "0100100010010000101";
    string text_lz78 = "упупапекапекаупуп";
    string text_shannon_fano = "One, two, Freddy's coming for you Three, four, better lock your door Five, six, grab a crucifix Seven, eight, gonna stay up late.";
    string fio = "Ivanov Ivan Ivanovich"; // Замени на свое ФИО
    
    cout << "=== Test Data ===" << endl;
    cout << "LZ77 binary string: " << binary_lz77 << endl;
    cout << "LZ78 text: " << text_lz78 << endl;
    cout << "Shannon-Fano text: " << text_shannon_fano.substr(0, 50) << "..." << endl;
    cout << "FIO for Huffman: " << fio << endl;
    
    // ========== RLE Тестирование ==========
    cout << "\n=== RLE Encoding ===" << endl;
    
    // Тест 1: Длинная серия повторяющихся символов
    string repeat_test = "AAAAABBBBBCCCCCDDDDDEEEEE";
    string rle_encoded_repeat = rle_encode(repeat_test);
    string rle_decoded_repeat = rle_decode(rle_encoded_repeat);
    double rle_ratio_repeat = compression_ratio(repeat_test, rle_encoded_repeat);
    
    cout << "\nTest 1 - Repeated characters:" << endl;
    cout << "Original: " << repeat_test << endl;
    cout << "Encoded: " << rle_encoded_repeat << endl;
    cout << "Decoded: " << rle_decoded_repeat << endl;
    cout << "Compression ratio: " << fixed << setprecision(2) << rle_ratio_repeat << "%" << endl;
    cout << "Correct decoding: " << (repeat_test == rle_decoded_repeat ? "YES" : "NO") << endl;
    
    // Тест 2: Неповторяющиеся символы
    string non_repeat_test = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string rle_encoded_non = rle_encode(non_repeat_test);
    string rle_decoded_non = rle_decode(rle_encoded_non);
    double rle_ratio_non = compression_ratio(non_repeat_test, rle_encoded_non);
    
    cout << "\nTest 2 - Non-repeating characters:" << endl;
    cout << "Original: " << non_repeat_test << endl;
    cout << "Encoded: " << rle_encoded_non << endl;
    cout << "Decoded: " << rle_decoded_non << endl;
    cout << "Compression ratio: " << fixed << setprecision(2) << rle_ratio_non << "%" << endl;
    cout << "Correct decoding: " << (non_repeat_test == rle_decoded_non ? "YES" : "NO") << endl;
    
    // ========== LZ77 Тестирование ==========
    cout << "\n=== LZ77 Encoding (Binary) ===" << endl;
    auto lz77_tokens = lz77_encode(binary_lz77);
    print_lz77_tokens(lz77_tokens);
    
    string lz77_decoded = lz77_decode(lz77_tokens);
    cout << "Original: " << binary_lz77 << endl;
    cout << "Decoded:  " << lz77_decoded << endl;
    cout << "Correct decoding: " << (binary_lz77 == lz77_decoded ? "YES" : "NO") << endl;
    
    // ========== LZ78 Тестирование ==========
    cout << "\n=== LZ78 Encoding ===" << endl;
    auto lz78_tokens = lz78_encode(text_lz78);
    print_lz78_tokens(lz78_tokens);
    
    string lz78_decoded = lz78_decode(lz78_tokens);
    cout << "Original: " << text_lz78 << endl;
    cout << "Decoded:  " << lz78_decoded << endl;
    cout << "Correct decoding: " << (text_lz78 == lz78_decoded ? "YES" : "NO") << endl;
    
    // ========== Shannon-Fano Тестирование ==========
    cout << "\n=== Shannon-Fano Encoding ===" << endl;
    vector<SFNode> sf_nodes;
    string sf_encoded = shannon_fano_encode(text_shannon_fano, sf_nodes);
    string sf_decoded = shannon_fano_decode(sf_encoded, sf_nodes);
    
    print_shannon_fano_table(sf_nodes);
    
    cout << "\nOriginal length: " << text_shannon_fano.length() << " chars" << endl;
    cout << "Encoded length: " << sf_encoded.length() << " bits" << endl;
    cout << "Compression ratio: " << fixed << setprecision(2) 
         << compression_ratio(text_shannon_fano, sf_encoded) << "%" << endl;
    cout << "Correct decoding: " << (text_shannon_fano == sf_decoded ? "YES" : "NO") << endl;
    
    // ========== Huffman Тестирование ==========
    cout << "\n=== Huffman Encoding (FIO) ===" << endl;
    
    vector<pair<char, double>> huffman_stats;
    auto huffman_result = huffman_encode(fio, huffman_stats);
    string huffman_encoded = huffman_result.first;
    map<char, string> huffman_codes = huffman_result.second;
    
    // Декодирование
    map<string, char> reverse_codes;
    for (const auto& pair : huffman_codes) {
        reverse_codes[pair.second] = pair.first;
    }
    string huffman_decoded = huffman_decode(huffman_encoded, reverse_codes);
    
    print_huffman_stats(fio, huffman_codes, huffman_stats);
    
    cout << "\nOriginal FIO: " << fio << endl;
    cout << "Encoded: ";
    print_binary_string(huffman_encoded);
    cout << "Decoded: " << huffman_decoded << endl;
    cout << "Original size: " << fio.length() * 8 << " bits" << endl;
    cout << "Compressed size: " << huffman_encoded.length() << " bits" << endl;
    cout << "Compression ratio: " << fixed << setprecision(2) 
         << compression_ratio(fio, huffman_encoded) << "%" << endl;
    cout << "Correct decoding: " << (fio == huffman_decoded ? "YES" : "NO") << endl;
    
    // ========== Сравнение с архиватором ==========
    cout << "\n=== Comparison with Archiver ===" << endl;
    
    // Создаем тестовый файл
    ofstream test_file("test_text.txt");
    if (test_file.is_open()) {
        test_file << text_shannon_fano;
        test_file.close();
        
        // Получаем размер файла
        ifstream file("test_text.txt", ios::binary | ios::ate);
        size_t original_size = file.tellg();
        file.close();
        
        cout << "Original file size: " << original_size << " bytes" << endl;
        cout << "Note: To compare with archiver (zip, gzip), compress the file manually:" << endl;
        cout << "  gzip test_text.txt" << endl;
        cout << "  ls -la test_text.txt*" << endl;
        cout << "\nThen compare the sizes to see actual compression ratio." << endl;
    }
    
    cout << "\n========== Program Complete ==========" << endl;
    
    return 0;
}