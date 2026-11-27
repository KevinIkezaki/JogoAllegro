
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "perguntas.h"
#include "inimigo.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60.0f

#define VIDA_MAX_JOGADOR 100
#define VIDA_MAX_INIMIGO 100

/* Opcoes de melhoria */
#define MELHORIA_VIDA 1
#define MELHORIA_VELOCIDADE 2
#define MELHORIA_DANO 3

typedef enum {
    MENU_INICIAL,
    JOGANDO,
    SALA_MELHORIAS,
    PERGUNTA,
    RESULTADO_PERGUNTA,
    FIM_DE_JOGO,
    VITORIA
} EstadoJogo;

int main(void) {

    if (!al_init() || !al_install_keyboard() ||
        !al_init_primitives_addon() || !al_init_font_addon()) {

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

    al_set_window_title(janela, "White Cell Warrior");

    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_keyboard_event_source());

    EstadoJogo estado = MENU_INICIAL;

    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true;
    bool redesenhar = true;

    /* Inicializando jogador */
    Jogador jogador = {
        LARGURA_TELA / 2.0f,
        ALTURA_TELA / 2.0f,
        40.0f,
        4.0f,
        VIDA_MAX_JOGADOR,
        0,
        0.0f,
        -1.0f,
        0
    };

    /* Inicializando inimigo */
    Inimigo inimigo = {
        600.0f, 150.0f, 30.0f, 0.5f, VIDA_MAX_INIMIGO, 0
    };

    /* Inicializando ameba */
    Ameba ameba = {
        200.0f, 150.0f, 35.0f, 0.3f, 150
    };

    Projetil tiro_inimigo = { 0, 0, 0, 0, 5.0f, false };
    Projetil tiro_jogador = { 0, 0, 0, 0, 5.0f, false };

    /* Variaveis das melhorias */
    int vidaMaximaJogador = VIDA_MAX_JOGADOR;
    int danoJogador = 25;

    /* Controle das salas e fases */
    int salaAtual = 1;
    int faseAtual = 1;

    /* Controle das melhorias e perguntas */
    int melhoriaEscolhida = 0;
    int indicePerguntaAtual = -1;

    bool tentativaMelhoriaUsada = false;
    bool respostaCorreta = false;

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

            /* ESC */
            if (tecla == ALLEGRO_KEY_ESCAPE) {
                if (estado == JOGANDO) {
                    estado = MENU_INICIAL;
                }
                else {
                    rodando = false;
                }
            }

            /* Iniciar jogo */
            if (estado == MENU_INICIAL && tecla == ALLEGRO_KEY_ENTER) {
                estado = JOGANDO;
            }

            /* Reiniciar jogo depois de morrer ou vencer */
            else if ((estado == FIM_DE_JOGO || estado == VITORIA) &&
                tecla == ALLEGRO_KEY_ENTER) {

                jogador.x = LARGURA_TELA / 2.0f;
                jogador.y = ALTURA_TELA / 2.0f;
                jogador.vida = VIDA_MAX_JOGADOR;
                jogador.vel = 4.0f;
                jogador.invulnerabilidade = 0;
                jogador.tempo_recarga = 0;
                jogador.direcao_x = 0.0f;
                jogador.direcao_y = -1.0f;

                inimigo.x = 600.0f;
                inimigo.y = 150.0f;
                inimigo.vida = VIDA_MAX_INIMIGO;
                inimigo.timer_tiro = 0;

                ameba.x = 200.0f;
                ameba.y = 150.0f;
                ameba.vida = 150;

                tiro_inimigo.ativo = false;
                tiro_jogador.ativo = false;

                vidaMaximaJogador = VIDA_MAX_JOGADOR;
                danoJogador = 25;

                salaAtual = 1;
                faseAtual = 1;

                melhoriaEscolhida = 0;
                indicePerguntaAtual = -1;

                tentativaMelhoriaUsada = false;
                respostaCorreta = false;

                estado = JOGANDO;
            }

            /* Escolher melhoria */
            else if (estado == SALA_MELHORIAS &&
                !tentativaMelhoriaUsada) {

                if (tecla == ALLEGRO_KEY_1) {
                    melhoriaEscolhida = MELHORIA_VIDA;
                }
                else if (tecla == ALLEGRO_KEY_2) {
                    melhoriaEscolhida = MELHORIA_VELOCIDADE;
                }
                else if (tecla == ALLEGRO_KEY_3) {
                    melhoriaEscolhida = MELHORIA_DANO;
                }

                if (melhoriaEscolhida != 0) {

                    int indices[1];

                    int quantidade = sortearPerguntasDaFase(
                        faseAtual,
                        indices,
                        1
                    );

                    if (quantidade > 0) {
                        indicePerguntaAtual = indices[0];
                        respostaCorreta = false;
                        estado = PERGUNTA;
                    }
                    else {
                        printf("Nao foi possivel encontrar uma pergunta!\n");
                        melhoriaEscolhida = 0;
                    }
                }
            }

            /* Responder pergunta */
            else if (estado == PERGUNTA) {

                int resposta = -1;

                if (tecla == ALLEGRO_KEY_1) {
                    resposta = 0;
                }
                else if (tecla == ALLEGRO_KEY_2) {
                    resposta = 1;
                }
                else if (tecla == ALLEGRO_KEY_3) {
                    resposta = 2;
                }
                else if (tecla == ALLEGRO_KEY_4) {
                    resposta = 3;
                }

                if (resposta != -1 && indicePerguntaAtual >= 0) {

                    tentativaMelhoriaUsada = true;

                    respostaCorreta = verificarResposta(
                        indicePerguntaAtual,
                        resposta
                    );

                    if (respostaCorreta) {

                        /* Melhoria de vida */
                        if (melhoriaEscolhida == MELHORIA_VIDA) {

                            vidaMaximaJogador += 25;
                            jogador.vida += 25;

                            if (jogador.vida > vidaMaximaJogador) {
                                jogador.vida = vidaMaximaJogador;
                            }
                        }

                        /* Melhoria de velocidade */
                        else if (melhoriaEscolhida == MELHORIA_VELOCIDADE) {
                            jogador.vel += 1.0f;
                        }

                        /* Melhoria de dano */
                        else if (melhoriaEscolhida == MELHORIA_DANO) {
                            danoJogador += 10;
                        }
                    }

                    estado = RESULTADO_PERGUNTA;
                }
            }

            /* Continuar depois do resultado */
            else if (estado == RESULTADO_PERGUNTA &&
                tecla == ALLEGRO_KEY_ENTER) {

                /* Avancar para a proxima sala */
                salaAtual++;

                /* Avancar a fase, ate a ultima fase do banco */
                if (faseAtual < TOTAL_FASES) {
                    faseAtual++;
                }

                /* Preparar inimigo da nova sala */
                inimigo.x = 600.0f;
                inimigo.y = 150.0f;
                inimigo.vida = VIDA_MAX_INIMIGO;
                inimigo.timer_tiro = 0;

                /* Preparar ameba da nova sala */
                ameba.x = 200.0f;
                ameba.y = 150.0f;
                ameba.vida = 150;

                /* Reposicionar jogador */
                jogador.x = LARGURA_TELA / 2.0f;
                jogador.y = ALTURA_TELA / 2.0f;
                jogador.invulnerabilidade = 0;
                jogador.tempo_recarga = 0;

                /* Desativar tiros antigos */
                tiro_inimigo.ativo = false;
                tiro_jogador.ativo = false;

                melhoriaEscolhida = 0;
                indicePerguntaAtual = -1;
                respostaCorreta = false;

                estado = JOGANDO;
            }

            /* Atirar */
            if (estado == JOGANDO &&
                tecla == ALLEGRO_KEY_SPACE &&
                !tiro_jogador.ativo &&
                jogador.tempo_recarga == 0) {

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

            if (tecla >= 0 && tecla < ALLEGRO_KEY_MAX) {
                teclas[tecla] = false;
            }

            break;
        }

        case ALLEGRO_EVENT_TIMER:

            if (estado == JOGANDO) {

                /* Movimentacao do jogador */
                if (teclas[ALLEGRO_KEY_A] || teclas[ALLEGRO_KEY_LEFT]) {
                    jogador.x -= jogador.vel;
                    jogador.direcao_x = -1.0f;
                    jogador.direcao_y = 0.0f;
                }

                if (teclas[ALLEGRO_KEY_D] || teclas[ALLEGRO_KEY_RIGHT]) {
                    jogador.x += jogador.vel;
                    jogador.direcao_x = 1.0f;
                    jogador.direcao_y = 0.0f;
                }

                if (teclas[ALLEGRO_KEY_W] || teclas[ALLEGRO_KEY_UP]) {
                    jogador.y -= jogador.vel;
                    jogador.direcao_x = 0.0f;
                    jogador.direcao_y = -1.0f;
                }

                if (teclas[ALLEGRO_KEY_S] || teclas[ALLEGRO_KEY_DOWN]) {
                    jogador.y += jogador.vel;
                    jogador.direcao_x = 0.0f;
                    jogador.direcao_y = 1.0f;
                }

                /* Limites da tela */
                if (jogador.x < jogador.tamanho / 2) {
                    jogador.x = jogador.tamanho / 2;
                }

                if (jogador.x > LARGURA_TELA - jogador.tamanho / 2) {
                    jogador.x = LARGURA_TELA - jogador.tamanho / 2;
                }

                if (jogador.y < jogador.tamanho / 2) {
                    jogador.y = jogador.tamanho / 2;
                }

                if (jogador.y > ALTURA_TELA - jogador.tamanho / 2) {
                    jogador.y = ALTURA_TELA - jogador.tamanho / 2;
                }

                if (jogador.tempo_recarga > 0) {
                    jogador.tempo_recarga--;
                }

                if (jogador.invulnerabilidade > 0) {
                    jogador.invulnerabilidade--;
                }

                /* Colisao com inimigo */
                if (inimigo.vida > 0 && jogador.invulnerabilidade == 0) {

                    float dx = jogador.x - inimigo.x;
                    float dy = jogador.y - inimigo.y;

                    float limite = (jogador.tamanho + inimigo.tamanho) / 2.0f;

                    if (dx < limite && dx > -limite &&
                        dy < limite && dy > -limite) {

                        jogador.vida -= 25;
                        jogador.invulnerabilidade = 90;

                        if (jogador.vida <= 0) {
                            jogador.vida = 0;
                            estado = FIM_DE_JOGO;
                        }
                    }
                }

                /* Colisao com a ameba */
                if (ameba.vida > 0 && jogador.invulnerabilidade == 0) {

                    float dx = jogador.x - ameba.x;
                    float dy = jogador.y - ameba.y;

                    float limite = (jogador.tamanho + ameba.tamanho) / 2.0f;

                    if (dx < limite && dx > -limite &&
                        dy < limite && dy > -limite) {

                        jogador.vida -= 25;
                        jogador.invulnerabilidade = 90;

                        if (jogador.vida <= 0) {
                            jogador.vida = 0;
                            estado = FIM_DE_JOGO;
                        }
                    }
                }

                /* Acoes do inimigo */
                if (inimigo.vida > 0) {

                    atualizar_inimigo(&inimigo, &jogador, &tiro_inimigo);

                    atualizar_tiro(
                        &tiro_inimigo,
                        LARGURA_TELA,
                        ALTURA_TELA
                    );

                    if (verificar_colisao(
                        &tiro_inimigo,
                        jogador.x,
                        jogador.y,
                        jogador.tamanho)) {

                        tiro_inimigo.ativo = false;

                        if (jogador.invulnerabilidade == 0) {

                            jogador.vida -= 25;
                            jogador.invulnerabilidade = 90;

                            if (jogador.vida <= 0) {
                                jogador.vida = 0;
                                estado = FIM_DE_JOGO;
                            }
                        }
                    }
                }

                /* Acoes da ameba */
                if (ameba.vida > 0) {
                    atualizar_ameba(&ameba, &jogador);
                }

                /* Atualizacao do tiro do jogador */
                atualizar_tiro(
                    &tiro_jogador,
                    LARGURA_TELA,
                    ALTURA_TELA
                );

                /* Tiro atingindo o inimigo */
                if (inimigo.vida > 0 &&
                    verificar_colisao(
                        &tiro_jogador,
                        inimigo.x,
                        inimigo.y,
                        inimigo.tamanho)) {

                    inimigo.vida -= danoJogador;
                    tiro_jogador.ativo = false;

                    if (inimigo.vida <= 0) {
                        inimigo.vida = 0;
                    }
                }

                /* Tiro atingindo a ameba */
                if (ameba.vida > 0 &&
                    verificar_colisao(
                        &tiro_jogador,
                        ameba.x,
                        ameba.y,
                        ameba.tamanho)) {

                    ameba.vida -= danoJogador;
                    tiro_jogador.ativo = false;

                    if (ameba.vida <= 0) {
                        ameba.vida = 0;
                    }
                }

                /* Fim do combate */
                if (inimigo.vida <= 0 && ameba.vida <= 0) {

                    estado = SALA_MELHORIAS;

                    tentativaMelhoriaUsada = false;
                    melhoriaEscolhida = 0;
                    indicePerguntaAtual = -1;
                    respostaCorreta = false;
                }
            }

            redesenhar = true;
            break;
        }

        /* Desenho da tela */
        if (redesenhar && al_is_event_queue_empty(fila)) {

            redesenhar = false;

            al_clear_to_color(al_map_rgb(15, 15, 25));

            /* MENU INICIAL */
            if (estado == MENU_INICIAL) {

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 200,
                    ALLEGRO_ALIGN_CENTER, "WHITE CELL WARRIOR"
                );

                al_draw_text(
                    fonte, al_map_rgb(0, 255, 100),
                    LARGURA_TELA / 2, 280,
                    ALLEGRO_ALIGN_CENTER, "ENTER - Iniciar jogo"
                );

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200),
                    LARGURA_TELA / 2, 320,
                    ALLEGRO_ALIGN_CENTER, "WASD ou SETAS - Mover"
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

            /* COMBATE */
            else if (estado == JOGANDO) {

                /* Jogador piscando durante invulnerabilidade */
                if (jogador.invulnerabilidade % 10 < 5) {

                    al_draw_filled_rectangle(
                        jogador.x - jogador.tamanho / 2,
                        jogador.y - jogador.tamanho / 2,
                        jogador.x + jogador.tamanho / 2,
                        jogador.y + jogador.tamanho / 2,
                        al_map_rgb(255, 255, 255)
                    );
                }

                /* Barra de vida do jogador */
                al_draw_filled_rectangle(
                    jogador.x - jogador.tamanho / 2,
                    jogador.y - jogador.tamanho / 2 - 10,
                    jogador.x + jogador.tamanho / 2,
                    jogador.y - jogador.tamanho / 2 - 5,
                    al_map_rgb(100, 0, 0)
                );

                al_draw_filled_rectangle(
                    jogador.x - jogador.tamanho / 2,
                    jogador.y - jogador.tamanho / 2 - 10,
                    jogador.x - jogador.tamanho / 2 +
                    jogador.tamanho * ((float)jogador.vida / vidaMaximaJogador),
                    jogador.y - jogador.tamanho / 2 - 5,
                    al_map_rgb(0, 255, 0)
                );

                /* Inimigo */
                if (inimigo.vida > 0) {

                    desenhar_inimigo(&inimigo);

                    float barra_x = inimigo.x - inimigo.tamanho / 2;
                    float barra_y = inimigo.y - inimigo.tamanho / 2 - 10;

                    al_draw_filled_rectangle(
                        barra_x, barra_y,
                        barra_x + inimigo.tamanho, barra_y + 5,
                        al_map_rgb(100, 0, 0)
                    );

                    al_draw_filled_rectangle(
                        barra_x, barra_y,
                        barra_x + inimigo.tamanho *
                        (inimigo.vida / 100.0f),
                        barra_y + 5,
                        al_map_rgb(255, 0, 0)
                    );

                    desenhar_tiro(&tiro_inimigo, 255, 0, 0);
                }

                /* Ameba */
                if (ameba.vida > 0) {
                    desenhar_ameba(&ameba);
                    desenhar_barra_vida_ameba(&ameba);
                }

                /* Tiro do jogador */
                desenhar_tiro(&tiro_jogador, 0, 150, 255);

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    10, 10, 0,
                    "WASD ou SETAS: mover | ESPACO: atirar | ESC: menu"
                );
            }

            /* SALA DE MELHORIAS */
            else if (estado == SALA_MELHORIAS) {

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 100,
                    ALLEGRO_ALIGN_CENTER, "SALA DE MELHORIAS"
                );

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200),
                    LARGURA_TELA / 2, 160,
                    ALLEGRO_ALIGN_CENTER, "Escolha uma melhoria:"
                );

                al_draw_text(
                    fonte, al_map_rgb(100, 255, 100),
                    LARGURA_TELA / 2, 240,
                    ALLEGRO_ALIGN_CENTER,
                    "1 - Vida extra (+25 de vida)"
                );

                al_draw_text(
                    fonte, al_map_rgb(100, 200, 255),
                    LARGURA_TELA / 2, 290,
                    ALLEGRO_ALIGN_CENTER,
                    "2 - Velocidade (+1)"
                );

                al_draw_text(
                    fonte, al_map_rgb(255, 200, 100),
                    LARGURA_TELA / 2, 340,
                    ALLEGRO_ALIGN_CENTER,
                    "3 - Dano aumentado (+10)"
                );

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 430,
                    ALLEGRO_ALIGN_CENTER,
                    "Pressione 1, 2 ou 3 para escolher"
                );
            }

            /* PERGUNTA */
            else if (estado == PERGUNTA) {

                al_draw_text(
                    fonte,
                    al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2,
                    80,
                    ALLEGRO_ALIGN_CENTER,
                    "RESPONDA PARA CONQUISTAR A MELHORIA!"
                );

                if (indicePerguntaAtual >= 0) {

                    const Pergunta* pergunta =
                        &bancoPerguntas[indicePerguntaAtual];

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 255, 255),
                        LARGURA_TELA / 2,
                        160,
                        ALLEGRO_ALIGN_CENTER,
                        pergunta->enunciado
                    );

                    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {

                        char alternativa[300];

                        snprintf(
                            alternativa,
                            sizeof(alternativa),
                            "%d - %s",
                            i + 1,
                            pergunta->alternativas[i]
                        );

                        al_draw_text(
                            fonte,
                            al_map_rgb(200, 220, 255),
                            LARGURA_TELA / 2,
                            250 + (i * 55),
                            ALLEGRO_ALIGN_CENTER,
                            alternativa
                        );
                    }
                }

                al_draw_text(
                    fonte,
                    al_map_rgb(255, 200, 100),
                    LARGURA_TELA / 2,
                    500,
                    ALLEGRO_ALIGN_CENTER,
                    "Pressione 1, 2, 3 ou 4 para responder"
                );
            }

            /* RESULTADO DA PERGUNTA */
            else if (estado == RESULTADO_PERGUNTA) {

                if (respostaCorreta) {

                    al_draw_text(
                        fonte,
                        al_map_rgb(0, 255, 100),
                        LARGURA_TELA / 2,
                        160,
                        ALLEGRO_ALIGN_CENTER,
                        "RESPOSTA CORRETA!"
                    );

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 255, 255),
                        LARGURA_TELA / 2,
                        220,
                        ALLEGRO_ALIGN_CENTER,
                        "Voce conquistou a melhoria!"
                    );
                }
                else {

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 80, 80),
                        LARGURA_TELA / 2,
                        160,
                        ALLEGRO_ALIGN_CENTER,
                        "RESPOSTA INCORRETA!"
                    );

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 255, 255),
                        LARGURA_TELA / 2,
                        220,
                        ALLEGRO_ALIGN_CENTER,
                        "Voce perdeu a oportunidade de melhoria."
                    );
                }

                if (indicePerguntaAtual >= 0) {

                    al_draw_text(
                        fonte,
                        al_map_rgb(200, 220, 255),
                        LARGURA_TELA / 2,
                        310,
                        ALLEGRO_ALIGN_CENTER,
                        bancoPerguntas[indicePerguntaAtual].explicacao
                    );
                }

                al_draw_text(
                    fonte,
                    al_map_rgb(255, 200, 100),
                    LARGURA_TELA / 2,
                    450,
                    ALLEGRO_ALIGN_CENTER,
                    "ENTER - Ir para o proximo combate"
                );
            }

            /* FIM DE JOGO */
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
                    "ENTER - Tentar novamente"
                );
            }

            /* VITORIA */
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
                    "ENTER - Jogar novamente"
                );
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
 