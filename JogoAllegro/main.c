#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "inimigo.h"

#define LARGURA_TELA 800
#define ALTURA_TELA  600
#define FPS          60.0

int main(void) {
    /* 1. Inicializacao da biblioteca e dos modulos */
    if (!al_init()) {
        fprintf(stderr, "Falha ao iniciar o Allegro.\n");
        return 1;
    }
    if (!al_install_keyboard() || !al_init_primitives_addon() || !al_init_font_addon()) {
        fprintf(stderr, "Falha ao iniciar teclado ou add-ons.\n");
        return 1;
    }

    srand(time(NULL));

    /* 2. Criacao dos recursos */
    ALLEGRO_DISPLAY* janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_FONT* fonte = al_create_builtin_font();

    if (!janela || !timer || !fila || !fonte) {
        fprintf(stderr, "Falha ao criar janela, timer, fila ou fonte.\n");
        return 1;
    }
    al_set_window_title(janela, "Meu primeiro jogo em Allegro");

    /* 3. A fila recebe eventos da janela, do timer e do teclado */
    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_keyboard_event_source());

    /* 4. Estado do jogo */
    float jogador_x = LARGURA_TELA / 2.0f;
    float jogador_y = ALTURA_TELA / 2.0f;
    const float JOGADOR_TAMANHO = 40.0f;
    const float JOGADOR_VELOCIDADE = 4.0f;
    int vida_jogador = 100;
    bool jogador_morto = false;

    float inimigo_x = rand() % LARGURA_TELA;
    float inimigo_y = rand() % ALTURA_TELA;
    const float INIMIGO_TAMANHO = 20.0f;

    float tiro_x = 0;
    float tiro_y = 0;
    float tiro_velocidade_x = 0;
    float tiro_velocidade_y = 0;
    bool tiro_ativo = false;
    int tempo_para_atirar = 0;

    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true, redesenhar = true;

    /* 5. Laco principal */
    al_start_timer(timer);
    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        switch (ev.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            rodando = false;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            teclas[ev.keyboard.keycode] = true;
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) rodando = false;
            if (jogador_morto && ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                jogador_morto = false;
                vida_jogador = 100;
                jogador_x = LARGURA_TELA / 2.0f;
                jogador_y = ALTURA_TELA / 2.0f;

                inimigo_x = rand() % LARGURA_TELA;
                inimigo_y = rand() % ALTURA_TELA;

                tiro_ativo = false;
                tempo_para_atirar = 0;
            }
            break;
        case ALLEGRO_EVENT_KEY_UP:
            teclas[ev.keyboard.keycode] = false;
            break;
        case ALLEGRO_EVENT_TIMER:          /* logica: 60 vezes por segundo */
            if (!jogador_morto) {
                if (teclas[ALLEGRO_KEY_A]) jogador_x -= JOGADOR_VELOCIDADE;
                if (teclas[ALLEGRO_KEY_D]) jogador_x += JOGADOR_VELOCIDADE;
                if (teclas[ALLEGRO_KEY_W]) jogador_y -= JOGADOR_VELOCIDADE;
                if (teclas[ALLEGRO_KEY_S]) jogador_y += JOGADOR_VELOCIDADE;

                if (jogador_x < JOGADOR_TAMANHO / 2) jogador_x = JOGADOR_TAMANHO / 2;
                if (jogador_x > LARGURA_TELA - JOGADOR_TAMANHO / 2) jogador_x = LARGURA_TELA - JOGADOR_TAMANHO / 2;
                if (jogador_y < JOGADOR_TAMANHO / 2) jogador_y = JOGADOR_TAMANHO / 2;
                if (jogador_y > ALTURA_TELA - JOGADOR_TAMANHO / 2) jogador_y = ALTURA_TELA - JOGADOR_TAMANHO / 2;

                atualizar_inimigo(&inimigo_x, &inimigo_y, jogador_x, jogador_y, &tempo_para_atirar, &tiro_ativo, &tiro_x, &tiro_y, &tiro_velocidade_x, &tiro_velocidade_y);
                atualizar_tiro(&tiro_ativo, &tiro_x, &tiro_y, tiro_velocidade_x, tiro_velocidade_y, LARGURA_TELA, ALTURA_TELA);

                if (verificar_colisao(tiro_ativo, tiro_x, tiro_y, jogador_x, jogador_y, JOGADOR_TAMANHO)) {
                    vida_jogador -= 25;
                    tiro_ativo = false;
                    if (vida_jogador <= 0) jogador_morto = true;
                }
            }
            redesenhar = true;
            break;
        }

        /* 6. Desenho: so quando a fila esvazia, para nao acumular atraso */
        if (redesenhar && al_is_event_queue_empty(fila)) {
            redesenhar = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (!jogador_morto) {
                al_draw_filled_rectangle(jogador_x - JOGADOR_TAMANHO / 2, jogador_y - JOGADOR_TAMANHO / 2, jogador_x + JOGADOR_TAMANHO / 2, jogador_y + JOGADOR_TAMANHO / 2, al_map_rgb(255, 255, 255));

                al_draw_filled_rectangle(jogador_x - JOGADOR_TAMANHO / 2, jogador_y - JOGADOR_TAMANHO / 2 - 10, jogador_x + JOGADOR_TAMANHO / 2, jogador_y - JOGADOR_TAMANHO / 2 - 5, al_map_rgb(255, 0, 0));
                al_draw_filled_rectangle(jogador_x - JOGADOR_TAMANHO / 2, jogador_y - JOGADOR_TAMANHO / 2 - 10, jogador_x - JOGADOR_TAMANHO / 2 + (JOGADOR_TAMANHO * (vida_jogador / 100.0f)), jogador_y - JOGADOR_TAMANHO / 2 - 5, al_map_rgb(0, 255, 0));

                desenhar_inimigo(inimigo_x, inimigo_y, INIMIGO_TAMANHO);
                desenhar_tiro(tiro_ativo, tiro_x, tiro_y);
            }
            else {
                al_draw_text(fonte, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 10, ALLEGRO_ALIGN_CENTER, "Voce morreu");
                al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 10, ALLEGRO_ALIGN_CENTER, "Aperte ESPACO para recomecar");
            }

            al_flip_display();
        }
    }

    /* 7. Liberacao dos recursos, na ordem inversa da criacao */
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    return 0;
}