#include "vc.h"
#include "gram.h"
#include <stdio.h>
Coordenadas ciclo;
extern IVC *imagem;
extern Lista listavar;
extern ListaARV listacomando ;
extern RGB core;


//EXECUTADORES

Coordenadas executar_cords(nodeType *p) {
	Coordenadas l={-1,-1};
	if (!p) ;
		
	else{
		if(p->operator.nops != 2)		
			l = executar_coordenada(p);
		
		else
		{	
			switch(p->operator.op[0]->type)
			{
				case typeIdVar:
				l.x = valor_variavel(listavar,p->operator.op[0]->idVar.idvar);
				break;

				case typeIntVal:
				l.x = p->operator.op[0]->intVal.value;
				break;
			}

			switch(p->operator.op[1]->type)
			{
				case typeIdVar:
				l.y = valor_variavel(listavar,p->operator.op[1]->idVar.idvar);
				break;

				case typeIntVal:
				l.y = p->operator.op[1]->intVal.value;
				break;
			}
			
		}
	}
	return l;
}


RGB executar_cordT(nodeType *p) {
	RGB l;
	if (!p || p == NULL)l = core ;
		
	else{
		if(p->operator.nops != 3)
		l = executar_cor(p);
		
		else
		{
			switch(p->operator.op[0]->type)
			{
				case typeIdVar:
				l.r = valor_variavel(listavar,p->operator.op[0]->idVar.idvar);
				break;
		
				case typeIntVal:
				l.r = p->operator.op[0]->intVal.value;
				break;
			}
			
			switch(p->operator.op[1]->type)
			{
				case typeIdVar:
				l.g = valor_variavel(listavar,p->operator.op[1]->idVar.idvar);
				break;
		
				case typeIntVal:
				l.g = p->operator.op[1]->intVal.value;
				break;
			}
		
			switch(p->operator.op[2]->type)
			{
				case typeIdVar:
				l.b = valor_variavel(listavar,p->operator.op[2]->idVar.idvar);
				break;
		
				case typeIntVal:
				l.b = p->operator.op[2]->intVal.value;
				break;
			}
		}
	}
	return l;
}

uint8_t executar_cordC(nodeType *p) {
	uint8_t l;
	if (!p)  ;
		
	else{
		
		switch(p->operator.op[0]->type)
		{
			case typeIdVar:
			l = valor_variavel(listavar,p->operator.op[0]->idVar.idvar);
			break;
		
			case typeIntVal:
			l = p->operator.op[0]->intVal.value;
			break;
		}
	}
	return l;
}

Coordenadas executar_coordenada(nodeType *p) {
	if (!p) printf("Serious error cord");
	switch(p->type) {
		case typeCoordenada: return    p->Coordenada.coordenada;	
		}
	}

RGB executar_cor(nodeType *p) {
	if (!p) printf("Serious error cor");
	switch(p->type) {
		case typeCor: return    p->Cor.cor;	
		}
	}
	
ListaARV executar_ListaARV(nodeType *p) {
	if (!p) printf("Serious error cor");
	switch(p->type) {
		case typeListaARV:{ return p->ListARV.lista;}	
		}
	}	

int executar_cord(nodeType *p) {
	int l;
	if (!p) l=0;
		
	else{
		
		switch(p->operator.op[0]->type)
		{
			case typeIdVar:
			l = valor_variavel(listavar,p->operator.op[0]->idVar.idvar);
			break;
		
			case typeIntVal:
			l = p->operator.op[0]->intVal.value;
			break;
		}
	}
	return l;
}

char * executar_varid(nodeType *p) {
	if (!p) return NULL;
	switch(p->type) {
		case typeIdVar: return    p->idVar.idvar;	
		}
}

ListaINT  executar_list(nodeType *p) 
{
if (!p) return NULL;
switch(p->type) 
{
	case typeOperator : 
	{ 
		switch(p->operator.oper)
		{
			case LIST:
                        { 
                        	if (p->operator.nops == 2) 
                                {
					ListaINT lista= executar_list(p->operator.op[0]);
					int valor_expr = executar(p->operator.op[1]); 
                                   	return inserir(lista, valor_expr); 
				 }
				 else return NULL;
			}
		}
						
 	}
}
}

int executar(nodeType *p) 
{
	if (!p || p == NULL) return 0;
					
	switch(p->type) 
	{
		case typeIntVal:  return p->intVal.value; 
		
		case typeIdVar: {int i= valor_variavel(listavar, p->idVar.idvar);
                              	//printf("[%d]",i); 
                              	return i;
                              	}

		case typeOperator:
			switch(p->operator.oper) 
			{
		case '=':  {
		   	 int  valor_expr =  executar(p->operator.op[1]);  
			 //printf("expr=%d", valor_expr);
                         define_variavel(&listavar, executar_varid(p->operator.op[0]), valor_expr );  
                         return valor_expr; /* 0; */ 
                           }
			
			case '+': return soma(executar_list(p->operator.op[0]));
			case '*': return mult(executar_list(p->operator.op[0]));
			case '-': return subtrai(executar_list(p->operator.op[0]));
			case '/': return divide(executar_list(p->operator.op[0]));		
						
			case PARA:
			{		
				Coordenadas c;
				char* temp = strdup(p->operator.op[0]->idVar.idvar);
				c.y =(unsigned int)p->operator.op[2]->intVal.value;

				for(c.x=(unsigned int)p->operator.op[1]->intVal.value ;c.x <= c.y;c.x++)
				{
				//printf("\nVAR =%s | Y = %d",p->operator.op[0]->idVar.idvar,c.x);
				executar(operator('=',2,idvar(p->operator.op[0]->idVar.idvar),intval(c.x)));
				
				ListaARV temp = executar_ListaARV(p->operator.op[3]);
				
				if(!temp);
				else if( temp->seguinte == NULL)
					executar(temp->valor);
					
					else
					{
					
						while( temp != NULL)
						{
							executar(temp->valor);
							temp = temp->seguinte;
						}
					}
				}
			
				return 0;
			}

			case PONTO:
			{	
				Coordenadas l=executar_cords(p->operator.op[0]);
				RGB cor;

				if ( l.x == -1 || l.y == -1);

				else
				{
					if(p->operator.nops == 2)
					{  
						cor =executar_cordT(p->operator.op[1]);
						PintaPonto(imagem,l,cor);			
					}

					else			
						PintaPonto(imagem,l,core);
				}							
				return 0;
			} 

			case LINHA:
			{	
				Coordenadas lU=executar_cords(p->operator.op[0]);
				Coordenadas lD=executar_cords(p->operator.op[1]);
				RGB cor;

				if(     lU.x == -1|| lU.y == -1);
				else if(lD.x == -1|| lD.y == -1);

				else
				{
					if(p->operator.nops == 3)
					{  
						cor = executar_cordT(p->operator.op[2]);
						PintaLinha(imagem,lU,lD,cor);
					}

					else
						PintaLinha(imagem,lU,lD,core);

				}						
				return 0;
			} 
					
			case LINHAS:
			{	} 

			case CIRCUNF:
			{	
				Coordenadas l=executar_cords(p->operator.op[0]);
				int raio = executar_cord(p->operator.op[1]);
				RGB cor;

				if ( l.x == -1 || l.y == -1);
				else if (raio == -1);

				else
				{
					if(p->operator.nops == 3)
					{  
						cor = executar_cordT(p->operator.op[2]);
						PintaCirculo(imagem,l,raio,cor);
					}

					else			
					PintaCirculo(imagem,l,raio,core);
				}								
				return 0;
			} 

			case RETANGULO:
			{	
				Coordenadas lU=executar_cords(p->operator.op[0]);
				Coordenadas lD=executar_cords(p->operator.op[1]);
				RGB cor;

				if ( lU.x == -1 || lU.y == -1 || lD.x == -1 || lD.y == -1 );

				else
				{
					if(p->operator.nops == 3)
					{  
						cor = executar_cordT(p->operator.op[2]);
						PinRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,cor);
					}

					else			
					PinRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,core);
				}								
				return 0;
			} 

			case 'r':
			{	
				Coordenadas lU=executar_cords(p->operator.op[0]);
				Coordenadas lD=executar_cords(p->operator.op[1]);
				RGB cor;

				if ( lU.x == -1 || lU.y == -1 || lD.x == -1 || lD.y == -1 );

				else
				{
					if(p->operator.nops == 3)
					{  
						cor = executar_cordT(p->operator.op[2]);
						PintRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,cor);
					}

					else			
					PintRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,core);
				}								
				return 0;
			} 

			case RETFILL:
			{
				Coordenadas lU=executar_cords(p->operator.op[0]);
				Coordenadas lD=executar_cords(p->operator.op[1]);
				RGB cor;

				if ( lU.x == -1 || lU.y == -1 || lD.x == -1 || lD.y == -1 );		

				else

				{
					if(p->operator.nops == 3)

					{  
						cor = executar_cordT(p->operator.op[2]);
						PintaRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,cor);
					}			

					else	
					PintaRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,core);
				}
				return 0;
			} 

			case 'R':
			{
				Coordenadas lU=executar_cords(p->operator.op[0]);
				Coordenadas lD=executar_cords(p->operator.op[1]);
				RGB cor;

				if ( lU.x == -1 || lU.y == -1 || lD.x == -1 || lD.y == -1 );		
				else
				{
					if(p->operator.nops == 3)
					{  
						cor = executar_cordT(p->operator.op[2]);
						PintaXRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,cor);
					}			
					else	
					PintaXRetangulo(imagem,lU.x,lU.y,lD.x,lD.y,core);
				}
				return 0;
			
			} 

			case COR:
			{	
				RGB l = executar_cordT(p->operator.op[0]);

				if ( l.r == -1 ||l.g == -1 || l.b == -1);

				else
				core = l;

				return 0;
			} 
			
			case COLOR:
			{return 0;} 

			case COORDENADA:
			{return 0;}

			case DIMENSAO:
			{return 0;}
	}
	
	}
	return 0;
}

