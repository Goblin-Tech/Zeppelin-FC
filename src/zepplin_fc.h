/*
 * zepplin_fc.h
 *
 * Created: 4/8/2013 6:12:12 PM
 *  Author: ViDAR
 */ 




void Zepplin_Init(void);
bool Zepplin_Button(void);
void Zepplin_ButtonHandler(void);
void Zepplin_Loop(void);
void ZP_WriteToFile(uint16_t *buffer);
void ZP_CreateFile(void);
void ZP_Close(void);
