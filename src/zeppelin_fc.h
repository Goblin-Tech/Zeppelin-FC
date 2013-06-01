/*
 * zeppelin_fc.h
 *
 * Created: 4/8/2013 6:12:12 PM
 *  Author: ViDAR
 */ 




void ZP_Init(void);
bool ZP_Button(void);
void ZP_ButtonHandler(void);
void ZP_Loop(void);
void ZP_WriteToFile(uint16_t *buffer);
void ZP_CreateFile(void);
void ZP_Close(void);
