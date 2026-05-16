#include "CNC.hpp"

#define Command_G0(cnc,command) Command_G1(cnc,command)

void Command_G1(CNC* cnc,const char* command);

void CNC::ReadGCode(const char* Command) {

    using namespace GCode;

    int commandNumber = 0;
    if(sscanf(Command, "G%d", &commandNumber) == 0) {
        // TODO LOG
        return;
    }

    switch(commandNumber) {
    case G0:
        Command_G0(this,Command);
        break;
    case G1:
        Command_G1(this,Command);
        break;
    case G4:
        break;
    case G10:
        break;
    case G11:
        break;
    case G28:
        break;
    case G90:
        break;
    case G91:
        break;
    case G92:
        break;
    case 999:
        break;
    default:
        break;
    }
}



void Command_G1(CNC* cnc,const char* command){
    float X = NAN, Y = NAN, Z = NAN, E = NAN, F = NAN;

    while(*command != '\0') {
        char* end = NULL;
        if(*command == 'X' || *command == 'x') {
            X = strtof(command + 1, &end);
        } else if(*command == 'Y' || *command == 'y') {
            Y = strtof(command + 1, &end);
        } else if(*command == 'Z' || *command == 'z') {
            Z = strtof(command + 1, &end);
        } else if(*command == 'F' || *command == 'f') {
            F = strtof(command + 1, &end) / 60.0f;
        } else if(*command == 'E' || *command == 'e') {
            E = strtof(command + 1, &end);
        }

        if(end == command + 1 || end == command) {
            command++;
        } else if(end != NULL) {
            command = end;
        } else {
            command++;
        }
    }


    if (isnan(F)) {
        F = cnc->GetSpeed();
    } else {
        cnc->SetSpeed(F);
    }


    // if (cnc->CheckFlag(FLAG_ExtruderIsAbsalute)) {
    //     X = (!isnan(X) ? X - cnc->CurrentPosition.X : 0);
    //     Y = (!isnan(Y) ? Y - iPrinter.CurrentPosition.Y : 0);
    //     Z = (!isnan(Z) ? Z - iPrinter.CurrentPosition.Z : 0);
    // } else {
    //     X = (isnan(X) ? 0 : X);
    //     Y = (isnan(Y) ? 0 : Y);
    //     Z = (isnan(Z) ? 0 : Z);
    // }

    // if (iPrinter.Flags & (1 << FlagExtruderIsAbsalute)) {
    //     E = (!isnan(E) ? E - iPrinter.CurrentPosition.E : 0);
    // } else {
    //     E = (isnan(E) ? 0 : E);
    // }

    // ESP_LOGE("","X:%f,Y:%f,Z:%f,E:%f,F:%f",X,Y,Z,E,F);
    // cnc->WriteLog(CNC::LogLevel::Information,"X:%f,Y:%f,Z:%f,E:%f,F:%f",X,Y,Z,E,F);
    if(X != 0 || Y != 0 || Z != 0 || E != 0) { cnc->sheduler.Push(X, Y, Z, E, F); }
}
