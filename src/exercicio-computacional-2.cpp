#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

using namespace std;
mutex labirinto_mutex;  // Mutex para sincronizar o acesso ao labirinto
atomic<bool> encontrou_saida(false);  // Flag para indicar se a saída foi encontrada

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

// Função recursiva para resolver o labirinto
bool resolver_labirinto(vector<vector<char>>& labirinto, int linhas, int colunas, pair<int, int> inicio) {
    stack<pair<int, int>> pilha;
    pilha.push(inicio);

    while (!pilha.empty()) {
        if (encontrou_saida.load()) {
            // Se a saída foi encontrada, esta thread termina
            return false;
        }

        auto [x, y] = pilha.top();
        pilha.pop();

        // Verifica se é a saída
        if (labirinto[x][y] == 's') {
            {
                lock_guard<mutex> guard(labirinto_mutex);
                if (!encontrou_saida.load()) {
                    encontrou_saida.store(true);  // Marca que a saída foi encontrada
                    labirinto[x][y] = 'o';
                    cout << "Saída encontrada!\n";
                    imprimir_labirinto(labirinto);
                }
            }
            return true;
        }

        if (labirinto[x][y] == 'x' || labirinto[x][y] == 'e') {
            lock_guard<mutex> guard(labirinto_mutex);
            labirinto[x][y] = 'o';
        }

        imprimir_labirinto(labirinto);

        {
            lock_guard<mutex> guard(labirinto_mutex);
            labirinto[x][y] = '.';
        }

        // Lista de possíveis direções (cima, baixo, esquerda, direita)
        vector<pair<int, int>> movimentos = {
            {x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1}
        };

        // Filtra movimentos válidos
        vector<pair<int, int>> caminhos_validos;
        for (const auto& [nx, ny] : movimentos) {
            if (nx >= 0 && nx < linhas && ny >= 0 && ny < colunas) {
                lock_guard<mutex> guard(labirinto_mutex);
                if (labirinto[nx][ny] != '#' && labirinto[nx][ny] != '.') {
                    caminhos_validos.push_back({nx, ny});
                }
            }
        }

        // Se houver mais de um caminho válido, explorar com threads
        if (caminhos_validos.size() > 1) {
            vector<thread> threads;

            // Cria threads para cada um dos caminhos válidos adicionais
            for (size_t i = 1; i < caminhos_validos.size(); i++) {
                threads.emplace_back([&labirinto, linhas, colunas, caminho = caminhos_validos[i]] {
                    resolver_labirinto(labirinto, linhas, colunas, caminho);
                });
            }

            // Explora o primeiro caminho na thread atual
            pilha.push(caminhos_validos[0]);

            // Espera todas as threads adicionais terminarem
            for (auto& t : threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
        } else if (!caminhos_validos.empty()) {
            pilha.push(caminhos_validos[0]);
        }
    }

    if (!encontrou_saida.load()) {
        cout << "Nenhum caminho para a saída foi encontrado." << endl;
    }
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
