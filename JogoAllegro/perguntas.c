
#include <stdio.h>
#include <stdlib.h>
#include "perguntas.h"

/* ==========================================
   BANCO DE PERGUNTAS - WHITE CELL WARRIOR
   ========================================== */

const Pergunta bancoPerguntas[TOTAL_PERGUNTAS] = {

    /* ======================================
       FASE 1 - CONHECENDO O ORGANISMO
       Perguntas 1 a 10
       ====================================== */

    {
        "Qual e a principal funcao do sistema imunologico?",
        {
            "Controlar os movimentos do corpo.",
            "Defender o organismo contra agentes infecciosos.",
            "Produzir energia para as celulas.",
            "Transportar oxigenio pelo sangue."
        },
        1,
        "O sistema imunologico protege o organismo contra virus, bacterias e outros agentes prejudiciais.",
        1
    },

    {
        "Qual componente do sangue transporta oxigenio?",
        {
            "Plaquetas.",
            "Leucocitos.",
            "Hemacias.",
            "Linfocitos."
        },
        2,
        "As hemacias possuem hemoglobina, proteina responsavel pelo transporte de oxigenio.",
        1
    },

    {
        "Qual destas estruturas faz parte do sistema imunologico?",
        {
            "Baco.",
            "Estomago.",
            "Pancreas.",
            "Bexiga."
        },
        0,
        "O baco participa da defesa imunologica e tambem filtra o sangue.",
        1
    },

    {
        "O que sao microrganismos?",
        {
            "Apenas organismos que causam doencas.",
            "Seres ou entidades biologicas microscopicas.",
            "Celulas responsaveis pelo transporte de oxigenio.",
            "Substancias produzidas pelo sangue."
        },
        1,
        "Existem microrganismos beneficos e prejudiciais. Nem todos provocam doencas.",
        1
    },

    {
        "Qual e o principal papel da pele na defesa do organismo?",
        {
            "Produzir anticorpos.",
            "Transportar celulas sanguineas.",
            "Atuar como barreira fisica contra agentes externos.",
            "Destruir todos os virus do corpo."
        },
        2,
        "A pele e uma das primeiras barreiras de protecao do organismo.",
        1
    },

    {
        "O que sao leucocitos?",
        {
            "Celulas responsaveis pela defesa do organismo.",
            "Celulas responsaveis pela coagulacao.",
            "Celulas que transportam oxigenio.",
            "Celulas que produzem hormonios."
        },
        0,
        "Os leucocitos, tambem chamados de globulos brancos, participam das respostas imunologicas.",
        1
    },

    {
        "Qual orgao e responsavel por bombear o sangue pelo corpo?",
        {
            "Pulmao.",
            "Figado.",
            "Rim.",
            "Coracao."
        },
        3,
        "O coracao bombeia o sangue, permitindo o transporte de oxigenio e nutrientes.",
        1
    },

    {
        "O que pode acontecer quando uma bacteria prejudicial invade o organismo?",
        {
            "O sistema imunologico pode reconhecer e combater a invasao.",
            "Todas as celulas deixam de funcionar imediatamente.",
            "O sangue para de circular.",
            "As hemacias se transformam em anticorpos."
        },
        0,
        "O sistema imunologico possui mecanismos capazes de identificar e combater bacterias.",
        1
    },

    {
        "Qual habito ajuda a prevenir a transmissao de muitas doencas infecciosas?",
        {
            "Compartilhar objetos pessoais.",
            "Evitar beber agua.",
            "Higienizar corretamente as maos.",
            "Evitar dormir durante a noite."
        },
        2,
        "A higienizacao das maos reduz a transmissao de diversos agentes infecciosos.",
        1
    },

    {
        "O que e uma infeccao?",
        {
            "A entrada e multiplicacao de agentes infecciosos no organismo.",
            "O aumento natural das hemacias.",
            "A producao de energia pelas celulas.",
            "A circulacao normal do sangue."
        },
        0,
        "Uma infeccao ocorre quando agentes como virus, bacterias ou fungos entram no organismo e se multiplicam.",
        1
    },


    /* ======================================
       FASE 2 - CELULAS DO SANGUE
       Perguntas 11 a 20
       ====================================== */

    {
        "Qual e a principal funcao das plaquetas?",
        {
            "Transportar oxigenio.",
            "Auxiliar na coagulacao do sangue.",
            "Combater diretamente os virus.",
            "Produzir anticorpos."
        },
        1,
        "As plaquetas participam da coagulacao, ajudando a interromper sangramentos.",
        2
    },

    {
        "Qual celula de defesa atua principalmente no combate a bacterias por meio da fagocitose?",
        {
            "Neutrofilo.",
            "Hemacia.",
            "Plaqueta.",
            "Linfocito B."
        },
        0,
        "Os neutrofilos sao leucocitos que englobam e destroem principalmente bacterias.",
        2
    },

    {
        "O que os linfocitos B podem produzir quando ativados?",
        {
            "Hemoglobina.",
            "Plaquetas.",
            "Anticorpos.",
            "Insulina."
        },
        2,
        "Os linfocitos B podem se diferenciar em plasmocitos, que produzem anticorpos.",
        2
    },

    {
        "Qual e a funcao dos linfocitos T citotoxicos?",
        {
            "Transportar oxigenio.",
            "Produzir plaquetas.",
            "Destruir celulas infectadas por virus.",
            "Produzir hemoglobina."
        },
        2,
        "Os linfocitos T citotoxicos reconhecem e destroem celulas infectadas ou alteradas.",
        2
    },

    {
        "O que e fagocitose?",
        {
            "A producao de anticorpos.",
            "O processo em que uma celula engloba particulas ou microrganismos.",
            "O transporte de oxigenio pelo sangue.",
            "A multiplicacao de hemacias."
        },
        1,
        "Na fagocitose, algumas celulas de defesa englobam e digerem microrganismos e particulas.",
        2
    },

    {
        "Qual e uma das funcoes dos macrofagos?",
        {
            "Produzir hemoglobina.",
            "Transportar nutrientes.",
            "Realizar fagocitose e apresentar antigenos.",
            "Controlar os batimentos cardiacos."
        },
        2,
        "Os macrofagos englobam microrganismos e podem apresentar fragmentos deles aos linfocitos T.",
        2
    },

    {
        "Onde sao produzidas a maioria das celulas do sangue?",
        {
            "Na medula ossea.",
            "No estomago.",
            "Nos pulmoes.",
            "Na bexiga."
        },
        0,
        "A medula ossea e o principal local de producao das celulas sanguineas.",
        2
    },

    {
        "Qual tipo de leucocito esta associado principalmente ao combate de parasitas e a algumas reacoes alergicas?",
        {
            "Hemacia.",
            "Eosinofilo.",
            "Plaqueta.",
            "Monocito."
        },
        1,
        "Os eosinofilos participam da defesa contra determinados parasitas e de processos alergicos.",
        2
    },

    {
        "Qual e uma funcao dos linfocitos T auxiliares, tambem chamados de CD4?",
        {
            "Coordenar respostas imunologicas por meio de sinais quimicos.",
            "Transportar oxigenio.",
            "Produzir fibrina.",
            "Realizar a coagulacao do sangue."
        },
        0,
        "Os linfocitos T CD4 ajudam a coordenar a resposta imunologica, liberando citocinas.",
        2
    },

    {
        "O que pode acontecer quando a quantidade de leucocitos diminui muito?",
        {
            "O organismo fica mais protegido contra infeccoes.",
            "O transporte de oxigenio aumenta automaticamente.",
            "A defesa do organismo contra infeccoes pode ficar comprometida.",
            "As plaquetas se transformam em hemacias."
        },
        2,
        "Uma quantidade muito baixa de leucocitos pode comprometer a capacidade do organismo de combater infeccoes.",
        2
    },


    /* ======================================
       FASE 3 - VIRUS E BACTERIAS
       Perguntas 21 a 30
       ====================================== */

    {
        "Por que os virus dependem de celulas hospedeiras para se reproduzir?",
        {
            "Porque possuem muitas celulas.",
            "Porque nao possuem estrutura celular nem maquinaria propria para se reproduzir.",
            "Porque precisam de oxigenio do sangue.",
            "Porque sao maiores que as bacterias."
        },
        1,
        "Os virus nao possuem estrutura celular e utilizam a maquinaria das celulas hospedeiras para produzir novas particulas virais.",
        3
    },

    {
        "Qual microrganismo e responsavel pela tuberculose?",
        {
            "Um fungo.",
            "Um virus.",
            "Uma bacteria.",
            "Um protozoario."
        },
        2,
        "A tuberculose e causada pela bacteria Mycobacterium tuberculosis.",
        3
    },

    {
        "Qual mosquito transmite principalmente o virus da dengue?",
        {
            "Aedes aegypti.",
            "Anopheles darlingi.",
            "Culex quinquefasciatus.",
            "Lutzomyia longipalpis."
        },
        0,
        "O Aedes aegypti e o principal mosquito transmissor da dengue nas areas urbanas do Brasil.",
        3
    },

    {
        "Por que os antibioticos nao sao utilizados para tratar infeccoes virais?",
        {
            "Porque nao conseguem chegar ao sangue.",
            "Porque atuam em estruturas e processos bacterianos que os virus nao possuem.",
            "Porque destroem todas as celulas humanas.",
            "Porque aumentam a multiplicacao dos virus."
        },
        1,
        "Os antibioticos atuam contra bacterias, nao contra virus. Infeccoes virais podem exigir outros tratamentos.",
        3
    },

    {
        "Qual caracteristica geralmente define as bacterias?",
        {
            "Sao organismos unicelulares procariontes.",
            "Sao sempre organismos multicelulares.",
            "Nao possuem material genetico.",
            "Precisam obrigatoriamente de outra celula para se reproduzir."
        },
        0,
        "As bacterias sao seres unicelulares procariontes, ou seja, nao possuem nucleo delimitado por membrana.",
        3
    },

    {
        "Qual afirmacao sobre os fungos esta correta?",
        {
            "Todos sao virus.",
            "Sao organismos eucariontes, podendo ser unicelulares ou multicelulares.",
            "Nao possuem material genetico.",
            "Sao sempre bacterias."
        },
        1,
        "Os fungos possuem celulas eucariontes e podem ser unicelulares, como as leveduras, ou multicelulares.",
        3
    },

    {
        "Qual e a funcao do capsideo em um virus?",
        {
            "Produzir energia.",
            "Proteger o material genetico viral.",
            "Transportar oxigenio.",
            "Realizar a divisao celular."
        },
        1,
        "O capsideo e uma estrutura formada por proteinas que envolve e protege o material genetico do virus.",
        3
    },

    {
        "O que significa resistencia bacteriana aos antibioticos?",
        {
            "As bacterias deixam de se reproduzir para sempre.",
            "Algumas bacterias conseguem sobreviver e se multiplicar mesmo na presenca de determinado antibiotico.",
            "Os antibioticos se transformam em virus.",
            "As bacterias passam a produzir oxigenio."
        },
        1,
        "Bacterias resistentes conseguem sobreviver a antibioticos que normalmente seriam capazes de combate-las.",
        3
    },

    {
        "Qual e a diferenca entre uma doenca contagiosa e uma nao transmissivel entre pessoas?",
        {
            "Todas as doencas sao contagiosas.",
            "Doencas contagiosas podem ser transmitidas entre pessoas, enquanto as nao transmissiveis nao passam diretamente de uma pessoa para outra.",
            "Doencas nao transmissiveis sao sempre causadas por virus.",
            "Doencas contagiosas nunca possuem tratamento."
        },
        1,
        "Doencas contagiosas podem ser transmitidas entre individuos por diferentes vias, enquanto outras nao sao transmitidas dessa forma.",
        3
    },

    {
        "O que e uma zoonose?",
        {
            "Uma doenca causada exclusivamente por fungos.",
            "Uma doenca transmitida apenas pela agua.",
            "Uma doenca que pode ser transmitida naturalmente entre animais vertebrados e seres humanos.",
            "Uma doenca que afeta somente plantas."
        },
        2,
        "Zoonoses sao doencas ou infeccoes que podem ser transmitidas naturalmente entre animais vertebrados e seres humanos.",
        3
    },


    /* ======================================
       FASE 4 - IMUNIDADE E PREVENCAO
       Perguntas 31 a 40
       ====================================== */

    {
        "Qual e a funcao dos anticorpos?",
        {
            "Transportar oxigenio.",
            "Reconhecer antigenos e ajudar na neutralizacao ou eliminacao de agentes estranhos.",
            "Produzir celulas musculares.",
            "Realizar a coagulacao do sangue."
        },
        1,
        "Os anticorpos reconhecem antigenos especificos e podem ajudar o organismo a combater agentes infecciosos.",
        4
    },

    {
        "Como as vacinas ajudam a proteger o organismo?",
        {
            "Eliminando todas as bacterias do corpo imediatamente.",
            "Substituindo os leucocitos.",
            "Estimulando uma resposta imunologica especifica e a formacao de memoria imunologica.",
            "Impedindo qualquer tipo de infeccao para sempre."
        },
        2,
        "As vacinas estimulam o sistema imunologico a reconhecer determinados antigenos e desenvolver memoria imunologica.",
        4
    },

    {
        "O que caracteriza a imunidade inata?",
        {
            "E uma defesa presente desde o nascimento e que responde rapidamente.",
            "Depende exclusivamente de vacinas.",
            "Reconhece apenas um unico virus.",
            "So aparece depois da adolescencia."
        },
        0,
        "A imunidade inata e a primeira linha de defesa do organismo e responde rapidamente a diferentes ameacas.",
        4
    },

    {
        "Qual e uma caracteristica da imunidade adaptativa?",
        {
            "Nao possui especificidade.",
            "Esta presente somente na pele.",
            "Atua apenas por meio das plaquetas.",
            "Apresenta especificidade e pode gerar memoria imunologica."
        },
        3,
        "A imunidade adaptativa reconhece antigenos especificos e pode desenvolver memoria para futuras exposicoes.",
        4
    },

    {
        "O que e memoria imunologica?",
        {
            "A capacidade das hemacias de transportar oxigenio.",
            "A capacidade do sistema imunologico de responder melhor a um antigeno ja encontrado.",
            "A capacidade dos virus de produzir anticorpos.",
            "O armazenamento de sangue no baco."
        },
        1,
        "A memoria imunologica permite que o organismo responda de maneira mais rapida e eficiente a exposicoes futuras.",
        4
    },

    {
        "O que e imunidade passiva?",
        {
            "A transferencia de anticorpos prontos para o organismo.",
            "A producao de anticorpos pelo proprio organismo apos uma vacina.",
            "A ausencia completa de defesa imunologica.",
            "A destruicao de todas as celulas de defesa."
        },
        0,
        "Na imunidade passiva, o organismo recebe anticorpos prontos, como acontece na transferencia materna ou no uso de imunoglobulinas.",
        4
    },

    {
        "O que sao citocinas?",
        {
            "Celulas que transportam oxigenio.",
            "Tipos de bacterias.",
            "Moleculas que permitem a comunicacao e regulacao das respostas imunologicas.",
            "Proteinas que formam as hemacias."
        },
        2,
        "As citocinas sao moleculas de sinalizacao que ajudam a regular e coordenar as respostas do sistema imunologico.",
        4
    },

    {
        "O que caracteriza uma alergia?",
        {
            "Uma resposta exagerada do sistema imunologico a uma substancia geralmente inofensiva.",
            "A ausencia total de leucocitos.",
            "Uma infeccao causada exclusivamente por bacterias.",
            "A producao excessiva de hemacias."
        },
        0,
        "A alergia acontece quando o sistema imunologico reage de maneira exagerada a substancias normalmente inofensivas.",
        4
    },

    {
        "O que acontece em uma doenca autoimune?",
        {
            "O organismo deixa de produzir sangue.",
            "O sistema imunologico ataca estruturas do proprio organismo.",
            "As bacterias atacam exclusivamente os ossos.",
            "Os anticorpos deixam de existir em todos os tecidos."
        },
        1,
        "Nas doencas autoimunes, o sistema imunologico reconhece erroneamente estruturas do proprio corpo como alvos.",
        4
    },

    {
        "Como a vacinacao coletiva pode proteger pessoas que nao podem receber determinadas vacinas?",
        {
            "Eliminando a necessidade de higiene.",
            "Impedindo a existencia de todos os microrganismos.",
            "Reduzindo a circulacao do agente infeccioso e diminuindo a exposicao dessas pessoas.",
            "Transformando a imunidade inata em imunidade passiva."
        },
        2,
        "Quando muitas pessoas estao imunizadas, a circulacao de determinados agentes pode diminuir, protegendo indiretamente pessoas vulneraveis.",
        4
    },


    /* ======================================
       FASE 5 - IMUNOLOGIA AVANCADA
       Perguntas 41 a 50
       ====================================== */

    {
        "O que acontece durante a apresentacao de antigenos?",
        {
            "As hemacias produzem anticorpos.",
            "Os virus se transformam em bacterias.",
            "Celulas especializadas exibem fragmentos de antigenos para serem reconhecidos pelos linfocitos T.",
            "As plaquetas destroem os linfocitos."
        },
        2,
        "Celulas apresentadoras de antigenos exibem fragmentos de antigenos para que os linfocitos T possam reconhece-los.",
        5
    },

    {
        "Qual e a funcao das moleculas do complexo principal de histocompatibilidade (MHC)?",
        {
            "Produzir energia para as celulas.",
            "Exibir fragmentos de antigenos na superficie celular para reconhecimento pelos linfocitos T.",
            "Transportar oxigenio no sangue.",
            "Produzir diretamente todas as celulas sanguineas."
        },
        1,
        "As moleculas MHC apresentam fragmentos de antigenos na superficie das celulas, permitindo o reconhecimento pelos linfocitos T.",
        5
    },

    {
        "Qual alternativa apresenta corretamente uma diferenca entre linfocitos T CD4 e CD8?",
        {
            "CD4 auxilia na coordenacao da resposta imunologica, enquanto CD8 pode destruir celulas infectadas.",
            "CD4 transporta oxigenio, enquanto CD8 realiza coagulacao.",
            "CD4 produz plaquetas, enquanto CD8 produz hemacias.",
            "Ambos possuem exclusivamente a funcao de transportar nutrientes."
        },
        0,
        "Os linfocitos T CD4 auxiliam na coordenacao da resposta imunologica, enquanto os CD8 podem destruir celulas infectadas.",
        5
    },

    {
        "O que e apoptose?",
        {
            "A multiplicacao de bacterias.",
            "A producao de anticorpos.",
            "Um processo regulado de morte celular programada.",
            "O transporte de oxigenio."
        },
        2,
        "A apoptose e um processo controlado de morte celular que ajuda a manter o equilibrio dos tecidos.",
        5
    },

    {
        "Qual e uma das funcoes do sistema complemento?",
        {
            "Produzir hemacias.",
            "Auxiliar na destruicao de patogenos, na inflamacao e na marcacao de agentes para fagocitose.",
            "Impedir toda resposta imunologica.",
            "Transportar hormonios pelo corpo."
        },
        1,
        "O sistema complemento e formado por proteinas que auxiliam na defesa, favorecendo a inflamacao e a eliminacao de patogenos.",
        5
    },

    {
        "Qual e a importancia das celulas de memoria?",
        {
            "Impedir a producao de anticorpos.",
            "Transformar virus em bacterias.",
            "Destruir todas as celulas do organismo.",
            "Permitir uma resposta mais rapida em um novo contato com o mesmo antigeno."
        },
        3,
        "As celulas de memoria permanecem no organismo apos uma resposta imunologica e ajudam a responder mais rapidamente em exposicoes futuras.",
        5
    },

    {
        "O que e uma mutacao genetica?",
        {
            "Uma alteracao na sequencia do material genetico.",
            "A producao de anticorpos.",
            "A divisao de uma hemacia.",
            "A eliminacao de todas as bacterias."
        },
        0,
        "Uma mutacao e uma alteracao na sequencia do DNA ou de outro material genetico.",
        5
    },

    {
        "Como a resistencia bacteriana aos antibioticos pode surgir?",
        {
            "Porque todas as bacterias decidem se tornar resistentes.",
            "Por mutacoes ou aquisicao de genes de resistencia, favorecidas pela selecao causada pelo uso de antibioticos.",
            "Porque os antibioticos se transformam em vacinas.",
            "Porque as bacterias deixam de possuir material genetico."
        },
        1,
        "Mutacoes e transferencia de genes podem gerar resistencia. O uso inadequado de antibioticos favorece a selecao de bacterias resistentes.",
        5
    },

    {
        "O que e uma resposta inflamatoria?",
        {
            "Um processo que acontece somente no cerebro.",
            "A producao de oxigenio pelas hemacias.",
            "Uma resposta do organismo a lesoes ou infeccoes, envolvendo celulas e mediadores de defesa.",
            "A interrupcao permanente da circulacao sanguinea."
        },
        2,
        "A inflamacao e uma resposta de defesa a lesoes e infeccoes, ajudando a combater a ameaca e iniciar a recuperacao dos tecidos.",
        5
    },

    {
        "Como funciona a resposta imunologica adaptativa?",
        {
            "Atua sem reconhecer antigenos.",
            "Depende somente das hemacias.",
            "Impede a ativacao dos linfocitos.",
            "Reconhece antigenos especificos e ativa linfocitos, podendo envolver anticorpos e celulas citotoxicas."
        },
        3,
        "A resposta adaptativa reconhece antigenos especificos e ativa linfocitos B e T, que participam da eliminacao dos agentes infecciosos.",
        5
    }

};


/* ==========================================
   FUNCAO: EMBARALHAR PERGUNTAS
   ========================================== */

void embaralharPerguntas(int vetor[], int tamanho) {

    for (int i = tamanho - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        int temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
}


/* ==========================================
   FUNCAO: SORTEAR PERGUNTAS DE UMA FASE
   ========================================== */

int sortearPerguntasDaFase(int fase, int indices[], int quantidade) {

    int candidatos[TOTAL_PERGUNTAS];
    int totalEncontradas = 0;

    /* Verifica quais perguntas pertencem a fase */
    for (int i = 0; i < TOTAL_PERGUNTAS; i++) {

        if (bancoPerguntas[i].fase == fase) {

            candidatos[totalEncontradas] = i;
            totalEncontradas++;
        }
    }

    /* Embaralha os indices encontrados */
    embaralharPerguntas(candidatos, totalEncontradas);

    /* Nao permite sortear mais perguntas do que existem */
    if (quantidade > totalEncontradas) {
        quantidade = totalEncontradas;
    }

    if (quantidade < 0) {
        quantidade = 0;
    }

    /* Copia os indices sorteados */
    for (int i = 0; i < quantidade; i++) {

        indices[i] = candidatos[i];
    }

    return quantidade;
}


/* ==========================================
   FUNCAO: VERIFICAR RESPOSTA
   ========================================== */

int verificarResposta(int indicePergunta, int resposta) {

    /* Verifica se o indice da pergunta e valido */
    if (indicePergunta < 0 || indicePergunta >= TOTAL_PERGUNTAS) {
        return 0;
    }

    /* Verifica se a alternativa escolhida e valida */
    if (resposta < 0 || resposta >= TOTAL_ALTERNATIVAS) {
        return 0;
    }

    /* Retorna 1 se estiver correta e 0 se estiver errada */
    return bancoPerguntas[indicePergunta].respostaCorreta == resposta;
}
