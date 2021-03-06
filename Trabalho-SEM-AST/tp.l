%{
#include "vc.h"
#include "gram.h"

/*
x = colunas y = linhas
*/

%}


dimensao	[0-9]+[x][0-9]+
coordenada	[0-9]+[,][0-9]+
deepcolor	((0[0-9]{0,2})|(1[0-9]{0,2})|(2[0-4][0-9])|(25[0-5]))
color		{deepcolor}:{deepcolor}:{deepcolor}	
ignore		[\t\n ]+	


%%
{ignore}			{}
{color}				{
					yylval.cor.r = atol(yytext);
					yytext += PosicaoDigitos(yytext);
					yylval.cor.g = atol(yytext);
					yytext += PosicaoDigitos(yytext);
					yylval.cor.b = atol(yytext);
					return COLOR;
				}
{dimensao}			{
					yylval.cord.x = atol(yytext);
					yytext += (PosicaoDigitos(yytext));
					yylval.cord.y = atol(yytext);
					return DIMENSAO;
				}
{coordenada}			{
					yylval.cord.x = atol(yytext);
					yytext += (PosicaoDigitos(yytext));
					yylval.cord.y = atol(yytext);
					return COORDENADA;
				}
[;x,"+*-/=()]			{return yytext[0];}
[a-wy-z]+([0-9]+)?		{
					yylval.texto = strdup(yytext);
					return VAR;
				}
[0-9]+				{
					yylval.valor = atol(yytext);
					return RAIO;
				}	
RAND				{return RAND;}
FUNCAO				{return FUNCAO;}
COISAS				{return COISAS;}
PARA				{return PARA;}
EM					{return EM;}
FAZER				{return FAZER;}						
FIM					{return FIM;}
COR					{return COR;}
ABRIR				{return ABRIR;}
NOVA				{return NOVA;}
GUARDAR				{return GUARDAR;}
LINHA				{return LINHA;}
LINHAS				{return LINHAS;}
PONTO				{return PONTO;}
CIRCUNF				{return CIRCUNF;}
CIRCUN				{return CIRCUN;}
RETFILL				{return RETFILL;}
RETANGULO			{return RETANGULO;}
[A-Z_a-z\[\]]+\.pnm		{yylval.texto = strdup(yytext);return FILENAME;}
.				{yylval.texto = strdup(yytext);return yytext[0];}
<<EOF>> 			{return 0;}

%%


int yywrap() { return 1; }


