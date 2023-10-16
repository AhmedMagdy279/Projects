
#ifndef FIRE_SYSTEM_INTERFACE_H_
#define FIRE_SYSTEM_INTERFACE_H_

typedef enum{
	FINE=0,	//state 0
	HEAT,	//state 1
	FIRE	//state 2
	}Fire_LastState;

void Fire_Init(void);

void Fire_Check(void);
void Fire_Check_LCD(u16 t, u16 s);

void Fire_PassView(void);

void Fire_Pass_Change(void);

#endif /* FIRE_SYSTEM_INTERFACE_H_ */