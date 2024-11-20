// fila.h
#ifndef FILA_H
    #define FILA_H
    
    // Definição da struct Processo
    typedef struct Processo {
        int id;
        int tempo_chegada;
        int tempo_servico;
        int tempo_executado;
        int fila_origem;             
        int tempo_inicio_io[3];       
        int tempo_retorno_io;  
        int atual_io; // juntar 
        int controle_inicio_io;
        
    } Processo;


    // Definição da struct Nó da fila (onde o processo e o próximo nó são armazenados)
    typedef struct No {
        Processo processo;  // Processo da fila
        struct No* prox;    // Ponteiro para o próximo nó
    } No;
    
    // Definição da fila
    typedef struct Fila {
        No* frente;  // Ponteiro para o início da fila
        No* traseira; // Ponteiro para o final da fila
    } Fila;
    
    // Funções para manipular a fila
    Fila* criar_fila();
    int fila_vazia(Fila* fila);
    void inserir(Fila* fila, Processo processo);
    void furar_fila(Fila* fila, Processo processo);
    Processo remover(Fila* fila);
    void exibir_fila(Fila* fila);
    int tamanho_fila(Fila* fila);
    
#endif  // FILA_H
