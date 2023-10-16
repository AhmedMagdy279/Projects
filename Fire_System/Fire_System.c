#include "../../LIB/StdTypes.h"
#include "../../CFG/Fire_System_CFG.h"
#include "Fire_System_Interface.h"
#include "../../HAL/Sensors/Sensors_Interface.h"
#include "../../HAL/MOTOR/MOTOR_Interface.h"
#include "../../HAL/LCD/LCD_Interface.h"
#include "../../HAL/KeyPad/KeyPad_Interface.h"
#include "../Temp_Filter/Temp_Filter_Interface.h"
#define  F_CPU   8000000
#include <util/delay.h>

//creating default password for the app, it's better to be written on the eeprom to keep the password when the power cuts off
static u8 Fire_System_pass[4] = {0,0,0,0};	
//creating last state static global variable	
u8 last_state= FINE;


// The documentation for the schematic for this project is gonna be here:
// PORT A:  PA0->PA2 & P7 are used for ADC (INFREE)			//PA0 for temp sensor, PA1 for smoke sensor(potentiometer)
//			PA3->PA6 are used for the LCD (OUTPUT)
// PORT B:  PB0->PB1 are used for the LCD (OUTPUT)
//			PB2->PB3 NULL (OUTPUT)
//			PB4->PB7 are used for the KEYPAD (OUTPUT)
// PORT C:  PC0->PC7 NULL (OUTPUT)
// PORT D:  PD0->PD1 & PD6->PD7 NULL (OUTPUT)
//			PD2->PD5 are used for the KEYPAD (INPULL)

void Fire_Init(void)
{
	//start with the password 
	LCD_CLR();
	LCD_GoTo(1,0);
	LCD_WriteString("Pass: ");
	Fire_PassView();
	LCD_WriteString(" okay?");
	LCD_GoTo(2,0);
	LCD_WriteString("yes(+)     no(-)");
	while(1)
	{
		u8 key=KeyPad_getKey();
		if (key=='-')
		{
			Fire_Pass_Change();
			break;
		}
		else if (key=='+')
			break;
	}
	LCD_CLR();
	LCD_GoTo(1,0);
	LCD_WriteString("Initiation done");
	LCD_GoTo(2,0);
	LCD_WriteString("starting....");
	_delay_ms(300);
	LCD_CLR();
}

void Fire_Check(void)
{
	u16 temp=FILTER_GetFilterdTemp();
	if (temp>=500)
	{
		u16 smoke=FILTER_GetFilterdSmoke();
		//if we are still in the HEAT state but no smoke enough
		if (last_state==HEAT && smoke<50)
		{
			//runnable filter check for both temp and smoke 
			Fire_Check_LCD(temp,smoke);
		}
		else if (last_state==FINE)		//change to HEAT if last_state was FINE
		{
			//runnable filter for smoke only and change the state to HEAT
			last_state=HEAT;
			Fire_Check_LCD(temp,smoke);
		}
		if (smoke>=50)
		{
			//change to FIRE
			last_state=FIRE;
			Fire_Check_LCD(temp,smoke);
		}
		
		
	}
	else if(temp<=450)
	{
		//change to FINE
		if(last_state==HEAT)
		{
			Fire_Check_LCD(temp,0);
			last_state=FINE;
		}
		else
			Fire_Check_LCD(temp,0);		

	}
	
		
}

void Fire_Check_LCD(u16 t, u16 s)
{
	switch (last_state)
	{
		case FINE:
		LCD_CLR();
		LCD_GoTo(1,0);
		LCD_WriteString("Status: FINE :)");
		LCD_GoTo(2,0);
		LCD_WriteString("TMP:    SMK: N/A");
		LCD_GoTo(2,4);
		LCD_WriteNumber(t);
		_delay_ms(100);
		break;
		
		case HEAT:
		LCD_CLR();
		LCD_GoTo(1,0);
		LCD_WriteString("Status: HEAT :|");
		LCD_GoTo(2,0);
		LCD_WriteString("TMP:    SMK:");
		LCD_GoTo(2,4);
		LCD_WriteNumber(t);
		LCD_GoTo(2,13);
		LCD_WriteNumber(s);
		_delay_ms(100);
		break;
		
		case FIRE:
		LCD_CLR();
		LCD_GoTo(1,0);
		LCD_WriteString("Status: FIRE :(");
		LCD_GoTo(2,0);
		LCD_WriteString("TMP:    SMK:");
		LCD_GoTo(2,4);
		LCD_WriteNumber(t);
		LCD_GoTo(2,13);
		LCD_WriteNumber(s);
		_delay_ms(100);
		break;
	}
}


void Fire_PassView(void)
{
	for (u8 i=0;i<4;i++)
	{
		LCD_WriteNumber(Fire_System_pass[i]);
	}
}

void Fire_Pass_Change(void)
{
	u8 pass_key, verified=0;
	//we are copying the old password data in case the user decided not to change it mid-process
	//we might use another solution that the user can work on another variable then when done we can
	//copy the new values to the global variable it can save one loop in the CODE SIZE
	u8 def_pass[4];
	for (u8 i=0;i<4;i++)
		def_pass[i]=Fire_System_pass[i];
		
	LCD_CLR();
	LCD_GoTo(2,0);
	LCD_WriteString("to cancel 'C'");
	LCD_GoTo(1,0);
	LCD_WriteString("[    ] new pass");
	LCD_GoTo(1,1);
	Fire_PassView();
	LCD_GoTo(1,1);
	while(verified<4)
	{
		pass_key=KeyPad_getKey();
		if (pass_key=='0' || pass_key=='1' || pass_key=='2' || pass_key=='3' || pass_key=='4' || pass_key=='5' || pass_key=='6' || pass_key=='7' || pass_key=='8' || pass_key=='9')
		{
			Fire_System_pass[verified]=pass_key-'0';
			LCD_GoTo(1,1);	
			Fire_PassView();
			LCD_GoTo(1,(1+verified));
			verified++;
		}
		else if (pass_key=='=' || pass_key=='/' || pass_key=='*' || pass_key=='-' || pass_key=='+')
		{
			verified=0;
			LCD_GoTo(1,1);
			Fire_System_pass[0]=0;
			Fire_System_pass[1]=0;
			Fire_System_pass[2]=0;
			Fire_System_pass[3]=0;
			Fire_PassView();
			LCD_GoTo(1,1);
		}
		else if (pass_key=='C')
		{
			LCD_CLR();
			LCD_GoTo(1,0);
			LCD_WriteString("Change cancelled");
			LCD_GoTo(2,0);
			LCD_WriteString("pass: [    ]");
			LCD_GoTo(2,7);
			for (u8 i=0;i<4;i++)
				Fire_System_pass[i]=def_pass[i];
			Fire_PassView();
			_delay_ms(300);
			break;
		}
		
		if (verified>=4)
		{
			LCD_CLR();
			LCD_GoTo(1,0);
			LCD_WriteString("Pass changed");
			LCD_GoTo(2,0);
			LCD_WriteString("pass: [    ]");
			LCD_GoTo(2,7);
			Fire_PassView();
			_delay_ms(300);			
			break;
		}
	}
}


