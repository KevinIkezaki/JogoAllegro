#include "inimigo.h"
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void atualizar_inimigo(Inimigo* inimigo, Jogador* jogador, Projetil* tiro_inimigo) {
    if (inimigo->x < jogador->x) inimigo->x += inimigo->vel;
    if (inimigo->x > jogador->x) inimigo->x -= inimigo->vel;
    if (inimigo->y < jogador->y) inimigo->y += inimigo->vel;
    if (inimigo->y > jogador->y) inimigo->y -= inimigo->vel;

    inimigo->timer_tiro += 1;

    if (inimigo->timer_tiro > 60 && !(tiro_inimigo->ativo)) {
        tiro_inimigo->ativo = true;
        tiro_inimigo->x = inimigo->x;
        tiro_inimigo->y = inimigo->y;

        float dist_x = jogador->x - inimigo->x;
        float dist_y = jogador->y - inimigo->y;
        float soma = (dist_x < 0 ? -dist_x : dist_x) + (dist_y < 0 ? -dist_y : dist_y);

        if (soma > 0) {
            tiro_inimigo->vx = (dist_x / soma) * 8.0f;
            tiro_inimigo->vy = (dist_y / soma) * 8.0f;
        }
        else {
            tiro_inimigo->vx = 0;
            tiro_inimigo->vy = 0;
        }

        inimigo->timer_tiro = 0;
    }
}

void desenhar_inimigo(Inimigo* inimigo) {
    al_draw_filled_rectangle(
        inimigo->x - inimigo->tamanho / 2, inimigo->y - inimigo->tamanho / 2,
        inimigo->x + inimigo->tamanho / 2, inimigo->y + inimigo->tamanho / 2,
        al_map_rgb(184, 40, 91)
    );
}

void atualizar_ameba(Ameba* ameba, Jogador* jogador) {
    if (ameba->x < jogador->x)
        ameba->x += ameba->vel;

    if (ameba->x > jogador->x)
        ameba->x -= ameba->vel;

    if (ameba->y < jogador->y)
        ameba->y += ameba->vel;

    if (ameba->y > jogador->y)
        ameba->y -= ameba->vel;
}

void desenhar_ameba(Ameba* ameba) {
    al_draw_filled_circle(
        ameba->x,
        ameba->y,
        ameba->tamanho / 2,
        al_map_rgb(145, 60, 190)
    );
}

void desenhar_barra_vida_ameba(Ameba* ameba) {
    float largura = 40.0f;
    float altura = 5.0f;

    float x = ameba->x - largura / 2;
    float y = ameba->y - ameba->tamanho / 2 - 10;

    /* Fundo da barra */
    al_draw_filled_rectangle(
        x, y,
        x + largura, y + altura,
        al_map_rgb(80, 80, 80)
    );

    /* Vida atual */
    al_draw_filled_rectangle(
        x, y,
        x + largura * ameba->vida / 150.0f, y + altura,
        al_map_rgb(0, 220, 80)
    );
}

void atualizar_tiro(Projetil* tiro, float largura, float altura) {
    if (tiro->ativo) {
        tiro->x += tiro->vx;
        tiro->y += tiro->vy;

        if (tiro->x < 0 || tiro->x > largura || tiro->y < 0 || tiro->y > altura)
            tiro->ativo = false;
    }
}

void desenhar_tiro(Projetil* tiro, unsigned char r, unsigned char g, unsigned char b) {
    if (tiro->ativo)
        al_draw_filled_circle(tiro->x, tiro->y, tiro->raio, al_map_rgb(r, g, b));
}

bool verificar_colisao(Projetil* tiro, float alvo_x, float alvo_y, float alvo_tamanho) {
    if (!tiro->ativo) return false;

    return tiro->x > alvo_x - alvo_tamanho / 2 && tiro->x < alvo_x + alvo_tamanho / 2
        && tiro->y > alvo_y - alvo_tamanho / 2 && tiro->y < alvo_y + alvo_tamanho / 2;
}

static void limitar_na_sala(
    float* x, float* y, float tamanho,
    float largura_tela, float altura_tela, float espessura_parede
) {
    float min_x = espessura_parede + tamanho / 2.0f;
    float max_x = largura_tela - espessura_parede - tamanho / 2.0f;
    float min_y = espessura_parede + tamanho / 2.0f;
    float max_y = altura_tela - espessura_parede - tamanho / 2.0f;

    if (*x < min_x) *x = min_x;
    if (*x > max_x) *x = max_x;
    if (*y < min_y) *y = min_y;
    if (*y > max_y) *y = max_y;
}

static void separar_entidades(
    float* x1, float* y1, float tam1,
    float* x2, float* y2, float tam2
) {
    float dx = *x2 - *x1;
    float dy = *y2 - *y1;
    float dist_min = (tam1 + tam2) / 2.0f;
    float dist_sq = dx * dx + dy * dy;

    if (dist_sq < dist_min * dist_min) {
        float dist = sqrtf(dist_sq);
        float nx, ny;

        if (dist > 0.001f) {
            nx = dx / dist;
            ny = dy / dist;
        }
        else {
            nx = 1.0f;
            ny = 0.0f;
            dist = 0.0f;
        }

        float empurrao = (dist_min - dist) / 2.0f;
        *x1 -= nx * empurrao;
        *y1 -= ny * empurrao;
        *x2 += nx * empurrao;
        *y2 += ny * empurrao;
    }
}

void separar_todos_inimigos(
    Inimigo inimigos[], int qtd_inimigos,
    Ameba amebas[], int qtd_amebas,
    float largura_tela, float altura_tela, float espessura_parede
) {
    /* Inimigo x Inimigo */
    for (int i = 0; i < qtd_inimigos; i++) {
        if (inimigos[i].vida <= 0) continue;
        for (int j = i + 1; j < qtd_inimigos; j++) {
            if (inimigos[j].vida <= 0) continue;
            separar_entidades(
                &inimigos[i].x, &inimigos[i].y, inimigos[i].tamanho,
                &inimigos[j].x, &inimigos[j].y, inimigos[j].tamanho
            );
        }
    }

    /* Ameba x Ameba */
    for (int i = 0; i < qtd_amebas; i++) {
        if (amebas[i].vida <= 0) continue;
        for (int j = i + 1; j < qtd_amebas; j++) {
            if (amebas[j].vida <= 0) continue;
            separar_entidades(
                &amebas[i].x, &amebas[i].y, amebas[i].tamanho,
                &amebas[j].x, &amebas[j].y, amebas[j].tamanho
            );
        }
    }

    /* Inimigo x Ameba */
    for (int i = 0; i < qtd_inimigos; i++) {
        if (inimigos[i].vida <= 0) continue;
        for (int j = 0; j < qtd_amebas; j++) {
            if (amebas[j].vida <= 0) continue;
            separar_entidades(
                &inimigos[i].x, &inimigos[i].y, inimigos[i].tamanho,
                &amebas[j].x, &amebas[j].y, amebas[j].tamanho
            );
        }
    }

    /* Mantem todos dentro das paredes da sala */
    for (int i = 0; i < qtd_inimigos; i++) {
        if (inimigos[i].vida > 0) {
            limitar_na_sala(
                &inimigos[i].x, &inimigos[i].y, inimigos[i].tamanho,
                largura_tela, altura_tela, espessura_parede
            );
        }
    }
    for (int i = 0; i < qtd_amebas; i++) {
        if (amebas[i].vida > 0) {
            limitar_na_sala(
                &amebas[i].x, &amebas[i].y, amebas[i].tamanho,
                largura_tela, altura_tela, espessura_parede
            );
        }
    }
}