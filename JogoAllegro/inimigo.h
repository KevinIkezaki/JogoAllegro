#include <stdbool.h>

void atualizar_inimigo(float* inimigo_x, float* inimigo_y, float jogador_x, float jogador_y, int* tempo_para_atirar, bool* tiro_ativo, float* tiro_x, float* tiro_y, float* tiro_velocidade_x, float* tiro_velocidade_y);
void desenhar_inimigo(float inimigo_x, float inimigo_y, float inimigo_tamanho);
void atualizar_tiro(bool* tiro_ativo, float* tiro_x, float* tiro_y, float tiro_velocidade_x, float tiro_velocidade_y, float largura_tela, float altura_tela);
void desenhar_tiro(bool tiro_ativo, float tiro_x, float tiro_y);
bool verificar_colisao(bool tiro_ativo, float tiro_x, float tiro_y, float jogador_x, float jogador_y, float jogador_tamanho);