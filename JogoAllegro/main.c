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
    if (!al_init() || !al_install_keyboard() || !al_init_primitives_addon() || !al_init_font_addon()) {
        fprintf(stderr, "Falha ao iniciar o Allegro ou seus add-ons.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    ALLEGRO_DISPLAY* janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_FONT* fonte = al_create_builtin_font();

    if (!janela || !timer || !fila || !fonte) {
        if (fonte) al_destroy_font(fonte);
        if (fila) al_destroy_event_queue(fila);
        if (timer) al_destroy_timer(timer);
        if (janela) al_destroy_display(janela);
        return 1;
    }

    al_set_window_title(janela, "JogoAllegro");
    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_keyboard_event_source());

    EstadoJogo estado = MENU_INICIAL;
    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true, redesenhar = true;

    /* Inicializando as Structs */
    Jogador jogador = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f, 40.0f, 4.0f, VIDA_MAX_JOGADOR, 0, 0.0f, -1.0f, 0 };
    Inimigo inimigo = { 600.0f, 150.0f, 30.0f, 0.5f, VIDA_MAX_INIMIGO, 0 };
    Projetil tiro_inimigo = { 0, 0, 0, 0, 5.0f, false };
    Projetil tiro_jogador = { 0, 0, 0, 0, 5.0f, false };

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
            if (tecla >= 0 && tecla < ALLEGRO_KEY_MAX) teclas[tecla] = true;

            if (tecla == ALLEGRO_KEY_ESCAPE) {
                if (estado == JOGANDO) estado = MENU_INICIAL;
                else rodando = false;
            }

            if (estado == MENU_INICIAL && tecla == ALLEGRO_KEY_ENTER) {
                estado = JOGANDO;
            }
            else if ((estado == FIM_DE_JOGO || estado == VITORIA) && tecla == ALLEGRO_KEY_ENTER) {
                /* Resetando os dados das structs para recomecar */
                jogador.x = LARGURA_TELA / 2.0f;
                jogador.y = ALTURA_TELA / 2.0f;
                jogador.vida = VIDA_MAX_JOGADOR;
                jogador.invulnerabilidade = 0;
                jogador.tempo_recarga = 0;
                jogador.direcao_x = 0.0f;
                jogador.direcao_y = -1.0f;

                inimigo.x = 600.0f;
                inimigo.y = 150.0f;
                inimigo.vida = VIDA_MAX_INIMIGO;
                inimigo.timer_tiro = 0;

                tiro_inimigo.ativo = false;
                tiro_jogador.ativo = false;

                estado = JOGANDO;
            }

            if (estado == JOGANDO && tecla == ALLEGRO_KEY_SPACE && !tiro_jogador.ativo && jogador.tempo_recarga == 0) {
                tiro_jogador.ativo = true;
                tiro_jogador.x = jogador.x;
                tiro_jogador.y = jogador.y;
                tiro_jogador.vx = jogador.direcao_x * 7.0f;
                tiro_jogador.vy = jogador.direcao_y * 7.0f;
                jogador.tempo_recarga = 12;
            }
            break;
        }

        case ALLEGRO_EVENT_KEY_UP: {
            int tecla = ev.keyboard.keycode;
            if (tecla >= 0 && tecla < ALLEGRO_KEY_MAX) teclas[tecla] = false;
            break;
        }

        case ALLEGRO_EVENT_TIMER:
            if (estado == JOGANDO) {
                if (teclas[ALLEGRO_KEY_A] || teclas[ALLEGRO_KEY_LEFT]) {
                    jogador.x -= jogador.vel; jogador.direcao_x = -1.0f; jogador.direcao_y = 0.0f;
                }
                if (teclas[ALLEGRO_KEY_D] || teclas[ALLEGRO_KEY_RIGHT]) {
                    jogador.x += jogador.vel; jogador.direcao_x = 1.0f; jogador.direcao_y = 0.0f;
                }
                if (teclas[ALLEGRO_KEY_W] || teclas[ALLEGRO_KEY_UP]) {
                    jogador.y -= jogador.vel; jogador.direcao_x = 0.0f; jogador.direcao_y = -1.0f;
                }
                if (teclas[ALLEGRO_KEY_S] || teclas[ALLEGRO_KEY_DOWN]) {
                    jogador.y += jogador.vel; jogador.direcao_x = 0.0f; jogador.direcao_y = 1.0f;
                }

                if (jogador.x < jogador.tamanho / 2) jogador.x = jogador.tamanho / 2;
                if (jogador.x > LARGURA_TELA - jogador.tamanho / 2) jogador.x = LARGURA_TELA - jogador.tamanho / 2;
                if (jogador.y < jogador.tamanho / 2) jogador.y = jogador.tamanho / 2;
                if (jogador.y > ALTURA_TELA - jogador.tamanho / 2) jogador.y = ALTURA_TELA - jogador.tamanho / 2;

                if (jogador.tempo_recarga > 0) jogador.tempo_recarga--;
                if (jogador.invulnerabilidade > 0) jogador.invulnerabilidade--;

                /* Colisao de corpo a corpo */
                if (inimigo.vida > 0 && jogador.invulnerabilidade == 0) {
                    float dx = jogador.x - inimigo.x;
                    float dy = jogador.y - inimigo.y;
                    float limite = (jogador.tamanho + inimigo.tamanho) / 2.0f;

                    if (dx < limite && dx > -limite && dy < limite && dy > -limite) {
                        jogador.vida -= 25;
                        jogador.invulnerabilidade = 90;
                        if (jogador.vida <= 0) { jogador.vida = 0; estado = FIM_DE_JOGO; }
                    }
                }

                /* Acoes do Inimigo */
                if (inimigo.vida > 0) {
                    atualizar_inimigo(&inimigo, &jogador, &tiro_inimigo);
                    atualizar_tiro(&tiro_inimigo, LARGURA_TELA, ALTURA_TELA);

                    if (verificar_colisao(&tiro_inimigo, jogador.x, jogador.y, jogador.tamanho)) {
                        jogador.vida -= 25;
                        tiro_inimigo.ativo = false;
                        if (jogador.vida <= 0) { jogador.vida = 0; estado = FIM_DE_JOGO; }
                    }
                }

                /* Acoes do Tiro do Jogador */
                atualizar_tiro(&tiro_jogador, LARGURA_TELA, ALTURA_TELA);

                if (inimigo.vida > 0 && verificar_colisao(&tiro_jogador, inimigo.x, inimigo.y, inimigo.tamanho)) {
                    inimigo.vida -= 25;
                    tiro_jogador.ativo = false;
                    if (inimigo.vida <= 0) { inimigo.vida = 0; estado = VITORIA; }
                }
            }
            redesenhar = true;
            break;
        }

        if (redesenhar && al_is_event_queue_empty(fila)) {
            redesenhar = false;
            al_clear_to_color(al_map_rgb(15, 15, 25));

            if (estado == MENU_INICIAL) {
                al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTER, "JOGO ALLEGRO");
                al_draw_text(fonte, al_map_rgb(0, 255, 100), LARGURA_TELA / 2, 280, ALLEGRO_ALIGN_CENTER, "ENTER - Iniciar jogo");
                al_draw_text(fonte, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 320, ALLEGRO_ALIGN_CENTER, "WASD ou SETAS - Mover");
                al_draw_text(fonte, al_map_rgb(0, 150, 255), LARGURA_TELA / 2, 345, ALLEGRO_ALIGN_CENTER, "ESPACO - Atirar");
                al_draw_text(fonte, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 380, ALLEGRO_ALIGN_CENTER, "ESC - Sair");
            }
            else if (estado == JOGANDO) {
                /* Jogador piscando se estiver invulneravel */
                if (jogador.invulnerabilidade % 10 < 5) {
                    al_draw_filled_rectangle(
                        jogador.x - jogador.tamanho / 2, jogador.y - jogador.tamanho / 2,
                        jogador.x + jogador.tamanho / 2, jogador.y + jogador.tamanho / 2,
                        al_map_rgb(255, 255, 255)
                    );
                }

                al_draw_filled_rectangle(jogador.x - jogador.tamanho / 2, jogador.y - jogador.tamanho / 2 - 10, jogador.x + jogador.tamanho / 2, jogador.y - jogador.tamanho / 2 - 5, al_map_rgb(100, 0, 0));
                al_draw_filled_rectangle(jogador.x - jogador.tamanho / 2, jogador.y - jogador.tamanho / 2 - 10, jogador.x - jogador.tamanho / 2 + jogador.tamanho * (jogador.vida / 100.0f), jogador.y - jogador.tamanho / 2 - 5, al_map_rgb(0, 255, 0));

                if (inimigo.vida > 0) {
                    desenhar_inimigo(&inimigo);
                    float barra_x = inimigo.x - inimigo.tamanho / 2;
                    float barra_y = inimigo.y - inimigo.tamanho / 2 - 10;
                    al_draw_filled_rectangle(barra_x, barra_y, barra_x + inimigo.tamanho, barra_y + 5, al_map_rgb(100, 0, 0));
                    al_draw_filled_rectangle(barra_x, barra_y, barra_x + inimigo.tamanho * (inimigo.vida / 100.0f), barra_y + 5, al_map_rgb(255, 0, 0));

                    /* Tiro inimigo em vermelho */
                    desenhar_tiro(&tiro_inimigo, 255, 0, 0);
                }

                /* Tiro jogador em azul */
                desenhar_tiro(&tiro_jogador, 0, 150, 255);

                al_draw_text(fonte, al_map_rgb(255, 255, 255), 10, 10, 0, "WASD ou SETAS: mover | ESPACO: atirar | ESC: menu");
            }
            else if (estado == FIM_DE_JOGO) {
                al_draw_text(fonte, al_map_rgb(255, 50, 50), LARGURA_TELA / 2, 230, ALLEGRO_ALIGN_CENTER, "VOCE MORREU!");
                al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 290, ALLEGRO_ALIGN_CENTER, "ENTER - Tentar novamente");
            }
            else if (estado == VITORIA) {
                al_draw_text(fonte, al_map_rgb(0, 255, 100), LARGURA_TELA / 2, 230, ALLEGRO_ALIGN_CENTER, "VOCE VENCEU!");
                al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 290, ALLEGRO_ALIGN_CENTER, "ENTER - Jogar novamente");
            }

            al_flip_display();
        }
    }

    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    al_destroy_display(janela);

    return 0;
}