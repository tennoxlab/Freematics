#ifndef _TELE_CONFIG
#define _TELE_CONFIG
class teleConfig{
private:
    //PID param
    int     _PID_SPEED;
    int     _PID_RPM;
    int     _PID_THROTTLE;
    int     _PID_ENGINE_LOAD;
    int     _PID_FUEL_PRESSURE;
    int     _PID_TIMING_ADVANCE;
    int     _PID_COOLANT_TEMP;
    int     _PID_INTAKE_TEMP;

public:
    teleConfig();
    ~teleConfig();

    void    setPidSpeed(int);
    void    setPidRpm(int);
    void    setPidThrottle(int);
    void    setPidEngineLoad(int);
    void    setPidFuelPressure(int);
    void    setPidTimingAdvance(int);
    void    setPidCoolentTemp(int);
    void    setPidIntakeTemp(int);

    int     getPidSpeed(void);
    int     getPidRpm(void);
    int     getPidThrottle(void);
    int     getPidEngineLoad(void);
    int     getPidFuelPressure(void);
    int     getPidTimingAdvance(void);
    int     getPidCoolentTemp(void);
    int     getPidIntakeTemp(void);

    int     update(int);


};
#endif
