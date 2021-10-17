#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "ili9341.h"
#include "ili9341gfx.h"
#include "touchscreen.h"


#define POINTCOLOUR PINK
#define CELL_0 20,10
#define CELL_1 100,10
#define CELL_2 180,10
#define CELL_3 20,90
#define CELL_4 100,90
#define CELL_5 180,90
#define CELL_6 20,170
#define CELL_7 100,170
#define CELL_8 180,170
#define X_AXIS 5
#define Y_AXIS 4

volatile uint16_t Array_of_cells [9][2] = {CELL_0, CELL_1, CELL_2, CELL_3, CELL_4, CELL_5, CELL_6, CELL_7, CELL_8};
static FILE mydata = FDEV_SETUP_STREAM(ili9341_putchar_printf, NULL, _FDEV_SETUP_WRITE);

uint16_t ReadADC(uint8_t ADCchannel);
void DRAW_GRID ();
void DRAW_X (uint16_t x1,uint16_t y1);
void DRAW_O (uint16_t x2,uint16_t y2);
void DRAW_TURN(int b[9]);
int win(const int board[9]);
int minimax(int board[9], int player);
void computerMove(int board[9]);
void playerMove1(int board[9], uint16_t positionX, uint16_t positionY);
void playerMove2(int board[9], uint16_t positionX, uint16_t positionY);

int main(void)
{
	
	stdout = & mydata;
	ili9341_init();//initial driver setup to drive ili9341
	ili9341_clear(GREEN);//fill screen with black colour
	_delay_ms(100);
	ili9341_setRotation(3);//rotate screen
	

	DDRD &= ~(1<<DDD6);
	PORTD |= (1<<PORTD6);
	
	ADMUX |= (1<<REFS0);
	//Enabling the analog to digital converter
	ADCSRA |= (1<<ADEN);
	//Prescaling the clock for the conversion by 32
	
	int board[9] = {0,0,0,0,0,0,0,0,0};
	ili9341_setcursor(40,60);
	ili9341_settextcolour(RED,GREEN);
	ili9341_settextsize(4);
	printf("XO Station");
	
	ili9341_setcursor(40,110);
	ili9341_settextcolour(BLUE,GREEN);
	ili9341_settextsize(2);
	printf("-Single Player");
	
	ili9341_setcursor(40,130);
	ili9341_settextcolour(BLUE,GREEN);
	ili9341_settextsize(2);
	printf("Multiplayer");
	

	int gameplay_state;

	
	while(1)
	{
		gameplay_state = 1;
		do
		{
			if(ReadADC(X_AXIS)<=400)
			{
				gameplay_state = 1;
				ili9341_setcursor(40,110);
				ili9341_settextcolour(BLUE,GREEN);
				ili9341_settextsize(2);
				printf("-Single Player");
			
				ili9341_setcursor(40,130);
				ili9341_settextcolour(BLUE,GREEN);
				ili9341_settextsize(2);
				printf("Multiplayer");
			}
			else if(ReadADC(X_AXIS)>600)
			{
				gameplay_state = 2;
				ili9341_setcursor(40,110);
				ili9341_settextcolour(BLUE,GREEN);
				ili9341_settextsize(2);
				printf("Single Player");
			
				ili9341_setcursor(40,130);
				ili9341_settextcolour(BLUE,GREEN);
				ili9341_settextsize(2);
				printf("-Multiplayer");
			}
		}while(PIND &(1<<PIND6));
	
		if(gameplay_state == 1)
		{
				ili9341_clear(BLACK);
				DRAW_GRID ();
				uint16_t p1X = 20;
				uint16_t p1Y = 10;
				for(int turn = 0; turn < 9 && win(board) == 0; turn++)
				{
					if((turn) % 2 == 1)
					{
						ili9341_setcursor(250,10);
						ili9341_settextcolour(WHITE,BLACK);
						ili9341_settextsize(2);
						printf("CMP   ");
						ili9341_setcursor(250,30);
						printf("Turn");
						ili9341_setcursor(250,50);
						printf("    ");
						computerMove(board);
						_delay_ms(1000);
						DRAW_TURN(board);
					}
					else
					{
						ili9341_setcursor(250,10);
						ili9341_settextcolour(WHITE,BLACK);
						ili9341_settextsize(2);
						printf("Player");
						ili9341_setcursor(250,30);
						printf("One ");
						ili9341_setcursor(250,50);
						printf("Turn");
						playerMove1(board,p1X,p1Y);
						_delay_ms(1000);
						DRAW_TURN(board);	
					}
				}
				
				switch(win(board))
				{
					case 0:
					ili9341_setcursor(250,10);
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Draw  ");
					ili9341_setcursor(250,30);
					printf("No one");
					ili9341_setcursor(250,50);
					printf("wins");
					_delay_ms(2000);
					break;
					case 1:
					ili9341_setcursor(250,10);
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("CMP");
					ili9341_setcursor(250,30);
					printf("Wins");
					_delay_ms(2000);
					break;
					case -1:
					ili9341_setcursor(250,10);
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Player");
					ili9341_setcursor(250,30);
					printf("One");
					ili9341_setcursor(250,50);
					printf("Wins");
					_delay_ms(2000);
					break;
				}
		}
		else if(gameplay_state == 2)
		{
				ili9341_clear(BLACK);
				DRAW_GRID ();
				uint16_t p1X = 20;
				uint16_t p1Y = 10;
				for(int turn = 0; turn < 9 && win(board) == 0; turn++)
				{
					if((turn) % 2 == 1)
					{
						ili9341_setcursor(250,10);
						ili9341_settextcolour(WHITE,BLACK);
						ili9341_settextsize(2);
						printf("Player");
						ili9341_setcursor(250,30);
						printf("Two");
						ili9341_setcursor(250,50);
						printf("Turn");
						playerMove2(board,p1X,p1Y);
						_delay_ms(1000);
						DRAW_TURN(board);
	     				
					}
					else
					{
						ili9341_setcursor(250,10);
						ili9341_settextcolour(WHITE,BLACK);
						ili9341_settextsize(2);
						printf("Player");
						ili9341_setcursor(250,30);
						printf("One");
						ili9341_setcursor(250,50);
						printf("Turn");
						playerMove1(board,p1X,p1Y);
						_delay_ms(1000);
						DRAW_TURN(board);
						
					}
				}
				
				switch(win(board))
				{
					case 0:
					ili9341_setcursor(250,10);
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Draw  ");
					ili9341_setcursor(250,30);
					printf("No one");
					ili9341_setcursor(250,50);
					printf("wins");
					_delay_ms(2000);
					break;
					case 1:
					ili9341_setcursor(250,10);
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Player");
					ili9341_setcursor(250,30);
					printf("Two");
					ili9341_setcursor(250,50);
					printf("Wins");
					_delay_ms(2000);
					break;
					case -1:
					ili9341_setcursor(250,10);
					ili9341_settextcolour(WHITE,BLACK);
					ili9341_settextsize(2);
					printf("Player");
					ili9341_setcursor(250,30);
					printf("One");
					ili9341_setcursor(250,50);
					printf("Wins");
					_delay_ms(2000);
					break;
				}
		}
	ili9341_clear(RED);
	ili9341_setcursor(20,100);
	ili9341_settextcolour(GREEN,RED);
	ili9341_settextsize(5);
	printf("GAME OVER \n");
	_delay_ms(2000);
	return 0;
	}
	
  
}

uint16_t ReadADC(uint8_t ADCchannel)
{
	//Select the ADC channel entered
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	//Starting the conversion
	ADCSRA |= (1<<ADSC);
	//Waiting until ADC conversion is complete
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}

void DRAW_GRID ()
{
	
	ili9341_drawhline(1,80,240,WHITE);
	ili9341_drawhline(1,81,240,WHITE);
	ili9341_drawhline(1,79,240,WHITE);
	
	ili9341_drawhline(1,160,240,WHITE);
	ili9341_drawhline(1,159,240,WHITE);
	ili9341_drawhline(1,161,240,WHITE);
	
	ili9341_drawvline(80,1,240,WHITE);
	ili9341_drawvline(81,1,240,WHITE);
	ili9341_drawvline(79,1,240,WHITE);
	
	ili9341_drawvline(160,1,240,WHITE);
	ili9341_drawvline(161,1,240,WHITE);
	ili9341_drawvline(159,1,240,WHITE);
	
	ili9341_drawvline(240,1,240,WHITE);
	ili9341_drawvline(239,1,240,WHITE);
	ili9341_drawvline(241,1,240,WHITE);
}

void DRAW_X (uint16_t x1,uint16_t y1)
{
	ili9341_setcursor(x1,y1);
	_delay_ms(2);
	ili9341_settextcolour(RED,BLACK);
	_delay_ms(2);
	ili9341_settextsize(8);
	_delay_ms(2);
	printf("X");
	_delay_ms(2);
}

void DRAW_O (uint16_t x2,uint16_t y2)
{
	ili9341_setcursor(x2,y2);
	_delay_ms(2);
	ili9341_settextcolour(GREEN,BLACK);
	_delay_ms(2);
	ili9341_settextsize(8);
	_delay_ms(2);
	printf("O");
	_delay_ms(2);
}

void DRAW_TURN(int b[9])
{
	for (int i = 0; i < 9; i++)
	{
		switch(b[i]) {
			case 1:
			DRAW_O (Array_of_cells[i][0],Array_of_cells[i][1]);
			break;
			case 0:
			break;
			case -1:
			DRAW_X (Array_of_cells[i][0],Array_of_cells[i][1]);
			break;
		}
	}
}


int win(const int board[9]) {
	//determines if a player has won, returns 0 otherwise.
	unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
	int i;
	for(i = 0; i < 8; ++i) {
		if(board[wins[i][0]] != 0 && board[wins[i][0]] == board[wins[i][1]] && board[wins[i][0]] == board[wins[i][2]])
		return board[wins[i][2]];
	}
	return 0;
}

int minimax(int board[9], int player) {
	//How is the position like for player (their turn) on board?
	int winner = win(board);
	if(winner != 0) return winner*player;
	int move;
	move = -1;
	int score = -2;//Losing moves are preferred to no move
	int i;
	for(i = 0; i < 9; ++i) {//For all moves,
		if(board[i] == 0) {//If legal,
			board[i] = player;//Try the move
			int thisScore = -minimax(board, player*-1);
			if(thisScore > score) {
				score = thisScore;
				move = i;
			}//Pick the one that's worst for the opponent
			board[i] = 0;//Reset board after try
		}
	}
	if(move == -1) return 0;
	return score;
}

void computerMove(int board[9]) {
	int move = -1;
	int score = -2;
	int i;
	for(i = 0; i < 9; ++i) {
		if(board[i] == 0) {
			board[i] = 1;
			int tempScore = -minimax(board, -1);
			board[i] = 0;
			if(tempScore > score) {
				score = tempScore;
				move = i;
			}
		}
	}
	//returns a score based on minimax tree at a given node.
	board[move] = 1;
}

void playerMove1(int board[9], uint16_t positionX, uint16_t positionY) 
{
	ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
	int move;
	do
	{
		restart:
		if(ReadADC(X_AXIS) > 900 && (positionY < 160))
		{
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
		positionY += 80;
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}
		_delay_ms(100);
	
		if(ReadADC(Y_AXIS) < 200 && (positionX < 160))
		{
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
		positionX += 80;
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}
		_delay_ms(100);
	
		if(ReadADC(X_AXIS) < 200 && (positionY > 80))
		{
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
		positionY -= 80;
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}
		_delay_ms(100);
	
		if(ReadADC(Y_AXIS) > 900 && (positionX > 80))
		{
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
		positionX -= 80;
		ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}

	} while((PIND & (1 << PIND6)));

	for (int i = 0; i < 9; i++)
	{
		if(positionX == Array_of_cells[i][0])
		{
			if(positionY == Array_of_cells[i][1])
				{
					move = i;
					if(board[move]!= 0)
					{
						goto restart; //invalid move restarts the input
					}
				}
			
		}
	}

	ili9341_fillrect(positionX-17,positionY-7,20,10,BLACK);
	board[move] = -1;
	
}
void playerMove2(int board[9], uint16_t positionX, uint16_t positionY)
{
	ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
	int move;
	do
	{
		restart:
		if(ReadADC(X_AXIS) > 700 && (positionY < 160))
		{
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
			positionY += 80;
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}
		_delay_ms(100);
		
		if(ReadADC(Y_AXIS) < 300 && (positionX < 160))
		{
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
			positionX += 80;
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}
		_delay_ms(100);
		
		if(ReadADC(X_AXIS) < 300 && (positionY > 80))
		{
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
			positionY -= 80;
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}
		_delay_ms(100);
		
		if(ReadADC(Y_AXIS) > 700 && (positionX > 80))
		{
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLACK);
			positionX -= 80;
			ili9341_fillrect(positionX-17,positionY-7,17,10,BLUE);
		}

	} while((PIND & (1 << PIND6)));

	for (int i = 0; i < 9; i++)
	{
		if(positionX == Array_of_cells[i][0])
		{
			if(positionY == Array_of_cells[i][1])
			{
				move = i;
				if(board[move]!= 0)
				{
					goto restart; //invalid move restarts the input
				}
			}
			
		}
	}

	ili9341_fillrect(positionX-17,positionY-7,20,10,BLACK);
	board[move] = 1;
	
}
