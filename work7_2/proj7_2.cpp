#include <iostream>
#include <vector>
#include <tuple>
#include <functional>
#include <algorithm>
using namespace std;

/*
Список смежности (adjacency list) — это способ представления графа в виде коллекции списков для каждой вершины.
 Каждый список хранит вершины, смежные (соседние) с данной.
*/

class Graph {
private:
    int V; //количество вершин
    //Создаем список смежности: вектор векторов пар (вершина, вес)
    vector<vector<pair<int,int>>> adj; //с pair вся инфа в одном месте, не нужно 2 отдельных вектора

    //Определить мосты графа.
    //вспомогательная функция для поиска мостов
    void bridgeUtil(int u,vector<bool>& visited, vector<int>& disc, vector<int>& low, int parent, vector<pair<int,int>>& bridges, int& time) {
        //u - текущая вершина
        //visited - массив посещённых вершин
        //disc - время обнаружения вершин при DFS
        //low - низкое значение вершин
        //parent - родительская вершина в DFS дереве
        //bridges - вектор для сохранения найденных мостов
        //time - текущее время для меток времени

        visited[u] = true; //отмечаем текущую вершину как посещённую
        disc[u] = low[u] = ++time; //запоминаем время обнаружения и низкое значение

        for (auto& edge : adj[u]) { //проходим по всем смежным вершинам

            int v = edge.first; //извлекаем смежную вершину
            if (!visited[v]) { //если сосед v не посещён — рекурсивный вызов
                bridgeUtil(v, visited, disc, low, u, bridges, time);
                low[u] = min(low[u], low[v]);
                if (low[v] > disc[u]) bridges.push_back({u,v});
            } else if (v != parent) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }

public:
    Graph() : V(0) {}

    void addVertex() { //добавление узла
        adj.push_back({});
        V++;
    }

    //добавление ребра
    void addEdge(int u, int v, int w = 1) { //по умолчанию вес = 1 и вершины нумеруются с 0
        if (u >= V || v >= V) return; //существуют ли вершины
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    //получение количества вершин
    int numVertices() const { return V; }

    //получение смежных вершин
    vector<pair<int,int>> getAdj(int u) const {
        if (u >= V) return {}; //если вершины не существует, возвращаем пустой вектор
        return adj[u];//копируем вектор соседей
    }

    //ввод графа с клавиатуры
   void inputGraph() {
    int numVertices, numEdges;
    cin >> numVertices >> numEdges;
    
    // ОЧИСТКА СТАРОГО ГРАФА
    adj.clear();  // Очищаем список смежности
    V = 0;        // Сбрасываем счётчик вершин
    
    // Создаём пустые векторы для новых вершин
    adj.resize(numVertices);
    V = numVertices;
    
    for (int i=0; i<numEdges; i++) {
        int u,v,w;
        cin >> u >> v >> w;
        addEdge(u,v,w);
    }
}

    //нахождение мостов в графе
    vector<pair<int,int>> findBridges() {
        //инициализация вспомогательных структур
        vector<bool> visited(V,false); //все вершины не посещены
        vector<int> disc(V,-1), low(V,-1); //время обнаружения (пока неизвестно) и низкое значение
        vector<pair<int,int>> bridges; //пустой список мостов

        int time = 0;

        for(int i=0;i<V;i++)//если вершина не посещена, запускаем DFS
            if(!visited[i]) //начинаем обход в глубину
            //рекурсивный вызов функции для поиска мостов
                bridgeUtil(i, visited, disc, low, -1, bridges, time);

        return bridges;//возвращаем найденные мосты
    }

    //алгоритм Крускала для MST
    vector<tuple<int,int,int>> kruskalMST() {
        //создание списка всех рёбер
        //структура для хранения рёбер
        struct Edge { int u,v,w; };
        vector<Edge> edges;

        //заполнение списка рёбер из матрицы смежности
        for(int u=0; u<V; u++)
        //проходим по всем смежным вершинам
            for(auto& e: adj[u])
            //чтобы не добавлять ребро дважды
            //добавляем ребро только если u < v
            
                if(u < e.first) edges.push_back({u,e.first,e.second}); //добавляем ребро

                //сортируем рёбра по весу
        sort(edges.begin(), edges.end(), [](Edge a, Edge b){ return a.w < b.w; }); 

        //инициализация структур для объединения и поиска
        vector<int> parent(V), rank(V,0);
        for(int i=0;i<V;i++) parent[i]=i; //каждая вершина - свой родитель

        function<int(int)> find = [&](int u){ return parent[u]==u ? u : parent[u]=find(parent[u]); };
        auto unionSets = [&](int u,int v){
            int ru=find(u), rv=find(v);
            if(ru!=rv){
                if(rank[ru]<rank[rv]) parent[ru]=rv;
                else if(rank[ru]>rank[rv]) parent[rv]=ru;
                else { parent[rv]=ru; rank[ru]++; }
            }
        };

        //построение MST(основной цикл Крускала)
        vector<tuple<int,int,int>> mst;
        for(auto& e: edges){
            if(find(e.u)!=find(e.v)){ //если вершины в разных компонентах
                unionSets(e.u,e.v); //если вершины в разных компонентах
                mst.push_back({e.u,e.v,e.w}); //добавляем ребро в MST
            }
        }
        return mst;
    }
    /*пример:
    Рёбра: (2,3,4), (0,3,5), (0,2,6), (1,3,7), (0,1,9), (1,2,11)

Шаг 1: Берём (2,3,4) — добавляем в MST
Шаг 2: Берём (0,3,5) — добавляем
Шаг 3: Берём (0,2,6) — НЕ добавляем (0 и 2 уже в одной компоненте)
Шаг 4: Берём (1,3,7) — добавляем
MST готов (3 ребра для 4 вершин)
    */

};

int main() {
    Graph g;
    
    // Автоматически создаём тестовый граф из предоставленных данных
    cout << "Создание тестового графа из предоставленных данных из №2: \n";
    
    // данные графа №2
    vector<tuple<int, int, int>> testEdges = {
        {1, 7, 1},   // ребро 1-7 с весом 1
        {2, 2, 1},   // петля 2-2
        {4, 6, 1},   // ребро 4-6
        {8, 3, 1},   // ребро 8-3
        {2, 3, 1},   // ребро 2-3
        {6, 5, 1}    // ребро 6-5
    };
    
    // Находим максимальный номер вершины для создания всех вершин
    int maxVertex = 0;
    for (auto& edge : testEdges) {
        int u, v, w;
        tie(u, v, w) = edge;
        maxVertex = max(maxVertex, max(u, v));
    }
    
    // Создаём вершины
    for (int i = 0; i <= maxVertex; i++) {
        g.addVertex();
    }
    
    // Добавляем рёбра
    for (auto& edge : testEdges) {
        int u, v, w;
        tie(u, v, w) = edge;
        g.addEdge(u, v, w);
    }
    
    cout << "Граф создан! Вершин: " << g.numVertices() << endl;
    cout << "Рёбра:\n";
    for (int i = 0; i < g.numVertices(); i++) {
        cout << i << ": ";
        for (auto& e : g.getAdj(i)) {
            cout << "(" << e.first << ", w=" << e.second << ") ";
        }
        cout << endl;
    }
    cout << endl;
    
    // Теперь запускаем меню для тестирования
    int choice;

    while(true){
        cout << "\n=== Меню ===\n";
        cout << "1. Добавить вершину\n";
        cout << "2. Добавить ребро\n";
        cout << "3. Вывести граф\n";
        cout << "4. Найти мосты\n";
        cout << "5. Построить MST (Крускал)\n";
        cout << "6. Ввести граф с клавиатуры\n";
        cout << "0. Выход\n";
        cout << "Выберите: ";
        cin >> choice;

        if(choice==0) break;

        switch(choice){
            case 1:
                g.addVertex();
                cout << "Вершина " << (g.numVertices() - 1) << " добавлена\n";
                break;
            case 2: {
                int u,v,w;
                cout << "Введите u v w: ";
                cin >> u >> v >> w;
                g.addEdge(u,v,w);
                cout << "Ребро добавлено\n";
                break;
            }
            case 3:
                cout << "Текущий граф:\n";
                for(int i=0;i<g.numVertices();i++){
                    cout << i << ": ";
                    for(auto& e:g.getAdj(i))
                        cout << "(" << e.first << ", w=" << e.second << ") ";
                    cout << endl;
                }
                break;
            case 4: {
                auto bridges=g.findBridges();
                if(bridges.empty()) cout << "Мостов нет\n";
                else {
                    cout << "Мосты в графе:\n";
                    for(auto& b:bridges) cout << b.first << " -- " << b.second << endl;
                }
                break;
            }
            case 5: {
                auto mst=g.kruskalMST();
                if (mst.empty()) {
                    cout << "MST не существует (граф несвязный)\n";
                } else {
                    int total=0;
                    cout << "Минимальное остовное дерево (Крускал):\n";
                    for(auto& e:mst){
                        int u,v,w; tie(u,v,w)=e;
                        cout << u << " -- " << v << " (w=" << w << ")\n";
                        total+=w;
                    }
                    cout << "Общий вес MST: " << total << endl;
                }
                break;
            }
            case 6:
                cout << "Введите количество вершин и рёбер, затем рёбра (u v w):\n";
                g.inputGraph();
                break;
            default:
                cout << "Неверный выбор\n";
        }
    }

    cout << "Программа завершена\n";
    return 0;
}