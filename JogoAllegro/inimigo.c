#include "inimigo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void atualizar_inimigo(float* inimigo_x, float* inimigo_y, float jogador_x, float jogador_y, int* tempo_para_atirar, bool* tiro_ativo, float* tiro_x, float* tiro_y, float* tiro_velocidade_x, float* tiro_velocidade_y) {

    if (*inimigo_x < jogador_x) *inimigo_x += 0.5f;
    if (*inimigo_x > jogador_x) *inimigo_x -= 0.5f;
    if (*inimigo_y < jogador_y) *inimigo_y += 0.5f;
    if (*inimigo_y > jogador_y) *inimigo_y -= 0.5f;

    *tempo_para_atirar = *tempo_para_atirar + 1;
    if (*tempo_para_atirar > 60 && !(*tiro_ativo)) {
        *tiro_ativo = true;
        *tiro_x = *inimigo_x;
        *tiro_y = *inimigo_y;

        float dist_x = jogador_x - *inimigo_x;
        float dist_y = jogador_y - *inimigo_y;

        float dist_abs_x = dist_x;
        if (dist_abs_x < 0) dist_abs_x = -dist_abs_x;

        float dist_abs_y = dist_y;
        if (dist_abs_y < 0) dist_abs_y = -dist_abs_y;

        float soma_distancias = dist_abs_x + dist_abs_y;

        if (soma_distancias > 0) {
            *tiro_velocidade_x = (dist_x / soma_distancias) * 8.0f;
            *tiro_velocidade_y = (dist_y / soma_distancias) * 8.0f;
        }
        else {
            *tiro_velocidade_x = 0;
            *tiro_velocidade_y = 0;
        }

        *tempo_para_atirar = 0;
    }
}

void desenhar_inimigo(float inimigo_x, float inimigo_y, float inimigo_tamanho) {
    al_draw_filled_rectangle(inimigo_x - inimigo_tamanho / 2, inimigo_y - inimigo_tamanho / 2, inimigo_x + inimigo_tamanho / 2, inimigo_y + inimigo_tamanho / 2, al_map_rgb(255, 255, 255));
}

void atualizar_tiro(bool* tiro_ativo, float* tiro_x, float* tiro_y, float tiro_velocidade_x, float tiro_velocidade_y, float largura_tela, float altura_tela) {
    if (*tiro_ativo) {
        *tiro_x += tiro_velocidade_x;
        *tiro_y += tiro_velocidade_y;
        if (*tiro_x < 0 || *tiro_x > largura_tela || *tiro_y < 0 || *tiro_y > altura_tela) {
            *tiro_ativo = false;
        }
    }
}

void desenhar_tiro(bool tiro_ativo, float tiro_x, float tiro_y) {
    if (tiro_ativo) {
        al_draw_filled_circle(tiro_x, tiro_y, 5.0f, al_map_rgb(255, 0, 0));
    }
}

bool verificar_colisao(bool tiro_ativo, float tiro_x, float tiro_y, float jogador_x, float jogador_y, float jogador_tamanho) {
    if (!tiro_ativo) return false;
    if (tiro_x > jogador_x - jogador_tamanho / 2 && tiro_x < jogador_x + jogador_tamanho / 2 && tiro_y > jogador_y - jogador_tamanho / 2 && tiro_y < jogador_y + jogador_tamanho / 2) {
        return true;
    }
    return false;
}