%error-verbose
%{
#define PROF 3
#define MAXCOLOR 255
#include "vc.h"
#include <stdlib.h>

extern int yylineno;
IVC *imagem;
RGB core={255, 255, 255};
Listaint listanum ;
Lista listavar;
char* nomeFich ="a.pnm";
%}

%left '+' '-'
%left '*' '/'

%union{
	Coordenadas cord;
	RGB cor;
	char* texto;
	unsigned int valor;
	}
	
%type <cord> dimensao coordenada
%type <cor> color
%type <valor> multiplas varcont opcao

%token ABRIR NOVA LINHAS PONTO LINHA RETANGULO RETFILL CIRCUN
%token CIRCUNF FIM GUARDAR COR RAND PARA EM FAZER FUNCAO COISAS
%token <texto> FILENAME
%token <cord> DIMENSAO COORDENADA
%token <cor> COLOR
%token <valor> RAIO
%token <texto> VAR

%start programa
%%
programa :  comando s guardar	{
					printf("Fim do Ficheiro\n");
					vc_image_free(imagem);
					return 0;
				}
  	 ;

comando : abrir			{nomeProducao("Abrir");}

  	| nova			{nomeProducao("Nova");}	
	
	| cor comando		{nomeProducao("Cor");}		

	| var comando 		{nomeProducao("Var");}	
	
	| error comando 	{yyclearin;yyerrok;nomeProducao("Erro. Comando nao conhecido");}
	;

s 	: s cor			{nomeProducao("Cor");}	 

  	| s linhas 		{nomeProducao("Linhas");}	

  	| s ponto 		{nomeProducao("Ponto");}	

  	| s linha 		{nomeProducao("Linha");}	

  	| s retangulo 		{nomeProducao("Retangulo");}	

  	| s circunf 		{nomeProducao("Circunf");}
	
	| s circun 		{nomeProducao("Circun");}	

  	| s retfill 		{nomeProducao("Retfill");}	

  	| s error 		{yyclearin;yyerrok;nomeProducao("Erro. Comando nao conhecido");}

  	| s var 		{nomeProducao("Var");}
  
  	| s ciclo		{nomeProducao("Ciclo");}
  
  	| s funcao		{nomeProducao("Funcao");}
    
  	|			{nomeProducao("Epsilon");}
  	;
   


var 	: VAR '=' varcont ';'	{define_variavel(&listavar,$1,$3);}
    	;

varcont : VAR 			{
					int x = valor_variavel(listavar,$1);
					if (x >= 0)
					$$ = x;
					else 
					$$ = 0;				
				}

	| RAIO 			{
					$$ = $1;
				}

	| varcont '+' varcont 	{ $$ = $1 + $3;}

	| varcont '-' varcont 	{ $$ = $1 - $3;}

	| varcont '*' varcont 	{ $$ = $1 * $3;}
	
	| '(' varcont ')' 	{ $$ = $2;}
	
	| varcont '/' varcont 	{ $$ = $1 / $3;}

	| RAND RAIO		{ $$ = rand() % $2;}

	| RAND VAR		{ int x =  rand() % (valor_variavel(listavar,$2));
				 if(x >=0)
					$$ = x;
				else
					$$ = 0;
				}	
	;


linhas 	: LINHAS	coordenada coordenada multiplas color';'
{
	if(Filtrar(imagem, $2) == 1  || Filtrar(imagem, $2) == 1 || $4 > 0 );
	else 
	{
		listanum = inseri(listanum, $2.x,$2.y);
		listanum = inseri(listanum, $3.x,$3.y);
		int i;

		for(i = 0;listanum->seguinte->x > 0 ;)
		{
		PintLinha(imagem,listanum->x,listanum->y, listanum->seguinte->x,listanum->seguinte->y,$5);
		listanum = listanum->seguinte;
		}
		liber(listanum);
	}
}
       	;

multiplas : multiplas coordenada      {listanum = inseri(listanum, $2.x,$2.y);$$ = $1 + Filtrar(imagem, $2);}
	
	  |				{
						listanum = (Listaint)malloc(sizeof(struct nodoint));
						$$ = 0;
					}
	  ;

retangulo : RETANGULO coordenada coordenada color ';'
					{
						  if(Filtrar(imagem, $2) == 1 || Filtrar(imagem, $3) == 1);

						  else
						  {
							 
							  PinRetangulo(imagem,$2.x,$2.y,$3.x,$3.y,$4);
						  }
					}
						
	  | RETANGULO coordenada dimensao color ';'
					  
					{
						if(Filtrar(imagem, $2) == 1 ||FiltrarD(imagem, $2, $3) == 1);

						else
						{	
							PintRetangulo(imagem,$2.x,$2.y,$3.x,$3.y,$4);
						}
					}
	  ;

retfill	  :  RETFILL coordenada dimensao color';'	
					{
						if(Filtrar(imagem, $2) == 1 || Filtrar(imagem, $3) == 1);
						else 
						{
							PintaXRetangulo(imagem,$2.x,$2.y,$3.x,$3.y,$4);
						}
					}

	   | RETFILL coordenada coordenada color';'
					{
						if(Filtrar(imagem, $2) == 1 || Filtrar(imagem, $3) == 1);
						else 
						{
							PintaRetangulo(imagem,$2.x,$2.y,$3.x,$3.y,$4);
						}
					}
	  ;


circunf   : CIRCUNF coordenada opcao color';'	
					{
						if(Filtrar(imagem, $2) == 1);
						else 
						{
							PintaCirculo(imagem, $2, $3,$4);
						}
					}
	  ;
	  
circun    : CIRCUN coordenada opcao color';'	
					{
						if(Filtrar(imagem, $2) == 1);
						else 
						{
							PintaCircf(imagem, $2, $3,$4);
						}
					}
	  ;


linha 	  : LINHA coordenada coordenada color';' 	
					{
						if(Filtrar(imagem, $2) == 1 || Filtrar(imagem, $3) == 1);
						else
						{
							PintaLinha(imagem,$2,$3, $4);
						}
					}
         ;

ponto 	 :  PONTO coordenada color ';' 	
					{
						if(Filtrar(imagem, $2) == 1);
						else
						{
							PintaPonto(imagem,$2,$3);
						}
					}
     	 ;

cor : COR color ';'
    {
	core=$2;
    }
    ;

coordenada : opcao ',' opcao		{
						
						Coordenadas e = {$1,$3}; $$ = e;
						
					}

	   | COORDENADA			{	
						
	   					$$ = $1;
	   					
	   				}
	   ;
    
color 	: opcao':'opcao':'opcao		{ 
						
						RGB x = {$1,$3,$5}; $$ = x;
						
					}

	| COLOR				{	
						
						$$ = $1;
								
					}
	
	|				{
						
						$$ = core;
					}
	;
	
dimensao : DIMENSAO			{
						
						$$ = $1;
						
					}
					
	 | opcao 'x' opcao		{
						
	 					Coordenadas c = {$1,$3};
	 					$$ = c;
	 					
	 				}
	 ;

opcao   : VAR				{
						
						int x = valor_variavel(listavar,$1);
						if(x >= 0) $$ = x;
						else $$ = 0;
						
					}

	| RAIO				{
								
						$$ = $1;
					}
	
	;	


funcao 	: FUNCAO '(' parametros ')'	{}
	;

parametros : parametros COISAS 		{}
	   |
	   ;
	   
ciclo : PARA VAR EM '[' RAIO '.' '.' RAIO ']' s FIM PARA ';'
					{
					}
      ;
	  
abrir : ABRIR '"' FILENAME '"' ';'	{
						imagem = vc_read_imageB($3);
					}
      ;

nova : NOVA dimensao color ';'		{
					 	imagem = vc_image_news($2.x, $2.y, PROF, MAXCOLOR, $3);
					}
    ;

guardar : GUARDAR '"' FILENAME '"' ';'	{	nomeProducao("Guardar");
						vc_write_imageB($3, imagem);				
					}
					
		|			{vc_write_imageB(nomeFich, imagem);}
  ;
%%


int yyerror(char const *s) {
	fprintf(stderr,"Erro [linha : %d]: %s\n",yylineno,s);	
	return 0;
}


int main() {
	extern int yydebug;
	//yydebug = 1;
	yyparse(); 
	return 0;
}
