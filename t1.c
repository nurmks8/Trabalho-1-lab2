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

double crono_parcial(crono *c)
{
  crono agora;
  clock_gettime(CLOCK_MONOTONIC, &agora);

  double segundos = agora.tv_sec - c->tv_sec;
  double nanosegundos = agora.tv_nsec - c->tv_nsec;

  return segundos + 1e-9 * nanosegundos;
}

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

void processa_escudos(estado_t *est)
{
  for (int i = 0; i <= 2; i++) {
    if (est->inimigos[i] != ' ' && est->escudos[i]) {
      est->escudos[i] = false;
      est->inimigos[i] = ' ';
    }
  }
}

void verifica_base(estado_t *est)
{
  if (est->inimigos[0] != ' ') {
    est->terminou = true;
  }
}

void move_inimigos(estado_t *est)
{
  if (est->dia) {
    for (int i = 0; i < 12; i++) {
      est->inimigos[i] = est->inimigos[i + 1];
    }

    est->inimigos[12] = ' ';

    if (est->inimigos_inativos > 0) {
      est->inimigos[12] = sorteia_inimigo(est);
      est->inimigos_inativos--;
    }
  } else {
    for (int i = 0; i < 7; i++) {
      est->inimigos[i] = est->inimigos[i + 1];
    }

    est->inimigos[7] = ' ';

    if (est->inimigos_inativos > 0) {
      est->inimigos[7] = sorteia_inimigo(est);
      est->inimigos_inativos--;
    }
  }
}

void processa_tempo(estado_t *est)
{
  if (est->dia &&
      crono_parcial(&est->tempo) >= est->intervalo) {
    verifica_base(est);
    move_inimigos(est);
    processa_escudos(est);
    clock_gettime(CLOCK_MONOTONIC, &est->tempo);
  } else if (!est->dia &&
             crono_parcial(&est->tempo) >= est->intervalo * 3) {
    verifica_base(est);
    move_inimigos(est);
    processa_escudos(est);
    clock_gettime(CLOCK_MONOTONIC, &est->tempo);
  }
}

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
}
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

void acerta_inimigo(estado_t *est)
{
  for (int i = 0; i < 13; i++) {
    if (est->inimigos[i] == est->arma) {
      est->pontos = est->pontos + calcula_pontos(est, i);
      est->inimigos[i] = ' ';
      break;
    } else if (est->inimigos[i] == 'N' &&
               est->arma == 'n') {
      est->inimigos[i] = 'n';
      break;
    }
  }
}

void pontos_fim_onda(estado_t *est)
{
  est->pontos += est->tiros * 2;
  for (int i = 0; i < 3; i++) {
    if (est->escudos[i]) {
      est->pontos += 10;
    }
  }
}
void atira(estado_t *est)
{
  if (est->tiros > 0) {
    est->tiros--;
    acerta_inimigo(est);
  }
}

char lechar()
{
  fflush(stdout);
  char c;

  if (fread(&c, 1, 1, stdin) == 1) {
    return c;
  }

  return 0;
}

void sonar(estado_t *est)
{
  
}

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

void apresenta(estado_t *est)
{
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

void joga_onda(estado_t *est)
{
  while (!onda_terminou(est) && !est->terminou) {
    processa_teclado(est);
    processa_tempo(est);
    apresenta(est);
  }
}

void joga_partida(estado_t *est)
{
  while (!est->terminou) {
    define_dia(est);
    inicializa_onda(est);
    joga_onda(est);
    if (!est->terminou) {
      pontos_fim_onda(est);
      resumo_do_jogo(est);
      est->onda++;
    }
  }
}

void resumo_do_jogo(estado_t *est)
{
  printf("\nFim da onda %d\n", est->onda);
  printf("Pontos: %d\n", est->pontos);
  printf("Tiros restantes: %d\n", est->tiros);
  printf("Pressione r para continuar\n");
  char tecla = 0;

  while (tecla != 'r') {
    tecla = lechar();
  }
}


void inicializa_tela()
{
  system("stty raw -echo min 0 time 1 opost");
  setvbuf(stdin, NULL, _IONBF, 0);
}

void desinicializa_tela()
{
  system("stty sane");
}

int main()
{
  estado_t estado;

  inicializa_tela();
  inicializa_estado(&estado);
  srand(time(NULL));

  while (!estado.terminou) {
    joga_partida(&estado);
  }

  desinicializa_tela();
}