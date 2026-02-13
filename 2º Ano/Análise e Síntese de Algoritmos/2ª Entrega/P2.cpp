#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;

const int WHITE = 0;
const int GREY = 1;
const int BLACK = 2;

int BFS(vector<vector<int>>& graph, int source) {
    int n = graph.size();
    vector<int> color(n, WHITE);
    vector<int> visitOrder;
    queue<int> q;
    vector<int> dist(n, -1);
    
    // Inicialização da fonte
    color[source] = GREY;
    q.push(source);
    dist[source] = 0;
    
    while (!q.empty()) {
        int currentVertex = q.front();
        q.pop();
        
        // Adicionar vértice á queue
        visitOrder.push_back(currentVertex);
        
        // Explorar vértices adjacentes
        for (int adjacentVertex = 1; adjacentVertex < (int)graph.size(); adjacentVertex++) {
            if (graph[currentVertex][adjacentVertex] == 1 && color[adjacentVertex] == WHITE) {
                // Marcar vértice adjacente
                color[adjacentVertex] = GREY;
                q.push(adjacentVertex);
                dist[adjacentVertex] = dist[currentVertex] + 1;
            }
        }
        
        // Marcar vértice visitado a preto (completamente visitado)
        color[currentVertex] = BLACK;
    }
    
    return *max_element(dist.begin(), dist.end());
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    int n, m, l;
    cin >> n >> m >> l;
    
    //Rastreamento das estações nas linhas
    vector<vector<int>> linhasEstacoes(l + 1);
    
    // Verificar estações
    vector<bool> estacoes(n + 1, false);
    int contadorEstacoes = 0;
    
    // Ler conexões
    for (int i = 0; i < m; i++) {
        int e1, e2, linha;
        cin >> e1 >> e2 >> linha;
        
        // Adicionar estações à linha correspondente
        linhasEstacoes[linha].push_back(e1);
        linhasEstacoes[linha].push_back(e2);
        
        // Marcar estações
        if (!estacoes[e1]) {
            estacoes[e1] = true;
            contadorEstacoes++;
        }
        if (!estacoes[e2]) {
            estacoes[e2] = true;
            contadorEstacoes++;
        }
    }
    
    // Verificar se todas as estações estão conectadas
    if (contadorEstacoes != n) {
        cout << -1 << endl;
        return 0;
    }
    
    // Construir grafo de linhas
    vector<vector<int>> grafoLinhas(l + 1, vector<int>(l + 1, 0));
    
    // Encontrar linhas com estações em comum
    for (int linha1 = 1; linha1 <= l; linha1++) {
        for (int linha2 = linha1 + 1; linha2 <= l; linha2++) {
            // Verificar intersecção de estações
            set<int> estacoesComunsSet;
            for (int e : linhasEstacoes[linha1]) {
                estacoesComunsSet.insert(e);
            }
            // Verifica se há uma linha conectada a todas as estações
            if ((int)estacoesComunsSet.size() == n) {
                cout << 0 << endl;
                return 0;
            }
            
            bool temEstacaoComum = false;
            for (int e : linhasEstacoes[linha2]) {
                if (estacoesComunsSet.count(e)) {
                    temEstacaoComum = true;
                    break;
                }
            }
            
            // Se tiver uma estação em comum, criar conexão no grafo de linhas
            if (temEstacaoComum) {
                grafoLinhas[linha1][linha2] = 1;
                grafoLinhas[linha2][linha1] = 1;
            }

        }
    }
    
    // Calcular maior distância no grafo de linhas
    vector<int> vectorResultados(l + 1, 0);
    for (int i = 1; i <= l; ++i) {
        vectorResultados[i] = BFS(grafoLinhas, i);
    }
    
    cout << *max_element(vectorResultados.begin(), vectorResultados.end()) << endl;
    return 0;
}