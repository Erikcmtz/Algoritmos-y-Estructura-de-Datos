//VERSION FINAL...

//LIBRERÍAS A OCUPAR:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//ESTRUCTURAS:

// ESTRUCTURA PARA HACER EL ÁRBOL TIPO ESTRUCTURA - HUFFMAN:
typedef struct Node {
    char car; ///caracter
    int ocu; ///ocurrencia
    struct Node *izquierda; // Leemos un 0 descendemos un nivel del árbol posicionándonos en su hijo IZQUIERDO
    struct Node *derecha; // Leemos un 1 descendemos un nivel para posicionarnos en su hijo DERECHO
} Node;

// ESTRUCTURA QUE COLOCA UN NODO PARA LA LISTA ENLAZADA DE LOS NODOS
typedef struct ListNode {
    Node *node;
    struct ListNode *next;
} ListNode;

//FUNCIONES: 

Node *Tree(ListNode **list);
ListNode *insertSorted(ListNode *list, Node *newNode);

//FUNCION: Imprime arbol huffman, Pasar/Imprimir codigos al archivo, Decodificar el arbol huffman 
void print(Node *root, int aglo[], int top, char table[][500]);
void printFile(FILE *file, char tex[], char table[][500]);
void decodificar(Node *root, char ectex[], FILE *decodedFile);
	
//FUNCION PARA PASAR DE ARREGLOS A CONVERTIR A CADENA STRING
char *arrToString(int aglo[], int n);

int main() {
    char filename[200]; //ARREGLO CON TAMAÑO DE LETRAS PARA EL ARCHIVO
    
    printf("Ingrese el nombre del archivo(.txt) a codificar: "); //SE CREA EL 1er ARCHIVO 
	scanf("%s", filename);
	printf("\n-------------------------------------------------\n");

    // APERTURA DEL ARCHIVO
    FILE *file = fopen(filename, "r"); //1er ARCHIVO
    	if (file == NULL) {
        	fprintf(stderr, "Error al abrir el archivo\n");
        	exit(EXIT_FAILURE);
    }
    
    // LEER CONTENIDO DEL ARCHIVO
    char tex[500];

while(fgets(tex, 500 , file) !=NULL){
	printf("%s", tex);
}

	fclose(file);
	
    // CONTEO DE FRECUENCIA DE CADA UNO DE LOS CARACTERES EN EL TEXTO DEL ARCHIVO
    int freq[500] = {0};
    	for (int i = 0; tex[i] != '\0'; i++) {
      	  freq[(int)tex[i]]++;
    }
 // SE CREA LA LISTA ENLAZADA. USO DE NODOS ESTRUCTURA - HUFFMAN
    ListNode *list = NULL;
    	for (int i = 0; i < 500; i++) {
        	if (freq[i] > 0) {
            	Node *newNode = (Node *)malloc(sizeof(Node));
            	newNode->car = (char)i;
            	newNode->ocu = freq[i];
            	newNode->izquierda = newNode->derecha = NULL;
            	list = insertSorted(list, newNode);
        }
    }

    // SE CREA EL ÁRBOL A PARTIR DE LA LISTA ENLAZADA
    Node *root = Tree(&list);

    // SE IMPRIMEN LOS VALORES HUFFMAN - SE GUARDAN EN EL 2do ARCHIVO
    char table[500][500];
    int aglo[500], top = 0;
    print(root, aglo, top, table);

    FILE *encodedFile = fopen("codBinario.txt", "w"); //2do ARCHIVO
    	if (encodedFile == NULL) {
    		fprintf(stderr, "Error al abrir el archivo codificado\n");
        	exit(EXIT_FAILURE);
    }
    
    // FUNCION PARA IMPRIMIR LOS CODIGOS BINARIOS EN EL ARCHIVO:
    printFile(encodedFile, tex, table);
    fclose(encodedFile);

    printf("TEXTO CODIFICADO EN BINARIO EN EL ARCHIVO 'codBinario.txt'\n");

    // SOLICITAR NOMBRE DEL ARCHIVO (.txt) QUE SE VA A DECODIFICAR
    char decodedFilename[500];
    
    printf("Ingrese el nombre del archivo (.txt) a decodificar: ");
    scanf("%s", decodedFilename);

    // ABRIMOS EL ARCHIVO QUE SE VA A DECODIFICAR
    FILE *encodedFileToDecode = fopen(decodedFilename, "r");
    	if (encodedFileToDecode == NULL) {
    		fprintf(stderr, "Error al abrir el archivo a decodificar\n");
        	exit(EXIT_FAILURE);
    }

    // SE LEE EL CONTENIDO DEL ARCHIVO CODIFICADO "2DO ARCHIVO"
   fgets(tex, sizeof(tex), encodedFileToDecode); //fgets: PARA OBTENER EL CONTENIDO DEL ARCHIVO 
    fclose(encodedFileToDecode);

    // SE HACE LA DECODIFICACION DEL ARCHIVO
    printf("Texto decoficado:\n");
    decodificar(root, tex, stdout); //FUNCION: DECODIFICAR

    return 0;
}

//CONSTRUCCION DEL ÁRBOL HUFFMAN
Node *Tree(ListNode **list) {
    while (*list != NULL && (*list)->next != NULL) {
        Node *left = (*list)->node;
        *list = (*list)->next;
        Node *right = (*list)->node;
        *list = (*list)->next;

        Node *internalNode = (Node *)malloc(sizeof(Node));
        internalNode->car = '\0';
        internalNode->ocu = left->ocu + right->ocu;
        internalNode->izquierda = left;
        internalNode->derecha = right;

        *list = insertSorted(*list, internalNode);
    }

    return (*list)->node;
}

//INSERTA LISTA, ESPECIFICANDO PARAMETROS
ListNode *insertSorted(ListNode *list, Node *newNode) {
    ListNode *newListNode = (ListNode *)malloc(sizeof(ListNode));
    
    //APUNTADORES
    newListNode->node = newNode;
    newListNode->next = NULL;

    if (list == NULL || newNode->ocu <= list->node->ocu) {
        newListNode->next = list;
        list = newListNode;
    } else {
        ListNode *current = list;
        while (current->next != NULL && current->next->node->ocu < newNode->ocu) {
            current = current->next;
        }
        newListNode->next = current->next;
        current->next = newListNode;
    }

    return list; //REGRESA LA LISTA
}

//IMPRESION DEL ÁRBOL HUFFMAN
void print(Node *root, int aglo[], int top, char table[][500]) {
    if (root->izquierda) {
        aglo[top] = 0;
        print(root->izquierda, aglo, top + 1, table);
    }

    if (root->derecha) {
        aglo[top] = 1;
        print(root->derecha, aglo, top + 1, table);
    }

    if (!root->izquierda && !root->derecha) {
        table[(int)root->car][top] = '\0';
        strcat(table[(int)root->car], arrToString(aglo, top));
        printf("'%c': %s\n", root->car, table[(int)root->car]);
    }
}

char *arrToString(int aglo[], int n) {
    char *str = (char *)malloc((n + 1) * sizeof(char));
    for (int i = 0; i < n; i++) {
        str[i] = aglo[i] + '0';
    }
    str[n] = '\0';
    return str;
}

//IMPRIMIR CODIGOS BINARIOS AL ARCHIVO
void printFile(FILE *file, char tex[], char table[][500]) {
    for (int i = 0; tex[i] != '\0'; i++) {
        fprintf(file, "%s", table[(int)tex[i]]); //ARREGLO DE LA TABLA
    }
}

//FUNCION DECODIFICAR USANDO EL ARCHIVO QUE DECODIFICA
void decodificar(Node *root, char ectex[], FILE *decodedFile) {
    Node *current = root;
    int i = 0;
    while (ectex[i] != '\0') { 
        if (ectex[i] == '0' && current->izquierda != NULL) {
            current = current->izquierda;
        } else if (ectex[i] == '1' && current->derecha != NULL) {
            current = current->derecha;
        }

        if (current->izquierda == NULL && current->derecha == NULL) {
            fprintf(decodedFile, "%c", current->car);
            current = root;
        }

        i++;
    }
}

