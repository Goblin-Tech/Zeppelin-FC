/*
 * ZP_fc.c
 *
 * Created: 4/8/2013 6:07:28 PM
 *  Author: ViDAR
 */ 


#include <asf.h>
#include <string.h>
#include "zeppelin_fc.h"

#define DEBUG

uint8_t Script[512];

/* Global ul_ms_ticks in milliseconds since start of application */
volatile uint32_t ul_ms_ticks = 0;
int32_t bTemp = -1;

char test_file_name[] = "0:ZP-FC.bin";
TCHAR file_name[17] = "0:ZP-FC.bin";
Ctrl_status status;
FRESULT res;
DIR dir;
FATFS fs;
FILINFO fno, f_log, f_script;
FIL file_object, log;
UINT bw;

static uint8_t data_buffer[2048];

void die (FRESULT rc)
{
	printf("Failed with rc=%u.\n", rc);
	for (;;) ;
}


uint16_t tick = 0;


static FRESULT scan_files(char *path)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	int32_t i;
	char *pc_fn;
	#if _USE_LFN
	char c_lfn[_MAX_LFN + 1];
	fno.lfname = c_lfn;
	fno.lfsize = sizeof(c_lfn);
	#endif

	/* Open the directory */
	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) {
				break;
			}

			#if _USE_LFN
			pc_fn = *fno.lfname ? fno.lfname : fno.fname;
			#else
			pc_fn = fno.fname;
			#endif
			if (*pc_fn == '.') {
				continue;
			}

			/* Check if it is a directory type */
			if (fno.fattrib & AM_DIR) {
				sprintf(&path[i], "/%s", pc_fn);
				res = scan_files(path);
				if (res != FR_OK) {
					break;
				}

				path[i] = 0;
			} else {
				printf("%s/%s\n\r", path, pc_fn);
			}
		}
	}

	return res;
}

void SysTick_Handler(void)
{
	ul_ms_ticks++;
}

void ZP_Scan_Scripts(void)
{
	f_opendir(&dir, "0:Scripts");
}

void ZP_Init(void)
{
		#ifdef DEBUG
		printf("Setting up system clock...\n");
		#endif
	/* Setup SysTick Timer for 1 msec interrupts */
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000))
while (1) {}
	
	#ifdef DEBUG
		printf("ZP Init:\n");
		printf("Setting up pins...\n");
	#endif
	
	
			
	#ifdef DEBUG
		printf("Setting up SD stack...\n");
	#endif
	
	/* Initialize SD MMC stack */
	sd_mmc_init();	

	uint8_t sd_check = 0;
	/* Wait card present and ready */
	do {
		#ifdef DEBUG
		if (sd_check == 1)
		{
			printf("Please plug an SD card in slot.\n");
			sd_check = 2;
		}		
		#endif
		status = sd_mmc_test_unit_ready(0);
		if (CTRL_FAIL == status) {
			printf("Card install FAIL\n\r");
			printf("Please unplug and re-plug the card.\n\r");
			while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
			}
		}
		if (sd_check == 0)
			sd_check = 1;
	} while (CTRL_GOOD != status);
	
	#ifdef DEBUG
		printf("SD Card Found\n");
		printf("Mounting disk...");
	#endif
	
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
	if (FR_INVALID_DRIVE == res) {
		printf("[FAIL] res %d\r\n", res);
	}
	
	#ifdef DEBUG
		printf("Disk mounted\n");
	#endif
	
	TCHAR root_directory[3] = "0:";
	root_directory[0] = '0' + LUN_ID_SD_MMC_0_MEM;
	
	
	file_name[0] = '0' + LUN_ID_SD_MMC_0_MEM;
	DIR dirs;
	res = f_opendir(&dirs, root_directory);
	
	if(f_opendir(&dir, "0:Scripts") == FR_NO_PATH)
	{
		res = f_mkdir("0:Scripts");
		printf("Made Scripts folder\n");
	}
	
	printf("res: %d\n", res);
	
	
	printf("Open Script\n");
	f_open(&f_script, "0:script.bin", FA_OPEN_EXISTING | FA_READ);
	
}

void ZP_Loop(void)
{
	ZP_ButtonHandler();
}

void ZP_CreateLogFile(void)
{
	f_open(&f_log, "0:Logfile.bin", FA_CREATE_ALWAYS | FA_WRITE);
	f_puts("Goblin-Tech", &f_log);
	f_puts("GT101-0001", &f_log);
	f_puts("00.00.00", &f_log);
	uint16_t size = f_size(&f_script);
	uint8_t dummy;
	f_write(&f_log, &size, 2, &dummy);
	f_write(&f_log, &Script, size, &dummy);
	f_close(&f_log);
}


static uint32_t b_ms = 0;
static bool b_bool;

void ZP_ButtonHandler(void)
{
	/* Get time from button Release */
	if (ZP_Button())
	{
		if (b_ms > 50 && b_ms < 2000)
			printf("Change Script\n");
		
		if (b_ms > 2000 && b_ms < 5000)
		{
			printf("Arm Rocket\n");
			ZP_CreateLogFile();
		}
			
	}
	else
	/* Get time from Button Down */
	{
		if (b_ms > 2000 && b_ms < 5000)
		{
			printf("Arm Beep\n");
		}
		
		if (b_ms > 5000)
			printf("ShutDown\n");
	}
}

static bool b_read = true;

bool ZP_Button(void)
{
	if (!ioport_get_pin_level(PIO_PA0_IDX))
	{
		if (b_read)
		{
			bTemp = (int32_t) ul_ms_ticks;
			b_read = false;
			b_ms = 0;
		}		
		else
		{
			b_ms = (int32_t) ul_ms_ticks - bTemp;
		}
	}
	else
	{
		if (b_read)
		{
			b_ms = 0;
			return false;
		}
		else
		{
			b_read = true;
			return true;
		}
	}
	return false;
}
