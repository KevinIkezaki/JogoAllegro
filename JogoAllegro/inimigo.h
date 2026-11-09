#ifndef INIMIGO_H
#define INIMIGO_H

#include <stdbool.h>

typedef struct {
    float x, y;
    float vx, vy;
    float raio;
    bool ativo;
} Projetil;

typedef struct {
    float x, y;
    float tamanho;
    float vel;
    int vida;
    int timer_tiro;
} Inimigo;

typedef struct {
    float x, y;
    float tamanho;
    float vel;
    int vida;
    int invulnerabilidade;
    float direcao_x;
    float direcao_y;
    int tempo_recarga;
} Jogador;

void atualizar_inimigo(Inimigo* inimigo, Jogador* jogador, Projetil* tiro_inimigo);
void desenhar_inimigo(Inimigo* inimigo);

void atualizar_tiro(Projetil* tiro, float largura_tela, float altura_tela);
void desenhar_tiro(Projetil* tiro, unsigned char r, unsigned char g, unsigned char b);

bool verificar_colisao(Projetil* tiro, float alvo_x, float alvo_y, float alvo_tamanho);

#endif