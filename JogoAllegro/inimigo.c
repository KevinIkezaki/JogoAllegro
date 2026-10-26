
#include "inimigo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void atualizar_inimigo(float* inimigo_x, float* inimigo_y,
    float jogador_x, float jogador_y, int* tempo_para_atirar,
    bool* tiro_ativo, float* tiro_x, float* tiro_y,
    float* tiro_velocidade_x, float* tiro_velocidade_y) {

    if (*inimigo_x < jogador_x) *inimigo_x += 0.5f;
    if (*inimigo_x > jogador_x) *inimigo_x -= 0.5f;
    if (*inimigo_y < jogador_y) *inimigo_y += 0.5f;
    if (*inimigo_y > jogador_y) *inimigo_y -= 0.5f;

    *tempo_para_atirar += 1;

    if (*tempo_para_atirar > 60 && !(*tiro_ativo)) {
        *tiro_ativo = true;
        *tiro_x = *inimigo_x;
        *tiro_y = *inimigo_y;

        float dist_x = jogador_x - *inimigo_x;
        float dist_y = jogador_y - *inimigo_y;
        float soma = (dist_x < 0 ? -dist_x : dist_x)
            + (dist_y < 0 ? -dist_y : dist_y);

        if (soma > 0) {
            *tiro_velocidade_x = (dist_x / soma) * 8.0f;
            *tiro_velocidade_y = (dist_y / soma) * 8.0f;
        }
        else {
            *tiro_velocidade_x = 0;
            *tiro_velocidade_y = 0;
        }

        *tempo_para_atirar = 0;
    }
}

void desenhar_inimigo(float x, float y, float tamanho) {
    al_draw_filled_rectangle(
        x - tamanho / 2, y - tamanho / 2,
        x + tamanho / 2, y + tamanho / 2,
        al_map_rgb(184, 40, 91)
    );
}

void atualizar_tiro(bool* ativo, float* x, float* y,
    float vx, float vy, float largura, float altura) {

    if (*ativo) {
        *x += vx;
        *y += vy;

        if (*x < 0 || *x > largura || *y < 0 || *y > altura)
            *ativo = false;
    }
}

void desenhar_tiro(bool ativo, float x, float y) {
    if (ativo)
        al_draw_filled_circle(x, y, 5.0f, al_map_rgb(255, 0, 0));
}

bool verificar_colisao(bool ativo, float tx, float ty,
    float jx, float jy, float tamanho) {

    if (!ativo) return false;

    return tx > jx - tamanho / 2 && tx < jx + tamanho / 2
        && ty > jy - tamanho / 2 && ty < jy + tamanho / 2;
}

bool verificar_colisao_inimigo(bool ativo, float tx, float ty,
    float ix, float iy, float tamanho) {

    if (!ativo) return false;

    return tx > ix - tamanho / 2 && tx < ix + tamanho / 2
        && ty > iy - tamanho / 2 && ty < iy + tamanho / 2;
}
