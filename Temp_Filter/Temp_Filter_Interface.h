
#ifndef TEMP_FILTER_INTERFACE_H_
#define TEMP_FILTER_INTERFACE_H_

//we are gonna use this function for both the smoke and temperature FILTERING

void FILTER_Init(void);

void FILTER_Runnable_Temp(void);
void FILTER_Runnable_Smoke(void);

u16 FILTER_GetFilterdTemp(void);
u16 FILTER_GetFilterdSmoke(void);

u16 FILTER_GetUnFilterdTemp(void);
u16 FILTER_GetUnFilterdSmoke(void);

void SET_DATA(u8 x);

#endif /* TEMP_FILTER_INTERFACE_H_ */