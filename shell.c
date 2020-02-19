// STDIN, STDOUT, STDERR
/* Em UNIX, um arquivo pode ser...
    - Arquivo convencional
    - Dispositivo
    - Pipe

    Ignorar SIGHUP
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MAX_LIMIT 50

typedef struct argumentosComando {
    char** argv;
    int argc;
    int executaEmBackground;
} tipoComando;

void remove_newline_ch(char* line){
    // O fgets acaba incluindo caracter de nova linha
    // Rotina gambi pra remover ela
    int new_line = strlen(line) -1;
    if (line[new_line] == '\n')
        line[new_line] = '\0';
}

int dump_file_to_str_array(char* filename, char** str_arr){
    // Dump file into array and return the array size
    char * line = NULL;
    int j=0;
    size_t len = 0;
    ssize_t read;
    int pathQuantity = 0;
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    
    while ((read = getline(&line, &len, fp)) != -1) {
        remove_newline_ch(line);
        str_arr[j] = strcat(strdup(line), "/");
        j++;
    }
    fclose(fp);
    return j;
}


tipoComando monta_vetor_comando(char* usr_input){
    /*
        Fatia o input por espaço e devolve um vetor com cada pedaço
        O vetor também deverá contar com o valor null na última posição
        O valor nulo se dá por uma necessidade da API de execução de processos
    */
   int i, executaEmBackground=0, argcount=0;
   remove_newline_ch(usr_input);
   for(i = 0; i < strlen(usr_input); i++){
       if(usr_input[i] == ' ')
         argcount++;
   }
   ++argcount; // Conta corretamente pois o ultimo argumento n é um espaço
   char** argvector = (char**) malloc (sizeof(char*) * argcount);
   for(i=0; i<argcount;i++){
       argvector[i] = (char*) malloc (sizeof(char) * 50);
   }

   char ultimoCaractere = usr_input[strlen(usr_input) -1];
   if (ultimoCaractere == '&'){
       executaEmBackground = 1;
       usr_input[strlen(usr_input) -1] = ' ';
   }

   i=0;
   argvector[i] = strtok(usr_input, " ");
   while(argvector[i] != NULL){
       argvector[++i] = strtok(NULL, " ");
   }

   tipoComando comando;
   comando.argv = argvector;
   comando.argc = argcount;
   comando.executaEmBackground = executaEmBackground;
   return comando;
}

int main(int argc, char const *argv[])
{
    int pid, status, i;
    char arg_input[MAX_LIMIT];
    tipoComando comando;
    fgets(arg_input, MAX_LIMIT, stdin);

    comando = monta_vetor_comando(arg_input);


    pid = fork(); // Cria um novo processo, filho = 0; pai = pid real
    if(pid != 0){
        if(comando.executaEmBackground)
            main(argc, argv);
        wait(&status);
    }else{
        int errno = 0, i;
        char* pathList[50];
        char comandoPuro[50];
        strcpy(comandoPuro, comando.argv[0]);

        int dirsCount = dump_file_to_str_array("path_list.txt", pathList);

        for(i=0;i<dirsCount;i++){
            comando.argv[0] = (char*) malloc (sizeof(char) * 50);
            strcpy(comando.argv[0], strcat(pathList[i], comandoPuro));
            errno = execv(comando.argv[0], comando.argv); 
            free(comando.argv[0]);
        }

        printf("Comando não encontrado (tente novamente)\n");
    }

    return 0;
}

