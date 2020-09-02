#include "teleConfig.h"

teleConfig::teleConfig(){
    _PID_SPEED = 1;
    _PID_RPM = 1;
    _PID_THROTTLE =1;
    _PID_ENGINE_LOAD = 1;
    _PID_FUEL_PRESSURE = 2;
    _PID_TIMING_ADVANCE = 2;
    _PID_COOLANT_TEMP = 3;
    _PID_INTAKE_TEMP = 3;
}

teleConfig::~teleConfig(){

}

void teleConfig::setPidSpeed( int speed){
    _PID_SPEED = speed;
}

void teleConfig::setPidRpm(int rpm){
    _PID_RPM = rpm;
}

void teleConfig::setPidThrottle(int throttle){
    _PID_THROTTLE = throttle;
}

void teleConfig::setPidEngineLoad(int load){
    _PID_ENGINE_LOAD = load;
}

void teleConfig::setPidFuelPressure(int pressure){
    _PID_FUEL_PRESSURE = pressure;
}

void teleConfig::setPidTimingAdvance(int timingadvance){
    _PID_TIMING_ADVANCE = timingadvance;
}

void teleConfig::setPidCoolentTemp(int coolentTemp){
    _PID_COOLANT_TEMP = coolentTemp;
}

void teleConfig::setPidIntakeTemp(int intakeTemp){
    _PID_INTAKE_TEMP = intakeTemp;
}

int teleConfig::getPidSpeed(){
    return _PID_SPEED;
}

int teleConfig::getPidRpm(){
    return _PID_RPM;
}

int teleConfig::getPidThrottle(){
    return _PID_THROTTLE;
}

int teleConfig::getPidEngineLoad(){
    return _PID_ENGINE_LOAD;
}

int teleConfig::getPidFuelPressure(){
    return _PID_FUEL_PRESSURE;
}

int teleConfig::getPidTimingAdvance(){
    return _PID_TIMING_ADVANCE;
}

int teleConfig::getPidCoolentTemp(){
    return _PID_COOLANT_TEMP;
}

int teleConfig::getPidIntakeTemp(){
    return _PID_INTAKE_TEMP;
}

int teleConfig::update(int option){
    return 0;
}
