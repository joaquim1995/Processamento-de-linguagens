#include "vc.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <stdarg.h>


extern Lista listavar;



IVC *vc_image_news(unsigned int width, unsigned int height, unsigned int channels, unsigned int levels, RGB cor)
{
	int linhas, colunas;
	long int pos;
	IVC *image = (IVC *)malloc(sizeof(IVC));

	if (image == NULL) return NULL;
	if ((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *)malloc(image->width * image->height * image->channels * sizeof(char));

	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	

	for (colunas = 0; colunas < image->width; colunas++)
	{
		for (linhas = 0; linhas < image->height; linhas++)
		{
			pos = linhas * image->bytesperline + colunas * image->channels;

			
				image->data[pos] = cor.r;
				image->data[pos + 1] = cor.g;
				image->data[pos + 2] = cor.b;
		}
	}
	return image;
}

IVC *vc_image_new(unsigned int width, unsigned int height, unsigned int channels, unsigned int levels)
{
	IVC *image = (IVC *)malloc(sizeof(IVC));

	if (image == NULL) return NULL;
	if ((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *)malloc(image->width * image->height * image->channels * sizeof(char));

	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}

//eliminei o apontador IVC *vc_image_free(IVC *image)
// Libertar memória de uma imagem
IVC *vc_image_free(IVC *image)
{
	if (image != NULL)
	{
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


char *netpbm_get_token(FILE *file, char *tok, unsigned int len)
{
	char *t;
	int c;

	for (;;)
	{
		while (isspace(c = getc(file)));
		if (c != '#') break;
		do c = getc(file);
		while ((c != '\n') && (c != EOF));
		if (c == EOF) break;
	}

	t = tok;

	if (c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while ((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));

		if (c == '#') ungetc(c, file);
	}

	*t = 0;

	return tok;
}


long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, unsigned int width, unsigned int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}


void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, unsigned int width, unsigned int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}


IVC *vc_read_imageB(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P4") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if (strcmp(tok, "P5") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P6") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1)//PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL) return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else//PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL) return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}


int vc_write_imageB(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL) return 0;

	if ((file = fopen(filename, "wb")) != NULL)
	{
		//Se nivel = 1
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL) return 0;

			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);

			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}

		else
		{
			//canais ! 3 = RGB , 1= tons de cinza
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}


IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "r")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P1") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if (strcmp(tok, "P2") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P3") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1)//PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL) return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else//PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL) return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}


int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL) return 0;

	if ((file = fopen(filename, "w")) != NULL)
	{
		//Se nivel = 1
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL) return 0;

			fprintf(file, "%s %d %d\n", "P1", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);

			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}

		else
		{
			//canais ! 3 = RGB , 1= tons de cinza
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P2" : "P3", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}

void numeroProducao(int numero)
{
	printf("\nProducao %d ",numero);
}


int PosicaoDigitos(char* temp)
{
	int i;	

	//para quando encontra um nao digito
	for(i=0; temp[i] != '\0';i++)
	{
		if(temp[i] >= 48 && temp[i] <= 57);
		else break;
		
	}

	
	//sendo i a posicao do obstaculo i+1 já é um digito
	return (i+1);
}

void PintaPonto(IVC *imagem, Coordenadas cord, RGB core)
{
	unsigned int colunas = (cord.x) ,linhas = (cord.y);
	unsigned int pos;			
	pos = (linhas * imagem->bytesperline + colunas * imagem->channels);
	
	if(((imagem->channels * imagem->width * imagem->height) + 3 < pos) || pos < 0 || imagem->width <= cord.x || imagem->height <= cord.y);
		
	else{
	
	imagem->data[pos] = core.r;
	imagem->data[pos+1] = core.g;
	imagem->data[pos+2] = core.b;
	}
}
void Aviso()
{
printf("Fora de limite");
}

void PintPonto(IVC *imagem, unsigned int x , unsigned int y, RGB core)
{
	unsigned int colunas = (x-1) ,linhas =( y-1);
	unsigned int pos;			
	pos = (linhas * imagem->bytesperline + colunas * imagem->channels);
	
	if(((imagem->channels * imagem->width * imagem->height) + 3 < pos) || pos < 0 || imagem->width <= x || imagem->height <= y);
		
	else{
	imagem->data[pos] = core.r;
	imagem->data[pos+1] = core.g;
	imagem->data[pos+2] = core.b;
	}
}

void PintaLinha(IVC *imagem,Coordenadas cordU, Coordenadas cordD, RGB core)
{
 
  int dx = abs(cordD.x-cordU.x);//diferença entre x
  int sx = cordU.x < cordD.x ? 1 : -1;
  int dy = abs(cordD.y-cordU.y);//diferença entre y
  int sy = cordU.y < cordD.y ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2;
  int e2;

  for(;;){
    PintaPonto(imagem, cordU, core);

    if (cordU.x==cordD.x && cordU.y==cordD.y) 
	break;

    e2 = err;
    if (e2 >-dx) 
    { 
	err -= dy; 
	cordU.x += sx; 
     }
    if (e2 < dy) { err += dx; cordU.y += sy; }
  }
}




void PintLinha(IVC *imagem,  unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core)
{

  int dx = abs(xD-xU);//diferença entre x
  int sx = xU < xD ? 1 : -1;
  int dy = abs(yD-yU);//diferença entre y
  int sy = yU < yD ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2;
  int e2;

  for(;;){

    PintPonto(imagem, xU,yU, core);

    if (xU==xD && yU==yD) 
	break;

    e2 = err;
    if (e2 >-dx) 
    { 
	err -= dy; 
	xU += sx; 
     }
    if (e2 < dy) { err += dx; yU += sy; }
  }
}



//width = x ;
//height = y;

int Filtrar(IVC *imagem, Coordenadas cord)
{
	if (cord.x >= 0 && cord.x < imagem->width && cord.y >= 0 && cord.y < imagem->height)
		return 0;

	else 	{	
			Aviso();
			return 1;
		}

}

int Filtra(IVC *imagem, unsigned int x,  unsigned int y)
{
	if (x >= 0 && x < imagem->width && y >= 0 && y < imagem->height)
		return 0;

	else  	{
			Aviso();
			return 1;
		}
}


int FiltrarD(IVC *imagem, Coordenadas cordU, Coordenadas cordD)
{
	if (cordU.x >= 0 && cordU.x < imagem->width && cordU.y >= 0 && cordU.y < imagem->height &&
	    cordU.x + cordD.x <= imagem->width && cordU.y + cordD.y <= imagem->height)
		return 0;

	else 	{	
			Aviso();
			return 1;
		}

}

void PintRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core)
{

	PintLinha(imagem,xU,yU,xU+abs(xU-xD),yU,core);

	PintLinha(imagem,xU+abs(xU-xD),yU,xU+abs(xU-xD),yU+abs(yU-yD),core);

	PintLinha(imagem,xU+abs(xU-xD),yU+abs(yU-yD),xU,yU+abs(yU-yD),core);

	PintLinha(imagem,xU,yU+abs(yU-yD),xU,yU,core);
}

void PinRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core)
{

if((xU < xD) && (yU < yD)){

PintLinha(imagem,xU,yD,xD,yD,core);
PintLinha(imagem,xD,yU,xD,yD,core);
PintLinha(imagem,xU,yD,xU,yU,core);
PintLinha(imagem,xU,yU,xD,yU,core);
}

else if((xU > xD) && (yU > yD)){

PintLinha(imagem,xU,yD,xD,yD,core);
PintLinha(imagem,xD,yU,xD,yD,core);
PintLinha(imagem,xU,yD,xU,yU,core);
PintLinha(imagem,xU,yU,xD,yU,core);
}

else if((xU < xD) && (yU > yD)){

//printf("CoordU = %d,%d | CoordD = %d,%d",xU,yU,xD,yD);

PintLinha(imagem,xD,yU,xU,yU,core);
PintLinha(imagem,xD,yU,xD,yD,core);
PintLinha(imagem,xU,yD,xU,yU,core);
PintLinha(imagem,xD,yD,xU,yD,core);
}

else if((xU > xD) && (yU < yD)){

PintLinha(imagem,xD,yU,xU,yU,core);//linha de cima
PintLinha(imagem,xU,yU,xU,yD,core);//linha da direita
PintLinha(imagem,xD,yD,xD,yU,core);//linha da esquerda
PintLinha(imagem,xD,yD,xU,yD,core); //linha de baixo
}

}


//este é para o retfill
void PintaRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core)
{

int i;

if(xU < xD && yU < yD){
for(i = yU; i < yD; i++)
PintLinha(imagem,xU,i,xU + abs(xU-xD),i,core);
}

if(xU > xD && yU > yD ){
for(i = yD; i < yU; i++)
PintLinha(imagem,xD,i,xD + abs(xD-xU),i,core);
}

if(xU < xD && yU > yD ){
for(i = yD; i < yU; i++)
PintLinha(imagem,xU,i,xU + abs(xU-xD),i,core);
}

if(xU > xD && yU < yD){
for(i = yU; i < yD; i++)
PintLinha(imagem,xD,i,xD + abs(xD-xU),i,core);
}



}
//este é para o retfill para dimensoes e não coordenadas
void PintaXRetangulo(IVC *imagem, unsigned int xU,  unsigned int yU,  unsigned int xD,  unsigned int yD, RGB core)
{
int i;
for(i = yU; i < yU+yD; i++)
{
PintLinha(imagem,xU,i,xU + (xD-1),i,core);
}

}


void PintaCirculo(IVC *imagem,Coordenadas cordU, unsigned int radius,RGB core)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
 
    PintPonto(imagem,cordU.x, cordU.y + radius,core);
    PintPonto(imagem,cordU.x, cordU.y - radius,core);
    PintPonto(imagem,cordU.x + radius, cordU.y,core);
    PintPonto(imagem,cordU.x - radius, cordU.y,core);
 
    while(x < y) 
    {
        if(f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        PintPonto(imagem,cordU.x + x, cordU.y + y,core);
        PintPonto(imagem,cordU.x - x, cordU.y + y,core);
        PintPonto(imagem,cordU.x + x, cordU.y - y,core);
        PintPonto(imagem,cordU.x - x, cordU.y - y,core);
        PintPonto(imagem,cordU.x + y, cordU.y + x,core);
        PintPonto(imagem,cordU.x - y, cordU.y + x,core);
        PintPonto(imagem,cordU.x + y, cordU.y - x,core);
        PintPonto(imagem,cordU.x - y, cordU.y - x,core);
    }
}


//LISTAS_____________________________________________________________________

void liber(Listaint l) {
	Listaint aux;
	if (l != NULL) {
		aux = l->seguinte;
		free(l);
		liber(aux);
	}
}

void libert(Lista l) {
	Lista aux;
	if (l != NULL) {
		aux = l->seguinte;
		free(l);
		libert(aux);
	}
}

Listaint inseri(Listaint l, unsigned int v, unsigned int m)
{
	Listaint novoNodo=NULL;
	novoNodo=(Listaint)malloc(sizeof(struct nodoint));
        novoNodo->x = v; 
        novoNodo->y = m; 
        novoNodo->seguinte= l;
	return novoNodo;
	}



//LISTASINT_____________________________________________________________________

	
ListaINT inserir(ListaINT l, unsigned int v){
	ListaINT novoNodo=NULL;

	novoNodo=(ListaINT)malloc(sizeof(struct nodoINT));
        novoNodo->valor = v; 
        novoNodo->seguinte= l;
	return novoNodo;		
}

unsigned int soma(ListaINT l){
	if (l==NULL) 
		return 0;
	else
		return (l->valor + soma(l->seguinte));
}
unsigned int divide(ListaINT l){
	if (l==NULL) 
		return 1;
	else
		return (l->valor / divide(l->seguinte));
}
unsigned int subtrai(ListaINT l){
	if (l==NULL) 
		return 0;
	else
		return (l->valor - subtrai(l->seguinte));
}
unsigned int mult(ListaINT l){
	if (l==NULL) 
		return 1;
	else
		return (l->valor *mult(l->seguinte));
}

void liberta(ListaINT l){
	ListaINT aux;
	if (l!=NULL){
		aux = l->seguinte;
		free(l);
		liberta(aux);
	}
}
//_______________________________________________________________________________

ListaARV inserirNo(ListaARV l, nodeType* v)
{	
	if(!l)
	{
		l=(ListaARV)malloc(sizeof(struct nodoARV));
		l->valor = v;
		l->seguinte = NULL;
		return l;
	}
	else{
	ListaARV lista_variaveis=l;
	ListaARV novoNodo, actual, anterior=NULL;

	novoNodo=(ListaARV)malloc(sizeof(struct nodoARV));
	novoNodo->valor = v;
	novoNodo->seguinte = NULL;

	actual=lista_variaveis;
	while (actual!=NULL ){
		anterior = actual;
		actual = actual->seguinte;
	}
	if (anterior == NULL) { /* inserir no inicio */
		novoNodo->seguinte=lista_variaveis;
		l = novoNodo;
		return l;
	} else {
		novoNodo->seguinte = actual;
		anterior->seguinte = novoNodo;
		return anterior;
	}	
	}	
}

void libertaNo(ListaARV l){
	ListaARV aux;
	if (l!=NULL){
		aux = l->seguinte;
		free(l);
		libertaNo(aux);
	}
}

//LISTASVAR_____________________________________________________________________

/* inserir o par (varid, v) na lista l */
void define_variavel(Lista *l, char *varid, unsigned int v){
	Lista lista_variaveis=*l;
	Lista novoNodo, actual, anterior=NULL;

	novoNodo=(Lista)malloc(sizeof(struct nodo));
	novoNodo->nome = strdup(varid);
	novoNodo->valor = v;
	novoNodo->seguinte = NULL;

	actual=lista_variaveis;
	while (actual!=NULL && strcmp(varid,actual->nome)<0){
		anterior = actual;
		actual = actual->seguinte;
	}
	if (anterior == NULL) { /* inserir no inicio */
		novoNodo->seguinte=lista_variaveis;
		*l = novoNodo;
	} else {
		novoNodo->seguinte = actual;
		anterior->seguinte = novoNodo;
	}
}

/* procurar varid na lista e devolver o valor correspondente */
int valor_variavel(Lista l, char *varid){
	Lista aux = l;
	
	while(aux!=NULL && strcmp(varid,aux->nome)<0 ){
		aux = aux->seguinte;
	}
	if (aux!=NULL && strcmp(varid,aux->nome) == 0)
		return aux->valor;
	else
		return -1; /* erro */	
}

//_______________________________________________________________________________


//ARVORE_____________________________________________________________________

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)
nodeType *cor(RGB color) {
	nodeType *p;
	
	if ((p = malloc(sizeof(nodeType))) == NULL)
	yyerror("out of memory");
	
	p->type = typeCor;
	p->Cor.cor = color;
	return p;
}

nodeType *coordenada(Coordenadas cordU) {
	nodeType *p;
	
	if ((p = malloc(sizeof(nodeType))) == NULL)
	yyerror("out of memory");
	
	p->type = typeCoordenada;
	p->Coordenada.coordenada = cordU;
	return p;
}
nodeType *intval(unsigned int value) {
	nodeType *p;
	
	if ((p = malloc(sizeof(nodeType))) == NULL)
	yyerror("out of memory");
	
	p->type = typeIntVal;
	p->intVal.value = value;
	return p;
}


nodeType *idvar(char *id) {
	nodeType *p;
	
	if ((p = malloc(sizeof(nodeType))) == NULL)
		yyerror("out of memory");
	
	p->type = typeIdVar;
	p->idVar.idvar = id;
	return p;
}

nodeType *listaARV(ListaARV l) {
	nodeType *p;
	
	if ((p = malloc(sizeof(nodeType))) == NULL)
		yyerror("out of memory");
	
	p->type = typeListaARV;
	p->ListARV.lista = l;
	return p;
}

nodeType *operator(unsigned int oper, unsigned int nops, ...) {
	va_list ap;
	nodeType *p;
	unsigned int i;
	
	
	if ((p = malloc(sizeof(nodeType))) == NULL)
		yyerror("out of memory");
	if ((p->operator.op = malloc(nops * sizeof(nodeType *))) == NULL)
		yyerror("out of memory");
	
	p->type = typeOperator;
	p->operator.oper = oper;
	p->operator.nops = nops;
	va_start(ap, nops);
	for (i = 0; i < nops; i++)
		p->operator.op[i] = va_arg(ap, nodeType*);
	va_end(ap);
	return p;
}

void freeNode(nodeType *p) {
	unsigned int i;
	if (!p) return;
	if (p->type == typeOperator) {
		for (i = 0; i < p->operator.nops; i++)
			freeNode(p->operator.op[i]);
		free(p->operator.op);
	}
	free (p);
}

