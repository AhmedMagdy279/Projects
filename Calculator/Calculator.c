//documentation at the bottom of the file

#include "../../LIB/StdTypes.h"
#include "Calculator_Interface.h"
#include "../../CFG/Calculator_CFG.h"
#include "../../MCAL/DIO/DIO_Interface.h"
#include "../../HAL/LCD/LCD_Interface.h"
#include "../../HAL/KeyPad/KeyPad_Interface.h"
#define  F_CPU   8000000
#include <util/delay.h>


#define CELL_Position 0		//indicate where are  we on the LCD position for the cell usage ranges (0~15)

static s32 result=0;
static u16 last_Clicked=NULL;
static u16 var_ARR[MAX_VAR]= {0};		//we got a max variable number that can be shown on the lcd and operated as well
static u8  op_ARR [MAX_VAR-1]={0};		//for the same amount of vars we have operation to be done with check the documentation for further info
static u8 varCount=0,opCount=0,op_prioCheck=0 ;
void Calculator_Init()
{
	LCD_CLR();
	LCD_WriteString("Calc app ready:");
	LCD_GoTo(2,0);
}

static void Calculator_Calculate(void)
{
	u8 indx;		//varINDX is for special cases for the 'R' positions
	if (varCount!=0 && opCount!=0)
	{
		//first we need to check the priority count variable to finish all the multiply and divide cases in the eqn
		while(op_prioCheck!=0)
		{
			for (indx=0;indx<opCount;indx++)
			{
				u8 varINDX=0,varINDX1=1;
				if (op_ARR[indx]=='*' || op_ARR[indx]=='/')
				{
					//here we go for the hardest part
					//we need to make sure that if we took two numbers we have to save the new result in indx position
					//and the indx+1 shall be 'R', and also we need to the change the op[indx]=0 as well.
					//And if we hit 'R' before making the operation we need to check the previous or further array elements.
					while(var_ARR[indx-varINDX]=='R')
					{
						varINDX++;
					}
					while (var_ARR[indx+varINDX1]=='R')
					{
						varINDX1++;
					}
					
					if (op_ARR[indx]=='*')
					var_ARR[indx]=var_ARR[indx-varINDX] * var_ARR[indx+varINDX1];
					else
					var_ARR[indx]=var_ARR[indx-varINDX] / var_ARR[indx+varINDX1];
					
					var_ARR[indx+1]='R';
					if(varINDX!=0)
						var_ARR[indx-varINDX]='R';
					op_ARR[indx]=0;
				}
			}
			op_prioCheck--;
		}
		
		//then we can finish the plus and minus operations right after
		for (indx=0;indx<opCount;indx++)
		{
			u8 varINDX=0,varINDX1=1;
			if (op_ARR[indx]=='+' || op_ARR[indx]=='-')
			{
				while(var_ARR[indx-varINDX]=='R')
				{
					varINDX++;
				}
				while (var_ARR[indx+varINDX1]=='R')
				{
					varINDX1++;
				}
				if (op_ARR[indx]=='+')
				var_ARR[indx]=var_ARR[indx-varINDX] + var_ARR[indx+varINDX1];
				else
				var_ARR[indx]=var_ARR[indx-varINDX] - var_ARR[indx+varINDX1];
				
				var_ARR[indx+1]='R';
				if(varINDX!=0)
				var_ARR[indx-varINDX]='R';
				op_ARR[indx]=0;
			}
		}
		
		//then we can have the result output from the var_ARR
		for (u8 k=0; k<varCount;k++)
		{
			if (var_ARR[k]!='R')
			{
				result=var_ARR[k];
				break;
			}
		}
		LCD_CLR();
		LCD_GoTo(2,0);
		LCD_WriteString("result= ");
		LCD_WriteNumber(result);
		LCD_GoTo(1,0);
		_delay_ms(100);
	}
}

void Calculator_Operation()
{
	u8 keypadCHR = KeyPad_getKey();
	
	if (keypadCHR!='T')
	{
		switch (keypadCHR)
		{
			case 'C':
			Calculator_Reset();
			break;
			
			case '+':
			if (last_Clicked!=NULL || last_Clicked!='-' ||last_Clicked!='*'||last_Clicked!='/')
			{
				op_ARR[opCount]='+';
				opCount++;
				varCount++;
				LCD_WriteChar(keypadCHR);
				_delay_ms(20);
			}
			break;
			
			case '-':
			if (last_Clicked!=NULL || last_Clicked!='+' ||last_Clicked!='*'||last_Clicked!='/')
			{
				op_ARR[opCount]='-';
				opCount++;
				varCount++;
				LCD_WriteChar(keypadCHR);
				_delay_ms(20);
			}
			break;
			
			case '*':
			if (last_Clicked!=NULL || last_Clicked!='+' ||last_Clicked!='-'||last_Clicked!='/')
			{
				op_ARR[opCount]='*';
				opCount++;
				op_prioCheck++;
				varCount++;
				LCD_WriteChar(keypadCHR);
				_delay_ms(20);
			}
			break;
			
			case '/':
			if (last_Clicked!=NULL || last_Clicked!='+' ||last_Clicked!='-'||last_Clicked!='/')
			{
				op_ARR[opCount]='/';
				opCount++;
				op_prioCheck++;
				varCount++;
				LCD_WriteChar(keypadCHR);
				_delay_ms(20);
			}
			break;
			
			case '=':
			Calculator_Calculate();
			break;
			
			default:
			if (var_ARR[varCount]==0)
				var_ARR[varCount] += (keypadCHR-'0');
			else
				var_ARR[varCount] = var_ARR[varCount] * 10 + (keypadCHR - '0');
			LCD_WriteChar(keypadCHR);
			_delay_ms(20);
			break;
		}
		last_Clicked=keypadCHR;
	}
}

void Calculator_Reset()
{
	  for (u8 i = 0; i < MAX_VAR; i++)
	  {
		  var_ARR[i] = 0;
	  }
	  for (u8 i = 0; i < MAX_VAR-1; i++)
	  {
		  op_ARR[i] = 0;
	  }
	result=0;
	LCD_CLR();
	last_Clicked=NULL;
	varCount=0;
	opCount=0;
	op_prioCheck=0;
}


//our calculator can make operations on u16 variables max an the result is gonna be on the a variable of s32

/********************************************************************************************************************
for the var[0] and the var[1] we got the operation op[0]. 
Example for illustration--> var={4,5,6,4,8} && op={+,-,+,*}

first things first
-------------------
we have to prioritize the multiply and divide her so we are gonna do this:
1)using the multiply op[3] to be done on var[3] & var [4] and stored in var[3] and the change op[3]=0
	so op={+,-,+,0} & var={4,5,6,32,'R'}
2) it's gonna be like this until it reaches something like this:
	op={0,0,0,0} & var={35,'R','R','R','R'}
3) result is gonna be equal the only number in this array
**********************************************************************************************************************/

//Our program will be starting as follows:

/************************************************************************/
/*Calculator app is ready:                                              */
/*5*5																	*/																		
/************************************************************************/

//then the afterwards it's gonna be like this once he press '=', where the first line is empty and the second line got the result

/************************************************************************/
/*																		*/
/*result = 25															*/
/************************************************************************/

//if the user wrote any operation like '+' while the result is showing up it's gonna be like this:

/************************************************************************/
/*25+																	*/
/*result = 25															*/
/************************************************************************/

