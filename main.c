#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

//Estudiantes:
//David Serrano Medrano
//Joseph León Cabezas
//Raquel Hernandez Campos

#define max_archivo 100
#define max_Text 1000
#define max_palabra 100
#define max_linea 100
 
char palabras_validas[][max_palabra] = {
  "hago", "hice", "hicieron", "hizo","digo", "dije", "dijeron", "dijo","voy", "fui", "fueron", "fue","veo", "vi", "vieron", "vio",
 "estoy", "estuve", "estuvieron", "estuvo","tengo", "tuve", "tenia", "tendre","yo","el","ella","ellos","ellas","usted",
 "ustedes","nosotros","nosotras","tarea","a casa","comida","trabajo","television","auto","jardín", "eso","televisor", "teléfono", "silla","ventana","puerta",
"planta","el reloj", "casa","cuadro","ropa","cama","papel"
};

char verbos[40][20]={
 "hago", "hice", "hicieron", "hizo","digo", "dije", "dijeron", "dijo","voy", "fui", "fueron", "fue","veo", "vi", "vieron", "vio",
 "estoy", "estuve", "estuvieron", "estuvo","tengo", "tuve", "tenia", "tendre"
};

char pronombres[40][20]={
 "yo","el","ella","ellos","ellas","usted",
 "ustedes","nosotros","nosotras"
};

char complementos[40][20]={
"tarea","a casa","comida","trabajo","television","auto","jardín", "eso","televisor", "teléfono", "silla","ventana","puerta",
"planta","el reloj", "casa","cuadro","ropa","cama","papel"
};

struct datosArchivo{
     char nom_archivo[max_archivo];
     char *contenido;
     char palabra_buscar[max_palabra];
     char palabra_remplazar[max_palabra];
     char estado[20];
     pthread_mutex_t mutex_contenido;
};

//Limpieza de buffer
void limpiar_buffer();

//Interfaz
void menu_principal();
void menu_opciones(int);

//Proceso
void* proceso_CrearArchivo(void*);
void* proceso_LeerArchivo(void*);
void* proceso_ModificarArchivo(void*);
void* proceso_EditarArchivo(void*);

//Hilos
void* obtener_nombre(void *);
void* obtener_contenido(void*);
void* analizador_ortografico(void*);
void* crear_archivo(void*);
void* leer_archivo(void*);
void* editar_archivo(void*);
void* obtener_palabraBuscar(void*);
void* obtener_palabraRemplazar(void*);
void* analizador_ortografico_palabra(void*);
void* modificar_archivo(void*);
void* analizador_semantico(void*);

//Funciones
//Analizador ortografico
int comparar_palabras(const char *, const char *);
char *obtener_palabra_mas_cercana(const char *);
//Analizador semantico
int esVerbo(char*);
int esPronombre(char*);
int esComplemento(char*);


int main() {
    menu_principal();
    printf("Gracias por usar la aplicación\n");
    return 0;
    
}

void limpiar_buffer() {
    int clean;
    while ((clean = getchar()) != '\n' && clean != EOF);
}

void menu_principal() {
    int op;
    do {
        printf("\033[H\033[2J");
        printf("-------------- EDITOR DE TEXTO-----------------\n\n");
        printf("1---------Crear archivo\n");
        printf("2---------Leer archivo\n");
        printf("3---------Editar Contenido de Archivo\n");
        printf("4---------Seguir escribiendo en el archivo\n");
        printf("5---------Salir\n\n");
        
        printf("Selecciona una opción....\n");
        scanf("%d", &op);
        printf("\n\n");
        menu_opciones(op);
    } while (op != 5);
}


void menu_opciones(int op) {
int error;
    switch (op) {
        case 1:
         pthread_t hiloProcesoCrearArchivo;
        pthread_create(&hiloProcesoCrearArchivo, NULL, proceso_CrearArchivo, NULL);
        pthread_join(hiloProcesoCrearArchivo, NULL); 
             break; 
        case 2:
         pthread_t hiloProcesoLeerArchivo;
        pthread_create(&hiloProcesoLeerArchivo, NULL, proceso_LeerArchivo, NULL);
        pthread_join(hiloProcesoLeerArchivo, NULL); 
            break;
        case 3:
          pthread_t hiloProcesoModificarArchivo;
        pthread_create(&hiloProcesoModificarArchivo, NULL, proceso_ModificarArchivo, NULL);
        pthread_join(hiloProcesoModificarArchivo, NULL);
            break;
        case 4:
               pthread_t hiloProcesoEditarArchivo;
        pthread_create(&hiloProcesoEditarArchivo, NULL, proceso_EditarArchivo, NULL);
        pthread_join(hiloProcesoEditarArchivo, NULL);
             break;
             
         case 5:break;
        
        default:
            printf("La opcion elegida es incorrecta\n\n");
            limpiar_buffer();
            break;
    }
}


//Hilos

void * obtener_nombre(void *arg){
     struct datosArchivo *datos = (struct datosArchivo*)arg;
     pthread_mutex_lock(&datos->mutex_contenido);
     
     limpiar_buffer();
      if (strcmp(datos->estado, "crear") == 0) {
        printf("Introduce el nuevo archivo: ");
    }
    
    else if(strcmp(datos->estado, "leer") == 0){
        printf("Introduce el nombre del archivo que deseas leer: ");
    }
    
    else if(strcmp(datos->estado, "editar") == 0){
        printf("Introduce el nombre del archivo en el que deseas continuar escribiendo: ");
    }
    
    else if(strcmp(datos->estado, "modificar") == 0){
        printf("Introduce el nombre del archivo que deseas modificar: ");
    }
    
    fgets(datos->nom_archivo, max_archivo, stdin);
    datos->nom_archivo[strcspn(datos->nom_archivo, "\n")] = '\0';
    strcat(datos->nom_archivo, ".txt");
    
    
     pthread_mutex_unlock(&datos->mutex_contenido);
     pthread_exit(NULL);
}

void* obtener_contenido(void* arg){
       struct datosArchivo *datos = (struct datosArchivo*)arg;
       char *contenido = (char *)malloc(max_Text * sizeof(char));
       char caracter;
    int i = 0;
    pthread_mutex_lock(&datos->mutex_contenido);
       
        if (contenido == NULL) {
        printf("Error: No se pudo asignar memoria para el contenido.\n");
         limpiar_buffer();
         pthread_mutex_unlock(&datos->mutex_contenido);
         pthread_exit(NULL);
    }
       
    printf("\033[H\033[2J");
    printf("Introduce el texto a escribir en el archivo\n");
    printf("Presiona la tecla ESC y ENTER para guardar y salir\n");
     
            
     while ((caracter = getchar()) != 27 && i < max_Text - 1) {
        contenido[i] = caracter;
        i++;
    }
    
    contenido[i] = '\0'; 
    datos->contenido = contenido;
    
     pthread_mutex_unlock(&datos->mutex_contenido);
     pthread_exit(NULL);

}

void* analizador_ortografico(void* arg){
    struct datosArchivo *datos = (struct datosArchivo*)arg;
    char* token = strtok(datos->contenido, " ");
     char *contenido = (char *)malloc(max_Text * sizeof(char));
    
    pthread_mutex_lock(&datos->mutex_contenido);
    while (token != NULL) {
        char* palabra_mas_cercana = obtener_palabra_mas_cercana(token) ;
       if (palabra_mas_cercana != NULL) {
            strcat(contenido, palabra_mas_cercana);
            strcat(contenido, " "); 
        }
        
        token = strtok(NULL, " ");
    }
    
     free(datos->contenido);
     datos->contenido = contenido;
     
     pthread_mutex_unlock(&datos->mutex_contenido);
     pthread_exit(NULL);
}


void* crear_archivo(void* arg){
      struct datosArchivo *datos = (struct datosArchivo*)arg;
       pthread_mutex_lock(&datos->mutex_contenido);
      FILE* archivo = fopen(datos->nom_archivo,"w");
      if (archivo == NULL){
         printf("Error al abrir el archivo.\n");
         limpiar_buffer();
         pthread_mutex_unlock(&datos->mutex_contenido); 
         pthread_exit(NULL);
      }
      
      fprintf(archivo,"%s",datos->contenido);
      fclose(archivo);
       printf("Archivo creado correctamente... ");
        limpiar_buffer();
      pthread_mutex_unlock(&datos->mutex_contenido); 
        pthread_exit(NULL);
}


void* leer_archivo(void* arg){
    struct datosArchivo *datos = (struct datosArchivo*)arg;
     char linea[max_linea];
       pthread_mutex_lock(&datos->mutex_contenido);
      FILE* archivo = fopen(datos->nom_archivo,"r");
      if (archivo == NULL){
         printf("Error al abrir el archivo.\n");
          limpiar_buffer();
         pthread_mutex_unlock(&datos->mutex_contenido); 
         pthread_exit(NULL);
      }
      
      while (fgets(linea, sizeof(linea), archivo) != NULL) {
        printf("%s", linea);
    }
    fclose(archivo);
    printf("\n\n");
    printf("Archivo procesado correctamente... ");
    limpiar_buffer();
    pthread_mutex_unlock(&datos->mutex_contenido); 
    pthread_exit(NULL);
}

void* editar_archivo(void* arg){
      struct datosArchivo *datos = (struct datosArchivo*)arg;
       pthread_mutex_lock(&datos->mutex_contenido);
      FILE* archivo = fopen(datos->nom_archivo,"r+");
      if (archivo == NULL){
        limpiar_buffer();
         pthread_mutex_unlock(&datos->mutex_contenido); 
         pthread_exit(NULL);
      }
      
      fseek(archivo, 0, SEEK_END);
      fprintf(archivo,"\n");
      fprintf(archivo,"%s",datos->contenido);
      fclose(archivo);
      printf("Archivo editado correctamente... ");
    limpiar_buffer();
      pthread_mutex_unlock(&datos->mutex_contenido); 
      pthread_exit(NULL);
}


void* obtener_palabraRemplazar(void* arg){
       struct datosArchivo *datos = (struct datosArchivo*)arg;
       pthread_mutex_lock(&datos->mutex_contenido);
        printf("\n\n");
       printf("Introduce la palabra de reemplazo\n");
    printf("Nota: Si deseas eliminarla solo teclea ENTER: ");
    fgets(datos->palabra_remplazar, sizeof(datos->palabra_remplazar), stdin);
    datos->palabra_remplazar[strcspn(datos->palabra_remplazar, "\n")] = '\0';
    pthread_mutex_unlock(&datos->mutex_contenido); 
      pthread_exit(NULL);
}

void* obtener_palabraBuscar(void* arg){
       struct datosArchivo *datos = (struct datosArchivo*)arg;
       pthread_mutex_lock(&datos->mutex_contenido);
       
       printf("\n\n");
        printf("Introduce la palabra que deseas reemplazar: ");
    fgets(datos->palabra_buscar, sizeof(datos->palabra_buscar), stdin);
    datos->palabra_buscar[strcspn(datos->palabra_buscar, "\n")] = '\0';
    pthread_mutex_unlock(&datos->mutex_contenido); 
      pthread_exit(NULL);
}

void* analizador_ortografico_palabra(void* arg){
     struct datosArchivo *datos = (struct datosArchivo*)arg;
    pthread_mutex_lock(&datos->mutex_contenido);
    
        if (datos->palabra_remplazar[0] == '\0') {
           pthread_mutex_unlock(&datos->mutex_contenido);
           pthread_exit(NULL);
        }
        
        char* palabra_mas_cercana = obtener_palabra_mas_cercana(datos->palabra_remplazar) ;
      strcpy(datos->palabra_remplazar, palabra_mas_cercana);
     pthread_mutex_unlock(&datos->mutex_contenido);
     pthread_exit(NULL);
}


void* modificar_archivo(void* arg){
 struct datosArchivo *datos = (struct datosArchivo*)arg;
 char linea[max_linea];
    pthread_mutex_lock(&datos->mutex_contenido);
       
        FILE* archivo = fopen(datos->nom_archivo,"r");
      if (archivo == NULL){
         printf("Error al abrir el archivo.\n");
          limpiar_buffer();
         pthread_mutex_unlock(&datos->mutex_contenido); 
         pthread_exit(NULL);
      }
             
        FILE* temp = fopen("temp.txt","w");
      if (temp == NULL){
         printf("Error al abrir el archivo temporal.\n");
          limpiar_buffer();
         pthread_mutex_unlock(&datos->mutex_contenido); 
         pthread_exit(NULL);
      }
       
       while (fgets(linea, sizeof(linea), archivo) != NULL) {
        char *pos;
        while ((pos = strstr(linea, datos->palabra_buscar)) != NULL) {
            int offset = pos - linea; 
            char tempLinea[max_linea * 2]; 
snprintf(tempLinea, sizeof(tempLinea), "%.*s%s%s", offset, linea, datos->palabra_remplazar, pos + strlen(datos->palabra_buscar));
            strcpy(linea, tempLinea); 
        }
        fputs(linea, temp);
    }
     fclose(archivo);
    fclose(temp);

    remove(datos->nom_archivo);
    rename("temp.txt",datos->nom_archivo);  
   
    printf("Archivo modificado correctamente... ");
    limpiar_buffer();
     pthread_mutex_unlock(&datos->mutex_contenido);
     pthread_exit(NULL);
}


void* analizador_semantico(void* arg){
     struct datosArchivo *datos = (struct datosArchivo*)arg;
     char *contenido = (char *)malloc(max_Text * sizeof(char));
    char* token = strtok(datos->contenido, " ");
    char *palabras[3];
    int i = 0; 
    int pronomFlag = 0; 
    int verbFlag = 0; 
    int complementFlag = 0; 
    
    pthread_mutex_lock(&datos->mutex_contenido);
    
    while (token != NULL) { 
        palabras[i] = strdup(token); 
        token = strtok(NULL, " "); 
        i++; 
    } 
    
     if (i != 3) { 
         pthread_mutex_unlock(&datos->mutex_contenido);
         pthread_exit(NULL);
        
    } 
    for (int j = 0; j < 3; j++) { 
        if (j == 0 && !esPronombre(palabras[j])) { 
            if(esVerbo(palabras[j])){ 
                verbFlag = 1; 
            }else if(esComplemento(palabras[j])){ 
                complementFlag = 1; 
            } 
            printf("Error: '%s' no es un pronombre.\n", palabras[j]); 
        } else if (j == 1 && !esVerbo(palabras[j])) { 
            if(esPronombre(palabras[j])){ 
                pronomFlag = 1; 
            }else if(esComplemento(palabras[j])){ 
                complementFlag = 1; 
            } 
            printf("Error: '%s' no es un verbo.\n", palabras[j]); 
        } else if (j == 2 && !esComplemento(palabras[j])) { 
            if(esPronombre(palabras[j])){ 
                pronomFlag = 1; 
            }else if(esVerbo(palabras[j])){ 
                verbFlag = 1; 
            } 
            printf("Error: '%s' no es un complemento.\n", palabras[j]); 
        } 
    } 
    
     if (pronomFlag == 1 && verbFlag == 1 && complementFlag == 1) {
        printf("La oración está totalmente mal estructurada.\n");
    } else {
        if (pronomFlag == 0 && verbFlag == 1 && complementFlag == 1) {
            char *aux = palabras[1];
            palabras[1] = palabras[2];
            palabras[2] = aux;
        } else if (pronomFlag == 1 && verbFlag == 1 && complementFlag == 0) {
            char *aux = palabras[0];
            palabras[0] = palabras[1];
            palabras[1] = aux;
        } else if (pronomFlag == 1 && verbFlag == 0 && complementFlag == 1) {
            char *aux = palabras[0];
            palabras[0] = palabras[2];
            palabras[2] = aux;
        }
        memset(contenido, 0, max_Text * sizeof(char));
        printf("Oración corregida:\n");
        for (int j = 0; j < 3; j++) {
            strcat(contenido,palabras[j]);
            strcat(contenido," ");
            free(palabras[j]);
        }
        
        printf("%s\n", contenido);
    }
        
     free(datos->contenido);
     datos->contenido = contenido;
     limpiar_buffer();
     
     pthread_mutex_unlock(&datos->mutex_contenido);
     pthread_exit(NULL);
}

//Funciones

//Analizador ortografico
int comparar_palabras(const char *palabra1, const char *palabra2) {
    int distancia = 0;
    while (*palabra1 && *palabra2) {
        if (*palabra1 != *palabra2) {
            distancia++;
        }
        palabra1++;
        palabra2++;
    }
    distancia += abs(strlen(palabra1) - strlen(palabra2));
    return distancia;
}

char *obtener_palabra_mas_cercana(const char *palabra) {
    int mejor_coincidencia = max_palabra;
      char *palabra_mas_cercana = NULL;
    for (int i = 0; i < sizeof(palabras_validas) / sizeof(palabras_validas[0]); i++) {
        int distancia = comparar_palabras(palabra, palabras_validas[i]);
        if (distancia < mejor_coincidencia) {
            mejor_coincidencia = distancia;
            palabra_mas_cercana = palabras_validas[i];
        }
    }

    return palabra_mas_cercana;
}

//Analizador semantico
int esVerbo(char *palabra) {
    for (int i = 0; i < sizeof(verbos) / sizeof(verbos[0]); i++) {
        if (strcmp(palabra, verbos[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int esPronombre(char *palabra) {
    for (int i = 0; i < sizeof(pronombres) / sizeof(pronombres[0]); i++) {
        if (strcmp(palabra, pronombres[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int esComplemento(char *palabra) {
    for (int i = 0; i < sizeof(complementos) / sizeof(complementos[0]); i++) {
        if (strcmp(palabra, complementos[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

//Procesos
void* proceso_CrearArchivo(void* arg) {
       struct datosArchivo datos;
       strcpy(datos.estado, "crear");
       pthread_mutex_init(&datos.mutex_contenido, NULL);
       
      pid_t pidCrearArchivo = fork();
      
    if (pidCrearArchivo < 0) {
        perror("Error al crear proceso para revisión ortográfica");
         limpiar_buffer();
        exit(EXIT_FAILURE);
        
    } else if (pidCrearArchivo == 0) {
     
        pthread_t hiloObtenerNombreArchivo;
        pthread_create(&hiloObtenerNombreArchivo, NULL, obtener_nombre,(void*) &datos);
         pthread_join(hiloObtenerNombreArchivo, NULL);
        
         pthread_t hiloObtenerContenidoArchivo;
        pthread_create(&hiloObtenerContenidoArchivo, NULL, obtener_contenido, (void*) &datos);
        pthread_join(hiloObtenerContenidoArchivo, NULL); 
        
        pthread_t hiloCorregirOrtografia;
        pthread_create(&hiloCorregirOrtografia, NULL, analizador_ortografico, (void*) &datos);
        pthread_join(hiloCorregirOrtografia, NULL); 
        
       pthread_t hiloCorregirGramatica;
        pthread_create(&hiloCorregirGramatica, NULL, analizador_semantico, (void*) &datos);
        pthread_join(hiloCorregirGramatica, NULL); 
        
        pthread_t hiloCrearArchivo;
        pthread_create(&hiloCrearArchivo, NULL, crear_archivo, (void*) &datos);
         pthread_join(hiloCrearArchivo, NULL); 
      
      free(datos.contenido);
      pthread_mutex_destroy(&datos.mutex_contenido);
    }
    else{
         wait(NULL);
    }
     pthread_exit(NULL);
}


void* proceso_LeerArchivo(void* arg){
     struct datosArchivo datos;
       strcpy(datos.estado, "leer");
       pthread_mutex_init(&datos.mutex_contenido, NULL);
       
      pid_t pidLeerArchivo = fork();
      
    if (pidLeerArchivo < 0) {
        perror("Error al crear proceso para revisión ortográfica");
         limpiar_buffer();
        exit(EXIT_FAILURE);
        
    } else if (pidLeerArchivo == 0) {
     
        pthread_t hiloObtenerNombreArchivo;
        pthread_create(&hiloObtenerNombreArchivo, NULL, obtener_nombre,(void*) &datos);
         pthread_join(hiloObtenerNombreArchivo, NULL);
        
        pthread_t hiloLeerArchivo;
        pthread_create(&hiloLeerArchivo, NULL, leer_archivo, (void*) &datos);
         pthread_join(hiloLeerArchivo, NULL); 
      
      free(datos.contenido);
      pthread_mutex_destroy(&datos.mutex_contenido);
    }
    else{
         wait(NULL);
    }
     pthread_exit(NULL);

}

void* proceso_EditarArchivo(void* arg){
      struct datosArchivo datos;
       strcpy(datos.estado, "editar");
       pthread_mutex_init(&datos.mutex_contenido, NULL);
       
      pid_t pidEditarArchivo = fork();
      
    if (pidEditarArchivo < 0) {
        perror("Error al crear proceso para revisión ortográfica");
        limpiar_buffer();
        exit(EXIT_FAILURE);
        
    } else if (pidEditarArchivo == 0) {
     
        pthread_t hiloObtenerNombreArchivo;
        pthread_create(&hiloObtenerNombreArchivo, NULL, obtener_nombre,(void*) &datos);
         pthread_join(hiloObtenerNombreArchivo, NULL);
         
          pthread_t hiloLeerArchivo;
        pthread_create(&hiloLeerArchivo, NULL, leer_archivo, (void*) &datos);
         pthread_join(hiloLeerArchivo, NULL); 
         
         pthread_t hiloObtenerContenidoArchivo;
        pthread_create(&hiloObtenerContenidoArchivo, NULL, obtener_contenido, (void*) &datos);
        pthread_join(hiloObtenerContenidoArchivo, NULL); 
        
        pthread_t hiloCorregirOrtografia;
        pthread_create(&hiloCorregirOrtografia, NULL, analizador_ortografico, (void*) &datos);
        pthread_join(hiloCorregirOrtografia, NULL); 
        
        pthread_t hiloCorregirGramatica;
        pthread_create(&hiloCorregirGramatica, NULL, analizador_semantico, (void*) &datos);
        pthread_join(hiloCorregirGramatica, NULL); 
     
        pthread_t hiloEditarArchivo;
        pthread_create(&hiloEditarArchivo, NULL, editar_archivo, (void*) &datos);
         pthread_join(hiloEditarArchivo, NULL); 
      
      free(datos.contenido);
      pthread_mutex_destroy(&datos.mutex_contenido);
    }
    else{
         wait(NULL);
    }
     pthread_exit(NULL);
}


void* proceso_ModificarArchivo(void* arg){
      struct datosArchivo datos;
       strcpy(datos.estado, "modificar");
       pthread_mutex_init(&datos.mutex_contenido, NULL);
       
      pid_t pidModificarArchivo = fork();
      
    if (pidModificarArchivo < 0) {
        limpiar_buffer();
        perror("Error al crear proceso para revisión ortográfica");
        exit(EXIT_FAILURE);
        
    } else if (pidModificarArchivo == 0) {
     
        pthread_t hiloObtenerNombreArchivo;
        pthread_create(&hiloObtenerNombreArchivo, NULL, obtener_nombre,(void*) &datos);
         pthread_join(hiloObtenerNombreArchivo, NULL);
         
          pthread_t hiloLeerArchivo;
        pthread_create(&hiloLeerArchivo, NULL, leer_archivo, (void*) &datos);
         pthread_join(hiloLeerArchivo, NULL); 
         
          pthread_t hiloObtenerPalabraBuscar;
        pthread_create(&hiloObtenerPalabraBuscar, NULL, obtener_palabraBuscar, (void*) &datos);
        pthread_join(hiloObtenerPalabraBuscar, NULL); 

          pthread_t hiloObtenerPalabraRemplezar;
        pthread_create(&hiloObtenerPalabraRemplezar, NULL, obtener_palabraRemplazar, (void*) &datos);
        pthread_join(hiloObtenerPalabraRemplezar, NULL);

        pthread_t hiloCorregirOrtografiaPalabra;
        pthread_create(&hiloCorregirOrtografiaPalabra, NULL, analizador_ortografico_palabra, (void*) &datos);
        pthread_join(hiloCorregirOrtografiaPalabra, NULL); 
        
        pthread_t hiloModificarArchivo;
        pthread_create(&hiloModificarArchivo, NULL, modificar_archivo, (void*) &datos);
         pthread_join(hiloModificarArchivo, NULL); 
        
      pthread_mutex_destroy(&datos.mutex_contenido);
    }
    else{
         wait(NULL);
    }
     pthread_exit(NULL);
}