


// This function assigns the correct motor object to the drive, turret and recoil motors based on the user's selections which we retrieved from EEPROM
void InstantiateMotorObjects() 
{ 
    // Temp vars
    boolean RCOutput1_Assigned = false;
    boolean RCOutput2_Assigned = false;
    boolean RCOutput3_Assigned = false;
    boolean RCOutput4_Assigned = false;

    // Start these as true, and set to false below if necessary
    RCOutput1_Available = true;    
    RCOutput2_Available = true;    
    RCOutput3_Available = true;    
    RCOutput4_Available = true;    
    MotorA_Available = true;
    MotorB_Available = true;

   
    // DRIVE MOTORS
    // -------------------------------------------------------------------------------------------------------------------------------------->>
    // Sanity check - make sure DriveType is valid
    if (eeprom.ramcopy.DriveType < DT_TANK || eeprom.ramcopy.DriveType > LAST_DT)
    {   // Default to tank if we have some invalid value, and update EEPROM too
        eeprom.ramcopy.DriveType = DT_TANK;
        EEPROM.writeInt(offsetof(_eeprom_data, DriveType), DT_TANK);
    }
   
    if (eeprom.ramcopy.DriveType == DT_CAR)
    {   
        switch (eeprom.ramcopy.DriveMotors)
        {
            case SABERTOOTH:
                // For a single rear drive motor, connect it to SIDE A
                DriveMotor = new Sabertooth_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_DRIVE_Address,&MotorSerial);
                break;
    
            case POLOLU:
                // For a single rear drive motor, connect it to SIDE A
                DriveMotor = new Pololu_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Pololu_DRIVE_ID,&MotorSerial);
                break;
                
            case OP_SCOUT:
                // For a single rear drive motor, connect it to SIDE A
                DriveMotor = new OPScout_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);    
                break;
    
            case ONBOARD:
                // For a single rear drive motor, connect it to SIDE A
                DriveMotor = new Onboard_ESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                MotorA_Available = false;
                break;
                
            case SERVO_ESC:
                // For a single rear drive motor, connect it to the "Left" servo port
                DriveMotor = new Servo_ESC (SERVONUM_LEFTTREAD,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                RCOutput1_Available = false;
                break;
    
            default:
                // We shouldn't end up here but in case we do, we need to define something or else the program will croak at runtime.
                // We set it to SABERTOOTH, and save it to EEPROM so we don't end up here again next time
                eeprom.ramcopy.DriveMotors = SABERTOOTH;
                EEPROM.writeInt(offsetof(_eeprom_data, DriveMotors), SABERTOOTH);
                DriveMotor = new Sabertooth_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_DRIVE_Address,&MotorSerial);
        }
        // Now initialize the motor
        DriveMotor->begin();
    }
    else
    {   // The user wants independent tread speeds. 
        switch (eeprom.ramcopy.DriveMotors)
        {
            case SABERTOOTH:
                // The Sabertooth ESC additionally needs an address (values between 128-135) and the assignment of the serial port to use.
                // The address is set physically on the Sabertooth device with dip-switches, see the Sabertooth manual. 128 is probably the default.
                // We define different addresses for drive motors and turret, in the event the user wants to use two Sabertooth dual controllers,
                // one for drive and one for turret. 
                // Serial port gets set in OP_Settings.h. On the TCB board it is Serial2. 
                LeftTread = new Sabertooth_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_DRIVE_Address,&MotorSerial);
                RightTread = new Sabertooth_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_DRIVE_Address,&MotorSerial);
                break;
    
            case POLOLU:
                LeftTread = new Pololu_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Pololu_DRIVE_ID,&MotorSerial);
                RightTread = new Pololu_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Pololu_DRIVE_ID,&MotorSerial);
                break;
                
            case OP_SCOUT:
                LeftTread = new OPScout_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);    
                RightTread = new OPScout_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                break;
    
            case ONBOARD:
                LeftTread = new Onboard_ESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                RightTread = new Onboard_ESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                MotorA_Available = false;
                MotorB_Available = false;
                break;
                
            case SERVO_ESC:
                if (eeprom.ramcopy.DriveType == DT_HALFTRACK)
                {   // This case should not happen. If the user wants independent treads *and* a steering servo, 
                    // they need to use a dual motor serial controller, or the onboard motor drivers, but not the two ESC ports 
                    // because we are going to assign the steering servo to the Right Tread servo output.
    
                    // What do we do? We change drive type to tank and proceed as if that had been the selection. 
                    eeprom.ramcopy.DriveType = DT_TANK;
                    // We change it in EEPROM as well, so it will be fixed next time
                    EEPROM.writeInt(offsetof(_eeprom_data, DriveType), DT_TANK);
                }
                // In this case they have selected DriveType = DT_TANK. That means they won't be needing a steering servo. 
                LeftTread = new Servo_ESC (SERVONUM_LEFTTREAD,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                RightTread = new Servo_ESC (SERVONUM_RIGHTTREAD,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
                RCOutput1_Available = false;
                RCOutput2_Available = false;
                break;
    
            default:
                // We shouldn't end up here but in case we do, we need to define something or else the program will croak at runtime
                // We set it to SABERTOOTH, and save it to EEPROM so we don't end up here again next time
                eeprom.ramcopy.DriveMotors = SABERTOOTH;
                EEPROM.writeInt(offsetof(_eeprom_data, DriveMotors), SABERTOOTH);
                LeftTread = new Sabertooth_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_DRIVE_Address,&MotorSerial);
                RightTread = new Sabertooth_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_DRIVE_Address,&MotorSerial);
        }
        // Now initialize the motors
        RightTread->begin();        
        LeftTread->begin();
    }

    // STEERING SERVO
    // -------------------------------------------------------------------------------------------------------------------------------------->>
    if (eeprom.ramcopy.DriveType != DT_TANK)
    {   // If we are using a steering servo, it gets assigned to the Right tread servo port. Obviously the Right tread servo port can no longer
        // be used for the right tread. If a single rear drive axle is used, the drive ESC can be plugged into the Left tread servo port. Otherwise
        // if independent tread speeds are still desired in addition to the steering servo (halftracks with independent tread control), 
        // the user will have to use a serial dual motor controller or the onboard motor controllers. 
        SteeringServo = new Servo_ESC (SERVONUM_RIGHTTREAD,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
        // Initialize the servo
        SteeringServo->begin();
        // This slot is unavailable for general purpose servo
        RCOutput2_Available = false;
    }
    
    // TURRET MOTOR DEFINITION - ROTATION
    // -------------------------------------------------------------------------------------------------------------------------------------->>
    // We define turret rotation as SIDEA for all serial controllers
    switch (eeprom.ramcopy.TurretRotationMotor)
    {
        case SABERTOOTH:
            TurretRotation = new Sabertooth_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_TURRET_Address,&MotorSerial);
            break;
        case POLOLU:
             TurretRotation = new Pololu_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Pololu_TURRET_ID,&MotorSerial);
            break;
        case OP_SCOUT:
            TurretRotation = new OPScout_SerialESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            break;
        case ONBOARD:
            TurretRotation = new Onboard_ESC (SIDEA,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            MotorA_Available = false;
            break;
        case SERVO_ESC:
            TurretRotation = new Servo_ESC (SERVONUM_TURRETROTATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            RCOutput3_Available = false;
            break;
        case SERVO_PAN:
            TurretRotation = new Servo_PAN (SERVONUM_TURRETROTATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            RCOutput3_Available = false;
            break;
        default:
            // We shouldn't end up here but in case we do, we need to define something or else the program will croak at runtime
            // We set it to SERVO_ESC, and save it to EEPROM so we don't end up here next time. 
            eeprom.ramcopy.TurretRotationMotor = SERVO_ESC;
            EEPROM.writeInt(offsetof(_eeprom_data, TurretRotationMotor), SERVO_ESC);
            TurretRotation = new Servo_ESC (SERVONUM_TURRETROTATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            RCOutput3_Available = false;
    }
    // Now initialize the motor
    TurretRotation->begin(); 
    // The user also has the option of limiting max turret rotation speed. We can easily do this by using the set_MaxSpeedPct function of the motor object. 
    // Of course this will cause unintended consequences if the motor type is set to SERVO_ESC and they are using an unmodified hobby servo (ie, not a continuous rotation servo). 
    // In that case limiting the max speed will actually just limit the servo's travel. However for turret rotation it is unlikelyi an unmodified hobby servo would be used. 
    // We could do a check for SERVO_ESC, but that won't help, because the user could have plugged in a continuous rotation servo, or be using a hobby ESC, and still selected SERVO_ESC,
    // and in those cases limiting the max speed would indeed work as intended. 
    if (eeprom.ramcopy.TurretRotation_MaxSpeedPct < 100) { TurretRotation->set_MaxSpeedPct(eeprom.ramcopy.TurretRotation_MaxSpeedPct); }   

    // TURRET MOTOR DEFINITION - ELEVATION
    // -------------------------------------------------------------------------------------------------------------------------------------->>
    // We define turret elevation as SIDEB for all serial controllers
    switch (eeprom.ramcopy.TurretElevationMotor)
    {
        case SABERTOOTH:
            TurretElevation = new Sabertooth_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Sabertooth_TURRET_Address,&MotorSerial);
            break;
        case POLOLU:
            TurretElevation = new Pololu_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,Pololu_TURRET_ID,&MotorSerial);
            break;
        case OP_SCOUT:
            TurretElevation = new OPScout_SerialESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            break;
        case ONBOARD:
            TurretElevation = new Onboard_ESC (SIDEB,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            MotorB_Available = false;
            break;
        case SERVO_ESC:
            TurretElevation = new Servo_ESC (SERVONUM_TURRETELEVATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            RCOutput4_Available = false;
            break;
        case SERVO_PAN:
            TurretElevation = new Servo_PAN (SERVONUM_TURRETELEVATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            // TurretElevation is a pointer of class Motor. But if we are using barrel stabilization it will be useful to have an 
            // object of Servo_PAN type directly, we call this one Barrel. The two are the same, but Barrel will expose some methods that TurretElevation won't have.
            // FYI: Barrel stabilization can only be enabled if TurretElevation is set to pan servo, so you won't see this under any other category. 
            Barrel = new Servo_PAN (SERVONUM_TURRETELEVATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            Barrel->begin();    // Initialize the barrel
            RCOutput4_Available = false;
            break;
        default:
            // We shouldn't end up here but in case we do, we need to define something or else the program will croak at runtime
            // We set it to SERVO_ESC, and save it to EEPROM so we don't end up here next time. 
            eeprom.ramcopy.TurretElevationMotor = SERVO_ESC;
            EEPROM.writeInt(offsetof(_eeprom_data, TurretElevationMotor), SERVO_ESC);            
            TurretElevation = new Servo_ESC (SERVONUM_TURRETELEVATION,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0);
            RCOutput4_Available = false;
    }
    // Now initialize the motor
    TurretElevation->begin();    
    // The user also has the option of limiting max turret elevation speed. We can easily do this by using the set_MaxSpeedPct function of the motor object. 
    // Of course this will cause unintended consequences if the motor type is set to SERVO_ESC and they are using an unmodified hobby servo (ie, not a continuous rotation servo). 
    // For barrel elevation, that very well could be the case although we suggest that SERVO_PAN is a better choice for hobby servos in this application, which the user would
    // hopefully use instead, and in which case the speed limitation will work. 
    // Note we do not set a speed limitation on the Barrel copy object. That one is only used for barrel stabilization and in that case we don't want any limit on the speed of the servo. 
    if (eeprom.ramcopy.TurretElevation_MaxSpeedPct < 100) { TurretElevation->set_MaxSpeedPct(eeprom.ramcopy.TurretElevation_MaxSpeedPct); }
    
    // We may also have some custom end-points defined for the elevation motor, if it is set to servo. 
    // These end-points need to be set after the begin() statement, which initializes the endpoints to defaults.
    if (eeprom.ramcopy.TurretElevationMotor == SERVO_ESC || eeprom.ramcopy.TurretElevationMotor == SERVO_PAN)
    {
        // The end-points are actually handled in the servo class, not the motor class. Since TurretElevation is a pointer to the motor class, 
        // and not the servo class we need to call the servo class directly using TankServos (even though in this case TurretElevation is also a subclass of servo)
        TankServos.setMinPulseWidth(SERVONUM_TURRETELEVATION, eeprom.ramcopy.TurretElevation_EPMin);
        TankServos.setMaxPulseWidth(SERVONUM_TURRETELEVATION, eeprom.ramcopy.TurretElevation_EPMax);
        
        // For turret elevation, we use the reversed setting of the motor class. 
        // FYI, we don't have a reversed setting for traditional motors (Sabertooth, onboard, etc...) because in those cases you can just swap the motor wires. 
        TurretElevation->set_Reversed(eeprom.ramcopy.TurretElevation_Reversed);
        
        // If we have a duplicate barrel object, set its reversed flag as well. 
        if (eeprom.ramcopy.TurretElevationMotor == SERVO_PAN)
        { Barrel->set_Reversed(eeprom.ramcopy.TurretElevation_Reversed); }
    }

    // RECOIL SERVO DEFINITION
    // -------------------------------------------------------------------------------------------------------------------------------------->>
	// We still pass an external min/max speed although it won't be used for this object. 
	// What will be used are recoil/return times, along with a reverse setting if the servo needs to be reversed. These can be modified
	// later but will be initialized to sensible defaults.
        RecoilServo = new Servo_RECOIL (SERVONUM_RECOIL,MOTOR_MAX_REVSPEED,MOTOR_MAX_FWDSPEED,0,eeprom.ramcopy.RecoilServo_Recoil_mS,eeprom.ramcopy.RecoilServo_Return_mS,eeprom.ramcopy.RecoilReversed);
        RecoilServo->begin();
        // Recoil servos also have custom end-points. Because RecoilServo is a motor of class Servo, we can call setMin/MaxPulseWidth from the servo class directly, rather than from TankServos
        RecoilServo->setMinPulseWidth(SERVONUM_RECOIL, eeprom.ramcopy.RecoilServo_EPMin);
        RecoilServo->setMaxPulseWidth(SERVONUM_RECOIL, eeprom.ramcopy.RecoilServo_EPMax);
        // The reversed setting needs to be applied both to the motor class (flag) as well as to the servo class (actual recoil movement settings). 
        RecoilServo->set_Reversed(eeprom.ramcopy.RecoilReversed);                       // motor class method
        RecoilServo->setRecoilReversed(SERVONUM_RECOIL, eeprom.ramcopy.RecoilReversed); // servo class method

    // HENG LONG/OTHER SMOKER OUTPUT MOTOR DEFINITION
    // -------------------------------------------------------------------------------------------------------------------------------------->>
    	// There is only one smoker output, we use SIDEA. It does not do reverse, so MINSPEED is always 0. 
        // The smoker object also has two settings for idle and fast idle, which the user can save to EEPROM. Those are also passed to the constructor. 
        if (eeprom.ramcopy.SmokerControlAuto)
        {   // If the user wants the smoker to be controlled automatically with engine speed, we pass the user's  max speed setting
            Smoker = new Onboard_Smoker (SIDEA,0,eeprom.ramcopy.SmokerMaxSpeed,0,eeprom.ramcopy.SmokerIdleSpeed,eeprom.ramcopy.SmokerFastIdleSpeed);
        }
        else
        {   // But if the user wants to disconnect the smoker from the engine and control it manually, we set max speed to MOTOR_MAX_FWDSPEED (100% in effect)
            // We still pass the Idle and FastIdle speeds but they will not be used. 
            Smoker = new Onboard_Smoker (SIDEA,0,MOTOR_MAX_FWDSPEED,0,eeprom.ramcopy.SmokerIdleSpeed,eeprom.ramcopy.SmokerFastIdleSpeed);
        }
        Smoker->begin();

    // OPTIONAL RC OUTPUTS
    // -------------------------------------------------------------------------------------------------------------------------------------->>
        // If any of the first four RC outputs are unused by drive, turret or steering, we can set them to generic RC outputs and let the user 
        // control them directly for whatever purpose. They essentially "pass-through" whatever radio channel has been assigned to them. 
        // In OP Config the user has the option of doing regular RC passthrough (good for servos, continuous rotation servos, or ESCs), or they 
        // can select a Pan Servo passthrough. Which one they picked will influence what kind of object we need to create. The only way to know
        // is to run through the list of the user's function triggers and see. And they may not have any at all, in which case we don't need
        // to create anything. 

        // It is also technically possible for the user to assign more than one trigger to the same RC pass-through. This would be bad news, so
        // in this routine we make sure that no more than one trigger is assigned to an RC output.

        // Only check if one of these is even available for use
        if (RCOutput1_Available || RCOutput2_Available || RCOutput3_Available || RCOutput4_Available)
        {   // Loop through every function-trigger pair
            for (int i = 0; i <MAX_FUNCTION_TRIGGERS; i++)
            {   // A valid function-trigger will have a function number and a TriggerID > 0
                if (eeprom.ramcopy.SF_Trigger[i].specialFunction != SF_NULL_FUNCTION && eeprom.ramcopy.SF_Trigger[i].TriggerID > 0)
                {
                    switch (eeprom.ramcopy.SF_Trigger[i].specialFunction)
                    {
                        case SF_RC1_PASS:       // RC Output 1 passthrough
                            if (RCOutput1_Available && !RCOutput1_Assigned) 
                            {   //RC Output on LEFTTREAD
                                RCOutput1 = new Servo_ESC (SERVONUM_LEFTTREAD, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                RCOutput1->begin(); 
                                RCOutput1_Assigned = true;
                            }
                            break;
                        case SF_RC2_PASS:       // RC Output 2 passthrough
                            if (RCOutput2_Available && !RCOutput2_Assigned) 
                            {   //RC Output on RIGHTTREAD
                                RCOutput2 = new Servo_ESC (SERVONUM_RIGHTTREAD, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                RCOutput2->begin(); 
                                RCOutput2_Assigned = true;
                            }
                            break;
                        case SF_RC3_PASS:       // RC Output 3 passthrough
                            if (RCOutput3_Available && !RCOutput3_Assigned) 
                            {   //RC Output on TURRETROTATION
                                RCOutput3 = new Servo_ESC (SERVONUM_TURRETROTATION, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                RCOutput3->begin(); 
                                RCOutput3_Assigned = true;
                            }
                            break;
                        case SF_RC4_PASS:       // RC Output 4 passthrough
                            if (RCOutput4_Available && !RCOutput4_Assigned) 
                            {   //RC Output on TURRETELEVATION
                                RCOutput4 = new Servo_ESC (SERVONUM_TURRETELEVATION, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                RCOutput4->begin(); 
                                RCOutput4_Assigned = true;
                            }
                            break;
                        case SF_RC1_PASS_PAN:   // Pan Servo on RC Output 1
                            if (RCOutput1_Available && !RCOutput1_Assigned) 
                            {   // Servo_PAN on LEFTTTREAD
                                ServoOutput1 = new Servo_PAN (SERVONUM_LEFTTREAD, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                ServoOutput1->begin(); 
                                RCOutput1_Assigned = true;
                            }
                            break;
                        case SF_RC2_PASS_PAN:   // Pan Servo on RC Output 2
                            if (RCOutput2_Available && !RCOutput2_Assigned) 
                            {   // Servo_PAN on RIGHTTREAD
                                ServoOutput2 = new Servo_PAN (SERVONUM_RIGHTTREAD, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                ServoOutput2->begin(); 
                                RCOutput2_Assigned = true;
                            }
                            break;
                        case SF_RC3_PASS_PAN:   // Pan Servo on RC Output 3
                            if (RCOutput3_Available && !RCOutput3_Assigned) 
                            {   // Servo_PAN on TURRETROTATION
                                ServoOutput3 = new Servo_PAN (SERVONUM_TURRETROTATION, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                ServoOutput3->begin(); 
                                RCOutput3_Assigned = true;
                            }
                            break;
                        case SF_RC4_PASS_PAN:   // Pan Servo on RC Output 4
                            if (RCOutput4_Available && !RCOutput4_Assigned) 
                            {   // Servo_PAN on TURRETELEVATION
                                ServoOutput4 = new Servo_PAN (SERVONUM_TURRETELEVATION, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); 
                                ServoOutput4->begin(); 
                                RCOutput4_Assigned = true;
                            }
                            break;                                                        
                    }
                }
            }
        }
        //DebugSerial->print(F("RCOutput 1 Available: ")); PrintLnTrueFalse(RCOutput1_Available);
        //DebugSerial->print(F("RCOutput 2 Available: ")); PrintLnTrueFalse(RCOutput2_Available);
        //DebugSerial->print(F("RCOutput 3 Available: ")); PrintLnTrueFalse(RCOutput3_Available);
        //DebugSerial->print(F("RCOutput 4 Available: ")); PrintLnTrueFalse(RCOutput4_Available);

    // OPTIONAL ONBOARD MOTOR CONTROL
    // -------------------------------------------------------------------------------------------------------------------------------------->>
        // If either of the two onboard motor controllers (A & B) have not been assigned to any drive or turret function, we let the user
        // control them directly for whatever purpose, using an analog function trigger. We also can conveniently set the external speed range of the
        // motor objects to the actual range of the input we know we will receive from an analog trigger (0-1023 where 511 is center). 
        if (MotorA_Available) { MotorA = new Onboard_ESC (SIDEA, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); MotorA->begin(); }
        if (MotorB_Available) { MotorB = new Onboard_ESC (SIDEB, ANALOG_SPECFUNCTION_MIN_VAL, ANALOG_SPECFUNCTION_MAX_VAL, ANALOG_SPECFUNCTION_CENTER_VAL); MotorB->begin(); }
        //DebugSerial->print(F("Motor A Available: ")); PrintLnTrueFalse(MotorA_Available);
        //DebugSerial->print(F("Motor B Available: ")); PrintLnTrueFalse(MotorB_Available);
}




void SetupPins()
{
// These pins are defined in OP_Settings.h

// IO pins
    // These can be setup as input or output depending on the user's preference. 
    // IO "A" setup
    IO_Pin[IOA].Settings = eeprom.ramcopy.PortA_Settings;
    if (IO_Pin[IOA].Settings.dataDirection == OUTPUT)    
    {
        pinMode(pin_IO_A, OUTPUT);
        PortA_Off();    // Start with it low
    }   
    else                                            
    {   // Use pullups if the input is "digital" (only needs to read on/off)
        // But if they want a full analog range, don't use pullups because it will prevent us from going all the way to ground (0)
        if (IO_Pin[IOA].Settings.Digital) pinMode(pin_IO_A, INPUT_PULLUP); 
        else pinMode(pin_IO_A, INPUT); 
        PortA_ReadValue();  // Get the present value
    }
    // IO "B" setup
    IO_Pin[IOB].Settings = eeprom.ramcopy.PortB_Settings;
    if (IO_Pin[IOB].Settings.dataDirection == OUTPUT)
    {
        pinMode(pin_IO_B, OUTPUT);
        PortB_Off();    // Start with it low
    }
    else                                            
    {   // Use pullups if the input is "digital" (only needs to read on/off)
        // But if they want a full analog range, don't use pullups because it will prevent us from going all the way to ground (0)
        if (IO_Pin[IOB].Settings.Digital)   pinMode(pin_IO_B, INPUT_PULLUP);
        else pinMode(pin_IO_B, INPUT);        
        PortB_ReadValue(); // Get the present value
    }

// Voltage sensor
    pinMode(pin_BattVoltage, INPUT);
                                                
// Dipswitch pins
    // These are held to ground when On, but left floating when Off - so we use the input pullups to keep them high when Off
    pinMode(pin_Dip1, INPUT_PULLUP);        // Input    - Dipswitch 1
    pinMode(pin_Dip2, INPUT_PULLUP);        // Input    - Dipswitch 2
    pinMode(pin_Dip3, INPUT_PULLUP);        // Input    - Dipswitch 3
    pinMode(pin_Dip4, INPUT_PULLUP);        // Input    - Dipswitch 4
    pinMode(pin_Dip5, INPUT_PULLUP);        // Input    - Dipswitch 5

// Repair selection switch
    pinMode(pin_RepairTank, INPUT_PULLUP);  // Input    - Repair tank selection switch

// Pushbutton
    // Held to ground when pushed
    pinMode(pin_Button, INPUT_PULLUP);      // Input	- Pushbutton input

// Board LEDs
    pinMode(pin_RedLED, OUTPUT);            // Output   - Red   LED on board
    pinMode(pin_GreenLED, OUTPUT);	        // Output	- Green LED on board
    // Initialize all outputs to Off, EXCEPT for the Red LED, which we leave
    // on until the board has finished with all the startup routines
    GreenLedOff();
    RedLedOn();

// Transistorized Light outputs
    pinMode(pin_Light1, OUTPUT);	        // Output   - Light 1 output (Headlights)
    pinMode(pin_Light2, OUTPUT);            // Output   - Light 2 output
    analogWrite(pin_Light2, 1);
    pinMode(pin_Brakelights, OUTPUT);	    // Output   - Brake light output. PWM capable.
    pinMode(pin_AuxOutput, OUTPUT);         // Output   - Aux output. PWM capable. Has flyback diode, can drive a relay directly or a (very small) motor 
    pinMode(pin_HitNotifyLEDs, OUTPUT);	    // Output   - Hit notification LEDs if using the Tamiya apple
    // This one is PNP, so logic high is off
    digitalWrite(pin_MuzzleFlash, HIGH);
    pinMode(pin_MuzzleFlash, OUTPUT);	    // Output	- Trigger output for Taigen High Intensity muzzle flash unit
    // Machine gun LED has to be manipulated directly, we can't use the Arduino functions
    MG_DDR |= (1 << MG_PORTPIN);            // Output   - Machine gun - set the port pin to output ("or" that bit of the data direction register with a 1)

    // Initialize all these outputs to off
    // These are NPN MOSFETs, logic low is off
    digitalWrite(pin_Light1, LOW);
    digitalWrite(pin_Light2, LOW);
    digitalWrite(pin_Brakelights, LOW);
    digitalWrite(pin_AuxOutput, LOW);
    digitalWrite(pin_HitNotifyLEDs, LOW);
    MG_PORT &= ~(1 << MG_PORTPIN);          // Machine Gun LED must be set directly. We "and-not" the port pin bit with 1 to set it to 0, this turns it off


// Mechanical Recoil Trigger
    pinMode(pin_MechRecoilMotor, OUTPUT);	    // Output   - Transistor for Asiatam, Tamiya or similar mechanical recoil units
    // Also NPN, logic low is off
    digitalWrite(pin_MechRecoilMotor, LOW);
    
}







