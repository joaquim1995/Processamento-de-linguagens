

//STRUCT's DA ARVORE
// ASSINATURAS DE METODOS DA ARVORE

typedef enum { typeCor, typeCoordenada,typeDimensao,typeIntVal, typeListInt, typeIdVar, typeOperator } nodeEnum;


typedef struct {
	struct RGB cor; 
} corNodeType;

typedef struct {
	struct Coordenadas dimensao; 
} dimensaoNodeType;

typedef struct {
	struct Coordenadas coordenada; 
} coordenadaNodeType;

/* integer constants */
typedef struct {
	int value; /* value of constant */
} intValNodeType;

typedef struct {
	struct nodoINT *front;
	int last;
} listIntNodeType;

/* variables identifiers */
typedef struct {
	char *idvar; 
} idVarNodeType;

/* operators */
typedef struct {
	int oper; /* operator */
	int nops; /* number of operands */
	struct nodeTypeTag **op; /* operands */
} operatorNodeType;

typedef struct nodeTypeTag {
	nodeEnum type; /* type of node */
	union {
		coordenadaNodeType Coordenada;
		dimensaoNodeType Dimensao;
		corNodeType Cor;
		intValNodeType intVal;      /* integer constants   */
		listIntNodeType listInt;      /* lista de inteiros   */
		idVarNodeType  idVar;        /* vars identifiers */
		operatorNodeType operator;       /* operators   */
	};
} nodeType;

nodeType *cor(RGB color); 
nodeType *dimensao(Coordenadas cordU); 
nodeType *coordenada(Coordenadas cordU); 
nodeType *operator(int oper, int nops, ...);
nodeType *idvar(char *);
nodeType *intval(int value); 
nodeType *list(ListaINT l) ;
void freeNode(nodeType *p); 
int executar(nodeType *p);


