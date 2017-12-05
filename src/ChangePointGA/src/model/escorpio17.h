//****************************************************//
// Escorpio 2017 model                                //
//****************************************************//

#include <math.h>


//#define ESCORPIOEVO3
//#define POWERTRAIN_FLAT
//#define POWERTRAIN_2016

#define POWERTRAIN_FLAT_EFF 0.89

#define gravity           9.81       //  m/s^2
#define WheelDiameter     0.466      //  m

// Flag Modello sequenziale
//static unsigned char Sequential = 0;
//static float         Tstep      = 0;
//static float FrontWheelMaxFy   = 0;
//static float FrontWheelMaxSlip = 0;
// !!! Se utilizzato, chaimare la funzione Escorpio_init() prima della simulazione!


 
// RE40 misurato
/*
#define TransmissionRatio (1.0/30.0)
#define MotorSC           33.7       // (RAD/s)/V
#define MotorSTG          400.0      // (RAD/s)/Nm
#define MotorTC           0.032      //  Nm/A
#define MotorMattrito     0.006      //  Nm
*/


// RE40 (targa)
/*
#define TransmissionRatio (1.0/30.0)
#define MotorSC           33.2       // (RAD/s)/V
#define MotorSTG          348.5      // (RAD/s)/Nm
#define MotorTC           0.0302     //  Nm/A
#define MotorMattrito     0.0045     //  Nm
*/

#ifdef POWERTRAIN_2017
// RE50 (targa) 
#define MotorSC           25.2         // (RAD/s)/V
#define MotorSTG          71.93        // (RAD/s)/Nm
#define MotorTC           0.0396       //  Nm/A
static float  MotorMattrito  =   0.00986;      //  Nm
#define MotorR     0.103   // ohm
#define alpha_cu   0.00426  
#else
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// RE50 (36V targa) 
#define MotorSC           16.5         // (RAD/s)/V
#define MotorSTG          66.77        // (RAD/s)/Nm
#define MotorTC           0.0604       //  Nm/A
static float  MotorMattrito = 0.00986;      //  Nm
#define MotorR     0.244   // ohm
#define alpha_cu   0.00426  
#endif


static float RthCaseAmbient = 3.81; // K/W
#define RthWindingCase 1.24 // K/W
#define CthWinding     (71.1/RthWindingCase)  // J/K
#define CthCase        (383.0/RthCaseAmbient) // J/K

//static float LastTractiveForce = 0;
//float MotorStartPenaltyThr = 0.1;    // N 
//float MotorStartPenalty =  70;     // J Energy to switch on motor
static float MotorStartPenaltyThr = 0.00001;    // N
//float MotorStartPenalty =  70;     // J Energy to switch on motor
static float MotorStartPenalty = 70;     // J Energy to switch on motor

static float Tamb = 25.0; 	//°C
static float Trear = 25.0;              // °C Temp vano posteriore
//static float TmotorCase;          // °C (Motor Case Temperature)
//static float Twindings;          // °C (Motor Windings Temperature)
//static float TmotorCaseStart;     // °C (Motor Case Temperature)
//static float TwindingsStart;     // °C (Motor Windings Temperature)

// Forze sulle ruote
//static float FRAD_fi, FRAD_fo, FRAD_r;
//static float FLAT_fi, FLAT_fo, FLAT_r;

// Trasmissione
#define TransmissionEff   0.99
//float TransmissionRatio = (1.0/24);   //RE50 24V
//float TransmissionRatio = (1.0/16.1);    //RE50 36V
//float TransmissionRatio = (1.0 / 17.05);    //RE50 36V
//float TransmissionRatio = (17.0 / 290.0);    //RE50 36V Z17
//float TransmissionRatio = (16.0 / 290.0);    //RE50 36V Z16
//float TransmissionRatio = (15.0 / 290.0);    //RE50 36V Z15
//float TransmissionRatio = (14.0 / 290.0);    //RE50 36V  Z14  <-----
//float TransmissionRatio = (13.0 / 290.0);    //RE50 36V  Z13
//float TransmissionRatio = (1.0 / 15.26);    //RE50 36V
//#define TransmissionRatio (1.0/20)
//float TransmissionRatio = (1.0 / 28.0);    //RE50 24V  

#ifdef POWERTRAIN_2017
static float TransmissionRatio = (1 / 32.0);    //RE50 24V new ratio
//float TransmissionRatio = (1 / 30.0);    //RE50 24V new ratio
#else
static float TransmissionRatio = (14.0 / 290.0);    //RE50 36V  Z14  <-----
#endif


// Rendimento a 300W, 40Vout: 97%
#ifdef POWERTRAIN_2017
//Legacy
#define DriverReq         0.08       //  ohm
#define DriverIq          0.02       //  A
// New accurate model
#define DRV17_Pq 1.183 
#define DRV17_ki -0.04425 
#define DRV17_kv 0.001047 
#define DRV17_req 0.03708
#else
#define DriverReq         0.08       //  ohm
#define DriverIq          0.02       //  A
#endif
//#define DriverReq         (0.18)      //  ohm  <-----
//#define DriverIq           0.035       //  A   <-----
//#define DriverReq         (0.25)      //  ohm
//#define DriverIq           0.035       //  A


#ifdef ESCORPIOEVO3
static float BareBoneMass = 27.0; //28.0
#else
static float BareBoneMass         = 35.0;
#endif
#define DriverMass           56.0
#define VehicleMass          (BareBoneMass+DriverMass)  //  Kg
static float RotationalEqivalentMass = 4.0;
#define VehicleInertialMass  (VehicleMass + RotationalEqivalentMass)  //  Kg
#define BearingCr            0.00008
//#define AirDensity           1.225     // 15°C
//#define AirDensity           1.17     // 30°C
#define AirDensity       (101325/287.058/(273+Tamb)) 
#ifdef ESCORPIOEVO3
#define FrontalArea          0.35      // m^2
#define VehicleCx            0.12     // 0.11 //0.16 //0.12 
#else
static float FrontalArea = 0.41;      // m^2
#define VehicleCx            0.12 //0.16 //0.12 
#endif
#define TyresCalpha       105// 90  (non più utilizzato)
#define a30 57.806 // Costanti della "Magic Formula"
#define a31 15.101
#define a40 -0.082
#define a41 0.186
#define TyresCr           0.0012 // (Michelin official 4bar: 0.001)   // Cr a 4bar
#define TyresCorneringCorrFactor 1.35

static float VehicleWheelBase  = 1.60; // m (passo)
static float VehicleTrack      = 0.52; // m (carreggiata)
static float VehicleXg         = 0.90; // m (distanza centro di massa - ruota posteriore)
static float VehicleZg         = 0.52; // m (distanza centro di massa - piano stradale)
static float TyrePressure      = 6.00; // bar (London pressure) 5.00; // bar (!!!! tra 4 e 6.5 bar)

/*
float TyresFrontCamber        = 0.2; // deg (camber ruote anteriori)
float TyresFrontToeIn         = 0.2; // deg (toe in ruote anteriori)
float TyresFrontMisAlignement = 0.4; // deg (non-allineamento tra ruote anteriori e posteriori)
*/
//Nuovo assale Carbonio
static float TyresFrontCamber = 0.1; // deg (camber ruote anteriori)
static float TyresFrontToeIn = 0.0; // deg (toe in ruote anteriori)
static float TyresFrontMisAlignement = 0.0; // deg (non-allineamento tra ruote anteriori e posteriori)

#define MaxMotorPower      500//400 // W
#define MaxWheelForce       65//60 // N
//float BatteryVoltage     = 44;  // V, used for fmax
static float BatteryVoltage     = 46;  // V, used for fmax

// Stima momento di inerzia oggetti in moto
// Iruota = 0,075 kg*m^2
// Meq = (I/r^2)*(speed/VehicleSpeed)
// Meq(3ruote) = 0,075*3/0,233^2 = 4,1 kg

