#include "../../LIB/StdTypes.h"
#include "../../HAL/Sensors/Sensors_Interface.h"

u16 FILTER_TempArr[10], FILTER_SmokeArr[10];
u16 FILTER_FilteredTemp, FILTER_FilteredSmoke;
u16 FILTER_UnFilteredTemp, FILTER_UnFilteredSmoke;
u16 temptemp, smokesmoke;

void FILTER_Init(void)
{
	SENSORS_GetTEMP();
	SENSORS_GetSmoke();
	u16 tempInit=SENSORS_GetTEMP();
	u16 smokeInit=SENSORS_GetSmoke();
	FILTER_UnFilteredTemp=tempInit;
	FILTER_UnFilteredSmoke=smokeInit;
	for (u8 j=0;j<10;j++)
	{
		FILTER_TempArr[j]=tempInit;
		FILTER_SmokeArr[j]=smokeInit;
	}
	temptemp=tempInit;
	smokesmoke=smokeInit;
}

/* not more than 250 us*/
void FILTER_Runnable_Temp(void)
{
	static u8 i=0;
	u16 tempsum=0;
	SENSORS_TEMP_Read();
	FILTER_TempArr[i]=SENSORS_GetTEMP();
	FILTER_UnFilteredTemp=FILTER_TempArr[i];
	i++;
	if (i==10)
	{
		i=0;
	}
	for (u8 j=0;j<10;j++)
	{
		tempsum+=FILTER_TempArr[j];
	}
	FILTER_FilteredTemp=tempsum/10;
}
void FILTER_Runnable_Smoke(void)
{
	static u8 k=0;
	u16 smokesum=0;
	SENSORS_Smoke_Read();
	FILTER_SmokeArr[k]=SENSORS_GetSmoke();
	FILTER_UnFilteredSmoke=FILTER_SmokeArr[k];
	k++;
	if (k==10)
	{
		k=0;
	}
	for (u8 j=0;j<10;j++)
	{
		smokesum+=FILTER_SmokeArr[j];
	}
	FILTER_FilteredSmoke=smokesum/10;
}

u16 FILTER_GetFilterdTemp(void)
{
	return FILTER_FilteredTemp;
}

u16 FILTER_GetFilterdSmoke(void)
{
	return FILTER_FilteredSmoke;
}

u16 FILTER_GetUnFilterdTemp(void)
{
	return FILTER_UnFilteredTemp;
}

u16 FILTER_GetUnFilterdSmoke(void)
{
	return FILTER_UnFilteredSmoke;
}

void SET_DATA(u8 x)
{
	FILTER_FilteredTemp=x;
}
