#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

#define DISCO 0 // Identifica Tipos de I/O. Usado nos Switch Cases e ternários
#define FITA 1
#define IMPRESSORA 2

#define TEMPO_DISCO 1 // Usado para saber quando o processo deve retornar do I/O. 
#define TEMPO_FITA 2
#define TEMPO_IMPRESSORA 2

#define FALSE 0 
#define TRUE 1
#define NONE -1

#define ALTA 0    
#define BAIXA 1

int ler_processos(const char* nome_arquivo, Processo** processos);
void carregar_processos_iniciais(Fila* fila_alta_prioridade, Processo* processos, int num_processos, int tempo);
void executar_processo_alta(Fila* fila_alta_prioridade, Fila* fila_baixa_prioridade, Fila* fila_io, int tempo, int* processos_concluidos,
                        Processo* processos, int num_processos, int * alta_concluido);
void executar_processo_baixa(Fila* fila_alta_prioridade, Fila* fila_baixa_prioridade, Fila* fila_io, int tempo, int* processos_concluidos,
                        Processo* processos, int num_processos);
int verificar_processos_iniciais(Processo* processos, int num_processos, int tempo);
void tratar_io(Fila* fila_io, Fila* fila_alta_prioridade, Fila* fila_baixa_prioridade, int* tempo);

void olhar_io_isolado_alta(Fila* fila_alta_prioridade, Fila* fila_io, int* tempo);
void olhar_io_isolado_baixa(Fila* fila_baixa_prioridade, Fila* fila_io, int* tempo);

int main(int argc, char *argv[]) {
    int tempo = 0;
    int processos_concluidos = 0;
    int fez_io = FALSE;
    int num_processos;
    int alta_concluido;

    // Criacao das filas
    Fila *fila_alta_prioridade = criar_fila();
    Fila *fila_baixa_prioridade = criar_fila();
    Fila *fila_io = criar_fila();
    
    // Carregar processos de um arquivo txt
    Processo* processos = NULL;
    num_processos = ler_processos("processos.txt", &processos);
    
    // Verifica se pelo menos um processo foi carregado
    if (num_processos == 0) {
        printf("Nenhum processo carregado. Encerrando...\n");
        return 1;
    }
    
    /* ESCALONADOR */
    while(processos_concluidos<num_processos)
    {
        alta_concluido=FALSE;
        printf("-------------------\n");
        printf("INSTANTE %d:\n", tempo);
        carregar_processos_iniciais(fila_alta_prioridade, processos, num_processos, tempo);

        if(!fila_vazia(fila_alta_prioridade)){
            olhar_io_isolado_alta(fila_alta_prioridade, fila_io, &tempo);
        }

        if(!fila_vazia(fila_baixa_prioridade)){
            olhar_io_isolado_baixa(fila_baixa_prioridade, fila_io, &tempo);
        }

        // Tratar processos na fila de I/O
        tratar_io(fila_io, fila_alta_prioridade, fila_baixa_prioridade, &tempo);
        
        // Executar processo de alta prioridade, se disponível
        if (!fila_vazia(fila_alta_prioridade)) {
            executar_processo_alta(fila_alta_prioridade, fila_baixa_prioridade, fila_io, tempo, &processos_concluidos,
                        processos, num_processos, &alta_concluido);
            
        } 
        if (fila_vazia(fila_alta_prioridade) && !fila_vazia(fila_baixa_prioridade) && !alta_concluido) {
            executar_processo_baixa(fila_alta_prioridade, fila_baixa_prioridade, fila_io, tempo, &processos_concluidos,
                        processos, num_processos);
        }
        tempo++;
    }
    printf("Escalonador Encerrou depois de %d ut\n", tempo);
    /* FIM ESCALONADOR */
  return 0;
}

void carregar_processos_iniciais(Fila* fila_alta_prioridade, Processo* processos, int num_processos, int tempo) {
    for (int i = 0; i < num_processos; i++) {
        if (processos[i].tempo_chegada == tempo ) {
            furar_fila(fila_alta_prioridade, processos[i]);
            printf("P%d iniciou e foi adicionado à fila de ALTA prioridade\n", processos[i].id);
        }
    }
}

int verificar_processos_iniciais(Processo* processos, int num_processos, int tempo){
    for (int i = 0; i < num_processos; i++) {
        if (processos[i].tempo_chegada == tempo ) {
            return TRUE;
        }
    }
    return FALSE;
}

void executar_processo_alta(Fila* fila_alta_prioridade, Fila* fila_baixa_prioridade, Fila* fila_io, int tempo, int* processos_concluidos,
                        Processo* processos, int num_processos, int * alta_concluido) {
    
    int fez_io = FALSE;
    Processo processo_executado = remover(fila_alta_prioridade);
    for (int i = 0; i < 3; i++) {
        if (processo_executado.tempo_executado == processo_executado.tempo_inicio_io[i]) {
            switch (i) {
            case DISCO:
                break;  
            case FITA:
                break;
            case IMPRESSORA:
                break;
            }
            fez_io = TRUE;
            break;
        }
    }

    if (!fez_io) {
        if (verificar_processos_iniciais(processos, num_processos, tempo) && tempo!=0){
            if (!fila_vazia(fila_alta_prioridade )){
                Processo processo_executado2 =  remover(fila_alta_prioridade); 
                printf("P%d realocado para fila de BAIXA prioridade\n",processo_executado2.id);
                inserir(fila_baixa_prioridade, processo_executado2);
            }            
            
        }
        processo_executado.tempo_executado++;
        printf("\nP%d está sendo executado na fila de ALTA prioridade\n", processo_executado.id);
        
        if (processo_executado.tempo_executado >= processo_executado.tempo_servico) {
            (*processos_concluidos)++;
            (*alta_concluido) = TRUE;
            printf("P%d concluído após %d ciclos de execução.\n",
                processo_executado.id,
                processo_executado.tempo_executado); 
            printf("Fila BAIXA: \n");
            exibir_fila(fila_baixa_prioridade);
            printf("Fila ALTA: \n");
            exibir_fila(fila_alta_prioridade);
            printf("Fila I/O: \n");
            exibir_fila(fila_io);
        } else if (fila_vazia(fila_alta_prioridade)) { // retorna true se estiver vazia
            // Mantém o processo na fila de Alta Prioridade se ainda não foi concluído
            inserir(fila_alta_prioridade, processo_executado);
            printf("Fila de ALTA está vazia. Pode ser executado mais 1 vez\n\n");
            printf("Fila BAIXA: \n");
            exibir_fila(fila_baixa_prioridade);
            printf("Fila ALTA: \n");
            exibir_fila(fila_alta_prioridade);
            printf("Fila I/O: \n");
            exibir_fila(fila_io);
        } else {
            // Insere o processo na fila de Baixa Prioridade se ainda não foi concluído
            inserir(fila_baixa_prioridade, processo_executado);
            printf("Existe outro processo na fila de ALTA. Foi preemptado para fila de BAIXA:\n\n");
            printf("Fila BAIXA: \n");
            exibir_fila(fila_baixa_prioridade);
            printf("Fila ALTA: \n");
            exibir_fila(fila_alta_prioridade);
            printf("Fila I/O: \n");
            exibir_fila(fila_io);
        }
    } 
}

void executar_processo_baixa(Fila* fila_alta_prioridade, Fila* fila_baixa_prioridade, Fila* fila_io, int tempo, int* processos_concluidos,
                        Processo* processos, int num_processos) {
    
    int fez_io = FALSE;
    Processo processo_executado = remover(fila_baixa_prioridade);
    for (int i = 0; i < 3; i++) {
        if (processo_executado.tempo_executado == processo_executado.tempo_inicio_io[i]) {
            switch (i) {
            case DISCO:
                break;
            case FITA:
                break;
            case IMPRESSORA:
                break;
            }
            fez_io = TRUE;
            break;
        }
    }

    if (!fez_io) {
        processo_executado.tempo_executado++;
        printf("\nP%d está sendo executado na fila de BAIXA prioridade\n", processo_executado.id);
        if (processo_executado.tempo_executado >= processo_executado.tempo_servico) {
            (*processos_concluidos)++;
            printf("P%d concluído após %d ciclos de execução.\n",
                processo_executado.id,
                processo_executado.tempo_executado);
            printf("Fila BAIXA: \n");
            exibir_fila(fila_baixa_prioridade);
            printf("Fila ALTA: \n");
            exibir_fila(fila_alta_prioridade);
            printf("Fila I/O: \n");
            exibir_fila(fila_io);
        } else {
            if(fila_vazia(fila_baixa_prioridade)){
                furar_fila(fila_baixa_prioridade, processo_executado);
                printf("Fila BAIXA está vazia. Pode ser executado mais 1 vez\n");
                printf("Fila BAIXA: \n");
                exibir_fila(fila_baixa_prioridade);
                printf("Fila ALTA: \n");
                exibir_fila(fila_alta_prioridade);
                printf("Fila I/O: \n");
                exibir_fila(fila_io);
            }
            else{
                inserir(fila_baixa_prioridade,processo_executado);
                printf("Fila BAIXA não está vazia, logo vai para o final da fila de BAIXA\n");
                printf("Fila BAIXA: \n");
                exibir_fila(fila_baixa_prioridade);
                printf("Fila ALTA: \n");
                exibir_fila(fila_alta_prioridade);
                printf("Fila I/O: \n");
                exibir_fila(fila_io);
                
            }
        }
    } 
}

void olhar_io_isolado_alta(Fila* fila_alta_prioridade, Fila* fila_io, int* tempo){
    int fez_io = FALSE;
    Processo processo_executado = remover(fila_alta_prioridade);
    for (int i = 0; i < 3; i++) {
        if (processo_executado.tempo_executado == processo_executado.tempo_inicio_io[i]) {
            switch (i) {
            case DISCO:
                processo_executado.tempo_retorno_io = *(tempo) + TEMPO_DISCO;
                processo_executado.controle_inicio_io = processo_executado.tempo_inicio_io[i];
                break;
            case FITA:
                processo_executado.tempo_retorno_io = *(tempo) + TEMPO_FITA;
                processo_executado.controle_inicio_io = processo_executado.tempo_inicio_io[i];
                break;
            case IMPRESSORA:
                processo_executado.tempo_retorno_io = *(tempo) + TEMPO_IMPRESSORA;
                processo_executado.controle_inicio_io = processo_executado.tempo_inicio_io[i];
                break;
            }
            processo_executado.atual_io = i;
            processo_executado.tempo_inicio_io[i] = NONE; // Prevenir repetição
            inserir(fila_io, processo_executado);
            printf("\nP%d movido da fila de ALTA para I/O (tipo %s) após executar por %d ciclos.\n",
                            processo_executado.id,
                            (i == ALTA ? "DISCO" : (i == BAIXA ? "FITA" : "IMPRESSORA")),
                            processo_executado.tempo_executado);
            fez_io = TRUE;
            break;
        }
    }
    if (!fez_io) furar_fila(fila_alta_prioridade,processo_executado);
}

// Mandar para fila de I/O caso seja necessário
void olhar_io_isolado_baixa(Fila* fila_baixa_prioridade, Fila* fila_io, int* tempo){
    int fez_io = FALSE;
    Processo processo_executado = remover(fila_baixa_prioridade);
    for (int i = 0; i < 3; i++) {
        if (processo_executado.tempo_executado == processo_executado.tempo_inicio_io[i]) {
            switch (i) {
            case DISCO:
                processo_executado.tempo_retorno_io = *(tempo) + TEMPO_DISCO;
                processo_executado.controle_inicio_io = processo_executado.tempo_inicio_io[i];
                break;
            case FITA:
                processo_executado.tempo_retorno_io = *(tempo) + TEMPO_FITA;
                processo_executado.controle_inicio_io = processo_executado.tempo_inicio_io[i];
                break;
            case IMPRESSORA:
                processo_executado.tempo_retorno_io = *(tempo) + TEMPO_IMPRESSORA;
                processo_executado.controle_inicio_io = processo_executado.tempo_inicio_io[i];
                break;
            }          
            processo_executado.atual_io = i;
            processo_executado.tempo_inicio_io[i] = NONE; // Prevenir repetição
            inserir(fila_io, processo_executado);
            printf("\nP%d movido da fila de BAIXA para I/O (tipo %s) após executar por %d ciclos.\n",
                            processo_executado.id,
                            (i == ALTA ? "DISCO" : (i == BAIXA ? "FITA" : "IMPRESSORA")),
                            processo_executado.tempo_executado);
            fez_io = TRUE;
            break;
        }
    }
    if (!fez_io) furar_fila(fila_baixa_prioridade,processo_executado);
}

// Função para tratar I/O
void tratar_io(Fila* fila_io, Fila* fila_alta_prioridade, Fila* fila_baixa_prioridade, int* tempo) {
    int tamanho_io = tamanho_fila(fila_io);
    int n = 0;
    int fez_io = FALSE;
    
    while(n++ < tamanho_io) { // Para cada tipo de io, DISCO, FITA e IMPRESSORA
        Processo processo_io = remover(fila_io);
        
        if (*(tempo) == processo_io.tempo_retorno_io) {
            if (processo_io.atual_io == DISCO) {
                furar_fila(fila_baixa_prioridade, processo_io);
            
            } else if(processo_io.atual_io == FITA){
                if (!fila_vazia(fila_alta_prioridade)){
                    Processo processo_fila_alta = remover(fila_alta_prioridade);
                    if (processo_fila_alta.tempo_executado==0){ // Testa se o 1° processo da fila Alta é novo
                            furar_fila(fila_alta_prioridade, processo_io); // io fura fila na frente do novo
                            furar_fila(fila_alta_prioridade,processo_fila_alta); // novo se vinga e fura fila do na frente do io
                    } 
                    else{
                        inserir(fila_baixa_prioridade, processo_fila_alta);
                    }  
                }
                    
                else{

                    furar_fila(fila_alta_prioridade, processo_io); // io fura fila e fica la
                } 

                
            
            } else if (processo_io.atual_io == IMPRESSORA){

                if (!fila_vazia(fila_alta_prioridade)){
                    Processo processo_fila_alta = remover(fila_alta_prioridade);
                    if (processo_fila_alta.tempo_executado==0){ // Testa se o 1° processo da fila Alta É NOVOOOOOOO
                            furar_fila(fila_alta_prioridade, processo_io); // io fura fila na frente do novo
                            furar_fila(fila_alta_prioridade,processo_fila_alta); // Novo se vinga e fura fila do io
                    } 
                    else{
                        inserir(fila_baixa_prioridade, processo_fila_alta);
                        
                    }  
                }

                else{
                    
                    furar_fila(fila_alta_prioridade, processo_io); // io fura fila e fica lá
                } 
                    
            }        
            printf("P%d voltou do I/O (tipo %s)\n", 
                    processo_io.id, 
                   (processo_io.atual_io == DISCO ? "DISCO" : (processo_io.atual_io == FITA ? "FITA" : "IMPRESSORA"))); 
        } else {
            inserir(fila_io, processo_io);
        }
    }
}

// Funcao para ler os processos de um arquivo txt
int ler_processos(const char* nome_arquivo, Processo** processos) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 0;  // Retorna 0 se nao for possivel abrir o arquivo
    }

    int num_processos = 0;
    Processo p;
    while (fscanf(arquivo, "%d %d %d %d %d %d",
                &p.id, &p.tempo_chegada, &p.tempo_servico,
                &p.tempo_inicio_io[0], &p.tempo_inicio_io[1], &p.tempo_inicio_io[2]) == 6) {
        
        p.tempo_executado = 0;
        p.tempo_retorno_io = 0;
        p.atual_io = NONE;
        p.fila_origem = 0;
        p.controle_inicio_io = 0;
        (*processos) = realloc(*processos, (num_processos + 1) * sizeof(Processo));
        (*processos)[num_processos++] = p;
    }

    fclose(arquivo);
    return num_processos;
}
