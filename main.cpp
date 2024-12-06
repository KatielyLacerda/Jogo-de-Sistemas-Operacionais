#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <string>
#include <thread>
volatile bool game_over = false; //Definimos uma variável global que vai indicar se o jogo acabou, o volatile
//garante que a mudança num there afete a outra.
#define RESET "\033[0m"
#define BOLD "\033[1m"         // Preto (ANSI)
#define GREEN "\033[32m"       // Verde (ANSI)
#define RED "\033[31m"         // Vermelho (ANSI)
#define DARK_YELLOW "\033[33m" // Amarelho escuro (ANSI)
#define DARK_BLUE "\033[34m"   // Azul (ANSI)

using namespace std;

// Semáforos que serão usados na sincronização
sem_t player1_ready;
sem_t player2_ready;
sem_t choices_done; // semafaro que ambos os jogadores escolheram

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
    return DARK_BLUE "##########################################\n"
                     "           Empate!           \n"
                     "##########################################" RESET;
  } else if ((p1 == "Pedra" && p2 == "Tesoura") ||
             (p1 == "Tesoura" && p2 == "Papel") ||
             (p1 == "Papel" && p2 == "Pedra")) {
    return BOLD GREEN "##########################################\n"
                      "        Jogador 1 venceu! :)        \n"
                      "##########################################" RESET;
  } else {
    return DARK_BLUE "##########################################\n"
                     "        Jogador 2 venceu! :)        \n"
                     "##########################################" RESET;
  }
}

// Limpa a tela (Linux)
void clear_screen() { (void)system("clear"); }

// Vez do jogador 1 jogar
void player1() {
  while (true) {
    sem_wait(&player1_ready); // Espera sua talvez
    if (game_over)
      break; // Sai se o jogo acabou

    do {
      cout
          << DARK_BLUE
          << "Jogador 1: Faça sua escolha (Pedra - Papel - Tesoura ou 'Sair'): "
          << RESET << endl;
      cin >> player1_choice;

      if (player1_choice == "Sair") {
        cout << RED << "Jogador 1 escolheu encerrar o jogo." << RESET << endl;
        game_over = true;         // Marca o fim do jogo
        sem_post(&player2_ready); // Libera o jogador 2 para encerrar
        sem_post(&choices_done);  // Libera o fluxo para encerrar o jogo
        return;
      }

      if (!is_valid_choice(player1_choice)) {
        cout << RED << "Escolha inválida. Tente novamente.\n" << RESET;
      }
    } while (!is_valid_choice(player1_choice));

    cout << GREEN << "Jogador 1 escolheu: " << RESET << player1_choice << endl;
    clear_screen();
    sem_post(&player2_ready); // Libera o jogador 2 para jogar
  }
}

void player2() {
  while (true) {
    sem_wait(&player2_ready); // Espera sua vez
    if (game_over)
      break; // Sai se o jogo acabou

    // Exibe o banner do jogo
    cout << BOLD << DARK_BLUE << "##########################################"
         << RESET << endl;
    cout << RED << "       Pedra Papel Tesoura" << RESET << endl;
    cout << BOLD << DARK_BLUE << "##########################################"
         << RESET << endl;
    cout << BOLD << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n"
         << RESET << endl;
    cout << DARK_BLUE
         << "Jogador 2: Faça sua escolha (Pedra - Papel - Tesoura ou 'Sair'): "
         << RESET << endl;

    do {
      cin >> player2_choice;

      if (player2_choice == "Sair") {
        cout << RED << "Jogador 2 escolheu encerrar o jogo." << RESET << endl;
        game_over = true;        // Marca o fim do jogo
        sem_post(&choices_done); // Libera o fluxo para encerrar
        return;
      }

      if (!is_valid_choice(player2_choice)) {
        cout << RED << "Escolha inválida. Tente novamente.\n" << RESET;
      }
    } while (!is_valid_choice(player2_choice));

    cout << GREEN << "Jogador 2 escolheu: " << RESET << player2_choice << endl;
    clear_screen();
    // Exibir resultado
    cout << BOLD << DARK_BLUE << "##########################################"
         << RESET << endl;
    cout << RED << "       Pedra Papel Tesoura" << RESET << endl;
    cout << BOLD << DARK_BLUE << "##########################################"
         << RESET << endl;
    cout << BOLD << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n"
         << RESET << endl;
    // Determina e exibe o resultado da rodada
    string winner = determine_winner(player1_choice, player2_choice);
    cout << DARK_BLUE << "\nResultado:\n"
         << RESET << RED << "Jogador 1 escolheu: " << RESET << player1_choice
         << "\n"
         << RED << "Jogador 2 escolheu: " << RESET << player2_choice << "\n"
         << winner << endl;

    sem_post(&choices_done); // Libera o jogador 1 para nova rodada
  }
}

void game_loop() {
  while (!game_over) {
    cout << BOLD << DARK_BLUE << "##########################################"
         << RESET << endl;
    cout << RED << "       Pedra Papel Tesoura" << RESET << endl;
    cout << BOLD << DARK_BLUE << "##########################################"
         << RESET << endl;
    cout << BOLD << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n"
         << RESET << endl;

    sem_post(&player1_ready); // Libera jogador 1 para começar
    sem_wait(&choices_done);  // Aguarda a rodada finalizar

    if (!game_over) {
      cout << "\nPressione Enter para começar outra rodada ou digite 'Sair' "
              "para encerrar o jogo.\n";
      cin.ignore();
      string input;
      getline(cin, input); // Lê a entrada do jogador

      if (input == "Sair") { // Verifica se o usuário quer acabar com o jogo
        game_over = true;
        break;
      }

      clear_screen(); // Limpa a tela para iniciar outra rodada
    }
  }

  // Libere todas as threads bloqueadas
  sem_post(&player1_ready);
  sem_post(&player2_ready);
  sem_post(&choices_done);
}

int main() {
  // Inicializa os semáforos
  sem_init(&player1_ready, 0, 0); // Jogador 1 espera inicialmente
  sem_init(&player2_ready, 0, 0); // Jogador 2 espera
  sem_init(&choices_done, 0, 0);  // Garantir que ambos escolheram

  // Cria as threads dos jogadores
  thread t1(player1);
  thread t2(player2);

  // Inicia o loop do jogo
  game_loop();

  // Aguarda as threads (finalizam quando um jogador escolhe "Sair")
  t1.join();
  t2.join();

  // Destrói os semáforos
  sem_destroy(&player1_ready);
  sem_destroy(&player2_ready);
  sem_destroy(&choices_done);

  cout << "\nJogo encerrado. Obrigado por jogar!\n";
  return 0;
}
