#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

// Variáveis para manipulação de cores
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
enum Color {
    RESET = 7,        // Branco padrão
    BOLD = 15,        // Branco brilhante
    GREEN = 10,       // Verde
    RED = 12,         // Vermelho
    DARK_YELLOW = 14, // Amarelo escuro
    DARK_BLUE = 9     // Azul escuro
};

// Função para mudar a cor do console
void set_color(Color color) {
    SetConsoleTextAttribute(hConsole, color);
}

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
        return "##########################################\n"
               "           Empate!           \n"
               "##########################################";
    } else if ((p1 == "Pedra" && p2 == "Tesoura") ||
               (p1 == "Tesoura" && p2 == "Papel") ||
               (p1 == "Papel" && p2 == "Pedra")) {
        return "##########################################\n"
               "        Jogador 1 venceu! :)        \n"
               "##########################################";
    } else {
        return "##########################################\n"
               "        Jogador 2 venceu! :)        \n"
               "##########################################";
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
            set_color(DARK_BLUE);
            cout << "Jogador 1: Faca sua escolha (Pedra - Papel - Tesoura ou 'Sair'): ";
            set_color(RESET);
            cin >> player1_choice;

            if (player1_choice == "Sair") {
                set_color(RED);
                cout << "Jogador 1 escolheu encerrar o jogo." << endl;
                set_color(RESET);
                ReleaseSemaphore(player2_ready, 1, NULL); // Libera o jogador 2 para finalizar
                return;
            }

            if (!is_valid_choice(player1_choice)) {
                set_color(RED);
                cout << "Escolha inválida. Tente novamente.\n";
                set_color(RESET);
            }
        } while (!is_valid_choice(player1_choice));

        // Limpa a tela após a escolha
        clear_screen();

        // Libera o jogador 2 para jogar
        ReleaseSemaphore(player2_ready, 1, NULL);
    }
}

// Vez do jogador 2 jogar
void player2() {
    while (true) {
        WaitForSingleObject(player2_ready, INFINITE); // Espera sua vez
        set_color(DARK_BLUE);
        cout << "##########################################\n";
        set_color(RED);
        cout << "       Pedra Papel Tesoura\n";
        set_color(DARK_BLUE);
        cout << "##########################################\n";
        set_color(RESET);
        set_color(BOLD);
        cout << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n";
        set_color(RESET);
        set_color(DARK_BLUE);
        cout << "Jogador 2: Faca sua escolha (Pedra - Papel - Tesoura ou 'Sair'): ";
        set_color(RESET);

        do {
            cin >> player2_choice;

            if (player2_choice == "Sair") {
                set_color(RED);
                cout << "Jogador 2 escolheu encerrar o jogo." << endl;
                set_color(RESET);
                exit(0);
            }

            if (!is_valid_choice(player2_choice)) {
                set_color(RED);
                cout << "Escolha inválida. Tente novamente.\n";
                set_color(RESET);
            }
        } while (!is_valid_choice(player2_choice));
        clear_screen();
        //Mostra o top do jogo novamente
       
       // Exibe o topo do jogo
    set_color(DARK_BLUE);
    cout << "##########################################\n";
    set_color(RED);
    cout << "       Pedra Papel Tesoura\n";
    set_color(DARK_BLUE);
    cout << "##########################################\n";
    set_color(RESET);
    set_color(BOLD);
    cout << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n";
    set_color(RESET);


    // Determina o resultado e exibe a rodada
    string winner = determine_winner(player1_choice, player2_choice);

    set_color(DARK_BLUE);
    cout << "\nResultado:\n";
    set_color(RESET);

    set_color(RED);
    cout << "Jogador 1 escolheu: ";
    set_color(RESET); 
    cout << player1_choice << "\n";

    set_color(RED);
    cout << "Jogador 2 escolheu: ";
    set_color(RESET); 
    cout << player2_choice << "\n";

    set_color(DARK_BLUE);
    cout << winner << endl;
    set_color(RESET);

        // Libera o fluxo de volta para o jogador 1
        ReleaseSemaphore(choices_done, 1, NULL);
    }
}

void game_loop() {
    while (true) {
        set_color(DARK_BLUE);
        cout << "##########################################\n";
        set_color(RED);
        cout << "       Pedra Papel Tesoura\n";
        set_color(DARK_BLUE);
        cout << "##########################################\n";
        set_color(RESET);
        set_color(BOLD);
        cout << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n";
        set_color(RESET);


        // Inicializa os semáforos para a rodada
        ReleaseSemaphore(player1_ready, 1, NULL); // Libera jogador 1 para comecar
        WaitForSingleObject(choices_done, INFINITE); // Aguarda a rodada finalizar

        
        cout << "\nPressione Enter para comecar outra rodada.\n";

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
    thread t1(player1);
    thread t2(player2);

    // Inicia o loop do jogo
    game_loop();

    // Aguarda as threads finalizarem
    t1.join();
    t2.join();

    // Fecha os semáforos
    CloseHandle(player1_ready);
    CloseHandle(player2_ready);
    CloseHandle(choices_done);

    set_color(DARK_BLUE);
    cout << "\nJogo encerrado. Obrigado por jogar!\n";
    set_color(RESET);
    return 0;
}
