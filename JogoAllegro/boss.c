#include "boss.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define PI_BOSS 3.14159265f

/* Retorna a coordenada Y de uma das 3 bocas/olhos da parede */
static float obter_y_boca(const Boss* boss, int indice) {
    float altura = boss->y_max - boss->y_min;
    if (indice == 0) return boss->y_min + altura * 0.22f;
    if (indice == 1) return boss->y_min + altura * 0.50f;
    return boss->y_min + altura * 0.78f;
}

/* Cria um projetil do boss em um slot livre */
static void criar_tiro_direto(
    Boss* boss,
    float x,
    float y,
    float vx,
    float vy,
    float raio,
    int dano,
    int tempo_vida,
    TipoTiroBoss tipo
) {
    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) {
            boss->tiros[i].ativo = true;
            boss->tiros[i].x = x;
            boss->tiros[i].y = y;
            boss->tiros[i].vx = vx;
            boss->tiros[i].vy = vy;
            boss->tiros[i].raio = raio;
            boss->tiros[i].dano = dano;
            boss->tiros[i].tempo_vida = tempo_vida;
            boss->tiros[i].tipo = tipo;
            break;
        }
    }
}

/* Dispara um projetil mirando no jogador com possivel desvio angular */
static void disparar_tiro_boss(
    Boss* boss,
    float origem_x,
    float origem_y,
    const Jogador* jogador,
    float velocidade,
    float raio,
    int dano,
    int tempo_vida,
    TipoTiroBoss tipo,
    float desvio_rad
) {
    float dx = jogador->x - origem_x;
    float dy = jogador->y - origem_y;
    float dist = sqrtf(dx * dx + dy * dy);

    float nx = -1.0f;
    float ny = 0.0f;

    if (dist > 0.001f) {
        nx = dx / dist;
        ny = dy / dist;
    }

    if (desvio_rad != 0.0f) {
        float c = cosf(desvio_rad);
        float s = sinf(desvio_rad);
        float rnx = nx * c - ny * s;
        float rny = nx * s + ny * c;
        nx = rnx;
        ny = rny;
    }

    criar_tiro_direto(
        boss, origem_x, origem_y,
        nx * velocidade, ny * velocidade,
        raio, dano, tempo_vida, tipo
    );
}

/* Explode a bola gigante em mini bolinhas disparadas em 360 graus */
static void explodir_tiro_grande(Boss* boss, float origem_x, float origem_y) {
    int qtd = boss->fase2 ? BOSS_MINI_BOLINHAS_F2 : BOSS_MINI_BOLINHAS_F1;
    float angulo_base = ((float)(rand() % 360)) * (PI_BOSS / 180.0f);
    float passo = (2.0f * PI_BOSS) / (float)qtd;

    for (int k = 0; k < qtd; k++) {
        float ang = angulo_base + k * passo;
        criar_tiro_direto(
            boss, origem_x, origem_y,
            cosf(ang) * 4.6f, sinf(ang) * 4.6f,
            5.5f, 15, 0, TIRO_BOSS_MINI
        );
    }
}

/* Fase 2: Laser duplo telegrafado (2 olhos distintos, sempre deixando 1 faixa segura) */
static void iniciar_ataque_laser(Boss* boss) {
    for (int i = 0; i < MAX_LASERS_BOSS; i++) {
        boss->lasers[i].ativo = false;
    }

    int olho1 = rand() % 3;
    int olho2 = (olho1 + 1 + (rand() % 2)) % 3;

    boss->lasers[0].ativo = true;
    boss->lasers[0].y = obter_y_boca(boss, olho1);
    boss->lasers[0].meia_altura = 20.0f;
    boss->lasers[0].timer = 0;

    boss->lasers[1].ativo = true;
    boss->lasers[1].y = obter_y_boca(boss, olho2);
    boss->lasers[1].meia_altura = 20.0f;
    boss->lasers[1].timer = 0;
}

void inicializar_boss(Boss* boss, float largura_tela, float altura_tela, float espessura_parede) {
    boss->largura = BOSS_LARGURA;
    boss->espessura_parede = espessura_parede;
    boss->x = largura_tela - espessura_parede - boss->largura;
    boss->y_min = espessura_parede;
    boss->y_max = altura_tela - espessura_parede;

    boss->vida = VIDA_MAX_BOSS;
    boss->vida_max = VIDA_MAX_BOSS;
    boss->ativo = true;
    boss->fase2 = false;
    boss->primeiro_especial_f2 = false;

    boss->timer_tiro_normal = 0;
    boss->timer_especial = 0;
    boss->ultimo_especial = BOSS_ATAQUE_NENHUM;

    boss->rajada_restante = 0;
    boss->timer_rajada = 0;
    boss->boca_atual = 1;

    boss->timer_dano = 0;
    boss->tempo_animacao = 0.0f;

    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        boss->tiros[i].ativo = false;
        boss->tiros[i].x = 0.0f;
        boss->tiros[i].y = 0.0f;
        boss->tiros[i].vx = 0.0f;
        boss->tiros[i].vy = 0.0f;
        boss->tiros[i].raio = 6.0f;
        boss->tiros[i].dano = 20;
        boss->tiros[i].tempo_vida = 0;
        boss->tiros[i].tipo = TIRO_BOSS_NORMAL;
    }

    for (int i = 0; i < MAX_LASERS_BOSS; i++) {
        boss->lasers[i].ativo = false;
        boss->lasers[i].y = 0.0f;
        boss->lasers[i].meia_altura = 20.0f;
        boss->lasers[i].timer = 0;
    }
}

void atualizar_boss(
    Boss* boss,
    Jogador* jogador,
    float largura_tela,
    float altura_tela
) {
    if (!boss->ativo || boss->vida <= 0) {
        return;
    }

    /* Transicao dinamica para a Fase 2 sempre aos 50% de vida_max */
    if (!boss->fase2 && (boss->vida * 2 <= boss->vida_max)) {
        boss->fase2 = true;
        boss->primeiro_especial_f2 = true;
    }

    boss->tempo_animacao += boss->fase2 ? 0.14f : 0.08f;

    if (boss->timer_dano > 0) boss->timer_dano--;

    /* 1) Execucao em andamento do ataque de RAJADA */
    if (boss->rajada_restante > 0) {
        boss->timer_rajada++;
        if (boss->timer_rajada >= BOSS_RAJADA_INTERVAL) {
            boss->timer_rajada = 0;

            float origem_y = obter_y_boca(boss, boss->boca_atual);
            boss->boca_atual = (boss->boca_atual + 1) % 3;

            float desvio = ((float)(rand() % 25) - 12.0f) * 0.01f;
            disparar_tiro_boss(
                boss, boss->x - 6.0f, origem_y, jogador,
                6.2f, 6.0f, 20, 0, TIRO_BOSS_NORMAL, desvio
            );
            boss->rajada_restante--;
        }
    }
    else {
        /* 2) Ataque NORMAL continuo */
        int cadenciaNormal = boss->fase2 ? BOSS_CADENCIA_NORMAL_F2 : BOSS_CADENCIA_NORMAL_F1;
        boss->timer_tiro_normal++;
        if (boss->timer_tiro_normal >= cadenciaNormal) {
            boss->timer_tiro_normal = 0;

            int boca = rand() % 3;
            float origem_y = obter_y_boca(boss, boca);
            disparar_tiro_boss(
                boss, boss->x - 6.0f, origem_y, jogador,
                5.2f, 6.5f, 20, 0, TIRO_BOSS_NORMAL, 0.0f
            );
        }
    }

    /* 3) Timer para ataques ESPECIAIS */
    int tempoEspecial = boss->fase2 ? BOSS_TEMPO_ESPECIAL_F2 : BOSS_TEMPO_ESPECIAL_F1;
    boss->timer_especial++;

    if (boss->timer_especial >= tempoEspecial) {
        boss->timer_especial = 0;

        int sorteio;
        if (boss->primeiro_especial_f2) {
            /* Primeiro especial da Fase 2 dispara o Laser Duplo */
            boss->primeiro_especial_f2 = false;
            sorteio = BOSS_ATAQUE_LASER;
        }
        else if (!boss->fase2) {
            sorteio = rand() % TOTAL_ATAQUES_FASE_1;
        }
        else {
            sorteio = rand() % TOTAL_ATAQUES_FASE_2;
            if (sorteio == boss->ultimo_especial) {
                sorteio = (sorteio + 1 + (rand() % (TOTAL_ATAQUES_FASE_2 - 1))) % TOTAL_ATAQUES_FASE_2;
            }
        }

        boss->ultimo_especial = sorteio;

        switch (sorteio) {
        case BOSS_ATAQUE_RAJADA:
            boss->rajada_restante = BOSS_RAJADA_QTD;
            boss->timer_rajada = BOSS_RAJADA_INTERVAL;
            boss->boca_atual = rand() % 3;
            break;

        case BOSS_ATAQUE_TIRO_GRANDE: {
            float origem_y = obter_y_boca(boss, 1);
            disparar_tiro_boss(
                boss, boss->x - 16.0f, origem_y, jogador,
                4.4f, 26.0f, 35, BOSS_TEMPO_EXPLOSAO_GRANDE,
                TIRO_BOSS_GRANDE, 0.0f
            );
            break;
        }

        case BOSS_ATAQUE_LASER:
            iniciar_ataque_laser(boss);
            break;
        }
    }

    /* 4) Atualiza lasers */
    for (int i = 0; i < MAX_LASERS_BOSS; i++) {
        if (!boss->lasers[i].ativo) continue;
        boss->lasers[i].timer++;
        if (boss->lasers[i].timer >= BOSS_LASER_AVISO + BOSS_LASER_DURACAO) {
            boss->lasers[i].ativo = false;
        }
    }

    /* 5) Atualiza projeteis (esfera explosiva inclusa) */
    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) continue;

        boss->tiros[i].x += boss->tiros[i].vx;
        boss->tiros[i].y += boss->tiros[i].vy;

        if (boss->tiros[i].tipo == TIRO_BOSS_GRANDE) {
            boss->tiros[i].tempo_vida--;
            if (boss->tiros[i].tempo_vida <= 0) {
                float ex = boss->tiros[i].x;
                float ey = boss->tiros[i].y;
                boss->tiros[i].ativo = false;
                explodir_tiro_grande(boss, ex, ey);
                continue;
            }
        }

        float r = boss->tiros[i].raio;
        if (boss->tiros[i].x < -r || boss->tiros[i].x > largura_tela + r ||
            boss->tiros[i].y < -r || boss->tiros[i].y > altura_tela + r) {
            boss->tiros[i].ativo = false;
        }
    }
}

bool verificar_tiro_jogador_no_boss(Boss* boss, Projetil* tiro_jogador, int dano_jogador) {
    if (!boss->ativo || boss->vida <= 0 || !tiro_jogador->ativo) {
        return false;
    }

    if (tiro_jogador->x + tiro_jogador->raio >= boss->x &&
        tiro_jogador->y >= boss->y_min &&
        tiro_jogador->y <= boss->y_max) {

        tiro_jogador->ativo = false;
        boss->vida -= dano_jogador;
        boss->timer_dano = 5;

        if (boss->vida <= 0) {
            boss->vida = 0;
            boss->ativo = false;
        }
        return true;
    }

    return false;
}

static bool aplicar_dano_no_jogador(Jogador* jogador, int dano) {
    if (jogador->invulnerabilidade > 0) {
        return false;
    }

    jogador->vida -= dano;
    jogador->invulnerabilidade = 90;

    if (jogador->vida <= 0) {
        jogador->vida = 0;
        return true;
    }
    return false;
}

bool verificar_dano_boss_no_jogador(Boss* boss, Jogador* jogador) {
    if (!boss->ativo || boss->vida <= 0) {
        return false;
    }

    float meia_jog = jogador->tamanho / 2.0f;

    /* 1) Colisao com projeteis do boss */
    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) continue;

        float dx = jogador->x - boss->tiros[i].x;
        float dy = jogador->y - boss->tiros[i].y;
        float limite = meia_jog + boss->tiros[i].raio * 0.85f;

        if (dx > -limite && dx < limite && dy > -limite && dy < limite) {
            int dano = boss->tiros[i].dano;
            boss->tiros[i].ativo = false;

            if (aplicar_dano_no_jogador(jogador, dano)) {
                return true;
            }
        }
    }

    /* 2) Colisao com lasers ativos */
    for (int i = 0; i < MAX_LASERS_BOSS; i++) {
        if (!boss->lasers[i].ativo) continue;
        if (boss->lasers[i].timer < BOSS_LASER_AVISO) continue;

        if (fabsf(jogador->y - boss->lasers[i].y) <= meia_jog + boss->lasers[i].meia_altura * 0.78f) {
            if (aplicar_dano_no_jogador(jogador, 25)) {
                return true;
            }
        }
    }

    /* 3) Colisao direta se o jogador encostar na parede */
    if (jogador->x + meia_jog >= boss->x - 1.0f) {
        if (aplicar_dano_no_jogador(jogador, 25)) {
            return true;
        }
    }

    return false;
}

void desenhar_boss(const Boss* boss, const Jogador* jogador) {
    if (!boss->ativo || boss->vida <= 0) {
        return;
    }

    bool flash = (boss->timer_dano > 0);

    ALLEGRO_COLOR corBase;
    ALLEGRO_COLOR corCarne;
    ALLEGRO_COLOR corVeia;

    if (flash) {
        corBase = al_map_rgb(235, 115, 120);
        corCarne = al_map_rgb(250, 150, 150);
        corVeia = al_map_rgb(120, 25, 35);
    }
    else if (boss->fase2) {
        corBase = al_map_rgb(175, 18, 28);
        corCarne = al_map_rgb(215, 42, 48);
        corVeia = al_map_rgb(110, 8, 18);
    }
    else {
        corBase = al_map_rgb(138, 24, 42);
        corCarne = al_map_rgb(172, 38, 60);
        corVeia = al_map_rgb(95, 14, 28);
    }

    al_draw_filled_rectangle(
        boss->x, boss->y_min,
        boss->x + boss->largura, boss->y_max,
        corBase
    );

    int segmentos = 16;
    float passo = (boss->y_max - boss->y_min) / (float)segmentos;
    float ampPulso = boss->fase2 ? 6.0f : 4.0f;

    for (int i = 0; i <= segmentos; i++) {
        float cy = boss->y_min + i * passo;
        float pulso = sinf(boss->tempo_animacao + i * 0.9f) * ampPulso;
        float raio = 18.0f + sinf(boss->tempo_animacao * 1.3f + i) * 3.0f;

        al_draw_filled_circle(boss->x + 6.0f + pulso, cy, raio, corCarne);
        al_draw_circle(boss->x + 6.0f + pulso, cy, raio, corVeia, 2.0f);
    }

    for (int i = 0; i < 6; i++) {
        float vy = boss->y_min + 45.0f + i * 85.0f;
        float vx = boss->x + 36.0f + sinf(boss->tempo_animacao + i) * 3.0f;
        al_draw_filled_circle(vx, vy, 11.0f, corVeia);
    }

    /* 3 Olhos / bocas que disparam e acompanham o jogador */
    int tempoEspecial = boss->fase2 ? BOSS_TEMPO_ESPECIAL_F2 : BOSS_TEMPO_ESPECIAL_F1;
    bool carregandoEspecial = (boss->timer_especial >= tempoEspecial - 40);

    for (int b = 0; b < 3; b++) {
        float by = obter_y_boca(boss, b);
        float bx = boss->x + 22.0f;

        if (carregandoEspecial) {
            al_draw_filled_circle(bx, by, 21.0f, al_map_rgba(255, 190, 40, 120));
        }

        al_draw_filled_circle(bx, by, 16.0f, al_map_rgb(75, 10, 20));
        al_draw_filled_circle(bx, by, 12.0f, carregandoEspecial
            ? al_map_rgb(255, 220, 110)
            : (boss->fase2 ? al_map_rgb(255, 195, 160) : al_map_rgb(235, 225, 195)));

        float dx = jogador->x - bx;
        float dy = jogador->y - by;
        float dist = sqrtf(dx * dx + dy * dy);
        float px = bx;
        float py = by;

        if (dist > 0.001f) {
            px += (dx / dist) * 5.0f;
            py += (dy / dist) * 5.0f;
        }

        al_draw_filled_circle(px, py, 5.5f, al_map_rgb(200, 20, 35));
        al_draw_filled_circle(px, py, 2.5f, al_map_rgb(20, 5, 10));
    }
}

void desenhar_tiros_boss(const Boss* boss) {
    /* 1) Desenho dos Lasers (aviso ou feixe ativo) */
    for (int i = 0; i < MAX_LASERS_BOSS; i++) {
        if (!boss->lasers[i].ativo) continue;

        float y = boss->lasers[i].y;
        float mh = boss->lasers[i].meia_altura;
        float x_esq = boss->espessura_parede;
        float x_dir = boss->x + 12.0f;

        if (boss->lasers[i].timer < BOSS_LASER_AVISO) {
            bool pisca = (boss->lasers[i].timer % 8 < 4);
            al_draw_filled_rectangle(
                x_esq, y - mh, x_dir, y + mh,
                al_map_rgba(255, 40, 40, pisca ? 55 : 25)
            );
            al_draw_line(
                x_esq, y, x_dir, y,
                al_map_rgba(255, 220, 80, pisca ? 230 : 120), 2.5f
            );
        }
        else {
            al_draw_filled_rectangle(x_esq, y - mh - 4.0f, x_dir, y + mh + 4.0f, al_map_rgba(255, 40, 30, 110));
            al_draw_filled_rectangle(x_esq, y - mh, x_dir, y + mh, al_map_rgb(235, 40, 40));
            al_draw_filled_rectangle(x_esq, y - mh * 0.55f, x_dir, y + mh * 0.55f, al_map_rgb(255, 165, 60));
            al_draw_filled_rectangle(x_esq, y - mh * 0.22f, x_dir, y + mh * 0.22f, al_map_rgb(255, 245, 210));
        }
    }

    /* 2) Desenho dos projeteis do boss */
    for (int i = 0; i < MAX_TIROS_BOSS; i++) {
        if (!boss->tiros[i].ativo) continue;

        float x = boss->tiros[i].x;
        float y = boss->tiros[i].y;
        float r = boss->tiros[i].raio;

        switch (boss->tiros[i].tipo) {
        case TIRO_BOSS_GRANDE: {
            bool prestesAExplodir = (boss->tiros[i].tempo_vida <= 15);
            float pulso = prestesAExplodir ? ((boss->tiros[i].tempo_vida % 4 < 2) ? 4.0f : -2.0f) : 0.0f;
            float rDesenho = r + pulso;

            al_draw_filled_circle(x, y, rDesenho + 4.0f, al_map_rgba(255, 90, 20, 100));
            al_draw_filled_circle(x, y, rDesenho, prestesAExplodir
                ? al_map_rgb(255, 100, 40)
                : al_map_rgb(220, 35, 35));
            al_draw_filled_circle(x, y, rDesenho * 0.6f, al_map_rgb(255, 165, 55));
            al_draw_filled_circle(x, y, rDesenho * 0.3f, al_map_rgb(255, 245, 190));
            break;
        }

        case TIRO_BOSS_MINI:
            al_draw_filled_circle(x, y, r, al_map_rgb(255, 150, 35));
            al_draw_filled_circle(x, y, r * 0.45f, al_map_rgb(255, 240, 160));
            break;

        default:
            al_draw_filled_circle(x, y, r, al_map_rgb(255, 70, 50));
            al_draw_filled_circle(x, y, r * 0.45f, al_map_rgb(255, 210, 120));
            break;
        }
    }
}

void desenhar_barra_vida_boss(const Boss* boss, ALLEGRO_FONT* fonte, float largura_tela, float altura_tela) {
    if (!boss->ativo || boss->vida <= 0) {
        return;
    }

    float largura_barra = 460.0f;
    float altura_barra = 20.0f;
    float x1 = (largura_tela - largura_barra) / 2.0f;
    float y1 = altura_tela - 26.0f;

    float proporcao = (float)boss->vida / (float)boss->vida_max;
    if (proporcao < 0.0f) proporcao = 0.0f;

    al_draw_filled_rounded_rectangle(
        x1, y1,
        x1 + largura_barra, y1 + altura_barra,
        5, 5, al_map_rgb(40, 12, 18)
    );

    if (proporcao > 0.0f) {
        ALLEGRO_COLOR corBarra = boss->fase2
            ? al_map_rgb(235, 65, 35)
            : al_map_rgb(210, 35, 55);

        al_draw_filled_rounded_rectangle(
            x1, y1,
            x1 + largura_barra * proporcao, y1 + altura_barra,
            5, 5, corBarra
        );
    }

    /* Marcador visual de 50% (inicio da Fase 2) */
    float meio_x = x1 + largura_barra * 0.5f;
    al_draw_line(meio_x, y1, meio_x, y1 + altura_barra, al_map_rgba(255, 255, 255, 180), 2.0f);

    al_draw_rounded_rectangle(
        x1, y1,
        x1 + largura_barra, y1 + altura_barra,
        5, 5, al_map_rgb(245, 220, 220), 2.0f
    );

    char texto[64];
    snprintf(texto, sizeof(texto), "%d / %d", boss->vida, boss->vida_max);

    float alturaFonte = (float)al_get_font_line_height(fonte);
    al_draw_text(
        fonte, al_map_rgb(255, 255, 255),
        largura_tela / 2.0f, y1 + (altura_barra - alturaFonte) / 2.0f,
        ALLEGRO_ALIGN_CENTER,
        texto
    );
}