#define _POSIX_C_SOURCE 199309L

#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct timespec crono;
typedef struct {
  bool terminou;
  bool dia;
  int pontos;
  int inimigos_inativos;
  int tiros;
  int onda;
  char arma;
  char inimigos[13];
  bool escudos[3];
  crono tempo;
  double intervalo;
} estado_t;

// inicializa os dados da nova partida
void inicializa_estado(estado_t *est)
{
  est->pontos = 0;
  est->tiros = 30;
  est->onda = 1;
  est->arma = '0';
  est->dia = true;
  est->terminou = false;
  for (int i = 0; i < 13; i++) {
    est->inimigos[i] = ' ';
  }
  for (int i = 0; i < 3; i++) {
    est->escudos[i] = true;
  }
}

// Retorna o tempo transcorrido desde o inicio do cronometro
double crono_parcial(crono *c)
{
  crono agora;
  clock_gettime(CLOCK_MONOTONIC, &agora);
  double segundos = agora.tv_sec - c->tv_sec;
  double nanosegundos = agora.tv_nsec - c->tv_nsec;
  return segundos + 1e-9 * nanosegundos;
}

// Processa colisoes entre inimigos e escudos
void processa_escudos(estado_t *est)
{
  for (int i = 0; i <= 2; i++) {
    if (est->inimigos[i] != ' ' && est->escudos[i]) {
      est->escudos[i] = false;
      est->inimigos[i] = ' ';
    }
  }
}

// Verifica se um inimigo alcancou a base
void verifica_base(estado_t *est)
{
  if (est->inimigos[0] != ' ') {
    est->terminou = true;
  }
}

// Sorteia o tipo de inimigo conforme a onda diurna ou noturna
char sorteia_inimigo(estado_t *est)
{
  if (est->dia) {
    int sorteia = rand() % 11;
    if (sorteia < 10) {
      return '0' + sorteia;
    } else {
      return 'N';
    }
  } else {
    int sorteia = rand() % 6;
    if (sorteia < 5) {
      return '0' + sorteia * 2;
    } else {
      return 'N';
    }
  }
}

// Toca o som correspondente ao tipo de inimigo
void toca_som_inimigo(char inimigo)
{
  char comando[40];
  sprintf(comando, "aplay -q Sons/%c.3.wav", inimigo);
  system(comando);
}

// Ativa um novo inimigo e toca o som correspondente
void ativa_inimigo(estado_t *est, int posicao)
{
  if (est->inimigos_inativos > 0) {
    est->inimigos[posicao] = sorteia_inimigo(est);
    est->inimigos_inativos--;
    if (est->inimigos[posicao] == 'N') {
      system("aplay -q Sons/11.3.wav &");
    } else {
      char comando[50];
      sprintf(comando, "aplay -q Sons/%c.3.wav &",
          est->inimigos[posicao]);
      system(comando);
}
  }
}

// Move os inimigos uma posicao para a esquerda
void move_inimigos(estado_t *est)
{
  if (est->dia) {
    for (int i = 0; i < 12; i++) {
      est->inimigos[i] = est->inimigos[i + 1];
    }
    est->inimigos[12] = ' ';
    ativa_inimigo(est, 12);
  } else {
    for (int i = 0; i < 7; i++) {
      est->inimigos[i] = est->inimigos[i + 1];
    }
    est->inimigos[7] = ' ';
    ativa_inimigo(est, 7);
  }
}

// Processa os eventos que ocorrem com a passagem do tempo.
void processa_tempo(estado_t *est)
{
  if (est->dia &&
      crono_parcial(&est->tempo) >= est->intervalo) {
    verifica_base(est);
    if (est->terminou) {
      return;
    }
    move_inimigos(est);
    processa_escudos(est);
    clock_gettime(CLOCK_MONOTONIC, &est->tempo);
  } else if (!est->dia &&
             crono_parcial(&est->tempo) >= est->intervalo * 3) {
    verifica_base(est);
    if (est->terminou) {
      return;
    }
    move_inimigos(est);
    processa_escudos(est);
    clock_gettime(CLOCK_MONOTONIC, &est->tempo);
  }
}

// Calcula os pontos obtidos ao destruir um inimigo
int calcula_pontos(estado_t *est, int posicao)
{
  int pontos;
  if (est->dia) {
    pontos = 13 - posicao;
  } else {
    pontos = (8 - posicao) * 2;
  }
  if (est->inimigos[posicao] == 'n') {
    pontos = pontos * 2;
  }
  return pontos;
}

// Verifica se o tiro acertou um inimigo e atualiza o estado
bool acerta_inimigo(estado_t *est)
{
  for (int i = 0; i < 13; i++) {
    if (est->inimigos[i] == est->arma) {
      est->pontos += calcula_pontos(est, i);
      est->inimigos[i] = ' ';
      return true;
    }
  }
  if (est->arma == 'n') {
    for (int i = 0; i < 13; i++) {
      if (est->inimigos[i] == 'N') {
        est->inimigos[i] = 'n';
        return true;
      }
    }
  }

  return false;
}

// Calcula e adiciona os pontos de bonus ao final da onda
void pontos_fim_onda(estado_t *est)
{
  int bonus = est->tiros * 2;
  for (int i = 0; i < 3; i++) {
    if (est->escudos[i]) {
      bonus += 10;
    }
  }
  if (!est->dia) {
    bonus *= 2;
  }
  est->pontos += bonus;
}

// Toca o som correspondente a arma selecionada
void toca_som_arma(char arma)
{
  char comando[40];
  if (arma == 'n') {
    system("aplay -q Sons/11.3.wav &");
  } else {
    sprintf(comando, "aplay -q Sons/%c.3.wav &", arma);
    system(comando);
  }
}

// Altera a arma selecionada para a proxima arma disponivel
void troca_arma(estado_t *est)
{
  if (est->dia) {
    if (est->arma >= '0' && est->arma <= '8') {
      est->arma++;
    } else if (est->arma == '9') {
      est->arma = 'n';
    } else if (est->arma == 'n') {
      est->arma = '0';
    }
  } else {
    if (est->arma >= '0' && est->arma <= '6') {
      est->arma += 2;
    } else if (est->arma == '8') {
      est->arma = 'n';
    } else if (est->arma == 'n') {
      est->arma = '0';
    }
  }
  toca_som_arma(est->arma);
}

// Realiza um tiro e toca o som de acerto ou erro
void atira(estado_t *est)
{
  if (est->tiros > 0) {
    est->tiros--;
    if (acerta_inimigo(est)) {
      toca_som_arma(est->arma);
    } else {
      system("aplay -q Sons/x.3.wav &");
    }
  }
}

// Le uma tecla do teclado sem bloquear a execucao
char lechar()
{
  fflush(stdout);
  char c;
  if (fread(&c, 1, 1, stdin) == 1) {
    return c;
  }
  return 0;
}

// Toca os sons das posicoes ocupadas, vazias e dos escudos
void sonar(estado_t *est)
{
  int limite;
  if (est->dia) {
    limite = 13;
  } else {
    limite = 8;
  }
  for (int i = 0; i < limite; i++) {
    if (i < 3 && est->escudos[i]) {
      system("aplay -q Sons/12.3.wav");
    } else if (est->inimigos[i] != ' ') {
      if (est->inimigos[i] == 'N' ||
          est->inimigos[i] == 'n') {
        system("aplay -q Sons/11.3.wav");
      } else {
        toca_som_inimigo(est->inimigos[i]);
      }
    } else {
      system("aplay -q Sons/x.3.wav");
    }
  }
}

// Processa os comandos digitados pelo jogador
void processa_teclado(estado_t *est)
{
  char tecla = lechar();
  if (tecla == 27) {
    est->terminou = true;
  } else if (tecla == '\t') {
    troca_arma(est);
  } else if (tecla == '\r') {
    atira(est);
  } else if (tecla == ' ') {
    sonar(est);
  }
}

// Verifica se todos os inimigos da onda foram eliminados
bool onda_terminou(estado_t *est)
{
  if (est->inimigos_inativos > 0) {
    return false;
  }
  for (int i = 0; i < 13; i++) {
    if (est->inimigos[i] != ' ') {
      return false;
    }
  }
  return true;
}

// Inicializa os dados necessarios para uma nova onda
void inicializa_onda(estado_t *est)
{
  if (est->onda == 1) {
    est->intervalo = 2.0;
  } else {
    est->intervalo = est->intervalo * 0.9;
  }
  clock_gettime(CLOCK_MONOTONIC, &est->tempo);
  for (int i = 0; i < 13; i++) {
    est->inimigos[i] = ' ';
  }
  if (est->dia) {
    est->inimigos_inativos = 20;
  } else {
    est->inimigos_inativos = 15;
  }
  est->tiros = 30;
}

// Define se a onda atual sera diurna ou noturna
void define_dia(estado_t *est)
{
  int chance;
  if (est->onda == 1) {
    chance = 100;
  } else if (est->onda == 2) {
    chance = 80;
  } else if (est->onda == 3) {
    chance = 60;
  } else if (est->onda == 4) {
    chance = 40;
  } else {
    chance = 20;
  }
  if (rand() % 100 < chance) {
    est->dia = true;
  } else {
    est->dia = false;
  }
}

// Apresenta na tela o estado atual do jogo
void apresenta(estado_t *est)
{
  printf("\r%30s\r", "");
  if (!est->dia) {
    printf("%d\r", est->pontos);
    return;
  }
  printf("%d %d %c", est->pontos, est->tiros, est->arma);
  for (int i = 0; i < 3; i++) {
    if (est->escudos[i]) {
      printf(")");
    } else {
      printf("%c", est->inimigos[i]);
    }
  }
  for (int i = 3; i < 13; i++) {
    printf("%c", est->inimigos[i]);
  }
  printf("\r");
}

// Executa uma onda ate que ela termine ou a partida seja encerrada
void joga_onda(estado_t *est)
{
  while (!onda_terminou(est) && !est->terminou) {
    processa_teclado(est);
    if (est->terminou) {
      return;
    }
    processa_tempo(est);
    apresenta(est);
  }
}

// Toca o som que identifica o final de uma onda
void toca_som_fim_onda()
{
  system("aplay -q Sons/12.3.wav Sons/x.3.wav");
}

// Toca o som que identifica o final da partida
void toca_som_fim_partida()
{
  system("aplay -q Sons/11.3.wav Sons/x.3.wav "
         "Sons/11.3.wav");
}

// Le do arquivo as tres maiores pontuacoes salvas
void le_recordes(int recordes[3])
{
  recordes[0] = 0;
  recordes[1] = 0;
  recordes[2] = 0;
  FILE *arquivo;
  arquivo = fopen("recordes.txt", "r");
  if (arquivo == NULL) {
    return;
  }
  for (int i = 0; i < 3; i++) {
    if (fscanf(arquivo, "%d", &recordes[i]) != 1) {
      break;
    }
  }
  fclose(arquivo);
}

// Atualiza a lista das tres maiores pontuacoes
bool atualiza_recordes(int recordes[3], int pontos)
{
  if (pontos >= recordes[0]) {
    recordes[2] = recordes[1];
    recordes[1] = recordes[0];
    recordes[0] = pontos;
    return true;
  } else if (pontos >= recordes[1]) {
    recordes[2] = recordes[1];
    recordes[1] = pontos;
    return true;
  } else if (pontos >= recordes[2]) {
    recordes[2] = pontos;
    return true;
  }

  return false;
}

// Salva no arquivo as tres maiores pontuacoes
void salva_recordes(int recordes[3])
{
  FILE *arquivo;
  arquivo = fopen("recordes.txt", "w");
  if (arquivo == NULL) {
    return;
  }
  for (int i = 0; i < 3; i++) {
    fprintf(arquivo, "%d\n", recordes[i]);
  }
  fclose(arquivo);
}

// Mostra a pontuacao final e informa se entrou nos recordes
void resumo_fim_partida(estado_t *est)
{
  int recordes[3];
  le_recordes(recordes);
  bool entrou = atualiza_recordes(recordes, est->pontos);
  if (entrou) {
    salva_recordes(recordes);
  }
  toca_som_fim_partida();
  printf("\nFim da partida!\n");
  printf("Pontuacao: %d\n", est->pontos);
  if (entrou) {
    printf("Voce entrou nos 3 maiores recordes!\n");
  } else {
    printf("Voce nao entrou nos 3 maiores recordes.\n");
  }
}

// Mostra o resumo da onda e aguarda o jogador continuar
void resumo_do_jogo(estado_t *est)
{
  char tecla = 0;

  toca_som_fim_onda();
  printf("\nFim da onda %d\n", est->onda);
  printf("Pontos: %d\n", est->pontos);
  printf("Tiros restantes: %d\n", est->tiros);
  printf("Pressione r para continuar\n");

  while (tecla != 'r') {
    tecla = lechar();
  }
}

// Ajusta a arma caso a onda noturna use uma arma indisponivel
void ajusta_arma_noturna(estado_t *est)
{
  if (!est->dia &&
      est->arma != 'n' &&
      (est->arma - '0') % 2 != 0) {
    est->arma = '0';
  }
}

// Executa as ondas da partida ate que o jogo termine
void joga_partida(estado_t *est)
{
  while (!est->terminou) {
    define_dia(est);
    ajusta_arma_noturna(est);
    inicializa_onda(est);
    joga_onda(est);
    if (!est->terminou) {
      pontos_fim_onda(est);
      resumo_do_jogo(est);
      est->onda++;
    }
  }
}

// Configura o terminal para leitura direta das teclas
void inicializa_tela()
{
  system("stty raw -echo min 0 time 1 opost");
  setvbuf(stdin, NULL, _IONBF, 0);
}

// Restaura as configuracoes normais do terminal
void desinicializa_tela()
{
  system("stty sane");
}

// Pergunta se o jogador deseja iniciar uma nova partida
bool quer_jogar_novamente()
{
  char tecla;

  printf("\nQuer jogar novamente? (s/n) ");

  while (true) {
    tecla = lechar();

    if (tecla == 's' || tecla == 'S') {
      printf("\n");
      return true;
    }

    if (tecla == 'n' || tecla == 'N') {
      printf("\n");
      return false;
    }
  }
}

// Inicializa o programa e controla a execucao das partidas.
int main()
{
  estado_t estado;
  bool novamente = true;
  inicializa_tela();
  srand(time(NULL));
  while (novamente) {
    inicializa_estado(&estado);
    joga_partida(&estado);
    resumo_fim_partida(&estado);
    novamente = quer_jogar_novamente();
  }
  desinicializa_tela();
  return 0;
}