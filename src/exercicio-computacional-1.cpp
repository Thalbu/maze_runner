#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <thread>
#include <chrono>

using namespace std;

// Função para carregar o labirinto a partir de um arquivo
bool carregar_labirinto(const string& caminho_arquivo, vector<vector<char>>& labirinto, int& linhas, int& colunas, pair<int, int>& inicio) {
    ifstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo." << endl;
        return false;
    }

    arquivo >> linhas >> colunas;
    labirinto.resize(linhas);
    string linha;
    getline(arquivo, linha); // Para ler o '\n' após as dimensões

    for (int i = 0; i < linhas; i++) {
        getline(arquivo, linha);
        labirinto[i] = vector<char>(linha.begin(), linha.end());
        size_t pos = linha.find('e');
        if (pos != string::npos) {
            inicio = {i, static_cast<int>(pos)};
        }
    }

    arquivo.close();
    return true;
}

// Função para imprimir o labirinto
void imprimir_labirinto(const vector<vector<char>>& labirinto) {
    this_thread::sleep_for(chrono::milliseconds(100));

    // Limpa a tela no terminal
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    for (const auto& linha : labirinto) {
        for (const auto& celula : linha) {
            cout << celula;
        }
        cout << endl;
    }
}

// Função para resolver o labirinto
bool resolver_labirinto(vector<vector<char>>& labirinto, int linhas, int colunas, pair<int, int> inicio) {
    stack<pair<int, int>> pilha;
    pilha.push(inicio);

    while (!pilha.empty()) {
        auto [x, y] = pilha.top();
        pilha.pop();

        // Verifica se é a saída
        if (labirinto[x][y] == 's') {
            labirinto[x][y] = 'o';
            cout << "Saída encontrada!\n";
            imprimir_labirinto(labirinto);
            return true;
        }

        if (labirinto[x][y] == 'x' || labirinto[x][y] == 'e') {
            labirinto[x][y] = 'o';
        }

        imprimir_labirinto(labirinto);
        cout << endl;

        labirinto[x][y] = '.';

        // Movimentos para as próximas posições (cima, baixo, esquerda, direita)
        for (const auto& [nx, ny] : vector<pair<int, int>>{{x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1}}) {
            if (nx >= 0 && nx < linhas && ny >= 0 && ny < colunas && labirinto[nx][ny] != '#' && labirinto[nx][ny] != '.') {
                pilha.push({nx, ny});
            }
        }
    }

    cout << "Nenhum caminho para a saída foi encontrado." << endl;
    return false;
}

int main() {
    vector<vector<char>> labirinto;
    int linhas, colunas;
    pair<int, int> inicio;

    if (!carregar_labirinto("../data/maze2.txt", labirinto, linhas, colunas, inicio)) {
        return 1;
    }

    imprimir_labirinto(labirinto);
    cout << "Resolvendo labirinto...\n";

    resolver_labirinto(labirinto, linhas, colunas, inicio);

    return 0;
}
