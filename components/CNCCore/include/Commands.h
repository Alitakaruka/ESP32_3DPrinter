namespace CNC_Data
{
#define Identification "Identification"
#define MyPositionX         "X\r"
#define MyPositionY         "Y\r"
#define MyPositionZ         "Z\r"
#define MyBufferCommandSize "S\r"
#define MyMaxBufferSize     "^\r"
#define MyWidth             "W\r"
#define MyLength            "L\r"
#define MyHeight            "H\r"
#define MyName              "n\r"
#define MyType              "T\r"
#define MyTemperatureN      "N\r"
#define MyTemperatureB      "B\r"
#define ImPrinting          "P\r"
}

namespace Commands
{
#define EndOfData          "\r\n"
#define Error              "E\r"
#define StopPrint          "!\r"
#define StartPrint         "!\r"
#define GetTemps           "M105"
#define GetState           "M114"
#define GetBaseInformation "&\r"
#define Check              "*\r"
#define NowTemperatureBed  "B\r"
#define TemperatureNozzle  "N\r"
#define IsPrinting         "P\r"
#define ReadyToRead        "R\r"
#define BufferCommandSize  "S\r"
#define ClearBuffer1       "C\r"
#define SetLightStatus     "L\r"

#define SOF                 0x02
#define EOF                 0x04

#define SOT                 0x01
#define EOT                 0x03

#define CommandACK "ok"
#define SYNC      "+\r"
} // namespace Commands
