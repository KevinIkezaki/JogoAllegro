#ifndef BOSS_H
#define BOSS_H

#include <stdbool.h>
#include <allegro5/allegro_font.h>
#include "inimigo.h"

/* =========================================================
   CONFIGURACAO DO BOSS (a Fase 2 sempre ativa em 50% da vida)
   ========================================================= */
#define VIDA_MAX_BOSS              2500
#define BOSS_LARGURA               65.0f

#define MAX_TIROS_BOSS             160
#define MAX_LASERS_BOSS            2

   /* Tempos em frames (baseado em 60 FPS) */
#define BOSS_CADENCIA_NORMAL_F1    40     /* Fase 1 */
#define BOSS_CADENCIA_NORMAL_F2    20        /* Fase 2 */
#define BOSS_TEMPO_ESPECIAL_F1     (3.5 * 60)  /* Fase 1: 4 segundos entre especiais */
#define BOSS_TEMPO_ESPECIAL_F2     (2 * 60)  /* Fase 2: 3 segundos entre especiais */

/* Parametros dos ataques */
#define BOSS_RAJADA_QTD            15
#define BOSS_RAJADA_INTERVAL       8
#define BOSS_TEMPO_EXPLOSAO_GRANDE 48        /* ~0.8s andando antes de explodir */
#define BOSS_MINI_BOLINHAS_F1      8
#define BOSS_MINI_BOLINHAS_F2      15

#define BOSS_LASER_AVISO           65        /* ~1.08s de linha de aviso */
#define BOSS_LASER_DURACAO         55        /* ~0.92s de feixe ativo */

typedef enum {
    BOSS_ATAQUE_NENHUM = -1,
    /* Fase 1 (0 e 1) */
    BOSS_ATAQUE_RAJADA = 0,
    BOSS_ATAQUE_TIRO_GRANDE = 1,
    /* Desbloqueado na Fase 2 (2) */
    BOSS_ATAQUE_LASER = 2
} TipoAtaqueBoss;

#define TOTAL_ATAQUES_FASE_1 2
#define TOTAL_ATAQUES_FASE_2 3

typedef enum {
    TIRO_BOSS_NORMAL = 0,
    TIRO_BOSS_GRANDE,
    TIRO_BOSS_MINI
} TipoTiroBoss;

typedef struct {
    float x, y;
    float vx, vy;
    float raio;
    int dano;
    int tempo_vida; /* Usado na esfera explosiva */
    TipoTiroBoss tipo;
    bool ativo;
} ProjetilBoss;

typedef struct {
    float y;
    float meia_altura;
    int timer;
    bool ativo;
} LaserBoss;

typedef struct {
    float x;
    float y_min, y_max;
    float largura;
    float espessura_parede;

    int vida;
    int vida_max;
    bool ativo;
    bool fase2;
    bool primeiro_especial_f2;

    int timer_tiro_normal;
    int timer_especial;
    int ultimo_especial;

    int rajada_restante;
    int timer_rajada;
    int boca_atual;

    int timer_dano;
    float tempo_animacao;

    ProjetilBoss tiros[MAX_TIROS_BOSS];
    LaserBoss lasers[MAX_LASERS_BOSS];
} Boss;

void inicializar_boss(Boss* boss, float largura_tela, float altura_tela, float espessura_parede);

void atualizar_boss(
    Boss* boss,
    Jogador* jogador,
    float largura_tela,
    float altura_tela
);

bool verificar_tiro_jogador_no_boss(Boss* boss, Projetil* tiro_jogador, int dano_jogador);
bool verificar_dano_boss_no_jogador(Boss* boss, Jogador* jogador);

void desenhar_boss(const Boss* boss, const Jogador* jogador);
void desenhar_tiros_boss(const Boss* boss);
void desenhar_barra_vida_boss(const Boss* boss, ALLEGRO_FONT* fonte, float largura_tela, float altura_tela);

#endif