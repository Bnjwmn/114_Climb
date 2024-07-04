#include "Climb.h"
#include <iostream>
#include "ClimbConstants.h"

Climb::Climb(std::string name, bool enabled, bool shuffleboard)
: Mechanism {name,enabled,shuffleboard}
{
    m_motor.SetNeutralMode(ctre::phoenix6::signals::NeutralModeValue::Brake);
    RemoveBrake();
}

void Climb::Zero(){
    m_position=0;
    m_location=AT_BOTTOM;
}

ClimbLocation Climb::GetLocation(){
    return m_location;
}

ClimbState Climb::GetState(){
    return m_state;
}

ClimbTarget Climb::GetTarget(){
    return m_target;
}

void Climb::ToggleState(){
    if(m_state!=MANUAL){
        m_state=MANUAL;
        return;
    }
    else {
        m_state=AUTO;
        return;
    }
}

void Climb::ClimbTeleopPeriodic(ControllerInput ButtonInput, double StickInput){
    switch (ButtonInput)
    {
        case A:
            ToggleState();
            break;
        case B:
            Zero(); 
            break;
        case X:
            m_target=DOWN;
            m_targ_position=0;
            break;
        case Y:
            m_target=UP;
            m_targ_position=ClimbConstants::Total_Length;
            break;
        case LB:
            RemoveBrake();
            break;
        case RB:
            Brake();
            break;
        default:
            break;
    }
    
    switch (m_target)
    {
        case UP:
            MoveUp();
            break;
        case DOWN:
            MoveDown();
            break;
        default:
            break;
    }

    if(m_state==MANUAL){
        Manual(StickInput);
    }
}

void Climb::Manual(double ManualInput){ 
    if(m_state!=MANUAL){
        return;
    }
    if(-0.05<ManualInput<0.05){
        ManualInput=0;
    }
    double Manual_Voltage = ClimbConstants::Max_Voltage*ManualInput;
    Manual_Voltage = std::clamp(Manual_Voltage, -ClimbConstants::Max_Voltage, ClimbConstants::Max_Voltage);
    m_motor.SetVoltage(units::volt_t(Manual_Voltage));
}

void Climb::MoveUp(){
     if (m_location==AT_TOP){
        return;
     }
     if(m_braking){
        RemoveBrake();
        m_timer=0;
        m_braking=false;
     }
     if(m_timer<0.24){
        m_timer+=0.2;
        return;
     }
     if (m_voltage<ClimbConstants::Max_Voltage){
            m_location=BETWEEN;
            m_voltage+=ClimbConstants::Accel_Voltage_Max;
            m_motor.SetVoltage(units::volt_t(m_voltage));
            m_position=((m_motor.GetPosition().GetValueAsDouble())*(ClimbConstants::Spindle_Radius));
        }
        else if (ClimbConstants::Total_Length>(m_position+(0.5*ClimbConstants::Max_Voltage*(ClimbConstants::Max_Voltage/ClimbConstants::Accel_Voltage_Max)))){
            m_motor.SetVoltage(units::volt_t(m_voltage));
            m_position=((m_motor.GetPosition().GetValueAsDouble())*(ClimbConstants::Spindle_Radius));
        }
        else if(m_voltage>0){
            m_voltage-=ClimbConstants::Accel_Voltage_Max;
            m_motor.SetVoltage(units::volt_t(m_voltage));
            m_position=((m_motor.GetPosition().GetValueAsDouble())*(ClimbConstants::Spindle_Radius));
            if(m_position==ClimbConstants::Total_Length){
                m_location=AT_TOP;
                Brake();
                m_braking=true;
            }
        }
        
}

void Climb::MoveDown(){
    if (m_location==AT_BOTTOM){
        return;
     }
    if(m_braking){
        RemoveBrake();
        m_timer=0;
        m_braking=false;
     }
     if(m_timer<0.24){
        m_timer+=0.2;
        return;
     }
    if (m_voltage<ClimbConstants::Max_Voltage){
            m_location=BETWEEN;
            m_voltage+=ClimbConstants::Accel_Voltage_Max;
            m_motor.SetVoltage(units::volt_t(-m_voltage));
            m_position=((m_motor.GetPosition().GetValueAsDouble())*(ClimbConstants::Spindle_Radius));
        }
        else if (m_position>(0.5*ClimbConstants::Max_Voltage*(ClimbConstants::Max_Voltage/ClimbConstants::Accel_Voltage_Max))){
            m_motor.SetVoltage(units::volt_t(-m_voltage));
            m_position=((m_motor.GetPosition().GetValueAsDouble())*(ClimbConstants::Spindle_Radius));
        }
        else if(m_voltage>0){
            m_voltage-=ClimbConstants::Accel_Voltage_Max;
            m_motor.SetVoltage(units::volt_t(-m_voltage));
            m_position=((m_motor.GetPosition().GetValueAsDouble())*(ClimbConstants::Spindle_Radius));
            if(m_position==ClimbConstants::Total_Length){
                m_location=AT_BOTTOM;
                Brake();
                m_braking=true;
            }
        }
}

void Climb::Brake(){
    m_brake.SetVoltage(units::volt_t(-12));
}

void Climb::RemoveBrake(){
    m_brake.SetVoltage(units::volt_t(12));
}

    