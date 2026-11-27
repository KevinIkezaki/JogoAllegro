#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "perguntas.h"
#include "inimigo.h"
#include "boss.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60.0f

#define VIDA_MAX_JOGADOR 100
#define VIDA_MAX_INIMIGO 100
#define VIDA_MAX_AMEBA 150

/* Velocidades de movimento (pixels por frame, a 60 FPS) */
#define VEL_JOGADOR 2.8f
#define VEL_INIMIGO 0.1f
#define VEL_AMEBA   1.5f

/* Configuracao das waves, tiros e salas */
#define MAX_INIMIGOS 8
#define MAX_TIROS_JOGADOR 20
#define CADENCIA_TIRO_JOGADOR 60 /* Frames entre cada tiro (menor = mais rapido) */
#define CADENCIA_MINIMA_JOGADOR 14
#define TOTAL_SALAS_COMBATE 5
#define SALA_BOSS 6
#define COMECAR_NO_BOSS 0 /* 1 = spawna direto no Boss | 0 = jogo normal (Sala 1) */

/* Balanceamento dos upgrades - Salas 1 a 4 */
#define BONUS_VELOCIDADE    0.3f
#define BONUS_CADENCIA_TIRO 8
#define BONUS_VIDA          25

/* Balanceamento reforcado - Ultima sala de upgrade (Sala 5, antes do Boss) */
#define BONUS_VELOCIDADE_FINAL    0.6f
#define BONUS_CADENCIA_TIRO_FINAL 14
#define BONUS_VIDA_FINAL          50

/* Identificadores dos upgrades */
#define UPGRADE_VELOCIDADE 1
#define UPGRADE_CADENCIA   2
#define UPGRADE_VIDA       3

/* Paredes, portas e corredor de upgrade */
#define ESPESSURA_PAREDE 30.0f
#define PORTA_Y_MIN 240.0f
#define PORTA_Y_MAX 360.0f
#define CORREDOR_Y_MIN 200.0f
#define CORREDOR_Y_MAX 400.0f

/* Area do botao PLAY no menu inicial */
#define BTN_PLAY_X1 300.0f
#define BTN_PLAY_Y1 275.0f
#define BTN_PLAY_X2 500.0f
#define BTN_PLAY_Y2 335.0f

/* Areas dos 3 cards de upgrade */
#define CARD_UPG_Y1 185.0f
#define CARD_UPG_Y2 425.0f
#define CARD_UPG_LARGURA 200.0f
#define CARD_UPG_ESPACO 20.0f
#define CARD_UPG_X_INICIAL 80.0f

/* Areas dos 4 botoes de alternativas na tela de pergunta */
#define BTN_ALT_X1 55.0f
#define BTN_ALT_X2 745.0f
#define BTN_ALT_Y_INICIAL 200.0f
#define BTN_ALT_ALTURA 70.0f
#define BTN_ALT_ESPACO 12.0f

/* Area do botao CONTINUAR na tela de resultado */
#define BTN_CONT_X1 290.0f
#define BTN_CONT_Y1 435.0f
#define BTN_CONT_X2 510.0f
#define BTN_CONT_Y2 487.0f

typedef enum {
    MENU_INICIAL,
    TUTORIAL,
    JOGANDO,
    SALA_UPGRADE,
    ESCOLHER_UPGRADE,
    PERGUNTA,
    RESULTADO_PERGUNTA,
    FIM_DE_JOGO,
    VITORIA
} EstadoJogo;

/* Retorna o valor de bonus conforme a sala (ultima sala concede bonus maior) */
float obter_bonus_velocidade(int salaAtual) {
    return (salaAtual == TOTAL_SALAS_COMBATE) ? BONUS_VELOCIDADE_FINAL : BONUS_VELOCIDADE;
}

int obter_bonus_cadencia(int salaAtual) {
    return (salaAtual == TOTAL_SALAS_COMBATE) ? BONUS_CADENCIA_TIRO_FINAL : BONUS_CADENCIA_TIRO;
}

int obter_bonus_vida(int salaAtual) {
    return (salaAtual == TOTAL_SALAS_COMBATE) ? BONUS_VIDA_FINAL : BONUS_VIDA;
}

/* Verifica se um ponto (mouse) esta dentro de um retangulo */
bool ponto_no_retangulo(float px, float py, float x1, float y1, float x2, float y2) {
    return px >= x1 && px <= x2 && py >= y1 && py <= y2;
}

/* Desenha texto com quebra de linha automatica (compativel com UTF-8) */
void desenhar_texto_quebrado(
    ALLEGRO_FONT* fonte,
    ALLEGRO_COLOR cor,
    float x,
    float y,
    float largura_max,
    float altura_linha,
    int alinhamento,
    const char* texto
) {
    char linha[512];
    char teste[512];
    char palavra[128];
    int len_linha = 0;
    linha[0] = '\0';

    const char* p = texto;
    while (*p != '\0') {
        while (*p == ' ') p++;
        if (*p == '\0') break;

        int len_palavra = 0;
        while (*p != '\0' && *p != ' ' && len_palavra < (int)sizeof(palavra) - 1) {
            palavra[len_palavra++] = *p++;
        }
        palavra[len_palavra] = '\0';

        if (len_linha == 0) {
            snprintf(teste, sizeof(teste), "%s", palavra);
        }
        else {
            snprintf(teste, sizeof(teste), "%s %s", linha, palavra);
        }

        if (al_get_text_width(fonte, teste) <= largura_max || len_linha == 0) {
            len_linha = snprintf(linha, sizeof(linha), "%s", teste);
        }
        else {
            al_draw_text(fonte, cor, x, y, alinhamento, linha);
            y += altura_linha;
            len_linha = snprintf(linha, sizeof(linha), "%s", palavra);
        }
    }

    if (len_linha > 0) {
        al_draw_text(fonte, cor, x, y, alinhamento, linha);
    }
}

/* Desativa todos os tiros ativos do jogador */
void limpar_tiros_jogador(Projetil tiros[]) {
    for (int i = 0; i < MAX_TIROS_JOGADOR; i++) {
        tiros[i].ativo = false;
        tiros[i].raio = 5.0f;
        tiros[i].x = 0.0f;
        tiros[i].y = 0.0f;
        tiros[i].vx = 0.0f;
        tiros[i].vy = 0.0f;
    }
}

/* Gera uma posicao dentro da sala mantendo distancia segura do jogador */
void sortear_posicao_segura(float* x, float* y, float jogador_x, float jogador_y) {
    float min_x = ESPESSURA_PAREDE + 50.0f;
    float max_x = LARGURA_TELA - ESPESSURA_PAREDE - 50.0f;
    float min_y = ESPESSURA_PAREDE + 50.0f;
    float max_y = ALTURA_TELA - ESPESSURA_PAREDE - 50.0f;

    for (int tentativa = 0; tentativa < 50; tentativa++) {
        float cand_x = min_x + (float)(rand() % (int)(max_x - min_x));
        float cand_y = min_y + (float)(rand() % (int)(max_y - min_y));

        float dx = cand_x - jogador_x;
        float dy = cand_y - jogador_y;

        if ((dx * dx + dy * dy) >= (180.0f * 180.0f)) {
            *x = cand_x;
            *y = cand_y;
            return;
        }
    }

    *x = LARGURA_TELA - 120.0f;
    *y = 120.0f;
}

/* Prepara a wave de cada uma das 5 salas de combate (3, 4, 5, 6 e 7 inimigos) */
void iniciar_sala_combate(
    int sala,
    Jogador* jogador,
    Inimigo inimigos[],
    Projetil tiros_inimigos[],
    int* qtd_inimigos,
    Ameba amebas[],
    int* qtd_amebas,
    Projetil tiros_jogador[],
    Boss* boss,
    bool* portaAberta
) {
    *portaAberta = false;
    boss->ativo = false;
    limpar_tiros_jogador(tiros_jogador);
    jogador->tempo_recarga = 0;

    if (sala == 1) {
        jogador->x = LARGURA_TELA / 2.0f;
        jogador->y = ALTURA_TELA / 2.0f;
    }
    else {
        jogador->x = ESPESSURA_PAREDE + jogador->tamanho;
        jogador->y = ALTURA_TELA / 2.0f;
    }

    if (sala == 1) {
        *qtd_inimigos = 1;
        *qtd_amebas = 1; 
    }
    else if (sala == 2) {
        *qtd_inimigos = 1;
        *qtd_amebas = 2; 
    }
    else if (sala == 3) {
        *qtd_inimigos = 2;
        *qtd_amebas = 2; 
    }
    else if (sala == 4) {
        *qtd_inimigos = 2;
        *qtd_amebas = 3; 
    }
    else {
        *qtd_inimigos = 3;
        *qtd_amebas = 3; 
    }

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        tiros_inimigos[i].ativo = false;
        tiros_inimigos[i].raio = 5.0f;
        tiros_inimigos[i].x = 0.0f;
        tiros_inimigos[i].y = 0.0f;
        tiros_inimigos[i].vx = 0.0f;
        tiros_inimigos[i].vy = 0.0f;
    }

    for (int i = 0; i < *qtd_inimigos; i++) {
        float x, y;
        sortear_posicao_segura(&x, &y, jogador->x, jogador->y);

        inimigos[i].x = x;
        inimigos[i].y = y;
        inimigos[i].tamanho = 30.0f;
        inimigos[i].vel = VEL_INIMIGO;
        inimigos[i].vida = VIDA_MAX_INIMIGO;
        inimigos[i].timer_tiro = i * 15;
    }

    for (int i = 0; i < *qtd_amebas; i++) {
        float x, y;
        sortear_posicao_segura(&x, &y, jogador->x, jogador->y);

        amebas[i].x = x;
        amebas[i].y = y;
        amebas[i].tamanho = 35.0f;
        amebas[i].vel = VEL_AMEBA;
        amebas[i].vida = VIDA_MAX_AMEBA;
    }
}

/* Prepara a sala da Boss Fight */
void iniciar_sala_boss(
    Jogador* jogador,
    Projetil tiros_inimigos[],
    int* qtd_inimigos,
    int* qtd_amebas,
    Projetil tiros_jogador[],
    Boss* boss,
    bool* portaAberta
) {
    *portaAberta = false;
    limpar_tiros_jogador(tiros_jogador);
    jogador->tempo_recarga = 0;

    jogador->x = ESPESSURA_PAREDE + jogador->tamanho;
    jogador->y = ALTURA_TELA / 2.0f;

    *qtd_inimigos = 0;
    *qtd_amebas = 0;

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        tiros_inimigos[i].ativo = false;
        tiros_inimigos[i].raio = 5.0f;
    }

    inicializar_boss(boss, LARGURA_TELA, ALTURA_TELA, ESPESSURA_PAREDE);
}

/* Reinicia toda a progressao do jogo */
void reiniciar_jogo(
    Jogador* jogador,
    int* vidaMaximaJogador,
    int* danoJogador,
    int* cadenciaTiroJogador,
    int* salaAtual,
    Inimigo inimigos[],
    Projetil tiros_inimigos[],
    int* qtd_inimigos,
    Ameba amebas[],
    int* qtd_amebas,
    Projetil tiros_jogador[],
    Boss* boss,
    bool* portaAberta
) {
    jogador->tamanho = 40.0f;
    jogador->vel = VEL_JOGADOR;
    jogador->vida = VIDA_MAX_JOGADOR;
    jogador->invulnerabilidade = 0;
    jogador->direcao_x = 1.0f;
    jogador->direcao_y = 0.0f;
    jogador->tempo_recarga = 0;

    *vidaMaximaJogador = VIDA_MAX_JOGADOR;
    *danoJogador = 25;
    *cadenciaTiroJogador = CADENCIA_TIRO_JOGADOR;

#if COMECAR_NO_BOSS
    * salaAtual = SALA_BOSS;

    /* Opcional: descomente abaixo para testar o boss com atributos ja upados */
    /* jogador->vel = 3.7f; */
    /* *cadenciaTiroJogador = 36; */
    /* *vidaMaximaJogador = 150; */
    /* jogador->vida = *vidaMaximaJogador; */

    iniciar_sala_boss(
        jogador,
        tiros_inimigos, qtd_inimigos,
        qtd_amebas,
        tiros_jogador, boss, portaAberta
    );
#else
    * salaAtual = 1;

    iniciar_sala_combate(
        *salaAtual, jogador,
        inimigos, tiros_inimigos, qtd_inimigos,
        amebas, qtd_amebas,
        tiros_jogador, boss, portaAberta
    );
#endif
}

/* Inicia a pergunta de multipla escolha da fase atual para o upgrade escolhido */
void selecionar_upgrade(
    int upgrade,
    int salaAtual,
    int* upgradeEscolhido,
    int* indicePerguntaAtual,
    bool* respostaCorreta,
    EstadoJogo* estado
) {
    *upgradeEscolhido = upgrade;
    int fasePergunta = salaAtual;
    if (fasePergunta < 1) fasePergunta = 1;
    if (fasePergunta > TOTAL_FASES) fasePergunta = TOTAL_FASES;

    int indices[1];
    int qtd = sortearPerguntasDaFase(fasePergunta, indices, 1);

    if (qtd > 0) {
        *indicePerguntaAtual = indices[0];
        *respostaCorreta = false;
        *estado = PERGUNTA;
    }
    else {
        *indicePerguntaAtual = -1;
        *estado = SALA_UPGRADE;
    }
}

/* Verifica a resposta escolhida e aplica o upgrade (normal ou turbinado na sala 5) */
void processar_resposta_pergunta(
    int resposta,
    int salaAtual,
    int indicePerguntaAtual,
    int upgradeEscolhido,
    Jogador* jogador,
    int* vidaMaximaJogador,
    int* cadenciaTiroJogador,
    bool* respostaCorreta,
    EstadoJogo* estado
) {
    if (indicePerguntaAtual < 0 || resposta < 0 || resposta >= TOTAL_ALTERNATIVAS) {
        return;
    }

    *respostaCorreta = (verificarResposta(indicePerguntaAtual, resposta) != 0);

    if (*respostaCorreta) {
        if (upgradeEscolhido == UPGRADE_VELOCIDADE) {
            jogador->vel += obter_bonus_velocidade(salaAtual);
        }
        else if (upgradeEscolhido == UPGRADE_CADENCIA) {
            *cadenciaTiroJogador -= obter_bonus_cadencia(salaAtual);
            if (*cadenciaTiroJogador < CADENCIA_MINIMA_JOGADOR) {
                *cadenciaTiroJogador = CADENCIA_MINIMA_JOGADOR;
            }
        }
        else if (upgradeEscolhido == UPGRADE_VIDA) {
            *vidaMaximaJogador += obter_bonus_vida(salaAtual);
            jogador->vida = *vidaMaximaJogador;
        }
    }

    *estado = RESULTADO_PERGUNTA;
}

/* Desenha o minimapa no canto superior direito (5 combates + 5 upgrades + Boss) */
void desenhar_minimapa(int salaAtual, EstadoJogo estado) {
    float x = LARGURA_TELA - 190.0f;
    float y = 8.0f;

    bool emAreaUpgrade = (estado == SALA_UPGRADE || estado == ESCOLHER_UPGRADE ||
        estado == PERGUNTA || estado == RESULTADO_PERGUNTA);

    for (int s = 1; s <= SALA_BOSS; s++) {
        bool estaNaSala = ((estado == JOGANDO || estado == TUTORIAL) && salaAtual == s);
        ALLEGRO_COLOR corSala = estaNaSala
            ? al_map_rgb(0, 220, 255)
            : (s == SALA_BOSS ? al_map_rgb(160, 40, 40) : al_map_rgb(90, 90, 110));

        al_draw_filled_rectangle(x, y, x + 18.0f, y + 14.0f, corSala);
        x += 20.0f;

        if (s < SALA_BOSS) {
            bool estaNoUpgrade = (emAreaUpgrade && salaAtual == s);
            ALLEGRO_COLOR corCorredor = estaNoUpgrade
                ? al_map_rgb(255, 220, 60)
                : al_map_rgb(60, 60, 75);

            al_draw_filled_rectangle(x, y + 4.0f, x + 10.0f, y + 10.0f, corCorredor);
            x += 12.0f;
        }
    }
}

/* Carrega fonte TTF (suporta acentos UTF-8) com fallback para a fonte padrao */
ALLEGRO_FONT* carregar_fonte_jogo(int tamanho) {
    ALLEGRO_FONT* f = al_load_ttf_font("arial.ttf", tamanho, 0);
    if (!f) f = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", tamanho, 0);
    if (!f) f = al_load_ttf_font("C:/Windows/Fonts/segoeui.ttf", tamanho, 0);
    if (!f) {
        fprintf(stderr, "Aviso: fonte TTF nao encontrada; usando fonte padrao.\n");
        f = al_create_builtin_font();
    }
    return f;
}

int main(void) {

    if (!al_init() || !al_install_keyboard() || !al_install_mouse() ||
        !al_init_primitives_addon() || !al_init_font_addon() || !al_init_ttf_addon()) {

        fprintf(stderr, "Falha ao iniciar o Allegro ou seus add-ons.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    ALLEGRO_DISPLAY* janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_FONT* fonte = carregar_fonte_jogo(15);

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
    al_register_event_source(fila, al_get_mouse_event_source());

    float alturaFonte = (float)al_get_font_line_height(fonte);

    EstadoJogo estado = MENU_INICIAL;
    EstadoJogo estadoAnterior = JOGANDO;

    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true;
    bool redesenhar = true;
    bool tutorialVisto = false;

    float mouseX = 0.0f;
    float mouseY = 0.0f;

    Jogador jogador;
    Inimigo inimigos[MAX_INIMIGOS];
    Projetil tiros_inimigos[MAX_INIMIGOS];
    Ameba amebas[MAX_INIMIGOS];
    Projetil tiros_jogador[MAX_TIROS_JOGADOR];
    Boss boss;
    boss.ativo = false;

    int qtd_inimigos = 0;
    int qtd_amebas = 0;
    int vidaMaximaJogador = VIDA_MAX_JOGADOR;
    int danoJogador = 25;
    int cadenciaTiroJogador = CADENCIA_TIRO_JOGADOR;
    int salaAtual = 1;
    bool portaAberta = false;

    /* Controle do sistema de upgrade e perguntas */
    int upgradeEscolhido = 0;
    int indicePerguntaAtual = -1;
    bool respostaCorreta = false;

    reiniciar_jogo(
        &jogador, &vidaMaximaJogador, &danoJogador, &cadenciaTiroJogador, &salaAtual,
        inimigos, tiros_inimigos, &qtd_inimigos,
        amebas, &qtd_amebas,
        tiros_jogador, &boss, &portaAberta
    );

    al_start_timer(timer);

    while (rodando) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        switch (ev.type) {

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            rodando = false;
            break;

        case ALLEGRO_EVENT_MOUSE_AXES:
            mouseX = (float)ev.mouse.x;
            mouseY = (float)ev.mouse.y;
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
            mouseX = (float)ev.mouse.x;
            mouseY = (float)ev.mouse.y;

            if (ev.mouse.button != 1) {
                break;
            }

            if (estado == MENU_INICIAL &&
                ponto_no_retangulo(mouseX, mouseY, BTN_PLAY_X1, BTN_PLAY_Y1, BTN_PLAY_X2, BTN_PLAY_Y2)) {
                if (!tutorialVisto) {
                    estado = TUTORIAL;
                }
                else {
                    estado = estadoAnterior;
                }
            }
            else if (estado == ESCOLHER_UPGRADE) {
                for (int i = 0; i < 3; i++) {
                    float cx1 = CARD_UPG_X_INICIAL + i * (CARD_UPG_LARGURA + CARD_UPG_ESPACO);
                    float cx2 = cx1 + CARD_UPG_LARGURA;

                    if (ponto_no_retangulo(mouseX, mouseY, cx1, CARD_UPG_Y1, cx2, CARD_UPG_Y2)) {
                        selecionar_upgrade(
                            i + 1, salaAtual,
                            &upgradeEscolhido, &indicePerguntaAtual,
                            &respostaCorreta, &estado
                        );
                        break;
                    }
                }
            }
            else if (estado == PERGUNTA) {
                for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
                    float ay1 = BTN_ALT_Y_INICIAL + i * (BTN_ALT_ALTURA + BTN_ALT_ESPACO);
                    float ay2 = ay1 + BTN_ALT_ALTURA;

                    if (ponto_no_retangulo(mouseX, mouseY, BTN_ALT_X1, ay1, BTN_ALT_X2, ay2)) {
                        processar_resposta_pergunta(
                            i, salaAtual, indicePerguntaAtual, upgradeEscolhido,
                            &jogador, &vidaMaximaJogador, &cadenciaTiroJogador,
                            &respostaCorreta, &estado
                        );
                        break;
                    }
                }
            }
            else if (estado == RESULTADO_PERGUNTA &&
                ponto_no_retangulo(mouseX, mouseY, BTN_CONT_X1, BTN_CONT_Y1, BTN_CONT_X2, BTN_CONT_Y2)) {
                portaAberta = true;
                estado = SALA_UPGRADE;
            }

            break;
        }

        case ALLEGRO_EVENT_KEY_DOWN: {

            int tecla = ev.keyboard.keycode;

            if (tecla >= 0 && tecla < ALLEGRO_KEY_MAX) {
                teclas[tecla] = true;
            }

            if (tecla == ALLEGRO_KEY_ESCAPE) {
                if (estado == JOGANDO || estado == SALA_UPGRADE || estado == TUTORIAL ||
                    estado == ESCOLHER_UPGRADE || estado == PERGUNTA || estado == RESULTADO_PERGUNTA) {
                    if (estado != TUTORIAL) {
                        estadoAnterior = estado;
                    }
                    estado = MENU_INICIAL;
                }
                else {
                    rodando = false;
                }
            }
            else if (estado == TUTORIAL && tecla == ALLEGRO_KEY_SPACE) {
                tutorialVisto = true;
                estado = JOGANDO;
            }
            else if (estado == ESCOLHER_UPGRADE) {
                if (tecla == ALLEGRO_KEY_1) {
                    selecionar_upgrade(UPGRADE_VELOCIDADE, salaAtual, &upgradeEscolhido, &indicePerguntaAtual, &respostaCorreta, &estado);
                }
                else if (tecla == ALLEGRO_KEY_2) {
                    selecionar_upgrade(UPGRADE_CADENCIA, salaAtual, &upgradeEscolhido, &indicePerguntaAtual, &respostaCorreta, &estado);
                }
                else if (tecla == ALLEGRO_KEY_3) {
                    selecionar_upgrade(UPGRADE_VIDA, salaAtual, &upgradeEscolhido, &indicePerguntaAtual, &respostaCorreta, &estado);
                }
            }
            else if (estado == PERGUNTA) {
                int resp = -1;
                if (tecla == ALLEGRO_KEY_1) resp = 0;
                else if (tecla == ALLEGRO_KEY_2) resp = 1;
                else if (tecla == ALLEGRO_KEY_3) resp = 2;
                else if (tecla == ALLEGRO_KEY_4) resp = 3;

                if (resp != -1) {
                    processar_resposta_pergunta(
                        resp, salaAtual, indicePerguntaAtual, upgradeEscolhido,
                        &jogador, &vidaMaximaJogador, &cadenciaTiroJogador,
                        &respostaCorreta, &estado
                    );
                }
            }
            else if (estado == RESULTADO_PERGUNTA &&
                (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_SPACE)) {
                portaAberta = true;
                estado = SALA_UPGRADE;
            }
            else if ((estado == FIM_DE_JOGO || estado == VITORIA) &&
                tecla == ALLEGRO_KEY_ENTER) {

                reiniciar_jogo(
                    &jogador, &vidaMaximaJogador, &danoJogador, &cadenciaTiroJogador, &salaAtual,
                    inimigos, tiros_inimigos, &qtd_inimigos,
                    amebas, &qtd_amebas,
                    tiros_jogador, &boss, &portaAberta
                );

                upgradeEscolhido = 0;
                indicePerguntaAtual = -1;
                respostaCorreta = false;
                estadoAnterior = JOGANDO;
                estado = JOGANDO;
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

            if (estado == JOGANDO || estado == SALA_UPGRADE) {

                /* Movimentacao do jogador com WASD */
                if (teclas[ALLEGRO_KEY_A]) jogador.x -= jogador.vel;
                if (teclas[ALLEGRO_KEY_D]) jogador.x += jogador.vel;
                if (teclas[ALLEGRO_KEY_W]) jogador.y -= jogador.vel;
                if (teclas[ALLEGRO_KEY_S]) jogador.y += jogador.vel;

                /* Limites da sala atual */
                float min_x = ESPESSURA_PAREDE + jogador.tamanho / 2.0f;
                float limite_direito = (estado == JOGANDO && salaAtual == SALA_BOSS && boss.ativo)
                    ? boss.x
                    : (LARGURA_TELA - ESPESSURA_PAREDE);
                float max_x = limite_direito - jogador.tamanho / 2.0f;

                float min_y = (estado == SALA_UPGRADE ? CORREDOR_Y_MIN : ESPESSURA_PAREDE) + jogador.tamanho / 2.0f;
                float max_y = (estado == SALA_UPGRADE ? CORREDOR_Y_MAX : ALTURA_TELA - ESPESSURA_PAREDE) - jogador.tamanho / 2.0f;

                if (jogador.x < min_x) jogador.x = min_x;
                if (jogador.x > max_x) jogador.x = max_x;
                if (jogador.y < min_y) jogador.y = min_y;
                if (jogador.y > max_y) jogador.y = max_y;

                /* Passagem pela porta da direita quando estiver aberta */
                if (portaAberta &&
                    jogador.x >= max_x - 2.0f &&
                    jogador.y >= PORTA_Y_MIN &&
                    jogador.y <= PORTA_Y_MAX) {

                    if (estado == JOGANDO) {
                        estado = ESCOLHER_UPGRADE;
                        portaAberta = false;
                        upgradeEscolhido = 0;
                        indicePerguntaAtual = -1;
                        respostaCorreta = false;

                        jogador.vida = vidaMaximaJogador;
                        jogador.x = ESPESSURA_PAREDE + jogador.tamanho;
                        jogador.y = ALTURA_TELA / 2.0f;
                        limpar_tiros_jogador(tiros_jogador);

                        for (int i = 0; i < MAX_INIMIGOS; i++) {
                            tiros_inimigos[i].ativo = false;
                        }
                        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                            teclas[i] = false;
                        }
                    }
                    else if (estado == SALA_UPGRADE) {
                        salaAtual++;

                        if (salaAtual <= TOTAL_SALAS_COMBATE) {
                            iniciar_sala_combate(
                                salaAtual, &jogador,
                                inimigos, tiros_inimigos, &qtd_inimigos,
                                amebas, &qtd_amebas,
                                tiros_jogador, &boss, &portaAberta
                            );
                        }
                        else {
                            iniciar_sala_boss(
                                &jogador,
                                tiros_inimigos, &qtd_inimigos,
                                &qtd_amebas,
                                tiros_jogador, &boss, &portaAberta
                            );
                        }

                        estado = JOGANDO;
                    }
                }
            }

            /* Logica de combate e disparo constante com as setas */
            if (estado == JOGANDO) {

                if (jogador.tempo_recarga > 0) jogador.tempo_recarga--;
                if (jogador.invulnerabilidade > 0) jogador.invulnerabilidade--;

                float tiro_dx = 0.0f;
                float tiro_dy = 0.0f;

                if (teclas[ALLEGRO_KEY_UP])         tiro_dy = -1.0f;
                else if (teclas[ALLEGRO_KEY_DOWN])  tiro_dy = 1.0f;
                else if (teclas[ALLEGRO_KEY_LEFT])  tiro_dx = -1.0f;
                else if (teclas[ALLEGRO_KEY_RIGHT]) tiro_dx = 1.0f;

                if (tiro_dx != 0.0f || tiro_dy != 0.0f) {
                    jogador.direcao_x = tiro_dx;
                    jogador.direcao_y = tiro_dy;

                    if (jogador.tempo_recarga == 0) {
                        for (int i = 0; i < MAX_TIROS_JOGADOR; i++) {
                            if (!tiros_jogador[i].ativo) {
                                tiros_jogador[i].ativo = true;
                                tiros_jogador[i].x = jogador.x;
                                tiros_jogador[i].y = jogador.y;
                                tiros_jogador[i].vx = tiro_dx * 7.0f;
                                tiros_jogador[i].vy = tiro_dy * 7.0f;
                                jogador.tempo_recarga = cadenciaTiroJogador;
                                break;
                            }
                        }
                    }
                }

                /* Atualizacao do Boss na sala 6 */
                if (salaAtual == SALA_BOSS && boss.ativo) {
                    atualizar_boss(&boss, &jogador, LARGURA_TELA, ALTURA_TELA);

                    if (verificar_dano_boss_no_jogador(&boss, &jogador)) {
                        estado = FIM_DE_JOGO;
                    }
                }

                /* Atualizar inimigos que atiram e seus projeteis */
                for (int i = 0; i < qtd_inimigos; i++) {

                    if (inimigos[i].vida > 0) {
                        atualizar_inimigo(&inimigos[i], &jogador, &tiros_inimigos[i]);

                        if (estado == JOGANDO && jogador.invulnerabilidade == 0) {
                            float dx = jogador.x - inimigos[i].x;
                            float dy = jogador.y - inimigos[i].y;
                            float limite = (jogador.tamanho + inimigos[i].tamanho) / 2.0f;

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
                    }

                    if (tiros_inimigos[i].ativo) {
                        atualizar_tiro(&tiros_inimigos[i], LARGURA_TELA, ALTURA_TELA);

                        if (estado == JOGANDO &&
                            verificar_colisao(&tiros_inimigos[i], jogador.x, jogador.y, jogador.tamanho)) {

                            tiros_inimigos[i].ativo = false;

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
                }

                /* Atualizar amebas */
                for (int i = 0; i < qtd_amebas; i++) {

                    if (amebas[i].vida > 0) {
                        atualizar_ameba(&amebas[i], &jogador);

                        if (estado == JOGANDO && jogador.invulnerabilidade == 0) {
                            float dx = jogador.x - amebas[i].x;
                            float dy = jogador.y - amebas[i].y;
                            float limite = (jogador.tamanho + amebas[i].tamanho) / 2.0f;

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
                    }
                }

                /* Impede que inimigos e amebas atravessem uns aos outros */
                if (salaAtual < SALA_BOSS) {
                    separar_todos_inimigos(
                        inimigos, qtd_inimigos,
                        amebas, qtd_amebas,
                        LARGURA_TELA, ALTURA_TELA, ESPESSURA_PAREDE
                    );
                }

                /* Atualizar todos os tiros ativos do jogador e testar colisoes */
                for (int t = 0; t < MAX_TIROS_JOGADOR; t++) {
                    if (!tiros_jogador[t].ativo) continue;

                    atualizar_tiro(&tiros_jogador[t], LARGURA_TELA, ALTURA_TELA);

                    if (salaAtual == SALA_BOSS && boss.ativo) {
                        if (verificar_tiro_jogador_no_boss(&boss, &tiros_jogador[t], danoJogador)) {
                            continue;
                        }
                    }

                    for (int i = 0; i < qtd_inimigos && tiros_jogador[t].ativo; i++) {
                        if (inimigos[i].vida > 0 &&
                            verificar_colisao(&tiros_jogador[t], inimigos[i].x, inimigos[i].y, inimigos[i].tamanho)) {

                            inimigos[i].vida -= danoJogador;
                            if (inimigos[i].vida < 0) inimigos[i].vida = 0;
                            tiros_jogador[t].ativo = false;
                        }
                    }

                    for (int i = 0; i < qtd_amebas && tiros_jogador[t].ativo; i++) {
                        if (amebas[i].vida > 0 &&
                            verificar_colisao(&tiros_jogador[t], amebas[i].x, amebas[i].y, amebas[i].tamanho)) {

                            amebas[i].vida -= danoJogador;
                            if (amebas[i].vida < 0) amebas[i].vida = 0;
                            tiros_jogador[t].ativo = false;
                        }
                    }
                }

                /* Condicao de vitoria da sala ou da Boss Fight */
                if (estado == JOGANDO) {
                    if (salaAtual == SALA_BOSS) {
                        if (!boss.ativo || boss.vida <= 0) {
                            estado = VITORIA;
                        }
                    }
                    else {
                        int inimigosVivos = 0;
                        for (int i = 0; i < qtd_inimigos; i++) {
                            if (inimigos[i].vida > 0) inimigosVivos++;
                        }
                        for (int i = 0; i < qtd_amebas; i++) {
                            if (amebas[i].vida > 0) inimigosVivos++;
                        }

                        if (inimigosVivos == 0) {
                            portaAberta = true;
                        }
                    }
                }
            }

            redesenhar = true;
            break;
        }

        /* ================= DESENHO DA TELA ================= */
        if (redesenhar && al_is_event_queue_empty(fila)) {

            redesenhar = false;
            al_clear_to_color(al_map_rgb(15, 15, 25));

            /* MENU INICIAL */
            if (estado == MENU_INICIAL) {

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    LARGURA_TELA / 2, 170,
                    ALLEGRO_ALIGN_CENTER, "WHITE CELL WARRIOR"
                );

                bool hoverPlay = ponto_no_retangulo(mouseX, mouseY, BTN_PLAY_X1, BTN_PLAY_Y1, BTN_PLAY_X2, BTN_PLAY_Y2);
                ALLEGRO_COLOR corBotao = hoverPlay
                    ? al_map_rgb(60, 230, 120)
                    : al_map_rgb(35, 165, 85);

                al_draw_filled_rounded_rectangle(
                    BTN_PLAY_X1, BTN_PLAY_Y1,
                    BTN_PLAY_X2, BTN_PLAY_Y2,
                    10, 10, corBotao
                );

                al_draw_rounded_rectangle(
                    BTN_PLAY_X1, BTN_PLAY_Y1,
                    BTN_PLAY_X2, BTN_PLAY_Y2,
                    10, 10, al_map_rgb(255, 255, 255), 2.0f
                );

                al_draw_text(
                    fonte, al_map_rgb(255, 255, 255),
                    (BTN_PLAY_X1 + BTN_PLAY_X2) / 2.0f,
                    (BTN_PLAY_Y1 + BTN_PLAY_Y2) / 2.0f - alturaFonte / 2.0f,
                    ALLEGRO_ALIGN_CENTER,
                    "PLAY"
                );

                al_draw_text(
                    fonte, al_map_rgb(180, 180, 180),
                    LARGURA_TELA / 2, 390,
                    ALLEGRO_ALIGN_CENTER, "ESC - Sair"
                );
            }

            /* SALAS DE COMBATE, BOSS, CORREDOR DE UPGRADE E OVERLAYS */
            else if (estado == JOGANDO || estado == SALA_UPGRADE || estado == TUTORIAL ||
                estado == ESCOLHER_UPGRADE || estado == PERGUNTA || estado == RESULTADO_PERGUNTA) {

                ALLEGRO_COLOR corParede = al_map_rgb(45, 45, 65);
                bool salaCombate = (estado == JOGANDO || estado == TUTORIAL);

                if (salaCombate) {
                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ESPESSURA_PAREDE, corParede);
                    al_draw_filled_rectangle(0, ALTURA_TELA - ESPESSURA_PAREDE, LARGURA_TELA, ALTURA_TELA, corParede);
                    al_draw_filled_rectangle(0, 0, ESPESSURA_PAREDE, ALTURA_TELA, corParede);
                    al_draw_filled_rectangle(LARGURA_TELA - ESPESSURA_PAREDE, 0, LARGURA_TELA, ALTURA_TELA, corParede);

                    if (salaAtual > 1) {
                        al_draw_filled_rectangle(
                            0, PORTA_Y_MIN,
                            ESPESSURA_PAREDE, PORTA_Y_MAX,
                            al_map_rgb(70, 70, 95)
                        );
                    }

                    if (salaAtual < SALA_BOSS) {
                        ALLEGRO_COLOR corPorta = portaAberta
                            ? al_map_rgb(40, 220, 100)
                            : al_map_rgb(190, 45, 45);

                        al_draw_filled_rectangle(
                            LARGURA_TELA - ESPESSURA_PAREDE, PORTA_Y_MIN,
                            LARGURA_TELA, PORTA_Y_MAX,
                            corPorta
                        );
                    }
                    else {
                        desenhar_boss(&boss, &jogador);
                    }
                }
                else {
                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, CORREDOR_Y_MIN, corParede);
                    al_draw_filled_rectangle(0, CORREDOR_Y_MAX, LARGURA_TELA, ALTURA_TELA, corParede);
                    al_draw_filled_rectangle(0, CORREDOR_Y_MIN, ESPESSURA_PAREDE, CORREDOR_Y_MAX, al_map_rgb(70, 70, 95));

                    ALLEGRO_COLOR corPorta = portaAberta
                        ? al_map_rgb(40, 220, 100)
                        : al_map_rgb(220, 170, 40);

                    al_draw_filled_rectangle(
                        LARGURA_TELA - ESPESSURA_PAREDE, PORTA_Y_MIN,
                        LARGURA_TELA, PORTA_Y_MAX,
                        corPorta
                    );

                    if (estado == SALA_UPGRADE) {
                        const char* textoProx = (salaAtual == TOTAL_SALAS_COMBATE)
                            ? "Passe pela porta verde à direita para a BOSS FIGHT!"
                            : "Passe pela porta verde à direita para a próxima sala de combate!";

                        al_draw_text(
                            fonte, al_map_rgb(80, 255, 130),
                            LARGURA_TELA / 2, 130,
                            ALLEGRO_ALIGN_CENTER,
                            textoProx
                        );
                    }
                }

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

                /* Desenho dos inimigos, tiros e barra de vida do boss */
                if (salaCombate) {
                    int inimigosVivos = 0;

                    for (int i = 0; i < qtd_inimigos; i++) {
                        if (inimigos[i].vida > 0) {
                            inimigosVivos++;
                            desenhar_inimigo(&inimigos[i]);

                            float barra_x = inimigos[i].x - inimigos[i].tamanho / 2;
                            float barra_y = inimigos[i].y - inimigos[i].tamanho / 2 - 10;

                            al_draw_filled_rectangle(
                                barra_x, barra_y,
                                barra_x + inimigos[i].tamanho, barra_y + 5,
                                al_map_rgb(100, 0, 0)
                            );

                            al_draw_filled_rectangle(
                                barra_x, barra_y,
                                barra_x + inimigos[i].tamanho * (inimigos[i].vida / (float)VIDA_MAX_INIMIGO),
                                barra_y + 5,
                                al_map_rgb(255, 0, 0)
                            );
                        }

                        if (tiros_inimigos[i].ativo) {
                            desenhar_tiro(&tiros_inimigos[i], 255, 0, 0);
                        }
                    }

                    for (int i = 0; i < qtd_amebas; i++) {
                        if (amebas[i].vida > 0) {
                            inimigosVivos++;
                            desenhar_ameba(&amebas[i]);
                            desenhar_barra_vida_ameba(&amebas[i]);
                        }
                    }

                    if (salaAtual == SALA_BOSS && boss.ativo) {
                        desenhar_tiros_boss(&boss);
                    }

                    for (int t = 0; t < MAX_TIROS_JOGADOR; t++) {
                        desenhar_tiro(&tiros_jogador[t], 0, 150, 255);
                    }

                    char hud[160];
                    if (salaAtual == SALA_BOSS) {
                        snprintf(hud, sizeof(hud), "BOSS FIGHT | HP:%d/%d Vel:%.1f Tiro:%.2f/s",
                            jogador.vida, vidaMaximaJogador, jogador.vel, FPS / cadenciaTiroJogador);
                    }
                    else {
                        snprintf(hud, sizeof(hud), "FASE %d/%d | Inimigos: %d | HP:%d/%d Vel:%.1f Tiro:%.2f/s",
                            salaAtual, TOTAL_SALAS_COMBATE, inimigosVivos,
                            jogador.vida, vidaMaximaJogador, jogador.vel, FPS / cadenciaTiroJogador);
                    }

                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 12, 7, 0, hud);

                    if (salaAtual == SALA_BOSS) {
                        desenhar_barra_vida_boss(&boss, fonte, LARGURA_TELA, ALTURA_TELA);
                    }
                    else if (portaAberta) {
                        al_draw_text(
                            fonte, al_map_rgb(80, 255, 130),
                            LARGURA_TELA / 2, ALTURA_TELA - 24,
                            ALLEGRO_ALIGN_CENTER,
                            "Wave derrotada! Vá até a porta verde à direita para a Sala de Upgrade."
                        );
                    }
                }
                else {
                    char hud[160];
                    snprintf(hud, sizeof(hud), "UPGRADE %d/%d | HP:%d/%d Vel:%.1f Tiro:%.2f/s",
                        salaAtual, TOTAL_SALAS_COMBATE,
                        jogador.vida, vidaMaximaJogador, jogador.vel, FPS / cadenciaTiroJogador);
                    al_draw_text(fonte, al_map_rgb(255, 220, 80), 12, 7, 0, hud);
                }

                desenhar_minimapa(salaAtual, estado);

                /* JANELINHA DE TUTORIAL */
                if (estado == TUTORIAL) {
                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 175));

                    float jx1 = 165.0f, jy1 = 180.0f;
                    float jx2 = 635.0f, jy2 = 420.0f;

                    al_draw_filled_rounded_rectangle(jx1, jy1, jx2, jy2, 12, 12, al_map_rgb(28, 30, 48));
                    al_draw_rounded_rectangle(jx1, jy1, jx2, jy2, 12, 12, al_map_rgb(100, 200, 255), 3.0f);

                    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, jy1 + 25, ALLEGRO_ALIGN_CENTER, "COMO JOGAR");
                    al_draw_text(fonte, al_map_rgb(180, 255, 180), LARGURA_TELA / 2, jy1 + 80, ALLEGRO_ALIGN_CENTER, "Use W, A, S, D para se mover");
                    al_draw_text(fonte, al_map_rgb(100, 200, 255), LARGURA_TELA / 2, jy1 + 115, ALLEGRO_ALIGN_CENTER, "Use as SETINHAS para atirar");
                    al_draw_text(fonte, al_map_rgb(255, 220, 80), LARGURA_TELA / 2, jy2 - 42, ALLEGRO_ALIGN_CENTER, "Pressione ESPAÇO para fechar e começar!");
                }

                /* TELA DE ESCOLHA DOS 3 UPGRADES */
                else if (estado == ESCOLHER_UPGRADE) {
                    bool ultimaSala = (salaAtual == TOTAL_SALAS_COMBATE);
                    float bonusVel = obter_bonus_velocidade(salaAtual);
                    int bonusCad = obter_bonus_cadencia(salaAtual);
                    int bonusVida = obter_bonus_vida(salaAtual);

                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 190));

                    al_draw_filled_rounded_rectangle(45, 75, 755, 525, 12, 12, al_map_rgb(24, 26, 44));
                    al_draw_rounded_rectangle(45, 75, 755, 525, 12, 12,
                        ultimaSala ? al_map_rgb(255, 140, 40) : al_map_rgb(255, 210, 70), 3.0f);

                    char tituloUpg[96];
                    if (ultimaSala) {
                        snprintf(tituloUpg, sizeof(tituloUpg), "ÚLTIMA SALA DE UPGRADE (%d / %d) — BÔNUS TURBINADO!",
                            salaAtual, TOTAL_SALAS_COMBATE);
                    }
                    else {
                        snprintf(tituloUpg, sizeof(tituloUpg), "SALA DE UPGRADE %d / %d", salaAtual, TOTAL_SALAS_COMBATE);
                    }
                    al_draw_text(fonte, ultimaSala ? al_map_rgb(255, 170, 60) : al_map_rgb(255, 220, 80),
                        LARGURA_TELA / 2, 98, ALLEGRO_ALIGN_CENTER, tituloUpg);

                    al_draw_text(
                        fonte, al_map_rgb(220, 225, 240),
                        LARGURA_TELA / 2, 130,
                        ALLEGRO_ALIGN_CENTER,
                        ultimaSala
                        ? "Prepare-se para o Boss! Escolha seu super upgrade:"
                        : "Vida restaurada! Escolha qual atributo deseja melhorar:"
                    );

                    const char* titulos[3] = { "1 - VELOCIDADE", "2 - CADÊNCIA", "3 - VIDA MÁXIMA" };
                    ALLEGRO_COLOR coresTitulo[3] = {
                        al_map_rgb(100, 210, 255),
                        al_map_rgb(255, 190, 80),
                        al_map_rgb(90, 255, 130)
                    };

                    char bonusTxt[3][48];
                    snprintf(bonusTxt[0], sizeof(bonusTxt[0]), "+%.1f velocidade", bonusVel);
                    snprintf(bonusTxt[1], sizeof(bonusTxt[1]), ultimaSala ? "++velocidade de tiro" : "+velocidade de tiro");
                    snprintf(bonusTxt[2], sizeof(bonusTxt[2]), "+%d HP (+%d %s)",
                        bonusVida, bonusVida / 25, (bonusVida / 25 > 1) ? "golpes" : "golpe");

                    char atualTxt[3][48];
                    char novoTxt[3][48];

                    snprintf(atualTxt[0], sizeof(atualTxt[0]), "Atual: %.1f", jogador.vel);
                    snprintf(novoTxt[0], sizeof(novoTxt[0]), "Novo:  %.1f", jogador.vel + bonusVel);

                    int proxCadencia = cadenciaTiroJogador - bonusCad;
                    if (proxCadencia < CADENCIA_MINIMA_JOGADOR) proxCadencia = CADENCIA_MINIMA_JOGADOR;
                    snprintf(atualTxt[1], sizeof(atualTxt[1]), "Atual: %.2f tiros/s", FPS / cadenciaTiroJogador);
                    snprintf(novoTxt[1], sizeof(novoTxt[1]), "Novo:  %.2f tiros/s", FPS / proxCadencia);

                    snprintf(atualTxt[2], sizeof(atualTxt[2]), "Atual: %d HP", vidaMaximaJogador);
                    snprintf(novoTxt[2], sizeof(novoTxt[2]), "Novo:  %d HP", vidaMaximaJogador + bonusVida);

                    for (int i = 0; i < 3; i++) {
                        float cx1 = CARD_UPG_X_INICIAL + i * (CARD_UPG_LARGURA + CARD_UPG_ESPACO);
                        float cx2 = cx1 + CARD_UPG_LARGURA;
                        float centroX = (cx1 + cx2) / 2.0f;
                        bool hover = ponto_no_retangulo(mouseX, mouseY, cx1, CARD_UPG_Y1, cx2, CARD_UPG_Y2);

                        ALLEGRO_COLOR fundoCard = hover
                            ? al_map_rgb(52, 58, 92)
                            : al_map_rgb(36, 40, 66);
                        ALLEGRO_COLOR bordaCard = hover
                            ? coresTitulo[i]
                            : (ultimaSala ? al_map_rgb(185, 130, 75) : al_map_rgb(110, 120, 160));

                        al_draw_filled_rounded_rectangle(cx1, CARD_UPG_Y1, cx2, CARD_UPG_Y2, 10, 10, fundoCard);
                        al_draw_rounded_rectangle(cx1, CARD_UPG_Y1, cx2, CARD_UPG_Y2, 10, 10, bordaCard, hover ? 3.0f : 2.0f);

                        al_draw_text(fonte, coresTitulo[i], centroX, CARD_UPG_Y1 + 24, ALLEGRO_ALIGN_CENTER, titulos[i]);
                        al_draw_text(fonte, al_map_rgb(255, 255, 255), centroX, CARD_UPG_Y1 + 70, ALLEGRO_ALIGN_CENTER, bonusTxt[i]);

                        al_draw_text(fonte, al_map_rgb(180, 185, 205), centroX, CARD_UPG_Y1 + 122, ALLEGRO_ALIGN_CENTER, atualTxt[i]);
                        al_draw_text(fonte, coresTitulo[i], centroX, CARD_UPG_Y1 + 148, ALLEGRO_ALIGN_CENTER, novoTxt[i]);

                        al_draw_text(fonte, hover ? al_map_rgb(255, 255, 255) : al_map_rgb(170, 175, 195),
                            centroX, CARD_UPG_Y2 - 34, ALLEGRO_ALIGN_CENTER, "CLIQUE PARA ESCOLHER");
                    }

                    al_draw_text(
                        fonte, al_map_rgb(255, 130, 130),
                        LARGURA_TELA / 2, 468,
                        ALLEGRO_ALIGN_CENTER,
                        "Acerte a pergunta a seguir para ganhar o upgrade (se errar, você perde!)"
                    );
                }

                /* TELA DA PERGUNTA DE MULTIPLA ESCOLHA */
                else if (estado == PERGUNTA && indicePerguntaAtual >= 0) {
                    const Pergunta* perg = &bancoPerguntas[indicePerguntaAtual];

                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 200));

                    al_draw_filled_rounded_rectangle(35, 22, 765, 578, 12, 12, al_map_rgb(24, 26, 44));
                    al_draw_rounded_rectangle(35, 22, 765, 578, 12, 12, al_map_rgb(100, 200, 255), 3.0f);

                    char nomeUpg[64];
                    if (upgradeEscolhido == UPGRADE_VELOCIDADE) {
                        snprintf(nomeUpg, sizeof(nomeUpg), "VELOCIDADE (+%.1f)", obter_bonus_velocidade(salaAtual));
                    }
                    else if (upgradeEscolhido == UPGRADE_CADENCIA) {
                        snprintf(nomeUpg, sizeof(nomeUpg), (salaAtual == TOTAL_SALAS_COMBATE)
                            ? "CADÊNCIA DE TIRO (TURBINADA)"
                            : "CADÊNCIA DE TIRO");
                    }
                    else {
                        snprintf(nomeUpg, sizeof(nomeUpg), "VIDA MÁXIMA (+%d HP)", obter_bonus_vida(salaAtual));
                    }

                    char cabecalho[128];
                    snprintf(cabecalho, sizeof(cabecalho), "FASE %d — DESAFIO DE UPGRADE: %s", salaAtual, nomeUpg);
                    al_draw_text(fonte, al_map_rgb(255, 220, 80), LARGURA_TELA / 2, 40, ALLEGRO_ALIGN_CENTER, cabecalho);

                    desenhar_texto_quebrado(
                        fonte, al_map_rgb(255, 255, 255),
                        LARGURA_TELA / 2.0f, 78.0f,
                        680.0f, 22.0f,
                        ALLEGRO_ALIGN_CENTER,
                        perg->enunciado
                    );

                    const char letras[4] = { 'A', 'B', 'C', 'D' };
                    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
                        float ay1 = BTN_ALT_Y_INICIAL + i * (BTN_ALT_ALTURA + BTN_ALT_ESPACO);
                        float ay2 = ay1 + BTN_ALT_ALTURA;
                        bool hover = ponto_no_retangulo(mouseX, mouseY, BTN_ALT_X1, ay1, BTN_ALT_X2, ay2);

                        ALLEGRO_COLOR fundoAlt = hover
                            ? al_map_rgb(55, 70, 115)
                            : al_map_rgb(36, 42, 70);
                        ALLEGRO_COLOR bordaAlt = hover
                            ? al_map_rgb(100, 220, 255)
                            : al_map_rgb(95, 110, 150);

                        al_draw_filled_rounded_rectangle(BTN_ALT_X1, ay1, BTN_ALT_X2, ay2, 8, 8, fundoAlt);
                        al_draw_rounded_rectangle(BTN_ALT_X1, ay1, BTN_ALT_X2, ay2, 8, 8, bordaAlt, hover ? 2.5f : 1.5f);

                        char textoAlt[320];
                        snprintf(textoAlt, sizeof(textoAlt), "%c) %s", letras[i], perg->alternativas[i]);

                        desenhar_texto_quebrado(
                            fonte, al_map_rgb(235, 240, 255),
                            BTN_ALT_X1 + 16.0f, ay1 + 14.0f,
                            (BTN_ALT_X2 - BTN_ALT_X1) - 32.0f, 20.0f,
                            ALLEGRO_ALIGN_LEFT,
                            textoAlt
                        );
                    }

                    al_draw_text(
                        fonte, al_map_rgb(175, 185, 210),
                        LARGURA_TELA / 2, 540,
                        ALLEGRO_ALIGN_CENTER,
                        "Clique na resposta correta (ou pressione 1, 2, 3 ou 4)"
                    );
                }

                /* TELA DE RESULTADO DA PERGUNTA */
                else if (estado == RESULTADO_PERGUNTA && indicePerguntaAtual >= 0) {
                    const Pergunta* perg = &bancoPerguntas[indicePerguntaAtual];

                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 200));

                    ALLEGRO_COLOR corBorda = respostaCorreta
                        ? al_map_rgb(60, 235, 120)
                        : al_map_rgb(235, 75, 75);

                    al_draw_filled_rounded_rectangle(65, 80, 735, 520, 12, 12, al_map_rgb(24, 26, 44));
                    al_draw_rounded_rectangle(65, 80, 735, 520, 12, 12, corBorda, 3.0f);

                    if (respostaCorreta) {
                        al_draw_text(fonte, al_map_rgb(60, 255, 120), LARGURA_TELA / 2, 110, ALLEGRO_ALIGN_CENTER, "RESPOSTA CORRETA!");

                        char msgGanho[128];
                        if (upgradeEscolhido == UPGRADE_VELOCIDADE) {
                            snprintf(msgGanho, sizeof(msgGanho), "Upgrade adquirido! Velocidade aumentada para %.1f!", jogador.vel);
                        }
                        else if (upgradeEscolhido == UPGRADE_CADENCIA) {
                            snprintf(msgGanho, sizeof(msgGanho), "Upgrade adquirido! Cadência aumentada para %.2f tiros/s!", FPS / cadenciaTiroJogador);
                        }
                        else {
                            snprintf(msgGanho, sizeof(msgGanho), "Upgrade adquirido! Vida máxima aumentada para %d HP!", vidaMaximaJogador);
                        }

                        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 152, ALLEGRO_ALIGN_CENTER, msgGanho);
                    }
                    else {
                        al_draw_text(fonte, al_map_rgb(255, 80, 80), LARGURA_TELA / 2, 110, ALLEGRO_ALIGN_CENTER, "RESPOSTA INCORRETA!");

                        al_draw_text(
                            fonte, al_map_rgb(255, 200, 160),
                            LARGURA_TELA / 2, 148,
                            ALLEGRO_ALIGN_CENTER,
                            "Você não ganhou o upgrade! A próxima sala será mais difícil."
                        );

                        char respCertaTxt[320];
                        snprintf(respCertaTxt, sizeof(respCertaTxt), "Resposta correta: %s",
                            perg->alternativas[perg->respostaCorreta]);

                        desenhar_texto_quebrado(
                            fonte, al_map_rgb(130, 255, 160),
                            LARGURA_TELA / 2.0f, 188.0f,
                            610.0f, 20.0f,
                            ALLEGRO_ALIGN_CENTER,
                            respCertaTxt
                        );
                    }

                    al_draw_text(fonte, al_map_rgb(140, 190, 255), LARGURA_TELA / 2, 268, ALLEGRO_ALIGN_CENTER, "EXPLICAÇÃO:");
                    desenhar_texto_quebrado(
                        fonte, al_map_rgb(215, 225, 245),
                        LARGURA_TELA / 2.0f, 298.0f,
                        610.0f, 22.0f,
                        ALLEGRO_ALIGN_CENTER,
                        perg->explicacao
                    );

                    bool hoverCont = ponto_no_retangulo(mouseX, mouseY, BTN_CONT_X1, BTN_CONT_Y1, BTN_CONT_X2, BTN_CONT_Y2);
                    ALLEGRO_COLOR corBtnCont = hoverCont
                        ? al_map_rgb(65, 215, 120)
                        : al_map_rgb(40, 155, 85);

                    al_draw_filled_rounded_rectangle(BTN_CONT_X1, BTN_CONT_Y1, BTN_CONT_X2, BTN_CONT_Y2, 10, 10, corBtnCont);
                    al_draw_rounded_rectangle(BTN_CONT_X1, BTN_CONT_Y1, BTN_CONT_X2, BTN_CONT_Y2, 10, 10, al_map_rgb(255, 255, 255), 2.0f);

                    al_draw_text(
                        fonte, al_map_rgb(255, 255, 255),
                        (BTN_CONT_X1 + BTN_CONT_X2) / 2.0f,
                        (BTN_CONT_Y1 + BTN_CONT_Y2) / 2.0f - alturaFonte / 2.0f,
                        ALLEGRO_ALIGN_CENTER,
                        "CONTINUAR"
                    );
                }
            }

            /* FIM DE JOGO */
            else if (estado == FIM_DE_JOGO) {

                al_draw_text(
                    fonte, al_map_rgb(255, 50, 50),
                    LARGURA_TELA / 2, 230,
                    ALLEGRO_ALIGN_CENTER, "VOCÊ MORREU!"
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
                    ALLEGRO_ALIGN_CENTER, "VOCÊ VENCEU A BOSS FIGHT!"
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