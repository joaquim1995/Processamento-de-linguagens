#include <stdint.h>
 
//Tipos de dados Usados
typedef struct  {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
}RGB;


typedef struct  {
	unsigned int x;
	unsigned int y;
	
}Coordenadas;
//__________________________________________________________________________

//Listas Ligadas
typedef struct nodoint {
	unsigned int x;
	unsigned int y;
	struct nodoint *seguinte;
} *Listaint, noI;

 typedef struct nodoINT {
        int valor;
	struct nodoINT *seguinte;
  } *ListaINT;


typedef struct nodo {
	char *nome;
   	unsigned int valor;
	struct nodo *seguinte;
} *Lista, no;

//__________________________________________________________________________

//IMAGEM
typedef struct {
	unsigned char *data;
	int width, height;
	int channels;			// Binário/Cinzentos=1; RGB=3
	int levels;				// Binário=1; Cinzentos 
	int bytesperline;		// width * channels
}IVC;
//___________________________________________________________________________


// FUNÇÕES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_news(int width, int height, int channels, int levels, RGB cor);
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);
IVC *vc_read_imageB(char *filename);
int vc_write_imageB(char *filename, IVC *image);

// FUNÇÕES : ALGORITMOS PARA PINTAR
void PintaPonto(IVC *image, Coordenadas cord, RGB core);
void PintPonto(IVC *image, unsigned int x , unsigned int y, RGB core);
void PintaLinha(IVC *imagem,Coordenadas cordU, Coordenadas cordD, RGB core);
void PintaCirculo(IVC *imagem,Coordenadas cordU, unsigned int radius,RGB core);
void PintLinha(IVC *imagem,  unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core);
void PintaRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core);
void PintaXRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core);
void PintRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core);
void PinRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core);
void PintaCircf(IVC *imagem,Coordenadas cordU, unsigned int radius,RGB core);

// FUNÇÕES : ALGORITMOS PARA FILTRAR COORDENADAS
int Filtrar(IVC *imagem, Coordenadas cord);
int Filtra(IVC *imagem, unsigned int x , unsigned int y);
int FiltrarD(IVC *imagem, Coordenadas cordU,Coordenadas cordD);

// FUNÇÕES : ALGORITMOS PARA LISTAS
Listaint inseri(Listaint l, unsigned int v, unsigned int m);
void liber(Listaint l);
void libert(Lista l);
Listaint inicializa(Listaint l);
int valor_variavel(Lista l, char *varid); // retorna  -1 se nao encontrar
void define_variavel(Lista *l, char *varid, unsigned int v);

// FUNÇÕES : TEXTO
void Aviso();
void nomeProducao(char *);
int PosicaoDigitos(char* temp);
void numeroProducao(int numero);
int yyerror(char const *s);
//_____________________________________________________________________________________________________
