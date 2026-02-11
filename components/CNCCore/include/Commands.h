#pragma once

#include "Configuration.h"

namespace CNC_Responce {
const char ChipName[]         = "Esp_32_Laser";
const char Device_chip_Name[] = "Device_chip_name:%s";
const char MyName[]           = "M_Name:%s";
const char MyType[]           = "M_Type:%d";

const char MyPositionXYZ[] = "X:%.2f\r\nY:%.2f\r\nZ:%.2f";

const char Flags[]       = "";
const char Error[]       = "Error:%s";
const char MyWidth[]     = "M_Width:%d";
const char MyLength[]    = "M_Length:%d";
const char MyHeight[]    = "M_Height:%d";
const char MyBufferLen[] = "M_Buff_Len:%d";

const char CommandACK[] = "ok";
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

} // namespace Commands

namespace GCode {
const char G0[]  = "G0";  // Fast move
const char G1[]  = "G1";  // move
const char G4[]  = "G4";  // sleep
const char G10[] = "G10"; // retract
const char G11[] = "G11"; // unretract
const char G28[] = "G28"; // Home position
const char G90[] = "G90"; // set absolute coord
const char G91[] = "G91";
const char G92[] = "G92"; // Set position
const char G20[] = "G20";
const char G21[] = "G21";
const char G29[] = "G29";
} // namespace GCode

namespace MCode {

#if MACHINE_TYPE == THREE_D_PRINTER
const char GetTemps[] = "M105";
#endif
const char GetState[] = "M114";

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

const char M200[] = "M200"; // Установить диаметр филамента
const char M201[] = "M201"; // Установить ускорение осей
const char M202[] = "M202"; // Установить максимальное ускорение осей
const char M203[] = "M203"; // Установить максимальную скорость осей
const char M204[] = "M204"; // Установить ускорения печати/переездов
const char M205[] = "M205"; // Установить параметры движения (jerk и др.)
const char M206[] = "M206"; // Установить offset домашней точки (home offset)
const char M207[] = "M207"; // Ретракт (параметры втягивания)
const char M208[] = "M208"; // Установить параметры антиретрака
const char M209[] = "M209"; // Автоматический ретракт вкл/выкл (auto retract)

const char M218[] = "M218"; // Offset для второго экструдера/головок

const char M221[] = "M221"; // Изменить flow (процент подачи материала)

const char M301[] = "M301"; // PID-настройка нагревателя хотэнда
const char M303[] = "M303"; // Авто-PID тюнинг нагревателя (PID autotune)

const char M404[] = "M404"; // Диаметр филамента (параметр для расчётов)

const char M420[] = "M420"; // Управление ABL сеткой (включение/выкл/загрузка)

const char M500[] = "M500"; // Сохранить настройки в EEPROM
const char M501[] = "M501"; // Загрузить настройки из EEPROM

const char M600[] = "M600"; // Замена филамента (pause & change filament)
} // namespace MCode