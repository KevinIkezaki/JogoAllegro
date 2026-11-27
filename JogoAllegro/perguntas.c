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
        "Qual é a principal função do sistema imunológico?",
        {
            "Controlar os movimentos do corpo.",
            "Defender o organismo contra agentes infecciosos.",
            "Produzir energia para as células.",
            "Transportar oxigênio pelo sangue."
        },
        1,
        "O sistema imunológico protege o organismo contra vírus, bactérias e outros agentes prejudiciais.",
        1
    },

    {
        "Qual componente do sangue transporta oxigênio?",
        {
            "Plaquetas.",
            "Leucócitos.",
            "Hemácias.",
            "Linfócitos."
        },
        2,
        "As hemácias possuem hemoglobina, proteína responsável pelo transporte de oxigênio.",
        1
    },

    {
        "Qual destas estruturas faz parte do sistema imunológico?",
        {
            "Baço.",
            "Estômago.",
            "Pâncreas.",
            "Bexiga."
        },
        0,
        "O baço participa da defesa imunológica e também filtra o sangue.",
        1
    },

    {
        "O que são microrganismos?",
        {
            "Apenas organismos que causam doenças.",
            "Seres ou entidades biológicas microscópicas.",
            "Células responsáveis pelo transporte de oxigênio.",
            "Substâncias produzidas pelo sangue."
        },
        1,
        "Existem microrganismos benéficos e prejudiciais. Nem todos provocam doenças.",
        1
    },

    {
        "Qual é o principal papel da pele na defesa do organismo?",
        {
            "Produzir anticorpos.",
            "Transportar células sanguíneas.",
            "Atuar como barreira física contra agentes externos.",
            "Destruir todos os vírus do corpo."
        },
        2,
        "A pele é uma das primeiras barreiras de proteção do organismo.",
        1
    },

    {
        "O que são leucócitos?",
        {
            "Células responsáveis pela defesa do organismo.",
            "Células responsáveis pela coagulação.",
            "Células que transportam oxigênio.",
            "Células que produzem hormônios."
        },
        0,
        "Os leucócitos, também chamados de glóbulos brancos, participam das respostas imunológicas.",
        1
    },

    {
        "Qual órgão é responsável por bombear o sangue pelo corpo?",
        {
            "Pulmão.",
            "Fígado.",
            "Rim.",
            "Coração."
        },
        3,
        "O coração bombeia o sangue, permitindo o transporte de oxigênio e nutrientes.",
        1
    },

    {
        "O que pode acontecer quando uma bactéria prejudicial invade o organismo?",
        {
            "O sistema imunológico pode reconhecer e combater a invasão.",
            "Todas as células deixam de funcionar imediatamente.",
            "O sangue para de circular.",
            "As hemácias se transformam em anticorpos."
        },
        0,
        "O sistema imunológico possui mecanismos capazes de identificar e combater bactérias.",
        1
    },

    {
        "Qual hábito ajuda a prevenir a transmissão de muitas doenças infecciosas?",
        {
            "Compartilhar objetos pessoais.",
            "Evitar beber água.",
            "Higienizar corretamente as mãos.",
            "Evitar dormir durante a noite."
        },
        2,
        "A higienização das mãos reduz a transmissão de diversos agentes infecciosos.",
        1
    },

    {
        "O que é uma infecção?",
        {
            "A entrada e multiplicação de agentes infecciosos no organismo.",
            "O aumento natural das hemácias.",
            "A produção de energia pelas células.",
            "A circulação normal do sangue."
        },
        0,
        "Uma infecção ocorre quando agentes como vírus, bactérias ou fungos entram no organismo e se multiplicam.",
        1
    },


    /* ======================================
       FASE 2 - CÉLULAS DO SANGUE
       Perguntas 11 a 20
       ====================================== */

    {
        "Qual é a principal função das plaquetas?",
        {
            "Transportar oxigênio.",
            "Auxiliar na coagulação do sangue.",
            "Combater diretamente os vírus.",
            "Produzir anticorpos."
        },
        1,
        "As plaquetas participam da coagulação, ajudando a interromper sangramentos.",
        2
    },

    {
        "Qual célula de defesa atua principalmente no combate a bactérias por meio da fagocitose?",
        {
            "Neutrófilo.",
            "Hemácia.",
            "Plaqueta.",
            "Linfócito B."
        },
        0,
        "Os neutrófilos são leucócitos que englobam e destroem principalmente bactérias.",
        2
    },

    {
        "O que os linfócitos B podem produzir quando ativados?",
        {
            "Hemoglobina.",
            "Plaquetas.",
            "Anticorpos.",
            "Insulina."
        },
        2,
        "Os linfócitos B podem se diferenciar em plasmócitos, que produzem anticorpos.",
        2
    },

    {
        "Qual é a função dos linfócitos T citotóxicos?",
        {
            "Transportar oxigênio.",
            "Produzir plaquetas.",
            "Destruir células infectadas por vírus.",
            "Produzir hemoglobina."
        },
        2,
        "Os linfócitos T citotóxicos reconhecem e destroem células infectadas ou alteradas.",
        2
    },

    {
        "O que é fagocitose?",
        {
            "A produção de anticorpos.",
            "O processo em que uma célula engloba partículas ou microrganismos.",
            "O transporte de oxigênio pelo sangue.",
            "A multiplicação de hemácias."
        },
        1,
        "Na fagocitose, algumas células de defesa englobam e digerem microrganismos e partículas.",
        2
    },

    {
        "Qual é uma das funções dos macrófagos?",
        {
            "Produzir hemoglobina.",
            "Transportar nutrientes.",
            "Realizar fagocitose e apresentar antígenos.",
            "Controlar os batimentos cardíacos."
        },
        2,
        "Os macrófagos englobam microrganismos e podem apresentar fragmentos deles aos linfócitos T.",
        2
    },

    {
        "Onde é produzida a maioria das células do sangue?",
        {
            "Na medula óssea.",
            "No estômago.",
            "Nos pulmões.",
            "Na bexiga."
        },
        0,
        "A medula óssea é o principal local de produção das células sanguíneas.",
        2
    },

    {
        "Qual tipo de leucócito está associado principalmente ao combate de parasitas e a algumas reações alérgicas?",
        {
            "Hemácia.",
            "Eosinófilo.",
            "Plaqueta.",
            "Monócito."
        },
        1,
        "Os eosinófilos participam da defesa contra determinados parasitas e de processos alérgicos.",
        2
    },

    {
        "Qual é uma função dos linfócitos T auxiliares, também chamados de CD4?",
        {
            "Coordenar respostas imunológicas por meio de sinais químicos.",
            "Transportar oxigênio.",
            "Produzir fibrina.",
            "Realizar a coagulação do sangue."
        },
        0,
        "Os linfócitos T CD4 ajudam a coordenar a resposta imunológica, liberando citocinas.",
        2
    },

    {
        "O que pode acontecer quando a quantidade de leucócitos diminui muito?",
        {
            "O organismo fica mais protegido contra infecções.",
            "O transporte de oxigênio aumenta automaticamente.",
            "A defesa do organismo contra infecções pode ficar comprometida.",
            "As plaquetas se transformam em hemácias."
        },
        2,
        "Uma quantidade muito baixa de leucócitos pode comprometer a capacidade do organismo de combater infecções.",
        2
    },


    /* ======================================
       FASE 3 - VÍRUS E BACTÉRIAS
       Perguntas 21 a 30
       ====================================== */

    {
        "Por que os vírus dependem de células hospedeiras para se reproduzir?",
        {
            "Porque possuem muitas células.",
            "Porque não possuem estrutura celular nem maquinaria própria para se reproduzir.",
            "Porque precisam de oxigênio do sangue.",
            "Porque são maiores que as bactérias."
        },
        1,
        "Os vírus não possuem estrutura celular e utilizam a maquinaria das células hospedeiras para produzir novas partículas virais.",
        3
    },

    {
        "Qual microrganismo é responsável pela tuberculose?",
        {
            "Um fungo.",
            "Um vírus.",
            "Uma bactéria.",
            "Um protozoário."
        },
        2,
        "A tuberculose é causada pela bactéria Mycobacterium tuberculosis.",
        3
    },

    {
        "Qual mosquito transmite principalmente o vírus da dengue?",
        {
            "Aedes aegypti.",
            "Anopheles darlingi.",
            "Culex quinquefasciatus.",
            "Lutzomyia longipalpis."
        },
        0,
        "O Aedes aegypti é o principal mosquito transmissor da dengue nas áreas urbanas do Brasil.",
        3
    },

    {
        "Por que os antibióticos não são utilizados para tratar infecções virais?",
        {
            "Porque não conseguem chegar ao sangue.",
            "Porque atuam em estruturas e processos bacterianos que os vírus não possuem.",
            "Porque destroem todas as células humanas.",
            "Porque aumentam a multiplicação dos vírus."
        },
        1,
        "Os antibióticos atuam contra bactérias, não contra vírus. Infecções virais podem exigir outros tratamentos.",
        3
    },

    {
        "Qual característica geralmente define as bactérias?",
        {
            "São organismos unicelulares procariontes.",
            "São sempre organismos multicelulares.",
            "Não possuem material genético.",
            "Precisam obrigatoriamente de outra célula para se reproduzir."
        },
        0,
        "As bactérias são seres unicelulares procariontes, ou seja, não possuem núcleo delimitado por membrana.",
        3
    },

    {
        "Qual afirmação sobre os fungos está correta?",
        {
            "Todos são vírus.",
            "São organismos eucariontes, podendo ser unicelulares ou multicelulares.",
            "Não possuem material genético.",
            "São sempre bactérias."
        },
        1,
        "Os fungos possuem células eucariontes e podem ser unicelulares, como as leveduras, ou multicelulares.",
        3
    },

    {
        "Qual é a função do capsídeo em um vírus?",
        {
            "Produzir energia.",
            "Proteger o material genético viral.",
            "Transportar oxigênio.",
            "Realizar a divisão celular."
        },
        1,
        "O capsídeo é uma estrutura formada por proteínas que envolve e protege o material genético do vírus.",
        3
    },

    {
        "O que significa resistência bacteriana aos antibióticos?",
        {
            "As bactérias deixam de se reproduzir para sempre.",
            "Algumas bactérias conseguem sobreviver e se multiplicar mesmo na presença de determinado antibiótico.",
            "Os antibióticos se transformam em vírus.",
            "As bactérias passam a produzir oxigênio."
        },
        1,
        "Bactérias resistentes conseguem sobreviver a antibióticos que normalmente seriam capazes de combatê-las.",
        3
    },

    {
        "Qual é a diferença entre uma doença contagiosa e uma não transmissível entre pessoas?",
        {
            "Todas as doenças são contagiosas.",
            "Doenças contagiosas podem ser transmitidas entre pessoas, enquanto as não transmissíveis não passam diretamente de uma pessoa para outra.",
            "Doenças não transmissíveis são sempre causadas por vírus.",
            "Doenças contagiosas nunca possuem tratamento."
        },
        1,
        "Doenças contagiosas podem ser transmitidas entre indivíduos por diferentes vias, enquanto outras não são transmitidas dessa forma.",
        3
    },

    {
        "O que é uma zoonose?",
        {
            "Uma doença causada exclusivamente por fungos.",
            "Uma doença transmitida apenas pela água.",
            "Uma doença que pode ser transmitida naturalmente entre animais vertebrados e seres humanos.",
            "Uma doença que afeta somente plantas."
        },
        2,
        "Zoonoses são doenças ou infecções que podem ser transmitidas naturalmente entre animais vertebrados e seres humanos.",
        3
    },


    /* ======================================
       FASE 4 - IMUNIDADE E PREVENÇÃO
       Perguntas 31 a 40
       ====================================== */

    {
        "Qual é a função dos anticorpos?",
        {
            "Transportar oxigênio.",
            "Reconhecer antígenos e ajudar na neutralização ou eliminação de agentes estranhos.",
            "Produzir células musculares.",
            "Realizar a coagulação do sangue."
        },
        1,
        "Os anticorpos reconhecem antígenos específicos e podem ajudar o organismo a combater agentes infecciosos.",
        4
    },

    {
        "Como as vacinas ajudam a proteger o organismo?",
        {
            "Eliminando todas as bactérias do corpo imediatamente.",
            "Substituindo os leucócitos.",
            "Estimulando uma resposta imunológica específica e a formação de memória imunológica.",
            "Impedindo qualquer tipo de infecção para sempre."
        },
        2,
        "As vacinas estimulam o sistema imunológico a reconhecer determinados antígenos e desenvolver memória imunológica.",
        4
    },

    {
        "O que caracteriza a imunidade inata?",
        {
            "É uma defesa presente desde o nascimento e que responde rapidamente.",
            "Depende exclusivamente de vacinas.",
            "Reconhece apenas um único vírus.",
            "Só aparece depois da adolescência."
        },
        0,
        "A imunidade inata é a primeira linha de defesa do organismo e responde rapidamente a diferentes ameaças.",
        4
    },

    {
        "Qual é uma característica da imunidade adaptativa?",
        {
            "Não possui especificidade.",
            "Está presente somente na pele.",
            "Atua apenas por meio das plaquetas.",
            "Apresenta especificidade e pode gerar memória imunológica."
        },
        3,
        "A imunidade adaptativa reconhece antígenos específicos e pode desenvolver memória para futuras exposições.",
        4
    },

    {
        "O que é memória imunológica?",
        {
            "A capacidade das hemácias de transportar oxigênio.",
            "A capacidade do sistema imunológico de responder melhor a um antígeno já encontrado.",
            "A capacidade dos vírus de produzir anticorpos.",
            "O armazenamento de sangue no baço."
        },
        1,
        "A memória imunológica permite que o organismo responda de maneira mais rápida e eficiente a exposições futuras.",
        4
    },

    {
        "O que é imunidade passiva?",
        {
            "A transferência de anticorpos prontos para o organismo.",
            "A produção de anticorpos pelo próprio organismo após uma vacina.",
            "A ausência completa de defesa imunológica.",
            "A destruição de todas as células de defesa."
        },
        0,
        "Na imunidade passiva, o organismo recebe anticorpos prontos, como acontece na transferência materna ou no uso de imunoglobulinas.",
        4
    },

    {
        "O que são citocinas?",
        {
            "Células que transportam oxigênio.",
            "Tipos de bactérias.",
            "Moléculas que permitem a comunicação e regulação das respostas imunológicas.",
            "Proteínas que formam as hemácias."
        },
        2,
        "As citocinas são moléculas de sinalização que ajudam a regular e coordenar as respostas do sistema imunológico.",
        4
    },

    {
        "O que caracteriza uma alergia?",
        {
            "Uma resposta exagerada do sistema imunológico a uma substância geralmente inofensiva.",
            "A ausência total de leucócitos.",
            "Uma infecção causada exclusivamente por bactérias.",
            "A produção excessiva de hemácias."
        },
        0,
        "A alergia acontece quando o sistema imunológico reage de maneira exagerada a substâncias normalmente inofensivas.",
        4
    },

    {
        "O que acontece em uma doença autoimune?",
        {
            "O organismo deixa de produzir sangue.",
            "O sistema imunológico ataca estruturas do próprio organismo.",
            "As bactérias atacam exclusivamente os ossos.",
            "Os anticorpos deixam de existir em todos os tecidos."
        },
        1,
        "Nas doenças autoimunes, o sistema imunológico reconhece erroneamente estruturas do próprio corpo como alvos.",
        4
    },

    {
        "Como a vacinação coletiva pode proteger pessoas que não podem receber determinadas vacinas?",
        {
            "Eliminando a necessidade de higiene.",
            "Impedindo a existência de todos os microrganismos.",
            "Reduzindo a circulação do agente infeccioso e diminuindo a exposição dessas pessoas.",
            "Transformando a imunidade inata em imunidade passiva."
        },
        2,
        "Quando muitas pessoas estão imunizadas, a circulação de determinados agentes pode diminuir, protegendo indiretamente pessoas vulneráveis.",
        4
    },


    /* ======================================
       FASE 5 - IMUNOLOGIA AVANÇADA
       Perguntas 41 a 50
       ====================================== */

    {
        "O que acontece durante a apresentação de antígenos?",
        {
            "As hemácias produzem anticorpos.",
            "Os vírus se transformam em bactérias.",
            "Células especializadas exibem fragmentos de antígenos para serem reconhecidos pelos linfócitos T.",
            "As plaquetas destroem os linfócitos."
        },
        2,
        "Células apresentadoras de antígenos exibem fragmentos de antígenos para que os linfócitos T possam reconhecê-los.",
        5
    },

    {
        "Qual é a função das moléculas do complexo principal de histocompatibilidade (MHC)?",
        {
            "Produzir energia para as células.",
            "Exibir fragmentos de antígenos na superfície celular para reconhecimento pelos linfócitos T.",
            "Transportar oxigênio no sangue.",
            "Produzir diretamente todas as células sanguíneas."
        },
        1,
        "As moléculas MHC apresentam fragmentos de antígenos na superfície das células, permitindo o reconhecimento pelos linfócitos T.",
        5
    },

    {
        "Qual alternativa apresenta corretamente uma diferença entre linfócitos T CD4 e CD8?",
        {
            "CD4 auxilia na coordenação da resposta imunológica, enquanto CD8 pode destruir células infectadas.",
            "CD4 transporta oxigênio, enquanto CD8 realiza coagulação.",
            "CD4 produz plaquetas, enquanto CD8 produz hemácias.",
            "Ambos possuem exclusivamente a função de transportar nutrientes."
        },
        0,
        "Os linfócitos T CD4 auxiliam na coordenação da resposta imunológica, enquanto os CD8 podem destruir células infectadas.",
        5
    },

    {
        "O que é apoptose?",
        {
            "A multiplicação de bactérias.",
            "A produção de anticorpos.",
            "Um processo regulado de morte celular programada.",
            "O transporte de oxigênio."
        },
        2,
        "A apoptose é um processo controlado de morte celular que ajuda a manter o equilíbrio dos tecidos.",
        5
    },

    {
        "Qual é uma das funções do sistema complemento?",
        {
            "Produzir hemácias.",
            "Auxiliar na destruição de patógenos, na inflamação e na marcação de agentes para fagocitose.",
            "Impedir toda resposta imunológica.",
            "Transportar hormônios pelo corpo."
        },
        1,
        "O sistema complemento é formado por proteínas que auxiliam na defesa, favorecendo a inflamação e a eliminação de patógenos.",
        5
    },

    {
        "Qual é a importância das células de memória?",
        {
            "Impedir a produção de anticorpos.",
            "Transformar vírus em bactérias.",
            "Destruir todas as células do organismo.",
            "Permitir uma resposta mais rápida em um novo contato com o mesmo antígeno."
        },
        3,
        "As células de memória permanecem no organismo após uma resposta imunológica e ajudam a responder mais rapidamente em exposições futuras.",
        5
    },

    {
        "O que é uma mutação genética?",
        {
            "Uma alteração na sequência do material genético.",
            "A produção de anticorpos.",
            "A divisão de uma hemácia.",
            "A eliminação de todas as bactérias."
        },
        0,
        "Uma mutação é uma alteração na sequência do DNA ou de outro material genético.",
        5
    },

    {
        "Como a resistência bacteriana aos antibióticos pode surgir?",
        {
            "Porque todas as bactérias decidem se tornar resistentes.",
            "Por mutações ou aquisição de genes de resistência, favorecidas pela seleção causada pelo uso de antibióticos.",
            "Porque os antibióticos se transformam em vacinas.",
            "Porque as bactérias deixam de possuir material genético."
        },
        1,
        "Mutações e transferência de genes podem gerar resistência. O uso inadequado de antibióticos favorece a seleção de bactérias resistentes.",
        5
    },

    {
        "O que é uma resposta inflamatória?",
        {
            "Um processo que acontece somente no cérebro.",
            "A produção de oxigênio pelas hemácias.",
            "Uma resposta do organismo a lesões ou infecções, envolvendo células e mediadores de defesa.",
            "A interrupção permanente da circulação sanguínea."
        },
        2,
        "A inflamação é uma resposta de defesa a lesões e infecções, ajudando a combater a ameaça e iniciar a recuperação dos tecidos.",
        5
    },

    {
        "Como funciona a resposta imunológica adaptativa?",
        {
            "Atua sem reconhecer antígenos.",
            "Depende somente das hemácias.",
            "Impede a ativação dos linfócitos.",
            "Reconhece antígenos específicos e ativa linfócitos, podendo envolver anticorpos e células citotóxicas."
        },
        3,
        "A resposta adaptativa reconhece antígenos específicos e ativa linfócitos B e T, que participam da eliminação dos agentes infecciosos.",
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

    for (int i = 0; i < TOTAL_PERGUNTAS; i++) {

        if (bancoPerguntas[i].fase == fase) {

            candidatos[totalEncontradas] = i;
            totalEncontradas++;
        }
    }

    embaralharPerguntas(candidatos, totalEncontradas);

    if (quantidade > totalEncontradas) {
        quantidade = totalEncontradas;
    }

    if (quantidade < 0) {
        quantidade = 0;
    }

    for (int i = 0; i < quantidade; i++) {

        indices[i] = candidatos[i];
    }

    return quantidade;
}


/* ==========================================
   FUNCAO: VERIFICAR RESPOSTA
   ========================================== */

int verificarResposta(int indicePergunta, int resposta) {

    if (indicePergunta < 0 || indicePergunta >= TOTAL_PERGUNTAS) {
        return 0;
    }

    if (resposta < 0 || resposta >= TOTAL_ALTERNATIVAS) {
        return 0;
    }

    return bancoPerguntas[indicePergunta].respostaCorreta == resposta;
}