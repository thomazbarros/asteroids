/*
	Programa : Asteroidsrain.c
	Autores : Gustavo Henrique Alves dos Santos;Leopoldo André Dutra Lusquino Filho;Thomaz Avila Barros
	Data : 05/12/2007
	Descrição : Este programa gera um jogo de naves, cujo objetivo é resisitir ao maior tempo possivel a chuva
	de asteroides
*/
#include<stdio.h>
#include<allegro.h>
#include<stdlib.h>
#include<string.h>
#define LARGURATELA 800
#define ALTURATELA 600
#define LARGURAJOGO 800
#define ALTURAJOGO 500
#define CORES 16
#define RAIO 50
#define VERMELHO makecol(255,0,0)
#define VERDE makecol(0,255,0)
#define AZUL makecol(0,0,255)
#define BRANCO makecol(255,255,255)
#define PRETO makecol(0,0,0)
#define VELOCIDADE 1
#define NUMEROASTEROIDES 11
#define VELOCIDADETIRO 10
#define MAXTIRO 4	
#define COMBUSTIVELINICIAL 2000
#define DISPAROINICIAL 25
#define LARGURAASTEROIDE 44
#define LARGURANAVE 68
#define ALTURAASTEROIDE 46
#define ALTURANAVE 64
#define ALTURATIRO 9
#define LARGURATIRO 20
#define LARGURAESCUDO 68
#define ALTURAESCUDO 63
#define TEMPOESCUDO 25
#define LARGURAASTEROIDE2 24
#define ALTURAASTEROIDE2 28
#define LARGURAASTEROIDE3 25
#define ALTURAASTEROIDE3 27
typedef struct _SPRITE
		{
			int x,y,largura,altura;
		}SPRITE;
int inicializacao(void);
void finalizar(void);
void trabalho(int,int,int,int,int);
int colisao(SPRITE *,SPRITE *);
void animacao(BITMAP*asteroide,BITMAP*buffer,SPRITE *,int *,int *,int *,int);
void animacaopowerup(BITMAP*,BITMAP*,SPRITE *);
volatile int contador;
volatile int escudo;
void frames(void);
void tempoescudo(void);
void reaparecer(SPRITE *,SPRITE *,int);
void criarsprite(SPRITE *,int,int,int,int);
void atirar(BITMAP*,BITMAP*,SPRITE*,int);
int adicionarrecord(int novorecorde,char *novonome);
int lerrecord(BITMAP *);
int resetarrecord(void);
int abrirmenu(int *,int *,int *,int *,int *);
void lernomejogador(char*,BITMAP*);

int main(void)
{
	int dificuldade = 2,quantidadetiros = 2,quantidadeescudo = 2,quantidadevidas = 2,totalcombustivel = 2;
    BITMAP *buffer;
	
	srand(time(NULL));
	if(inicializacao() == 1)
	{
		finalizar();
		return 1;		
	}
	buffer = create_bitmap(LARGURATELA,ALTURATELA);
	buffer = load_bitmap("logotipo.bmp",NULL);
	blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
	rest(4000);
	while(!key[KEY_ESC])
	{	
		if(abrirmenu(&dificuldade,&quantidadetiros,&quantidadeescudo,&quantidadevidas,&totalcombustivel)==1)
		{
			return 0;
		}
		contador = 0;			
		trabalho(dificuldade,quantidadetiros,quantidadeescudo,quantidadevidas,totalcombustivel);
		textprintf_centre(screen,font,400,300,BRANCO,"PRESSIONE ENTER PARA VOLTAR AO MENU OU ESC PARA SAIR");
		while(!key[KEY_ENTER] && !key[KEY_ESC])
		{
			contador == 0;
		}
		
	}
	finalizar();
	
	return 0;
}
END_OF_MAIN();

int inicializacao(void)
{
	allegro_init();
	install_keyboard();
	install_timer();
	install_mouse();
	LOCK_FUNCTION(frames);
	LOCK_VARIABLE(contador);
	LOCK_VARIABLE(escudo);
	LOCK_FUNCTION(tempoescudo);
	install_int(tempoescudo,1000);
	install_int(frames,16);
	set_color_depth(CORES);
	if(set_gfx_mode(GFX_AUTODETECT,LARGURATELA,ALTURATELA,0,0)<0)
	{
		return 1;
	}
	install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL);
	set_volume(255,255);
	return 0;
}
void finalizar(void)
{
	remove_mouse();
	remove_keyboard();
	remove_int(frames);
	remove_int(tempoescudo);
	allegro_exit();
}
void trabalho(int dificuldade,int quantidadetiros, int quantidadeescudo,int quantidadevidas, int totalcombustivel)
{
	int *randy,*velocidade,pressionado1 = 0,pressionado2 = 0,k = 0,achou = 0,l=0,m,estadopowerup[3] = {0,0,0};
	int escudouso = 0,vidas = 3,i=0,j,recomecar=0,combustivel,disparo = DISPAROINICIAL,pontos = 0,tempescudo = 0;
	int colidiu=0,numeroasteroidestela,maximoescudo,maximocombustivel,maximodisparo,menorrecord;
	BITMAP *buffer,*nave,*asteroide,*tiros,*bescudo,*asteroide2,*asteroide3,*fundo,*powerupescudo,*poweruptiro,*bitmaphelp;
	BITMAP *powerupcombustivel;
	SPRITE nave1,*a,*tiro,spriteescudo,*powerup;
	char *nomenovojogador,*c="recordesint.bin";
	MIDI *musicajogo,*musicagameover,*musicahelp;
	SAMPLE *barulhotiro,*barulhoescudo;
	FILE *fint;
		
	
	clear(screen);
	fint=fopen(c,"r");
	if (!fint)
	{
		textprintf_centre(buffer,font,400,300,VERMELHO,"ERRO");
		return;
	}
	fseek(fint,-(1*sizeof(int)),SEEK_END);
	fread(&menorrecord,sizeof(int),1,fint);
	criarsprite(&nave1,0,ALTURAJOGO-ALTURANAVE-1,LARGURANAVE,ALTURANAVE);
	a =(SPRITE *) malloc (NUMEROASTEROIDES*sizeof(SPRITE));
	powerup =(SPRITE *) malloc (3*sizeof(SPRITE));
	tiro =(SPRITE *) malloc (MAXTIRO*sizeof(SPRITE));
	randy = (int *) malloc (NUMEROASTEROIDES*sizeof(int));
	velocidade = (int *) malloc (NUMEROASTEROIDES*sizeof(int));
	nomenovojogador = calloc(11,sizeof(char));
	musicagameover = load_midi("game over.mid");
	musicahelp = load_midi("HELP!.mid");
	barulhotiro = load_sample("tiro.wav");
	barulhoescudo = load_sample("acertoescudo.wav");
	
	switch(dificuldade)
	{
		case 1:
		{
			numeroasteroidestela = 5;
			musicajogo = load_midi("facil.mid");
			if(musicajogo == NULL)
			{
				return;
			}
			break;
		}
		case 2:
		{
			numeroasteroidestela = 8;
			musicajogo = load_midi("medio.mid");
			if(musicajogo == NULL)
			{
				return;
			}
			break;
		}
		case 3:
		{
			numeroasteroidestela = NUMEROASTEROIDES;
			musicajogo = load_midi("dificil.mid");
			if(musicajogo == NULL)
			{
				return;
			}
			break;
		}
	}
	switch(quantidadevidas)
	{
		case 1:
		{
			vidas = 1;
			break;
		}
		case 2:
		{
			vidas = 3;
			break;
		}
		case 3:
		{
			vidas = 5;
			break;
		}
	}
	switch(quantidadetiros)
	{														
		case 1:
		{								
			maximodisparo = 10;									
			break;									
		}								
		case 2:
		{
			maximodisparo = 25;									
			break;
		}
		case 3:
		{
			maximodisparo = 50;									
			break;
		}
		case 4:
		{	
			maximodisparo = 51;									
			break;
		}
	}
	disparo = maximodisparo;
	switch(quantidadeescudo)
	{														
		case 1:
		{								
			maximoescudo = 10;									
			break;									
		}								
		case 2:
		{
			maximoescudo = 25;									
			break;
		}
		case 3:
		{
			maximoescudo = 50;									
			break;
		}
		case 4:
		{	
			maximoescudo = 51;									
			break;
		}
	}
	switch(totalcombustivel)
	{														
		case 1:
		{								
			maximocombustivel = 1000;									
			break;									
		}								
		case 2:
		{
			maximocombustivel = 2000;									
			break;
		}
		case 3:
		{
			maximocombustivel = 4000;									
			break;
		}
		case 4:
		{	
			maximocombustivel = 4001;									
			break;
		}
	}
	combustivel = maximocombustivel;
	criarsprite(&spriteescudo,LARGURANAVE,nave1.y,89,64);
	criarsprite(powerup,0,0,35,35);/* powerup de combustivel */
	criarsprite(powerup+1,0,0,34,67);/*powerup de escudo */
	criarsprite(powerup+2,0,0,63,22);/*powerup de disparo */
	fundo = create_bitmap(LARGURATELA,ALTURAJOGO);
	tiros = create_bitmap(LARGURATIRO,ALTURATIRO);
	buffer = create_bitmap(LARGURATELA,ALTURATELA);
	nave = create_bitmap(LARGURANAVE,ALTURANAVE);
	bescudo = create_bitmap(LARGURAESCUDO,ALTURAESCUDO);
	asteroide = create_bitmap(LARGURAASTEROIDE,ALTURAASTEROIDE);
	asteroide2 = create_bitmap(LARGURAASTEROIDE2,ALTURAASTEROIDE2);
	asteroide3 = create_bitmap(LARGURAASTEROIDE3,ALTURAASTEROIDE3);
	poweruptiro= load_bitmap("poweruptiros.bmp",NULL);
	powerupescudo= load_bitmap("powerupescudo.bmp",NULL);
	powerupcombustivel= load_bitmap("powerupcombustivel.bmp",NULL);
	fundo= load_bitmap("fundo3.bmp",NULL);
	nave = load_bitmap("nave.bmp",NULL);
	asteroide = load_bitmap("asteroide.bmp",NULL);
	asteroide2 = load_bitmap("asteroide2.bmp",NULL);
	asteroide3 = load_bitmap("asteroide3.bmp",NULL);
	tiros = load_bitmap("tiro.bmp",NULL);
	bescudo = load_bitmap("escudo.bmp",NULL);
	bitmaphelp = load_bitmap("ajuda.bmp",NULL);	
	draw_sprite(buffer,fundo,0,0);	
	draw_sprite(buffer,nave,nave1.x,nave1.y);
	if (dificuldade != 3)
	{
		for(l=0;l<numeroasteroidestela;l++)
		{
			if ((l%3) == 0)
			{
				criarsprite(a+l,LARGURAJOGO-LARGURAASTEROIDE,(l+1)*ALTURAASTEROIDE,LARGURAASTEROIDE,ALTURAASTEROIDE);
				velocidade[l]=2;
			}
			if ((l%3) == 1)
			{
				criarsprite(a+l,LARGURAJOGO-LARGURAASTEROIDE,(l+1)*ALTURAASTEROIDE,LARGURAASTEROIDE2,ALTURAASTEROIDE2);
				velocidade[l]=3;
			}
			if ((l%3) == 2)
			{
				criarsprite(a+l,LARGURAJOGO-LARGURAASTEROIDE,(l+1)*ALTURAASTEROIDE,LARGURAASTEROIDE3,ALTURAASTEROIDE3);
				velocidade[l]=4;
			}
		}
	}
	else	
	{
		for(l=0;l<numeroasteroidestela;l++)
		{
			if ((l%3) == 0)
			{
				criarsprite(a+l,LARGURAJOGO-(((l%2)+1)*LARGURAASTEROIDE),((l)%6+1)*ALTURAASTEROIDE,LARGURAASTEROIDE,ALTURAASTEROIDE);
				velocidade[l]=3 + l;
			}
			if ((l%3) == 1)
			{
				criarsprite(a+l,LARGURAJOGO-(((l%2)+1)*LARGURAASTEROIDE),((l)%6+1)*ALTURAASTEROIDE,LARGURAASTEROIDE2,ALTURAASTEROIDE2);
				velocidade[l]=4 + l;
			}
			if ((l%3) == 2)
			{
				criarsprite(a+l,LARGURAJOGO-(((l%2)+1)*LARGURAASTEROIDE),((l)%6+1)*ALTURAASTEROIDE,LARGURAASTEROIDE3,ALTURAASTEROIDE3);
				velocidade[l]=5+ l;
			}
		}	
	}	
	for(l=0;l<NUMEROASTEROIDES;l++)
	{
		if(l%3 == 0)
		{
			draw_sprite(buffer,asteroide,(a+l)->x,(a+l)->y);
		}
		if(l%3 == 1)
		{
			draw_sprite(buffer,asteroide2,(a+l)->x,(a+l)->y);
		}
		if(l%3 == 2)
		{
			draw_sprite(buffer,asteroide3,(a+l)->x,(a+l)->y);
		}
	}
	blit(buffer,screen,0,0,0,0,LARGURAJOGO,ALTURAJOGO);
	for(l=0;l<numeroasteroidestela;l++)
	{
		randy[l] =rand()%(ALTURAJOGO-(a+l)->altura);
	}
	contador = 0;
	play_midi(musicajogo,-1);
	while(!key[KEY_ESC] && vidas > 0)
	{	
		if(key[KEY_P] && !pressionado1)
		{	
			textprintf_centre(screen,font,400,300,BRANCO," PAUSE");
			while(!key[KEY_R])
					contador = 1;
		}
		if(key[KEY_H])
		{	
			clear(screen);
			clear(buffer);
			play_midi(musicahelp,-1);
			draw_sprite(buffer,bitmaphelp,0,0);
			draw_sprite(screen,buffer,0,0);
			while(!key[KEY_R])
					contador = 1;
		play_midi(musicajogo,-1);
		}
		clear(buffer);	
		rectfill(buffer,0,ALTURAJOGO,LARGURATELA-1,ALTURATELA-1,AZUL);
		line(buffer,0,ALTURAJOGO,LARGURATELA-1,ALTURAJOGO,BRANCO);
		textprintf(buffer,font,0,ALTURAJOGO+5,BRANCO,"Combustivel");
		if (combustivel>0)
		{
			rectfill(buffer,100,ALTURAJOGO+5,100+combustivel/8,ALTURAJOGO+10,PRETO);
		}
		textprintf(buffer,font,0,ALTURAJOGO+25,BRANCO,"Disparos");
		if (disparo > 0)
		{	
			rectfill(buffer,100,ALTURAJOGO+25,100 +disparo*4,ALTURAJOGO+30,VERMELHO);
		}
		textprintf(buffer,font,0,ALTURAJOGO+45,BRANCO,"Escudo");
		if (tempescudo < maximoescudo)
		{	
			rectfill(buffer,100,ALTURAJOGO+45,100+((maximoescudo-tempescudo)*4),ALTURAJOGO+50,VERDE);
		}
		textprintf(buffer,font,0,ALTURAJOGO+65,BRANCO,"Vidas %d",vidas);
		textprintf(buffer,font,0,ALTURAJOGO+85,BRANCO,"Pontos %d",pontos);
		if (escudo >0)
		{
			escudo--;
			if (escudouso)
			{
				if (maximoescudo != 51)
				{
					tempescudo ++;
				}
				if (tempescudo > maximoescudo)
				{
					escudouso = 0;
				}
			}
		}
		if (contador >0)
		{			
			draw_sprite(buffer,fundo,0,0);
			if (i!=0)
			{
				if (i > MAXTIRO)
				{
					i =MAXTIRO;
					if ((tiro[0].x+tiro[0].largura) > LARGURAJOGO||(tiro[1].x+tiro[1].largura) > LARGURAJOGO||(tiro[2].x+tiro[2].largura) > LARGURAJOGO||(tiro[3].x+tiro[0].largura) > LARGURAJOGO)
					{
						if (recomecar >= MAXTIRO) /*A funcao da variavel recomeçar é a cada vez que o vetor passar do limite se voltar para o começo do vetor*/
						{
							recomecar = 0;	
						}	
						tiro[recomecar].x = nave1.x+nave1.largura;
						tiro[recomecar].y = nave1.y+(ALTURANAVE/2);
						tiro[recomecar].largura = LARGURATIRO;	
						tiro[recomecar].altura = ALTURATIRO;
						play_sample(barulhotiro,255,128,1000,0);	
						if (disparo != 51)
						{
							disparo--;
						}
						recomecar++;
					}
				}
				for(j=0;j<i;j++)
				{
					atirar(buffer,tiros,tiro+j,VELOCIDADETIRO);			
					for(l=0;l<numeroasteroidestela;l++)
					{
						if(colisao(&tiro[j],a+l) == 1)
						{
							if(rand()%6 == 0)
							{
								if((l%3)==0 && estadopowerup[0] != 1)
								{
									powerup[0].x = a[l].x;
									powerup[0].y = a[l].y;
									estadopowerup[0] = 1;									
								}
								if((l%3)==1 && estadopowerup[1] != 1)
								{
									powerup[1].x = a[l].x;
									powerup[1].y = a[l].y;
									estadopowerup[1] = 1;	
								}
								if((l%3)==2 && estadopowerup[2] != 1)
								{
									powerup[2].x = a[l].x;
									powerup[2].y = a[l].y;
									estadopowerup[2] = 1;	
								}	
							}
							tiro[j].x = LARGURAJOGO + LARGURATIRO;
							(a+l)->x = -LARGURAASTEROIDE;
							pontos+=dificuldade*2;
							break;
						}
					}
				}	
			}
			if(estadopowerup[0] == 1)
			{
				if(powerup[0].x > LARGURATELA+100 || powerup[0].x < -100)
				{
					estadopowerup[0] = 0;
				}
				else
				{
					animacaopowerup(powerupcombustivel,buffer,powerup);
				}
			}
			if(estadopowerup[1] == 1)
			{
				if(powerup[1].x > LARGURATELA+100 || powerup[1].x < -100)
				{
					estadopowerup[1] = 0;
				}	
				else
				{
					animacaopowerup(powerupescudo,buffer,powerup+1);	
				}
			}
			if(estadopowerup[2] == 1)
			{
				if(powerup[2].x > LARGURATELA+100 || powerup[2].x < -100)
				{
					estadopowerup[2] = 0;
				}
				else
				{
					animacaopowerup(poweruptiro,buffer,powerup+2);	
				}	
			}	
			for(m=0;m<3;m++)
			{
				if(estadopowerup[m] == 1)
				{
					if(colisao(powerup+m,&nave1) == 1)
					{
						if(m == 0)
						{
							combustivel = maximocombustivel;
						}
						else
						{
							if(m == 1)
							{
								tempescudo = 0;
							}
							else
							{
								if(m == 2)
								{		
									disparo = maximodisparo;
								}
							}
						}
						powerup[m].x = -100;
						break;
					}
				}
			}
			contador --;
			for(l=0;l<numeroasteroidestela;l++)
			{
				if(l%3 == 0)
				{
					animacao(asteroide,buffer,a+l,&randy[l],&velocidade[l],&pontos,dificuldade);
				}
				if(l%3 == 1)
				{
					animacao(asteroide2,buffer,a+l,&randy[l],&velocidade[l],&pontos,dificuldade);
				}
				if(l%3 == 2)
				{
					animacao(asteroide3,buffer,a+l,&randy[l],&velocidade[l],&pontos,dificuldade);
				}
			}
			if (!escudouso)
			{
				draw_sprite(buffer,nave,nave1.x,nave1.y);
			}
			else
			{
				draw_sprite(buffer,bescudo,nave1.x,nave1.y);
			}			
			blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
			colidiu=0;
			for(l=0;l<numeroasteroidestela;l++)
			{
				if(colisao(&nave1,a+l))
				{
					colidiu = 1;
					break;
				}
			}
			if (!colidiu) 
			{	
				if((combustivel > 0))
				{
					if (key[KEY_LEFT])
					{	
						if (nave1.x-VELOCIDADE > 0)
						{
							if (combustivel != 4001)
							{
								combustivel--;	
							}	
							nave1.x-=VELOCIDADE;
							spriteescudo.x-=VELOCIDADE;							
						}	
						else
						{
							nave1.x = 0;
							spriteescudo.x = LARGURANAVE;
						}
					}
					else	
					{
						if (key[KEY_RIGHT])
						{
							if ((nave1.x+nave1.largura)+VELOCIDADE < LARGURAJOGO) 
							{
								
									if(combustivel != 4001)
									{
										combustivel--;
									}
									nave1.x+=VELOCIDADE;
									spriteescudo.x+=VELOCIDADE;								
							}
							else
							{		
								nave1.x = LARGURAJOGO-LARGURANAVE-1;
								spriteescudo.x = nave1.x+nave1.largura;
							}		
						}
						else
						{
							if (key[KEY_UP])  
							{
								if (nave1.y-VELOCIDADE> 0)
								{	
										if(combustivel != 4001)
										{	
											combustivel--;	
										}
										nave1.y-=VELOCIDADE;
										spriteescudo.y-=VELOCIDADE;		
								}
								else
								{	
									nave1.y = 0;
									spriteescudo.y = 0;		
								}							
							}
							else
							{
								if (key[KEY_DOWN])
								{
									if ((nave1.y+nave1.altura)+VELOCIDADE< ALTURAJOGO)
									{
											if(combustivel != 4001)
											{
												combustivel--;
											}
											nave1.y+=VELOCIDADE;
											spriteescudo.y += VELOCIDADE;
									}
									else
									{	
										nave1.y = ALTURAJOGO-ALTURANAVE-1;
										spriteescudo.y = nave1.y;	
									}	
										
								}
							}								
						}
					}		
				}
			}
			else
			{	
				if (!escudouso)
				{
					vidas --;		
					reaparecer(&nave1,a,numeroasteroidestela);		
					spriteescudo.x = nave1.x+nave1.largura;
					spriteescudo.y = nave1.y;
					i = 0;
				}
				else
				{
					achou = 0;
					k = 0;
					while(!achou && k < numeroasteroidestela)
					{
						if (colisao(&spriteescudo,a+k) == 1)
						{
							achou = 1;
						    a[k].x = -LARGURAASTEROIDE;
						    break;
						}
						else
							k++;
					}
					if (!achou)
					{
						vidas --;		
						reaparecer(&nave1,a,numeroasteroidestela);		
						spriteescudo.x = nave1.x+nave1.largura;
						spriteescudo.y = nave1.y;
						i = 0;
					}
					else
					{
						play_sample(barulhoescudo,255,128,1000,0);	
					}
				}
			}
			if((!pressionado2)&&(disparo>0)&&(!escudouso))
			{
				if(key[KEY_SPACE])
				{
					i++;
					if (i <= MAXTIRO)
					{
						play_sample(barulhotiro,255,128,1000,0);	
						tiro[i-1].x = nave1.x + nave1.largura;
						tiro[i-1].y = nave1.y+(ALTURANAVE/2);
						tiro[i-1].largura = LARGURATIRO;
						tiro[i-1].altura = ALTURATIRO;
						if(disparo != 51)
						{
							disparo--;
						}
					}
				}
			}
			if(!pressionado2 && tempescudo < maximoescudo)
			{
					if (key[KEY_C])
					{
						if (escudouso == 0 && tempescudo < maximoescudo)
							escudouso = 1;
						else
							escudouso = 0;
					}
			}
			if (key[KEY_DOWN] || key[KEY_UP] || key[KEY_LEFT] || key[KEY_RIGHT] || key[KEY_P])
				pressionado1 =1;
			else
				pressionado1 = 0;
			if(key[KEY_SPACE] || key[KEY_C])
				pressionado2 = 1;
			else
				pressionado2 = 0; 
		}
	}
	stop_midi();
	clear(screen);
	if (vidas == 0)
	{
		textprintf(screen,font,0,0,BRANCO,"Vidas = 0");
	}		
	clear(buffer);
	
	textprintf_centre(screen,font,400,300,BRANCO,"GAME OVER");
	play_midi(musicagameover,-1);
	while(escudo < 3);
	if(menorrecord < pontos)
	{	
		lernomejogador(nomenovojogador,buffer);
	}
	clear(buffer);
	clear(screen);
	destroy_bitmap(buffer);
	adicionarrecord(pontos,nomenovojogador);
	stop_midi();
}
int colisao(SPRITE *s1,SPRITE *s2)
{
	if((s1->x > s2->x + s2->largura)||(s1->y > s2->y + s2->altura)||(s1->x + s1->largura < s2->x)||(s1->y + s1->altura < s2->y)) 
	{
		return 0;
	}
	else
		return 1;
}
void animacao(BITMAP*asteroide,BITMAP*buffer,SPRITE *a,int *randy1,int *velocidade,int *pontos,int dificuldade)
{ 
	if(a->x > -(LARGURAASTEROIDE+50))
	{
		draw_sprite(buffer,asteroide,a->x,a->y);
		a->x-=*velocidade;
	}
	if (a->x <= -LARGURAASTEROIDE)
	{
		if((rand()%100) == 2)
		{
			a->x =800-LARGURAASTEROIDE;
			a->y = *randy1;
			draw_sprite(buffer,asteroide,a->x,a->y);
			*pontos= *pontos+(dificuldade*2);
			*randy1 = rand ()%(ALTURAJOGO-(a->altura));
			*velocidade = dificuldade + rand()%8;
		}	
	}	

}
void animacaopowerup(BITMAP*powerup,BITMAP*buffer,SPRITE *p)
{ 
	draw_sprite(buffer,powerup,p->x,p->y);
	p->x-=1;
}
void reaparecer(SPRITE *n,SPRITE *a,int numeroasteroidestela)
{
	int x,y,i,achou = 0;
	while(achou == 0)
	{
		y =rand()%(ALTURAJOGO-(n->altura));
		x =rand()%(LARGURATELA-(n->largura));
		n->y=y;
		n->x=x;
		achou = 1;
		for(i=0;i<numeroasteroidestela;i++)
		{
			if ((colisao(n,a+i) == 1)) 
			{
				achou = 0;
				break;
			}
		}
	}
}
void criarsprite(SPRITE *s,int x,int y,int largura,int altura)
{
	s->x=x;
	s->y=y;
	s->largura=largura;
	s->altura = altura;
}
void atirar(BITMAP*buffer,BITMAP*tiros,SPRITE *t,int velocidade)
{
	t->x = t->x+velocidade;
	draw_sprite(buffer,tiros,t->x,t->y);		

}
void frames(void)
{
	contador ++;
}
END_OF_FUNCTION(frames);
void tempoescudo(void)
{
	escudo++;
}
END_OF_FUNCTION(tempoescudo);
int adicionarrecord(int novorecorde,char *novonome)
{
	FILE *fint,*fnom;
	char *c="recordesint.bin",*c2="recordesnom.bin",string1[80],string2[80],nome[5][80];
	int vetor[5],i,temp,temp2,j;
	
	fint=fopen(c,"r+");
	if (!fint)
	{
		textprintf_centre(screen,font,400,300,VERMELHO,"ERRO");
		return 1;
	}
	fnom=fopen(c2,"r+");
	if (!fnom)
	{
		textprintf_centre(screen,font,400,300,VERMELHO,"ERRO");
		return 2;
	}
	for(i=0;i<5;i++)
	{
		fread(&vetor[i],sizeof(int),1,fint);
		fread(nome[i],sizeof(nome[i]),1,fnom);
	}
	for(i = 0;i<5;i++)
	{
		if (novorecorde >= vetor[i])
		{
			temp = vetor[i];
			vetor[i] = novorecorde;
			strcpy(string1, nome[i]);
			strcpy(nome[i], novonome);																		
			if (i < 4)/*caso i=4,nao é necessário fazer pois ja estara organizado*/								
			{     																					   
				for(j = i+1;j <5;j++)																			
				{
					if (j < 4)
					{
						temp2 = vetor[j];
						vetor[j] = temp;
						temp = temp2;
						strcpy(string2, nome[j]);
						strcpy(nome[j], string1);
						strcpy(string1, string2);											
					}
					else
					{	
						vetor[j] = temp;
						strcpy(nome[j],string1);
					}
				}
			}
			break;
		}						
	}			
	fseek(fint,0,SEEK_SET);
	fseek(fnom,0,SEEK_SET);
	for(i = 0;i<5;i++)
	{	
		fwrite(&vetor[i],sizeof(int),1,fint);		
		fwrite(nome[i],sizeof(nome[i]),1,fnom);
	}
	
	fclose(fint);
	fclose(fnom);		
	return 0;
}
int lerrecord(BITMAP *menus)
{
	FILE *fint,*fnom;
	char *c="recordesint.bin",*c2="recordesnom.bin",nome[5][80];
	int vetor[5],i;

	fint=fopen(c,"r+");
	if (!fint)
	{
		textprintf_centre(menus,font,400,300,VERMELHO,"ERRO");
		return 1;
	}
	fnom=fopen(c2,"r+");
	if (!fnom)
	{
		textprintf_centre(menus,font,400,300,VERMELHO,"ERRO");
		return 2;
	}
	for(i=0;i<5;i++)
	{
		fread(nome[i],sizeof(nome[i]),1,fnom);
		textprintf_centre(menus,font,50,(i+1)*50,VERMELHO,"%s",nome[i]);
		fread(&vetor[i],sizeof(int),1,fint);
		textprintf_centre(menus,font,150,(i+1)*50,VERMELHO,"%d",vetor[i]);
	}
	fclose(fint);
	fclose(fnom);
	return 0;
}
int resetarrecord(void)
{
	FILE *fint,*fnom;
	char *c="recordesint.bin",*c2="recordesnom.bin",nome[5][80];
	int vetor[5],i;
	
	fint=fopen(c,"w+");
	if (!fint)
	{
		printf("Erro");
		return 1;
	}
	fnom=fopen(c2,"w");
	if (!fnom)
	{
		printf("Erro");
		return 1;
	}
	for(i=0;i<5;i++)
	{
		strcpy(nome[i],"Default");
		vetor[i] = 0;
		fwrite(nome[i],sizeof(nome[i]),1,fnom);
		fwrite(vetor+i,sizeof(int),1,fint);
	}
	fclose(fint);
	fclose(fnom);
	
	return 0;
}
int abrirmenu(int *dificuldade, int *quantidadetiros, int *quantidadeescudo,int *quantidadevidas, int *totalcombustivel)
{
	BITMAP *fundorecord,*fundoopcao,*menus,*opcoes,*menurecord,*opcoesvariaveis,*voltarmenu;
	MIDI *musicamenu,*musicaopcao,*musicarecorde;
	
	menus = load_bitmap("menu.bmp",NULL);
	opcoes = load_bitmap("opcoes.bmp",NULL);
	menurecord = load_bitmap("opcoesrecord.bmp",NULL);
	opcoesvariaveis = load_bitmap("variaveis.bmp",NULL);
	voltarmenu = load_bitmap("voltarmenu.bmp",NULL);
	fundorecord = load_bitmap("fundo4.bmp",NULL);
	fundoopcao = load_bitmap("fundo2.bmp",NULL);
	musicamenu = load_midi("menu.mid");
	if(musicamenu == NULL)
	{
		return 1;
	}
	musicaopcao = load_midi("menuopcao.mid");
	if(musicaopcao == NULL)
	{
		return 1;
	}
	musicarecorde = load_midi("recorde.mid");
	if(musicarecorde == NULL)
	{
		return 1;
	}
	play_midi(musicamenu,-1);
	while(1)
	{
		if(contador >0)
		{
				draw_sprite(menus,opcoes,0,35);
				position_mouse(mouse_x,mouse_y);
				show_mouse(menus);
				blit(menus,screen,0,0,0,0,LARGURATELA,ALTURATELA);						
				contador--;
				if ((mouse_y > 38) && (mouse_y < 69) && (mouse_b & 1))
				{
					if(mouse_x > 40 && mouse_x < 181)
					{	
						break;					
					}	
					if(mouse_x > 232 && mouse_x < 373)
					{
						play_midi(musicaopcao,-1);
						clear(screen);
						show_mouse(NULL);
						while(!((mouse_y > 565) && (mouse_y < 599) && (mouse_x > 0) && (mouse_x < 258)&&(mouse_b & 1)))
						{						
							if(contador>0)
							{
								clear(menus);
								draw_sprite(menus,fundoopcao,0,0);
								textprintf_centre(menus,font,400,80,BRANCO,"Dificuldade");
								draw_sprite(menus,opcoesvariaveis,300,100);
								textprintf_centre(menus,font,400,180,BRANCO,"Tiros");
								draw_sprite(menus,opcoesvariaveis,300,200);
								textprintf_centre(menus,font,400,280,BRANCO,"Escudo");
								draw_sprite(menus,opcoesvariaveis,300,300);
								textprintf_centre(menus,font,400,380,BRANCO,"Vidas");
								draw_sprite(menus,opcoesvariaveis,300,400);
								textprintf_centre(menus,font,400,480,BRANCO,"Combustivel");
								draw_sprite(menus,opcoesvariaveis,300,500);
								draw_sprite(menus,voltarmenu,0,ALTURATELA-35);
								if ((mouse_x > 300) && (mouse_x < 315) && (mouse_b & 1))
								{
									if ((mouse_y > 101) && (mouse_y < 138))
									{
										if(*dificuldade > 1)
										{
											*dificuldade -=1;
											rest(100);
										}
									}
									if ((mouse_y > 201) && (mouse_y < 238))
									{
										if(*quantidadetiros > 1)
										{
											*quantidadetiros-=1;
											rest(100);
										}
									}
									if ((mouse_y > 301) && (mouse_y < 338))
									{
										if(*quantidadeescudo > 1)
										{
											*quantidadeescudo-=1;
											rest(100);
										}	
									}
									if ((mouse_y > 401) && (mouse_y < 438))
									{
										if(*quantidadevidas > 1)
										{
											*quantidadevidas-=1;
											rest(100);
										}	
									}
									if ((mouse_y > 501) && (mouse_y < 538))
									{
										if(*totalcombustivel > 1)
										{
											*totalcombustivel-=1;
											rest(100);
										}	
									}
							    }
								if ((mouse_x > 470) && (mouse_x < 485) && (mouse_b & 1))
								{
									if ((mouse_y > 101) && (mouse_y < 138))
									{
										if(*dificuldade < 3)
										{
											*dificuldade +=1;
											rest(100);
										}
									}
									if ((mouse_y > 201) && (mouse_y < 238))
									{
										if(*quantidadetiros < 4)
										{
											*quantidadetiros+=1;
											rest(100);
										}
									}
									if ((mouse_y > 301) && (mouse_y < 338))
									{
										if(*quantidadeescudo < 4)
										{
											*quantidadeescudo+=1;
											rest(100);
										}	
									}
									if ((mouse_y > 401) && (mouse_y < 438))
									{
										if(*quantidadevidas < 3)
										{
											*quantidadevidas+=1;
											rest(100);
										}	
									}
									if ((mouse_y > 501) && (mouse_y < 538))
									{
										if(*totalcombustivel < 4)
										{
											*totalcombustivel+=1;
											rest(100);
										}	
									}
								}
								switch(*dificuldade)
								{
									case 1:
									{
										textprintf_centre(menus,font,400,117,BRANCO,"Facil");
										break;
									}
									case 2:
									{
										textprintf_centre(menus,font,400,117,BRANCO,"Medio");
										break;
									}
									case 3:
									{
										textprintf_centre(menus,font,400,117,BRANCO,"Dificil");
										break;
									}
								}
								switch(*quantidadetiros)
								{
									case 1:
									{
										textprintf_centre(menus,font,400,217,BRANCO,"10");
										break;
									}
									case 2:
									{
										textprintf_centre(menus,font,400,217,BRANCO,"25");
										break;
									}
									case 3:
									{
										textprintf_centre(menus,font,400,217,BRANCO,"50");
										break;
									}
									case 4:
									{
										textprintf_centre(menus,font,400,217,BRANCO,"Infinito");
										break;
									}
								}
								switch(*quantidadeescudo)
								{
									case 1:
									{
										textprintf_centre(menus,font,400,317,BRANCO,"10");
										break;
									}
									case 2:
									{
										textprintf_centre(menus,font,400,317,BRANCO,"25");
										break;
									}
									case 3:
									{
										textprintf_centre(menus,font,400,317,BRANCO,"50");
										break;
									}
									case 4:
									{
										textprintf_centre(menus,font,400,317,BRANCO,"Infinito");
										break;
									}
								}
								switch(*quantidadevidas)
								{
									case 1:
									{
										textprintf_centre(menus,font,400,417,BRANCO,"1");
										break;
									}
									case 2:
									{
										textprintf_centre(menus,font,400,417,BRANCO,"3");
										break;
									}
									case 3:
									{
										textprintf_centre(menus,font,400,417,BRANCO,"5");
										break;
									}
								}
								switch(*totalcombustivel)
								{
									case 1:
									{
										textprintf_centre(menus,font,400,517,BRANCO,"1000");
										break;
									}
									case 2:
									{
										textprintf_centre(menus,font,400,517,BRANCO,"2000");
										break;
									}
									case 3:
									{
										textprintf_centre(menus,font,400,517,BRANCO,"4000");
										break;
									}
									case 4:
									{
										textprintf_centre(menus,font,400,517,BRANCO,"Infinito");
										break;
									}
								}
								position_mouse(mouse_x,mouse_y);
								show_mouse(menus);
								blit(menus,screen,0,0,0,0,LARGURATELA,ALTURATELA);
								contador --;
							}						
						}
						play_midi(musicamenu,-1);
					}		
					if(mouse_x > 425 && mouse_x < 566)
					{
						
						play_midi(musicarecorde,-1);
						lerrecord(menus);
						clear(screen);
						show_mouse(NULL);
						draw_sprite(menus,fundorecord,0,0);
						while(!((mouse_y > 568) && (mouse_y < 599) && (mouse_x > 234) && (mouse_x < 489)&&(mouse_b & 1)))
						{
							if(contador > 0)
								{
								clear(menus);
								show_mouse(NULL);
								draw_sprite(menus,fundorecord,0,0);
								draw_sprite(menus,menurecord,0,ALTURATELA-35);
								position_mouse(mouse_x,mouse_y);
								show_mouse(menus);						
								contador--;
								lerrecord(menus);
								blit(menus,screen,0,0,0,0,LARGURATELA,ALTURATELA);
							}							
							if(((mouse_y > 568) && (mouse_y < 599) && (mouse_x > 39) && (mouse_x < 180)&&(mouse_b & 1)))
							{
								resetarrecord();
								clear(menus);
								show_mouse(NULL);
								draw_sprite(menus,fundorecord,0,0);
								draw_sprite(menus,menurecord,0,ALTURATELA-35);
								position_mouse(mouse_x,mouse_y);
								show_mouse(menus); 
								contador--;
								lerrecord(menus);									
								blit(menus,screen,0,0,0,0,LARGURATELA,ALTURATELA);
							}						
						}
						menus = load_bitmap("menu.bmp",NULL);
						play_midi(musicamenu,-1);
					}
					if(mouse_x > 618 && mouse_x < 759)
					{
						finalizar();
						return 1;
					}	
				} 		
		}
		show_mouse(NULL);
		clear(menus);
		menus = load_bitmap("menu.bmp",NULL);	
	}
	destroy_midi(musicamenu);
	destroy_midi(musicaopcao);
	destroy_midi(musicarecorde);	
	return 0;
}
void lernomejogador(char *jogador,BITMAP * buffer)
{
	char a;
	int i=0;
	
	for(i=0;i<11;i++)
	{
		jogador[i] = '\0';
	}
	
	i=0;
	
	textprintf_centre(buffer,font,400,250,BRANCO,"ENTRE COM O NOME DO NOVO RECORDISTA");
	rest(3000);
	blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
	while(!key[KEY_ESC])
	{
		while(i < 10)
		{
			a = ('A'-1 + (readkey() >> 8));
			if((a>='A')&&(a<='Z'))
			{
				jogador[i] = a;
				i++;
			}
			if((key[KEY_BACKSPACE])&&(i>0))
			{
				i--;
				jogador[i] = '\0';
			}
			if((key[KEY_ENTER])||(i == 10))
			{
				textprintf_centre(buffer,font,400,250,BRANCO,"ENTRE COM O NOME DO NOVO RECORDISTA");
				textprintf_centre(buffer,font,400,300,makecol(255,255,255),"%s",jogador);
				textprintf_centre(buffer,font,400,330,makecol(255,255,255),"Esta certo?(S/N)");
				blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
				readkey();
				if(key[KEY_S])
				{
					textprintf_centre(buffer,font,400,250,BRANCO,"ENTRE COM O NOME DO NOVO RECORDISTA");
					textprintf_centre(buffer,font,400,300,makecol(255,255,255),"%s",jogador);
					blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
					return;
				}
				else
				{
					textprintf_centre(buffer,font,400,250,BRANCO,"ENTRE COM O NOME DO NOVO RECORDISTA");
					textprintf_centre(buffer,font,400,300,makecol(255,255,255),"%s",jogador);
					blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
					for(i=0;i<11;i++)
					{
						jogador[i] = '\0';	
					}
					i = 0;
				}
			}
			clear(buffer);
			textprintf_centre(buffer,font,400,250,BRANCO,"ENTRE COM O NOME DO NOVO RECORDISTA");
			textprintf_centre(buffer,font,400,300,makecol(255,255,255),"%s",jogador);
			blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
		}
		textprintf_centre(buffer,font,400,250,BRANCO,"ENTRE COM O NOME DO NOVO RECORDISTA");
		textprintf_centre(buffer,font,400,300,makecol(255,255,255),"%s",jogador);
		blit(buffer,screen,0,0,0,0,LARGURATELA,ALTURATELA);
	}
}

