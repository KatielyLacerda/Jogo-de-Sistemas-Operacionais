#include <cstdlib> 
#include <iostream>
#include <semaphore.h>
#include <string>
#include <thread>

using namespace std;

//Semáforos que serão usados para sincronização
sem_t player1_ready;
sem_t player2_ready;
sem_t choices_done; //Semáforo para garantir que ambos os jogadores escolheram

//Escolhas dos jogadores
string player1_choice;
string player2_choice;

//Função que valida as escolhas para garantir que nada que não seja o esperado passe
bool is_valid_choice(const string &choice) {
  return (choice == "Pedra" || choice == "Papel" || choice == "Tesoura");
}

//Regras para achar o vencedor
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

//Função para limpar a tela (Linux)
void clear_screen() {
  (void)system("clear"); // Ignora o valor de retorno de system
}

//Vez do jogador 1 jogar
void player1() {
  while (true) {
    sem_wait(&player1_ready); // Espera sua vez

    do {
      cout << "Jogador 1: Faça sua escolha (Pedra, Papel, Tesoura ou Sair para "
              "encerrar o jogo): ";
      cin >> player1_choice;

      if (player1_choice == "Sair") {
        sem_post(&player2_ready); // Libera o jogador 2 para finalizar também
        return;
      }

      if (!is_valid_choice(player1_choice)) {
        cout << "Escolha inválida. Tente novamente.\n";
      }
    } while (!is_valid_choice(player1_choice));

    //Após a escolha, libere o jogador 2 para jogar
    sem_post(&player2_ready);

    //Espera o jogador 2 fazer sua escolha
    sem_wait(&choices_done);
  }
}

//Vez do jogador 2 jogar
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

    // Agora que ambos os jogadores fizeram a escolha, mostra o resultado
    string winner = determine_winner(player1_choice, player2_choice);
    cout << "\nResultado:\n"
         << "Jogador 1 escolheu: " << player1_choice << "\n"
         << "Jogador 2 escolheu: " << player2_choice << "\n"
         << winner << endl;

    cout << "\nPressione Enter para começar outra rodada.\n";
    cin.ignore(); 
    cin.get();    // Aguarda o Enter do jogador

    // Limpa a tela para que as escolhas não apareçam na próxima rodada
    clear_screen();

    // Libera o jogador 1 para a próxima rodada
    sem_post(&choices_done);
  }
}

int main() {
  cout << "Jogo de Pedra, Papel e Tesoura\n";
  cout << "Digite 'Sair' a qualquer momento para encerrar o jogo.\n\n";

  // Inicializa os semáforos
  sem_init(&player1_ready, 0, 1); // Jogador 1 começa
  sem_init(&player2_ready, 0, 0); // Jogador 2 espera
  sem_init(&choices_done, 0, 0); // Semáforo para garantir que ambos escolheram

  // Cria as threads dos jogadores
  thread t1(player1);
  thread t2(player2);

  // Aguarda as threads (serão finalizadas quando um jogador escolher "Sair")
  t1.join();
  t2.join();

  // Destrói os semáforos
  sem_destroy(&player1_ready);
  sem_destroy(&player2_ready);
  sem_destroy(&choices_done);

  cout << "\nJogo encerrado. Obrigado por jogar!\n";
  return 0;
}