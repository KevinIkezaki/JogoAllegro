
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "inimigo.h"

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60.0f

#define VIDA_MAX_JOGADOR 100
#define VIDA_MAX_INIMIGO 100

typedef enum {
    MENU_INICIAL,
    JOGANDO,
    FIM_DE_JOGO,
    VITORIA
} EstadoJogo;

int main(void) {
    if (!al_init()) {
        fprintf(stderr, "Falha ao iniciar o Allegro.\n");
        return 1;
    }

    if (!al_install_keyboard()) {
        fprintf(stderr, "Falha ao iniciar o teclado.\n");
        return 1;
    }

    if (!al_init_primitives_addon() || !al_init_font_addon()) {
        fprintf(stderr, "Falha ao iniciar os add-ons.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    ALLEGRO_DISPLAY* janela =
        al_create_display(LARGURA_TELA, ALTURA_TELA);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_FONT* fonte = al_create_builtin_font();

    if (!janela || !timer || !fila || !fonte) {
        fprintf(stderr, "Falha ao criar os recursos do jogo.\n");

        if (fonte) al_destroy_font(fonte);
        if (fila) al_destroy_event_queue(fila);
        if (timer) al_destroy_timer(timer);
        if (janela) al_destroy_display(janela);

        return 1;
    }

    al_set_window_title(janela, "JogoAllegro");

    al_register_event_source(
        fila, al_get_display_event_source(janela));
    al_register_event_source(
        fila, al_get_timer_event_source(timer));
    al_register_event_source(
        fila, al_get_keyboard_event_source());

    EstadoJogo estado = MENU_INICIAL;

    float jogador_x = LARGURA_TELA / 2.0f;
    float jogador_y = ALTURA_TELA / 2.0f;

    const float JOGADOR_TAMANHO = 40.0f;
    const float JOGADOR_VELOCIDADE = 4.0f;

    int vida_jogador = VIDA_MAX_JOGADOR;
    int vida_inimigo = VIDA_MAX_INIMIGO;

    float inimigo_x = 600.0f;
    float inimigo_y = 150.0f;

    const float INIMIGO_TAMANHO = 30.0f;

    /* Tiro do inimigo */
    float tiro_inimigo_x = 0.0f;
    float tiro_inimigo_y = 0.0f;
    float tiro_inimigo_vx = 0.0f;
    float tiro_inimigo_vy = 0.0f;

    bool tiro_inimigo_ativo = false;
    int tempo_para_atirar = 0;

    /* Tiro da jogadora */
    float tiro_jogador_x = 0.0f;
    float tiro_jogador_y = 0.0f;
    float tiro_jogador_vx = 0.0f;
    float tiro_jogador_vy = -7.0f;

    bool tiro_jogador_ativo = false;
    int tempo_recarga = 0;

    /* Direcao inicial do tiro: para cima */
    float direcao_x = 0.0f;
    float direcao_y = -1.0f;

    bool teclas[ALLEGRO_KEY_MAX] = { false };

    bool rodando = true;
    bool redesenhar = true;

    al_start_timer(timer);

    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        switch (ev.type) {

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            rodando = false;
            break;

        case ALLEGRO_EVENT_KEY_DOWN: {
            int tecla = ev.keyboard.keycode;

            if (tecla >= 0 && tecla < ALLEGRO_KEY_MAX) {
                teclas[tecla] = true;
            }

            /* ESC: voltar ao menu ou sair */
            if (tecla == ALLEGRO_KEY_ESCAPE) {
                if (estado == JOGANDO) {
                    estado = MENU_INICIAL;
                }
                else {
                    rodando = false;
                }
            }

            /* MENU: iniciar com ENTER ou ESPACO */
            if (estado == MENU_INICIAL &&
                (tecla == ALLEGRO_KEY_ENTER ||
                    tecla == ALLEGRO_KEY_SPACE)) {

                estado = JOGANDO;
            }
            /* Reiniciar depois da vitoria ou derrota */
            else if ((estado == FIM_DE_JOGO ||
                estado == VITORIA) &&
                tecla == ALLEGRO_KEY_SPACE) {

                jogador_x = LARGURA_TELA / 2.0f;
                jogador_y = ALTURA_TELA / 2.0f;

                vida_jogador = VIDA_MAX_JOGADOR;
                vida_inimigo = VIDA_MAX_INIMIGO;

                inimigo_x = 600.0f;
                inimigo_y = 150.0f;

                tiro_inimigo_ativo = false;
                tiro_jogador_ativo = false;

                tempo_para_atirar = 0;
                tempo_recarga = 0;

                direcao_x = 0.0f;
                direcao_y = -1.0f;

                estado = JOGANDO;
            }

            /* Disparo da jogadora durante a partida */
            if (estado == JOGANDO &&
                tecla == ALLEGRO_KEY_SPACE &&
                !tiro_jogador_ativo &&
                tempo_recarga == 0) {

                tiro_jogador_ativo = true;

                tiro_jogador_x = jogador_x;
                tiro_jogador_y = jogador_y;

                tiro_jogador_vx = direcao_x * 7.0f;
                tiro_jogador_vy = direcao_y * 7.0f;

                tempo_recarga = 12;
            }

            break;
        }

        case ALLEGRO_EVENT_KEY_UP: {
            int tecla = ev.keyboard.keycode;

            if (tecla >= 0 && tecla < ALLEGRO_KEY_MAX) {
                teclas[tecla] = false;
            }

            break;
        }

        case ALLEGRO_EVENT_TIMER:

            if (estado == JOGANDO) {

                /* Esquerda: A ou seta esquerda */
                if (teclas[ALLEGRO_KEY_A] ||
                    teclas[ALLEGRO_KEY_LEFT]) {

                    jogador_x -= JOGADOR_VELOCIDADE;
                    direcao_x = -1.0f;
                    direcao_y = 0.0f;
                }

                /* Direita: D ou seta direita */
                if (teclas[ALLEGRO_KEY_D] ||
                    teclas[ALLEGRO_KEY_RIGHT]) {

                    jogador_x += JOGADOR_VELOCIDADE;
                    direcao_x = 1.0f;
                    direcao_y = 0.0f;
                }

                /* Cima: W ou seta para cima */
                if (teclas[ALLEGRO_KEY_W] ||
                    teclas[ALLEGRO_KEY_UP]) {

                    jogador_y -= JOGADOR_VELOCIDADE;
                    direcao_x = 0.0f;
                    direcao_y = -1.0f;
                }

                /* Baixo: S ou seta para baixo */
                if (teclas[ALLEGRO_KEY_S] ||
                    teclas[ALLEGRO_KEY_DOWN]) {

                    jogador_y += JOGADOR_VELOCIDADE;
                    direcao_x = 0.0f;
                    direcao_y = 1.0f;
                }

                /* Impedir que a jogadora saia da tela */
                if (jogador_x < JOGADOR_TAMANHO / 2)
                    jogador_x = JOGADOR_TAMANHO / 2;

                if (jogador_x >
                    LARGURA_TELA - JOGADOR_TAMANHO / 2)
                    jogador_x =
                    LARGURA_TELA - JOGADOR_TAMANHO / 2;

                if (jogador_y < JOGADOR_TAMANHO / 2)
                    jogador_y = JOGADOR_TAMANHO / 2;

                if (jogador_y >
                    ALTURA_TELA - JOGADOR_TAMANHO / 2)
                    jogador_y =
                    ALTURA_TELA - JOGADOR_TAMANHO / 2;

                /* Recarga do tiro */
                if (tempo_recarga > 0) {
                    tempo_recarga--;
                }

                /* Atualizar o inimigo enquanto estiver vivo */
                if (vida_inimigo > 0) {

                    atualizar_inimigo(
                        &inimigo_x, &inimigo_y,
                        jogador_x, jogador_y,
                        &tempo_para_atirar,
                        &tiro_inimigo_ativo,
                        &tiro_inimigo_x, &tiro_inimigo_y,
                        &tiro_inimigo_vx, &tiro_inimigo_vy
                    );

                    atualizar_tiro(
                        &tiro_inimigo_ativo,
                        &tiro_inimigo_x, &tiro_inimigo_y,
                        tiro_inimigo_vx, tiro_inimigo_vy,
                        LARGURA_TELA, ALTURA_TELA
                    );

                    /* Verificar colisao com a jogadora */
                    if (verificar_colisao(
                        tiro_inimigo_ativo,
                        tiro_inimigo_x, tiro_inimigo_y,
                        jogador_x, jogador_y,
                        JOGADOR_TAMANHO)) {

                        vida_jogador -= 25;
                        tiro_inimigo_ativo = false;

                        if (vida_jogador <= 0) {
                            vida_jogador = 0;
                            estado = FIM_DE_JOGO;
                        }
                    }
                }

                /* Atualizar tiro da jogadora */
                atualizar_tiro(
                    &tiro_jogador_ativo,
                    &tiro_jogador_x, &tiro_jogador_y,
                    tiro_jogador_vx, tiro_jogador_vy,
                    LARGURA_TELA, ALTURA_TELA
                );

                /* Verificar colisao com o inimigo */
                if (vida_inimigo > 0 &&
                    verificar_colisao_inimigo(
                        tiro_jogador_ativo,
                        tiro_jogador_x, tiro_jogador_y,
                        inimigo_x, inimigo_y,
                        INIMIGO_TAMANHO)) {

                    vida_inimigo -= 25;
                    tiro_jogador_ativo = false;

                    if (vida_inimigo <= 0) {
                        vida_inimigo = 0;
                        estado = VITORIA;
                    }
                }
            }

            redesenhar = true;
            break;
        }

        if (redesenhar && al_is_event_queue_empty(fila)) {

            redesenhar = false;

            al_clear_to_color(al_map_rgb(15, 15, 25));

            /* MENU INICIAL */
            if (estado == MENU_INICIAL) {

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 200,
                    ALLEGRO_ALIGN_CENTER, "JOGO ALLEGRO"
                );

                al_draw_text(
                    fonte, al_map_rgb(0, 255, 100),
                    LARGURA_TELA / 2, 280,
                    ALLEGRO_ALIGN_CENTER,
                    "ENTER ou ESPACO - Iniciar jogo"
                );

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200),
                    LARGURA_TELA / 2, 320,
                    ALLEGRO_ALIGN_CENTER,
                    "WASD ou SETAS - Mover"
                );

                al_draw_text(
                    fonte, al_map_rgb(0, 150, 255),
                    LARGURA_TELA / 2, 345,
                    ALLEGRO_ALIGN_CENTER, "ESPACO - Atirar"
                );

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200),
                    LARGURA_TELA / 2, 380,
                    ALLEGRO_ALIGN_CENTER, "ESC - Sair"
                );
            }

            /* JOGO */
            else if (estado == JOGANDO) {

                /* Jogadora */
                al_draw_filled_rectangle(
                    jogador_x - JOGADOR_TAMANHO / 2,
                    jogador_y - JOGADOR_TAMANHO / 2,
                    jogador_x + JOGADOR_TAMANHO / 2,
                    jogador_y + JOGADOR_TAMANHO / 2,
                    al_map_rgb(255, 255, 255)
                );

                /* Fundo da barra de vida da jogadora */
                al_draw_filled_rectangle(
                    jogador_x - JOGADOR_TAMANHO / 2,
                    jogador_y - JOGADOR_TAMANHO / 2 - 10,
                    jogador_x + JOGADOR_TAMANHO / 2,
                    jogador_y - JOGADOR_TAMANHO / 2 - 5,
                    al_map_rgb(100, 0, 0)
                );

                /* Vida atual da jogadora */
                al_draw_filled_rectangle(
                    jogador_x - JOGADOR_TAMANHO / 2,
                    jogador_y - JOGADOR_TAMANHO / 2 - 10,
                    jogador_x - JOGADOR_TAMANHO / 2 +
                    JOGADOR_TAMANHO * (vida_jogador / 100.0f),
                    jogador_y - JOGADOR_TAMANHO / 2 - 5,
                    al_map_rgb(0, 255, 0)
                );

                /* Inimigo e sua barra de vida */
                if (vida_inimigo > 0) {

                    desenhar_inimigo(
                        inimigo_x, inimigo_y,
                        INIMIGO_TAMANHO
                    );

                    float barra_x =
                        inimigo_x - INIMIGO_TAMANHO / 2;

                    float barra_y =
                        inimigo_y - INIMIGO_TAMANHO / 2 - 10;

                    al_draw_filled_rectangle(
                        barra_x, barra_y,
                        barra_x + INIMIGO_TAMANHO,
                        barra_y + 5,
                        al_map_rgb(100, 0, 0)
                    );

                    al_draw_filled_rectangle(
                        barra_x, barra_y,
                        barra_x + INIMIGO_TAMANHO *
                        (vida_inimigo / 100.0f),
                        barra_y + 5,
                        al_map_rgb(255, 0, 0)
                    );

                    /* Tiro vermelho do inimigo */
                    desenhar_tiro(
                        tiro_inimigo_ativo,
                        tiro_inimigo_x, tiro_inimigo_y
                    );
                }

                /* Tiro azul da jogadora */
                if (tiro_jogador_ativo) {

                    al_draw_filled_circle(
                        tiro_jogador_x, tiro_jogador_y,
                        5.0f, al_map_rgb(0, 150, 255)
                    );
                }

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    10, 10, 0,
                    "WASD ou SETAS: mover | ESPACO: atirar | ESC: menu"
                );
            }

            /* TELA DE DERROTA */
            else if (estado == FIM_DE_JOGO) {

                al_draw_text(
                    fonte, al_map_rgb(255, 50, 50),
                    LARGURA_TELA / 2, 230,
                    ALLEGRO_ALIGN_CENTER, "VOCE MORREU!"
                );

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 290,
                    ALLEGRO_ALIGN_CENTER,
                    "ESPACO - Tentar novamente"
                );
            }

            /* TELA DE VITORIA */
            else if (estado == VITORIA) {

                al_draw_text(
                    fonte, al_map_rgb(0, 255, 100),
                    LARGURA_TELA / 2, 230,
                    ALLEGRO_ALIGN_CENTER, "VOCE VENCEU!"
                );

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 290,
                    ALLEGRO_ALIGN_CENTER,
                    "ESPACO - Jogar novamente"
                );
            }

            al_flip_display();
        }
    }

    /* Liberar recursos */
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    al_destroy_display(janela);

    return 0;
}
