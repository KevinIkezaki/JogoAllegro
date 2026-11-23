
#ifndef PERGUNTAS_H
#define PERGUNTAS_H

#define TOTAL_PERGUNTAS 50
#define TOTAL_ALTERNATIVAS 4
#define TOTAL_FASES 5

typedef struct {
    const char* enunciado;
    const char* alternativas[TOTAL_ALTERNATIVAS];
    int respostaCorreta;
    const char* explicacao;
    int fase;
} Pergunta;

/* Banco de perguntas */
extern const Pergunta bancoPerguntas[TOTAL_PERGUNTAS];

/* Funcoes do sistema */
void embaralharPerguntas(int vetor[], int tamanho);

int sortearPerguntasDaFase(int fase, int indices[], int quantidade);

int verificarResposta(int indicePergunta, int resposta);

#endif
