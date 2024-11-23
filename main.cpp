#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <string>
#include <thread>

using namespace std;

// Semáforos que seraõ usados na sincronização
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

// Determina o vencedor com base nas regras dojogo
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

// Limpa a tela (Linux)
void clear_screen() { (void)system("clear"); }

// Vez do jogador 1 jogar
void player1() {
  while (true) {
    sem_wait(&player1_ready); // Espera sua vez

    do {
      cout << "Jogador 1: Faça sua escolha (Pedra, Papel, Tesoura ou Sair para "
              "encerrar o jogo): ";
      cin >> player1_choice;

      if (player1_choice == "Sair") {
        sem_post(&player2_ready); // Libera o jogador 2 para finalizar
        return;
      }

      if (!is_valid_choice(player1_choice)) {
        cout << "Escolha inválida. Tente novamente.\n";
      }
    } while (!is_valid_choice(player1_choice));

    // Limpa a tela após a escolha, para que o jogador 2 não veja o que foi
    // escolhido
    clear_screen();

    // Libera o jogador 2 para jogar
    sem_post(&player2_ready);
  }
}

// Vez do jogador 2 jogar
void player2() {
  while (true) {
    sem_wait(&player2_ready); // Espera sua vez

    do {
      cout << "Jogador 2: Faça sua escolha (Pedra, Papel, Tesoura ou Sair para "
              "encerrar o jogo): ";
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
    sem_post(&choices_done);
  }
}

void game_loop() {
  while (true) {
    cout << "Jogo de Pedra, Papel e Tesoura\n";
    cout << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n";

    // Inicializa os semáforos para a rodada
    sem_post(&player1_ready); // Libera jogador 1 para começar
    sem_wait(&choices_done);  // Aguarda a rodada finalizar

    cout << "\nPressione Enter para começar outra rodada.\n";
    cin.ignore();
    cin.get(); // Aguarda o Enter do jogador para começar tudo novamente

    // Limpa a tela para reiniciar o jogo
    clear_screen();
  }
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
