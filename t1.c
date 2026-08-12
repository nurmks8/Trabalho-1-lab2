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
   est->inimigos_inativos = 0;
   est->tiros = 30;
   est->onda = 1;
   est->arma = ' ';
   est->dia = true;
   est->terminou = false;
   for (int i = 0; i < 10; i++) {
      est->inimigos[i] = ' ';
   }
   for (int i = 0; i < 3; i++) {
      est->escudos[i] = false;
   }
}
  
void joga_onda(estado_t *est)
{
  bool onda_acabou;
   onda_acabou = true;

   for (int i = 0; i <10; i++) {
      if (est->inimigos [i] != ' ') {
        onda_acabou = false;
    processa_teclado(est);
    processa_tempo(est);
    apresenta(est);
}

void joga_partida(estado_t *est)
{
  while (!est->terminou) {
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