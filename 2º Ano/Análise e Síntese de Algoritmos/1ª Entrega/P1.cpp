#include <iostream>
#include <vector>
#include <list>
#include <string>
using namespace std;

struct ResultInfo {
    int resultadoOpBin;
    int posicaoParentisis;
    int leftResult;
    int rightResult;
};

string reconstructExpression(int limInf, int limSup, int target, vector<vector<list<ResultInfo*>>>& resultTable, vector<vector<int>>& operationTable) {
    // Caso base: se for um único elemento
    if (limInf == limSup) {
        return to_string(resultTable[limInf][limSup].front()->resultadoOpBin);
    }
    string seq = "";
    // Reconstrução da sequência
    for (const auto& result : resultTable[limInf][limSup]) {
        if (result->resultadoOpBin == target) {
            
            string leftExpr = reconstructExpression(limInf, result->posicaoParentisis, result->leftResult, resultTable, operationTable);
            string rightExpr = reconstructExpression(result->posicaoParentisis + 1, limSup, result->rightResult, resultTable, operationTable);
            return seq = seq + "(" + leftExpr + " " + rightExpr + ")";
        }
    }
    // Se nenhum resultado encontrado
    return seq;
}


int main() {
    
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n, m;
    cin >> n >> m;
    
    // Input da tabela de operação
    vector<vector<int>> operationTable(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> operationTable[i][j];

    // Input da sequência
    vector<int> sequence(m);
    for (int i = 0; i < m; ++i)
        cin >> sequence[i];
    

    // Input do resultado desejado
    int target;
    cin >> target;

    // Tabela de resultados
    vector<vector<list<ResultInfo*>>> resultTable(m + 1, vector<list<ResultInfo*>>(m +1)); 

        
    // Preenchimento da tabela para o caso base
    for (int i = 1; i <= m; i++) {
        ResultInfo* baseInfo = (ResultInfo*)malloc(sizeof (struct ResultInfo));
        baseInfo->resultadoOpBin = sequence[i -1];
        baseInfo->posicaoParentisis = i;
        baseInfo->leftResult = -1;
        baseInfo->rightResult = -1;
        
        resultTable[i][i].push_back(baseInfo);
        
    }
    // Iteração por diferentes tamanhos de subsequências
    for (int len = 1; len <= m-1; len++){
        
        for (int i = 1; i  <= m - len; i++){
            
            int j = i + len;

            //estamos a preencher a celula B[i,j]

            vector<bool> seen = vector<bool>(m + 1, false);
            int count = 0;

            // Iteração por todas as formas de dividir a subsequência
            for(int p = j - 1; p >= i && count < n; p--){

                list<ResultInfo*> L = resultTable[i][p];
                list<ResultInfo*> R = resultTable[p+1][j];

                for(list<ResultInfo*>::iterator il = L.begin(); il != L.end() && count < n; il++){
                    for(list<ResultInfo*>::iterator ir = R.begin(); ir != R.end() && count < n; ir++){
                        ResultInfo* rl = *il;
                        ResultInfo* rr = *ir;
                         int v = operationTable[rl -> resultadoOpBin -1] [rr -> resultadoOpBin - 1];

                         if(!seen[v]){
                            ResultInfo * baseInfo = (ResultInfo*)malloc(sizeof (struct ResultInfo));
                            baseInfo->resultadoOpBin = v;
                            baseInfo->posicaoParentisis = p;
                            baseInfo->leftResult = rl ->resultadoOpBin;
                            baseInfo->rightResult = rr -> resultadoOpBin;                           
                        
                            // Actualização das tabelas
                            resultTable[i][j].push_back(baseInfo);
                            seen[v] = true;
                            count++;   
                        }
                    }   
                }
            }
        }
    }

    bool foundTarget = false;
    for (const auto& result : resultTable[1][m]) {
                
        if (result->resultadoOpBin == target) {
            cout << "1\n";
            cout << reconstructExpression(1, m, target, resultTable, operationTable) << "\n";
            foundTarget = true;
            break; 
        }
    }

    // Se nenhum resultado encontrado
    if (!foundTarget) {
        cout << "0\n";
    }

    return 0;

}
