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
} Ameba;

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

void atualizar_ameba(Ameba* ameba, Jogador* jogador);
void desenhar_ameba(Ameba* ameba);
void desenhar_barra_vida_ameba(Ameba* ameba);

void atualizar_tiro(Projetil* tiro, float largura_tela, float altura_tela);
void desenhar_tiro(Projetil* tiro, unsigned char r, unsigned char g, unsigned char b);

bool verificar_colisao(Projetil* tiro, float alvo_x, float alvo_y, float alvo_tamanho);

void separar_todos_inimigos(
    Inimigo inimigos[], int qtd_inimigos,
    Ameba amebas[], int qtd_amebas,
    float largura_tela, float altura_tela, float espessura_parede
);

#endif