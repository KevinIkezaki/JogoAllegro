#include "inimigo.h"
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