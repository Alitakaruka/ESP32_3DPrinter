
#include "include/Commands.h"

#define TARGET_MACHINE_NAME "Huesosik"
#define MACHINE_TYPE LASER

#define BufferSize 1024

#define Width 50  // X
#define Length 50 // Y
#define Heigh 0   // Z

#define Switch_Timeout 1

#define StepEnablePort 11

#define FDM_PRINTER 1
#define LASER 2
#define SLA_PRINTER 3
#define SLS_PRINTER 4
#define MILLING 5

#pragma region Ports
#define STEPPERS_ENABLE_PORT 21

#define X_DIR_PORT 13
#define X_STEP_PORT 15

#define Y_DIR_PORT 14
#define Y_STEP_PORT 27

#define Z_DIR_PORT 17
#define Z_STEP_PORT 25

#define E_DIR_PORT 16
#define E_STEP_PORT 26

#define X_ENDSTOP_PORT 34
#define Y_ENDSTOP_PORT 35
#define Z_ENDSTOP_PORT 36
#define E_ENDSTOP_PORT 39

#define MEM_MISO 23
#define MEM_MOSI 19
#define MEM_CLK 18
#define MEM_CS 12

#define SD_MISO 23
#define SD_MOSI 19
#define SD_CLK 18
#define SD_CS 5

#pragma endregion

#if defined(MACHINE_TYPE) && (MACHINE_TYPE + 0 == 0)
#error MACHINE_TYPE not initialized
#endif