#define _POSIX_C_SOURCE 199309L
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  bool terminou;
  bool dia;
  int pontos;
  int inimigos_inativos;
  int tiros;
  int onda;
  char arma;
  char inimigos [10];
  bool escudos [3];

} estado_t;

void inicializa_estado(estado_t *est)
{
   est->pontos = 0;
   est->tiros = 30; 
   est->onda = 1;
   est->arma = '0';
   est->dia = true;
   est->terminou = false;
   for (int i = 0; i < 10; i++) {
      est->inimigos[i] = ' ';
   }
   for (int i = 0; i < 3; i++) {
   est->escudos[i] = true;
   }
}

void joga_onda(estado_t *est)
{
    while (!onda_terminou(est)) {
    processa_teclado(est);
    processa_tempo(est);
    apresenta(est);
    }
}

void processa_tempo(estado_t *est)
{
  for (int i = 0; i < 9; i++) {
    est->inimigos[i] = est->inimigos[i + 1];
  }
  est->inimigos[9] = ' ';
  if (est->inimigos_inativos > 0) {
    est->inimigos[9] = sorteia_inimigo(est);
    est->inimigos_inativos--;
  }
  if (est->inimigos[0] != ' ' && est->escudos[2]) {
    est->escudos[2] = false;
    est->inimigos[0] = ' ';
  }
  for (int i = 2; i >= 0; i--)
  if (!est->escudos[i]) {
}

char sorteia_inimigo(estado_t *est)
{
 if (est->dia) {
  int sorteia = rand () % 11;
  if (sorteia < 10) {
    return '0' + sorteia;
  } else {
    return 'N';
  }
 }
 else {
  int sorteia = rand () % 6;
  if (sorteia < 5) {
    return '0' + sorteia * 2;
  } else {
    return 'N';
  }
 }
}

  void inicializa_onda(estado_t *est)
  {
    if (est->dia){
      est->inimigos_inativos = 20;
    } else {
      est->inimigos_inativos = 15;
    }
    est->tiros = 30;
  }
bool onda_terminou(estado_t *est)
{
  if (est->inimigos_inativos > 0) {
    return false;
  }
    for (int i = 0; i < 10; i++) { 
      if (est->inimigos[i] != ' ') {
        return false;
      }
    }
    return true;
}
  
void joga_partida(estado_t *est)
{
  while (!est->terminou) {
    inicializa_onda(est);
    joga_onda(est);
  }
}

int main()
{
  estado_t estado;
  inicializa_tela();
  inicializa_estado(&estado);
  while (!estado.terminou) {
    joga_partida(&estado);
  }
  desinicializa_tela();
}