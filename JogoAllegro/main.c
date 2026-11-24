#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/* #include "perguntas.h" -> reative quando for ligar o quiz na sala de upgrade */
#include "inimigo.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60.0f

#define VIDA_MAX_JOGADOR 100
#define VIDA_MAX_INIMIGO 100
#define VIDA_MAX_AMEBA 150

/* Configuracao das waves, tiros e salas */
#define MAX_INIMIGOS 5
#define MAX_TIROS_JOGADOR 20
#define CADENCIA_TIRO_JOGADOR 60
#define TOTAL_SALAS_COMBATE 3
#define SALA_BOSS 4

/* Paredes e portas */
#define ESPESSURA_PAREDE 30.0f
#define PORTA_Y_MIN 240.0f
#define PORTA_Y_MAX 360.0f

/* Altura do corredor da sala de upgrade */
#define CORREDOR_Y_MIN 200.0f
#define CORREDOR_Y_MAX 400.0f

typedef enum {
    MENU_INICIAL,
    JOGANDO,
    SALA_UPGRADE,
    FIM_DE_JOGO,
    VITORIA
} EstadoJogo;

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

/* Prepara a wave de cada sala de combate (3, 4 e 5 inimigos) */
void iniciar_sala_combate(
    int sala,
    Jogador* jogador,
    Inimigo inimigos[],
    Projetil tiros_inimigos[],
    int* qtd_inimigos,
    Ameba amebas[],
    int* qtd_amebas,
    Projetil tiros_jogador[],
    bool* portaAberta
) {
    *portaAberta = false;
    limpar_tiros_jogador(tiros_jogador);
    jogador->tempo_recarga = 0;

    /* Sala 1 comeca no centro; salas 2 e 3 entram pela porta esquerda */
    if (sala == 1) {
        jogador->x = LARGURA_TELA / 2.0f;
        jogador->y = ALTURA_TELA / 2.0f;
    }
    else {
        jogador->x = ESPESSURA_PAREDE + jogador->tamanho;
        jogador->y = ALTURA_TELA / 2.0f;
    }

    /* Distribuicao das waves usando os dois tipos de inimigos ja prontos */
    if (sala == 1) {
        *qtd_inimigos = 2;
        *qtd_amebas = 1; /* Total: 3 inimigos */
    }
    else if (sala == 2) {
        *qtd_inimigos = 2;
        *qtd_amebas = 2; /* Total: 4 inimigos */
    }
    else {
        *qtd_inimigos = 3;
        *qtd_amebas = 2; /* Total: 5 inimigos */
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
        inimigos[i].vel = 0.5f;
        inimigos[i].vida = VIDA_MAX_INIMIGO;
        inimigos[i].timer_tiro = i * 20;
    }

    for (int i = 0; i < *qtd_amebas; i++) {
        float x, y;
        sortear_posicao_segura(&x, &y, jogador->x, jogador->y);

        amebas[i].x = x;
        amebas[i].y = y;
        amebas[i].tamanho = 35.0f;
        amebas[i].vel = 0.3f;
        amebas[i].vida = VIDA_MAX_AMEBA;
    }
}

/* Prepara a sala de Boss com 1 inimigo de teste */
void iniciar_sala_boss(
    Jogador* jogador,
    Inimigo inimigos[],
    Projetil tiros_inimigos[],
    int* qtd_inimigos,
    int* qtd_amebas,
    Projetil tiros_jogador[],
    bool* portaAberta
) {
    *portaAberta = false;
    limpar_tiros_jogador(tiros_jogador);
    jogador->tempo_recarga = 0;

    jogador->x = ESPESSURA_PAREDE + jogador->tamanho;
    jogador->y = ALTURA_TELA / 2.0f;

    *qtd_inimigos = 1;
    *qtd_amebas = 0;

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        tiros_inimigos[i].ativo = false;
        tiros_inimigos[i].raio = 5.0f;
    }

    inimigos[0].x = LARGURA_TELA - 150.0f;
    inimigos[0].y = ALTURA_TELA / 2.0f;
    inimigos[0].tamanho = 45.0f;
    inimigos[0].vel = 0.6f;
    inimigos[0].vida = VIDA_MAX_INIMIGO;
    inimigos[0].timer_tiro = 0;
}

/* Reinicia toda a progressao do jogo */
void reiniciar_jogo(
    Jogador* jogador,
    int* vidaMaximaJogador,
    int* danoJogador,
    int* salaAtual,
    Inimigo inimigos[],
    Projetil tiros_inimigos[],
    int* qtd_inimigos,
    Ameba amebas[],
    int* qtd_amebas,
    Projetil tiros_jogador[],
    bool* portaAberta
) {
    jogador->tamanho = 40.0f;
    jogador->vel = 4.0f;
    jogador->vida = VIDA_MAX_JOGADOR;
    jogador->invulnerabilidade = 0;
    jogador->direcao_x = 1.0f;
    jogador->direcao_y = 0.0f;
    jogador->tempo_recarga = 0;

    *vidaMaximaJogador = VIDA_MAX_JOGADOR;
    *danoJogador = 25;
    *salaAtual = 1;

    iniciar_sala_combate(
        *salaAtual, jogador,
        inimigos, tiros_inimigos, qtd_inimigos,
        amebas, qtd_amebas,
        tiros_jogador, portaAberta
    );
}

/* Desenha o minimapa no canto superior direito seguindo o esquema das 4 salas + 3 upgrades */
void desenhar_minimapa(int salaAtual, EstadoJogo estado) {
    float x = LARGURA_TELA - 195.0f;
    float y = 8.0f;

    for (int s = 1; s <= SALA_BOSS; s++) {
        bool estaNaSala = (estado == JOGANDO && salaAtual == s);
        ALLEGRO_COLOR corSala = estaNaSala
            ? al_map_rgb(0, 220, 255)
            : (s == SALA_BOSS ? al_map_rgb(160, 40, 40) : al_map_rgb(90, 90, 110));

        al_draw_filled_rectangle(x, y, x + 22.0f, y + 14.0f, corSala);
        x += 24.0f;

        if (s < SALA_BOSS) {
            bool estaNoUpgrade = (estado == SALA_UPGRADE && salaAtual == s);
            ALLEGRO_COLOR corCorredor = estaNoUpgrade
                ? al_map_rgb(255, 220, 60)
                : al_map_rgb(60, 60, 75);

            al_draw_filled_rectangle(x, y + 4.0f, x + 16.0f, y + 10.0f, corCorredor);
            x += 18.0f;
        }
    }
}

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
    EstadoJogo estadoAnterior = JOGANDO;

    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true;
    bool redesenhar = true;

    Jogador jogador;
    Inimigo inimigos[MAX_INIMIGOS];
    Projetil tiros_inimigos[MAX_INIMIGOS];
    Ameba amebas[MAX_INIMIGOS];
    Projetil tiros_jogador[MAX_TIROS_JOGADOR];

    int qtd_inimigos = 0;
    int qtd_amebas = 0;
    int vidaMaximaJogador = VIDA_MAX_JOGADOR;
    int danoJogador = 25;
    int salaAtual = 1;
    bool portaAberta = false;

    /* Inicializa a primeira sala antes de iniciar o loop */
    reiniciar_jogo(
        &jogador, &vidaMaximaJogador, &danoJogador, &salaAtual,
        inimigos, tiros_inimigos, &qtd_inimigos,
        amebas, &qtd_amebas,
        tiros_jogador, &portaAberta
    );

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
                if (estado == JOGANDO || estado == SALA_UPGRADE) {
                    estadoAnterior = estado;
                    estado = MENU_INICIAL;
                }
                else {
                    rodando = false;
                }
            }

            /* Iniciar ou retomar do menu */
            if (estado == MENU_INICIAL && tecla == ALLEGRO_KEY_ENTER) {
                estado = estadoAnterior;
            }

            /* Reiniciar jogo depois de morrer ou vencer */
            else if ((estado == FIM_DE_JOGO || estado == VITORIA) &&
                tecla == ALLEGRO_KEY_ENTER) {

                reiniciar_jogo(
                    &jogador, &vidaMaximaJogador, &danoJogador, &salaAtual,
                    inimigos, tiros_inimigos, &qtd_inimigos,
                    amebas, &qtd_amebas,
                    tiros_jogador, &portaAberta
                );

                estadoAnterior = JOGANDO;
                estado = JOGANDO;
            }

            /* Liberar proxima sala na Sala de Upgrade apertando E */
            else if (estado == SALA_UPGRADE && tecla == ALLEGRO_KEY_E) {
                portaAberta = true;
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

                /* Movimentacao do jogador apenas com WASD */
                if (teclas[ALLEGRO_KEY_A]) {
                    jogador.x -= jogador.vel;
                }

                if (teclas[ALLEGRO_KEY_D]) {
                    jogador.x += jogador.vel;
                }

                if (teclas[ALLEGRO_KEY_W]) {
                    jogador.y -= jogador.vel;
                }

                if (teclas[ALLEGRO_KEY_S]) {
                    jogador.y += jogador.vel;
                }

                /* Limites da sala atual (sala cheia vs corredor de upgrade) */
                float min_x = ESPESSURA_PAREDE + jogador.tamanho / 2.0f;
                float max_x = LARGURA_TELA - ESPESSURA_PAREDE - jogador.tamanho / 2.0f;
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
                        /* Sai da sala de combate, entra no corredor de upgrade e regenera toda a vida */
                        estado = SALA_UPGRADE;
                        portaAberta = false;
                        jogador.vida = vidaMaximaJogador;
                        jogador.x = ESPESSURA_PAREDE + jogador.tamanho;
                        jogador.y = ALTURA_TELA / 2.0f;
                        limpar_tiros_jogador(tiros_jogador);

                        for (int i = 0; i < MAX_INIMIGOS; i++) {
                            tiros_inimigos[i].ativo = false;
                        }
                    }
                    else if (estado == SALA_UPGRADE) {
                        /* Sai do corredor de upgrade e avanca para a proxima sala */
                        salaAtual++;

                        if (salaAtual <= TOTAL_SALAS_COMBATE) {
                            iniciar_sala_combate(
                                salaAtual, &jogador,
                                inimigos, tiros_inimigos, &qtd_inimigos,
                                amebas, &qtd_amebas,
                                tiros_jogador, &portaAberta
                            );
                        }
                        else {
                            iniciar_sala_boss(
                                &jogador,
                                inimigos, tiros_inimigos, &qtd_inimigos,
                                &qtd_amebas,
                                tiros_jogador, &portaAberta
                            );
                        }

                        estado = JOGANDO;
                    }
                }
            }

            /* Logica de combate e disparo constante com as setas (somente em JOGANDO) */
            if (estado == JOGANDO) {

                if (jogador.tempo_recarga > 0) {
                    jogador.tempo_recarga--;
                }

                if (jogador.invulnerabilidade > 0) {
                    jogador.invulnerabilidade--;
                }

                /* Direcao e disparo constante com as setas (estilo The Binding of Isaac) */
                float tiro_dx = 0.0f;
                float tiro_dy = 0.0f;

                if (teclas[ALLEGRO_KEY_UP]) {
                    tiro_dy = -1.0f;
                }
                else if (teclas[ALLEGRO_KEY_DOWN]) {
                    tiro_dy = 1.0f;
                }
                else if (teclas[ALLEGRO_KEY_LEFT]) {
                    tiro_dx = -1.0f;
                }
                else if (teclas[ALLEGRO_KEY_RIGHT]) {
                    tiro_dx = 1.0f;
                }

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
                                jogador.tempo_recarga = CADENCIA_TIRO_JOGADOR;
                                break;
                            }
                        }
                    }
                }

                /* Atualizar inimigos que atiram e seus projeteis */
                for (int i = 0; i < qtd_inimigos; i++) {

                    if (inimigos[i].vida > 0) {
                        atualizar_inimigo(&inimigos[i], &jogador, &tiros_inimigos[i]);

                        /* Colisao corpo a corpo */
                        if (jogador.invulnerabilidade == 0) {
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

                    /* Atualiza tiro inimigo mesmo se o inimigo ja morreu */
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

                /* Atualizar todos os tiros ativos do jogador e testar colisao contra a wave */
                for (int t = 0; t < MAX_TIROS_JOGADOR; t++) {
                    if (!tiros_jogador[t].ativo) continue;

                    atualizar_tiro(&tiros_jogador[t], LARGURA_TELA, ALTURA_TELA);

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

                /* Verificar se todos os inimigos da sala foram derrotados */
                if (estado == JOGANDO) {
                    int inimigosVivos = 0;

                    for (int i = 0; i < qtd_inimigos; i++) {
                        if (inimigos[i].vida > 0) inimigosVivos++;
                    }
                    for (int i = 0; i < qtd_amebas; i++) {
                        if (amebas[i].vida > 0) inimigosVivos++;
                    }

                    if (inimigosVivos == 0) {
                        if (salaAtual == SALA_BOSS) {
                            estado = VITORIA;
                        }
                        else {
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
                    LARGURA_TELA / 2, 200,
                    ALLEGRO_ALIGN_CENTER, "WHITE CELL WARRIOR"
                );

                al_draw_text(
                    fonte, al_map_rgb(0, 255, 100),
                    LARGURA_TELA / 2, 280,
                    ALLEGRO_ALIGN_CENTER, "ENTER - Iniciar / Continuar"
                );

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200),
                    LARGURA_TELA / 2, 320,
                    ALLEGRO_ALIGN_CENTER, "WASD - Mover"
                );

                al_draw_text(
                    fonte, al_map_rgb(0, 150, 255),
                    LARGURA_TELA / 2, 345,
                    ALLEGRO_ALIGN_CENTER, "SETAS - Atirar | E - Liberar sala de upgrade"
                );

                al_draw_text(
                    fonte, al_map_rgb(200, 200, 200),
                    LARGURA_TELA / 2, 380,
                    ALLEGRO_ALIGN_CENTER, "ESC - Sair"
                );
            }

            /* SALAS DE COMBATE, BOSS E CORREDOR DE UPGRADE */
            else if (estado == JOGANDO || estado == SALA_UPGRADE) {

                ALLEGRO_COLOR corParede = al_map_rgb(45, 45, 65);

                if (estado == JOGANDO) {
                    /* Paredes externas da sala de combate/boss */
                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ESPESSURA_PAREDE, corParede);
                    al_draw_filled_rectangle(0, ALTURA_TELA - ESPESSURA_PAREDE, LARGURA_TELA, ALTURA_TELA, corParede);
                    al_draw_filled_rectangle(0, 0, ESPESSURA_PAREDE, ALTURA_TELA, corParede);
                    al_draw_filled_rectangle(LARGURA_TELA - ESPESSURA_PAREDE, 0, LARGURA_TELA, ALTURA_TELA, corParede);

                    /* Marca visual da entrada esquerda (salas 2, 3 e Boss) */
                    if (salaAtual > 1) {
                        al_draw_filled_rectangle(
                            0, PORTA_Y_MIN,
                            ESPESSURA_PAREDE, PORTA_Y_MAX,
                            al_map_rgb(70, 70, 95)
                        );
                    }

                    /* Porta de saida para a sala de upgrade (salas 1, 2 e 3) */
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
                        /* Detalhe vermelho no fundo da sala do Boss */
                        al_draw_filled_rectangle(
                            LARGURA_TELA - ESPESSURA_PAREDE, ESPESSURA_PAREDE,
                            LARGURA_TELA, ALTURA_TELA - ESPESSURA_PAREDE,
                            al_map_rgb(140, 25, 25)
                        );
                    }
                }
                else {
                    /* Corredor horizontal da sala de upgrade */
                    al_draw_filled_rectangle(0, 0, LARGURA_TELA, CORREDOR_Y_MIN, corParede);
                    al_draw_filled_rectangle(0, CORREDOR_Y_MAX, LARGURA_TELA, ALTURA_TELA, corParede);
                    al_draw_filled_rectangle(0, CORREDOR_Y_MIN, ESPESSURA_PAREDE, CORREDOR_Y_MAX, al_map_rgb(70, 70, 95));

                    /* Porta direita do corredor de upgrade */
                    ALLEGRO_COLOR corPorta = portaAberta
                        ? al_map_rgb(40, 220, 100)
                        : al_map_rgb(220, 170, 40);

                    al_draw_filled_rectangle(
                        LARGURA_TELA - ESPESSURA_PAREDE, PORTA_Y_MIN,
                        LARGURA_TELA, PORTA_Y_MAX,
                        corPorta
                    );

                    al_draw_text(
                        fonte, al_map_rgb(80, 255, 130),
                        LARGURA_TELA / 2, 105,
                        ALLEGRO_ALIGN_CENTER,
                        "VIDA RESTAURADA!"
                    );

                    if (!portaAberta) {
                        const char* textoProx = (salaAtual == TOTAL_SALAS_COMBATE)
                            ? "Pressione [E] para liberar a BOSS FIGHT"
                            : "Pressione [E] para liberar a proxima sala de combate";

                        al_draw_text(
                            fonte, al_map_rgb(255, 220, 80),
                            LARGURA_TELA / 2, 145,
                            ALLEGRO_ALIGN_CENTER,
                            textoProx
                        );
                    }
                    else {
                        al_draw_text(
                            fonte, al_map_rgb(80, 255, 130),
                            LARGURA_TELA / 2, 145,
                            ALLEGRO_ALIGN_CENTER,
                            "Proxima sala liberada! Passe pela porta verde a direita."
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

                /* Desenho dos inimigos e tiros (somente em JOGANDO) */
                if (estado == JOGANDO) {
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

                    for (int t = 0; t < MAX_TIROS_JOGADOR; t++) {
                        desenhar_tiro(&tiros_jogador[t], 0, 150, 255);
                    }

                    /* HUD superior */
                    char hud[128];
                    if (salaAtual == SALA_BOSS) {
                        snprintf(hud, sizeof(hud), "BOSS FIGHT (TESTE) | Inimigos restantes: %d", inimigosVivos);
                    }
                    else {
                        snprintf(hud, sizeof(hud), "SALA DE COMBATE %d/%d | Inimigos restantes: %d",
                            salaAtual, TOTAL_SALAS_COMBATE, inimigosVivos);
                    }

                    al_draw_text(fonte, al_map_rgb(255, 255, 255), 12, 10, 0, hud);

                    if (portaAberta && salaAtual < SALA_BOSS) {
                        al_draw_text(
                            fonte, al_map_rgb(80, 255, 130),
                            LARGURA_TELA / 2, ALTURA_TELA - 20,
                            ALLEGRO_ALIGN_CENTER,
                            "Wave derrotada! Va ate a porta verde a direita para a Sala de Upgrade."
                        );
                    }
                }
                else {
                    char hud[128];
                    snprintf(hud, sizeof(hud), "SALA DE UPGRADE %d/%d", salaAtual, TOTAL_SALAS_COMBATE);
                    al_draw_text(fonte, al_map_rgb(255, 220, 80), 12, 10, 0, hud);
                }

                desenhar_minimapa(salaAtual, estado);
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
                    ALLEGRO_ALIGN_CENTER, "VOCE VENCEU A BOSS FIGHT!"
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