#pragma once

#include "Util/Mechanism.h"
#include <ctre/phoenix6/TalonFX.hpp>
#include "ClimbConstants.h"
#include <ctre/phoenix/motorcontrol/can/WPI_TalonSRX.h>

using ctre::phoenix6::hardware::TalonFX;
using ctre::phoenix::motorcontrol::can::WPI_TalonSRX;

enum ClimbLocation{
    AT_BOTTOM,
    AT_TOP,
    BETWEEN,
};

enum ClimbState{
    MANUAL,
    AUTO,
};

enum ClimbTarget{
    DOWN,
    UP,
    NONE,
};

enum ControllerInput{
  A,
  B,
  X,
  Y,
  RB,
  LB,
};

class Climb : public Mechanism {
    public:
        ClimbLocation GetLocation();
        ClimbState GetState();
        ClimbTarget GetTarget();
        Climb(std::string name, bool enabled = true, bool shuffleboard = false);
        void Manual(double ManualInput);
        void Zero();
        void ToggleState();
        void MoveUp();
        void MoveDown();
        void Brake();
        void RemoveBrake();
        void ClimbTeleopPeriodic(ControllerInput Input, double StickInput);
    private:
        TalonFX m_motor{ClimbIDs::CLIMB_MOTOR}; 
        double m_voltage=0; 
        double m_position;
        double m_targ_position;
        bool m_braking;
        double m_timer;
        ClimbState m_state=MANUAL; 
        ClimbLocation m_location=BETWEEN;
        ClimbTarget m_target=NONE;
        WPI_TalonSRX m_brake{ClimbIDs::BRAKE};
};
