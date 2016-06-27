%error-verbose
%{
//#define YYDEBUG 1
#define PROF 3
#define MAXCOLOR 255
#include "vc.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <stdarg.h>

extern int yylineno;

IVC *imagem;
RGB core={255, 255, 255};
Listaint listanum ;
Lista listavar;
//ListaARV listacomando = NULL;
%}

%left '+' '-'
%left '*' '/'

%union{
	Coordenadas cord;
	RGB cor;
	char* texto;
	unsigned int valor;
	ListaARV listacomando;
	nodeType *nPtr; /* node pointer */
	struct nodoINT *listaInt;
	}
	
%type <nPtr> ponto linha retangulo circunf retfill ciclo varcont opcao color dimensao coordenada var cor expr c s linhas multiplas list 
%type <listacomando> muitos

%token ABRIR NOVA PONTO LINHA RETANGULO RETFILL CIRCUNF FIM GUARDAR COR RAND PARA EM LINHAS LIST FAZER
%token <texto> FILENAME
%token <cord> DIMENSAO COORDENADA 
%token <cor> COLOR
%token <valor> RAIO
%token <texto> VAR

%start programa
%%
programa : comando s guardar	{
					numeroProducao(0);
					vc_image_free(imagem);
					return 0;
				}
	 ;

comando : abrir			{numeroProducao(2);}

  	| nova			{numeroProducao(3);}	
		
	| error	comando		{yyerrok;yyclearin;printf("Comando nao conhecido\n");return 0;}
	;

s : s cor			{numeroProducao(6);}  

  | s linhas 			{numeroProducao(7);executar($2); freeNode($2);}

  | s ponto 			{numeroProducao(8);executar($2); freeNode($2);}

  | s linha 			{numeroProducao(9);executar($2); freeNode($2);}

  | s retangulo 		{numeroProducao(10);executar($2); freeNode($2);}

  | s circunf 			{numeroProducao(11);executar($2); freeNode($2);}

  | s retfill 			{numeroProducao(12);executar($2); freeNode($2);}

  | s error 			{yyerrok;yyclearin;printf("Comando nao conhecido\n");}

  | s var 			{numeroProducao(13);executar($2); freeNode($2);}
  
  | s ciclo 			{numeroProducao(14);executar($2); freeNode($2);}
      
  |				{numeroProducao(15);}
  ;

muitos  : muitos c 	{ $$ = inserirNo($1,$2);}
	| 		{$$ = NULL; }
	;  
c :  cor 			{$$ = $1;numeroProducao(60);}  

  |  ponto 			{$$ = $1;numeroProducao(80);}

  |  linha 			{$$ = $1;numeroProducao(90);}

  |  retangulo   		{$$ = $1;numeroProducao(100);}

  |  circunf 			{$$ = $1;numeroProducao(110);}

  |  retfill  			{$$ = $1;numeroProducao(120);}

  |  error  			{yyerrok;yyclearin;printf("Comando nao conhecido\n");}

  |  var  			{$$ = $1;numeroProducao(130);}
  
  |  ciclo  			{$$ = $1;numeroProducao(140);}
      
  ;
  
  
ciclo : PARA VAR EM '[' RAIO '.' '.' RAIO ']' FAZER muitos FIM PARA ';'
{	nodeType *pi = listaARV($11);
	$$ = operator(PARA,4,idvar($2),intval($5),intval($8),pi);
}
      ;

var : VAR '=' varcont ';'	{ 
					$$ = operator('=',2,idvar($1),$3);
				}
    ;

varcont : VAR 			{
					$$ = idvar($1);
				}

	| RAIO 			{
					$$ = intval($1);		
				}
				
	| RAND RAIO		{	int num = rand() % $2;
					$$ = intval(num);
				}

	| RAND VAR		{ 
					int num = rand() % valor_variavel(listavar,$2);
					$$ = intval(num);	
				}
				
	| '(' expr ')' 		{ 
	 				$$ = $2;		
				}	
	;

expr : '+' list 		{
	 				$$ = operator('+',1, $2); 
				}

     | '-' list 		{ 
	 				$$ = operator('-',1, $2);				
				}

     | '*' list 		{
	 				$$ = operator('*',1, $2); 
				}

     | '/' list 		{ 
	 				$$ = operator('/',1, $2);				
				}
     ;

list 	: list RAIO 		{$$ = operator(LIST, 2, $1, intval($2));  }
	| list VAR 		{$$ = operator(LIST, 2, $1, idvar($2));  }
	|			{$$ = operator(LIST, 1, NULL); }
	;


retangulo : RETANGULO coordenada coordenada color ';'
	  {
		$$ = operator(RETANGULO,3,$2,$3,$4);		  
	  }
						
	  | RETANGULO coordenada dimensao color ';'	  
	  {
	  	$$ = operator('r',3,$2,$3,$4);	
          }
	  ;

retfill :  RETFILL coordenada dimensao color';'	
	{
		$$ = operator('R',2,3,$2,$3,$4);	
	}

	| RETFILL coordenada coordenada color';'
	{
		$$ = operator(RETFILL,3,3,$2,$3,$4);	
	}
	;


circunf : CIRCUNF coordenada opcao color';'	
	{
		$$ = operator(CIRCUNF,3,$2,$3,$4);		
	}
	;


linha : LINHA coordenada coordenada color';' 	
	{
		$$ = operator(LINHA,3,$2,$3,$4); 		
	}
      ;

ponto :  PONTO coordenada color ';' 
	 {	
	 	$$ = operator(PONTO,2,$2,$3);	 	
	 }
      ;
      
cor : COR opcao':'opcao':'opcao	';'	
					{
						$$ = operator(COR,3,$2,$4,$6);
					}

	| COR COLOR ';'			
					{	
						core = $2;				
					}		
    ;

coordenada : opcao ',' opcao		
					{	
						$$ = operator(COORDENADA,2,$1,$3);
					}

	   | COORDENADA	{
	   					$$ = coordenada($1);			
	   				}
	   ;
    
color 	: opcao':'opcao':'opcao		
					{	
						$$ = operator(COLOR,3,$1,$3,$5);
					}

	| COLOR				
					{	
						$$ = cor($1);		
					}
					
	| 				{ 	
						$$ = NULL;	
					}
	;
	
dimensao : DIMENSAO			{
						$$ = coordenada($1);				
					}
					
	 | opcao 'x' opcao		{
	 					$$ = operator(DIMENSAO,2,$1,$3);
	 				}
	 ;

opcao   : VAR				{
						$$ = idvar($1);					
					}

	| RAIO				{
						$$ = intval($1);					
					}	
	;
	
linhas : LINHAS	coordenada coordenada multiplas color';'
					{}
       	;

multiplas : multiplas coordenada      	{}
	
	  |				{}
	  ;
	
abrir : ABRIR '"' FILENAME '"' ';'	{
						imagem = vc_read_imageB($3);
					}
      ;

nova : NOVA DIMENSAO COLOR ';'		{
					 	imagem = vc_image_news($2.x, $2.y, PROF, MAXCOLOR, $3);
					}
					
     | NOVA DIMENSAO  ';'		{
					 	imagem = vc_image_news($2.x, $2.y, PROF, MAXCOLOR, core);
					}
     ;    

guardar : GUARDAR '"' FILENAME '"' ';'	{
						vc_write_imageB($3, imagem);				
					}
	|				{vc_write_imageB("a.pnm", imagem);}
  ;
%%


int yyerror(char const *s) {
	fprintf(stderr,"Erro [linha : %d]: %s\n",yylineno,s);	
	return 0;
}

int main() {
//extern int yydebug;
//yydebug = 1;
	yyparse(); 
	return 0;
}
