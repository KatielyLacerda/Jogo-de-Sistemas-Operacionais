#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

// Semáforos para sincronização
HANDLE player1_ready;
HANDLE player2_ready;
HANDLE choices_done; // Semáforo para indicar que ambos escolheram

// Escolhas dos jogadores
string player1_choice;
string player2_choice;

// Valida as escolhas para garantir que nada não esperado passe
bool is_valid_choice(const string &choice) {
    return (choice == "Pedra" || choice == "Papel" || choice == "Tesoura");
}

// Determina o vencedor com base nas regras do jogo
string determine_winner(const string &p1, const string &p2) {
    if (p1 == p2) {
        return "Empate!";
    } else if ((p1 == "Pedra" && p2 == "Tesoura") ||
               (p1 == "Tesoura" && p2 == "Papel") ||
               (p1 == "Papel" && p2 == "Pedra")) {
        return "Jogador 1 venceu!";
    } else {
        return "Jogador 2 venceu!";
    }
}

// Limpa a tela (Windows)
void clear_screen() {
    system("cls");
}

// Vez do jogador 1 jogar
void player1() {
    while (true) {
        WaitForSingleObject(player1_ready, INFINITE); // Espera sua vez

        do {
            cout << "Jogador 1: Faça sua escolha (Pedra, Papel, Tesoura ou Sair para encerrar o jogo): ";
            cin >> player1_choice;

            if (player1_choice == "Sair") {
                ReleaseSemaphore(player2_ready, 1, NULL); // Libera o jogador 2 para finalizar
                return;
            }

            if (!is_valid_choice(player1_choice)) {
                cout << "Escolha inválida. Tente novamente.\n";
            }
        } while (!is_valid_choice(player1_choice));

        // Limpa a tela após a escolha, para que o jogador 2 não veja o que foi escolhido
        clear_screen();

        // Libera o jogador 2 para jogar
        ReleaseSemaphore(player2_ready, 1, NULL);
    }
}

// Vez do jogador 2 jogar
void player2() {
    while (true) {
        WaitForSingleObject(player2_ready, INFINITE); // Espera sua vez

        do {
            cout << "Jogador 2: Faça sua escolha (Pedra, Papel, Tesoura ou Sair para encerrar o jogo): ";
            cin >> player2_choice;

            if (player2_choice == "Sair") {
                return;
            }

            if (!is_valid_choice(player2_choice)) {
                cout << "Escolha inválida. Tente novamente.\n";
            }
        } while (!is_valid_choice(player2_choice));

        // Mostra o resultado da rodada
        string winner = determine_winner(player1_choice, player2_choice);
        cout << "\nResultado:\n"
             << "Jogador 1 escolheu: " << player1_choice << "\n"
             << "Jogador 2 escolheu: " << player2_choice << "\n"
             << winner << endl;

        // Libera o fluxo de volta para o jogador 1
        ReleaseSemaphore(choices_done, 1, NULL);
    }
}

void game_loop() {
    while (true) {
        cout << "Jogo de Pedra, Papel e Tesoura\n";
        cout << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n";

        // Inicializa os semáforos para a rodada
        ReleaseSemaphore(player1_ready, 1, NULL); // Libera jogador 1 para começar
        WaitForSingleObject(choices_done, INFINITE); // Aguarda a rodada finalizar

        cout << "\nPressione Enter para começar outra rodada.\n";
        cin.ignore();
        cin.get(); // Aguarda o Enter do jogador para começar tudo novamente

        // Limpa a tela para reiniciar o jogo
        clear_screen();
    }
}

int main() {
    // Inicializa os semáforos
    player1_ready = CreateSemaphore(NULL, 0, 1, NULL);
    player2_ready = CreateSemaphore(NULL, 0, 1, NULL);
    choices_done = CreateSemaphore(NULL, 0, 1, NULL);

    // Cria as threads dos jogadores
    std::thread t1(player1);
    std::thread t2(player2);

    // Inicia o loop do jogo
    game_loop();

    // Aguarda as threads (finalizam quando um jogador escolhe "Sair")
    t1.join();
    t2.join();

    // Fecha os semáforos
    CloseHandle(player1_ready);
    CloseHandle(player2_ready);
    CloseHandle(choices_done);

    cout << "\nJogo encerrado. Obrigado por jogar!\n";
    return 0;
}
