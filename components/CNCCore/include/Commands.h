#pragma once

#include "Configuration.h"

namespace CNC_Responce {
const char ChipName[]         = "Esp_32_Laser";
const char Device_chip_Name[] = "Device_chip_name:%s";
const char MyName[]           = "M_Name:%s";
const char MyType[]           = "M_Type:%d";

const char MyPositionXYZ[] = "X:%.2f\r\nY:%.2f\r\nZ:%.2f";
const char MyPositionX[] = "X:%.2f";
const char MyPositionY[] = "Y:%.2f";
const char MyPositionZ[] = "Z:%.2f";

const char Flags[]       = "";
const char MyWidth[]     = "M_Width:%d";
const char MyLength[]    = "M_Length:%d";
const char MyHeight[]    = "M_Height:%d";
const char MyBufferLen[] = "M_Buff_Len:%d";

const char CommandACK[] = "ok";
const char Switch_Timeout_C[] = "Switch_Timeout:%d";
} // namespace CNC_Responce

namespace Commands {
const char Identification[] = "Identification";
const char EndOfData[]      = "\r\n";

const char Check[] = "*\6";

const char BufferCommandSize[] = "S\6";
const char ClearBuffer[]       = "C\6";
const char SetLightStatus[]    = "L\6";

const char StartOfTransmision[] = "F\1";
const char EndOfTransmision[]   = "F\4";
const char FILE_NAME[]          = "FILENAME:";
const char FILE_SIZE[]          = "SIZE:";
const char GET_FILE_FATA[]      = "GET_FILE_FATA:%d";

const char SYNC[] = "SYNC";

const char StartTask[] = "StartTask";
const char EndTask[] = "EndTask";
const char SetCompletion[] = "Completion:%d";

} // namespace Commands

namespace GCode {
const uint16_t G0  = 0;  // move
const uint16_t G1  = 1;  // move

const uint16_t G4  = 4;  // sleep
const uint16_t G10  = 10;  // retract
const uint16_t G11  = 11;  // unretract
const uint16_t G28  = 28;  // Home position
const uint16_t G90  = 90;  // Set ot coord
const uint16_t G91  = 91;  // set absolute coord
const uint16_t G92  = 92;  // Set position
const uint16_t G20  = 20;  // sleep
const uint16_t G21  = 21;  // sleep
const uint16_t G29  = 29;  // sleep
} // namespace GCode

namespace MCode {

const uint16_t M82 = 82;
const uint16_t M83 = 83;

const uint16_t M92 = 92;

const uint16_t M140 = 140;
const uint16_t M190 = 190;
const uint16_t M221 = 221; // flow
const uint16_t M220 = 220;
const uint16_t M73 =  73;
///////////////////////////////////////////////////// Unsuported

const uint16_t M900 = 900; // linear advance
const uint16_t M486 = 48;
const uint16_t M201 = 201; // todo
const uint16_t M203 = 203; // todo
const uint16_t M204 = 204;
const uint16_t M205 = 205;
const uint16_t GetPosition = 114;
const uint16_t GetVersion = 115;
const uint16_t EnableStepscommand = 17;
const uint16_t DisableStepscommand = 18;

const uint16_t TurnOnFan = 106;
const uint16_t TurnOfFan = 107;

#if MACHINE_TYPE == THREE_D_PRINTER
const char GetTemps[] = "M105";
#endif
const char     M67_f[]  = "M67 E%d Q%f";
const uint16_t M67      = 67;

const char FileList[]     = "M20"; // Показать список файлов на SD-карте
const char SDInit[]       = "M21"; // Инициализировать SD-карту
const char SDDisable[]    = "M22"; // Отмонтировать (выключить) SD-карту
const char SelectFile[]   = "M23";
const char StartTask[]    = "M24";
const char EndRecording[] = "M29"; // Завершить запись файла в SD (end recording)
const char DeleteFile[]   = "M30"; // Удалить файл на SD
const char M32[]          = "M32"; // Запустить файл на SD

const char Pause[] = "M25"; // Пауза печати (SD-print pause)

const char M80[] = "M80"; // Включить питание (PSU) — ATX power on
const char M81[] = "M81"; // Выключить питание (PSU)

// const char M92[] = "M92"; // Установить steps/mm для осей

const char M110[] = "M110"; // Сбросить номер слоя/линии (set line number)

const char M119[] = "M119"; // Прочитать состояние концевиков (endstops)

// const char M200[] = "M200"; // Установить диаметр филамента
// const char M201[] = "M201"; // Установить ускорение осей
// const char M202[] = "M202"; // Установить максимальное ускорение осей
// const char M203[] = "M203"; // Установить максимальную скорость осей
// const char M204[] = "M204"; // Установить ускорения печати/переездов
// const char M205[] = "M205"; // Установить параметры движения (jerk и др.)
// const char M206[] = "M206"; // Установить offset домашней точки (home offset)
// const char M207[] = "M207"; // Ретракт (параметры втягивания)
// const char M208[] = "M208"; // Установить параметры антиретрака
// const char M209[] = "M209"; // Автоматический ретракт вкл/выкл (auto retract)

// const char M218[] = "M218"; // Offset для второго экструдера/головок

// const char M221[] = "M221"; // Изменить flow (процент подачи материала)

// const char M301[] = "M301"; // PID-настройка нагревателя хотэнда
// const char M303[] = "M303"; // Авто-PID тюнинг нагревателя (PID autotune)

// const char M404[] = "M404"; // Диаметр филамента (параметр для расчётов)

// const char M420[] = "M420"; // Управление ABL сеткой (включение/выкл/загрузка)

// const char M500[] = "M500"; // Сохранить настройки в EEPROM
// const char M501[] = "M501"; // Загрузить настройки из EEPROM

// const char M600[] = "M600"; // Замена филамента (pause & change filament)
} // namespace MCode
#pragma once
