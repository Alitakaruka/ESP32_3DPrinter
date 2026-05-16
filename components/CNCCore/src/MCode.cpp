#include "CNC.hpp"


void CNC::ReadMCode(const char* Command) {

    int commandNumber = 0;
    if(sscanf(Command, "M%d", &commandNumber) == 0) {
        // TODO LOG
        return;
    }

    switch(commandNumber) {
    case MCode::M67:{
        int   device = 0;
        float power  = 0.0;
        sscanf(Command, MCode::M67_f, &device, &power);
        break;
    }
    #if MACHINE_TYPE == THREE_D_PRINTER
    case MCode::HeatBed:{
        heat_bed_command(command, 0);
        break;
    }
    case HeatBedAndwait:{
        heat_bed_command(command, 1);
        break;
    }
    case GetTemps:{
        UART_send_command(EndOfData, BedTemp, iPrinter.tempBed, iPrinter.BedPID.needValue);
        UART_send_command(EndOfData, ExtruderTemp, iPrinter.tempNozzle, iPrinter.NozzlePID.needValue);
        break;
    }
    case HeatNozzle:{
        heat_nozzle_command(command, 0);
        break;
    }
    case HeatNozzleAndWait:{
        heat_nozzle_command(command, 1);
        break;
    }
    case StopHeating:{
        set_temp_bed(0);
        set_temp_nozzle(0);
        break;
    }
    case STOP:{
        stop_axes_timer();
        break;
    }
    #endif
    case MCode::GetPosition:{
        this->SendCommand("");
        break;
    }
    case MCode::M82:{
        // Command_M82();
        break;
    }
    case MCode::M83:{
        // Command_M83();
        break;
    }
    case MCode::EnableStepscommand:{
        // enable_steps();
        break;
    }
    case MCode::DisableStepscommand:{
        // disable_steps();
        break;
    }
    case MCode::TurnOnFan:{
        // set_fan_value(command);
        break;
    }
    case MCode::TurnOfFan:{
        // diasble_fan(command);
        break;
    }
    case MCode::M220:{
        int Mult;
        // if(sscanf(command, "M220 S%d", &(iPrinter.feedrate))) { iPrinter.feedrate = Mult; };
        break;
    }
    case MCode::M221:{
        int flow;
        // if(sscanf(command, "M221 S%d", &(iPrinter.flowrate))) iPrinter.flowrate = flow;
        break;
    }
    case MCode::M92:{

    }
    case MCode::M486:{
        break;
    }
    case MCode::M73:{
        break;
    }
    case MCode::M201:{
        break;
    }
    case MCode::M204:{
        break;
    }
    case MCode::M205:{
        break;
    }
    case MCode::M900: {// todo
        break;
    }
    default:{
        // log_warning("command not defined:%s", command);
        // log_warning("Current code:%d",MCode);
    }
    }

}
