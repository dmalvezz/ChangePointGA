
import tensorflow as tf
import numpy as np

#Mass
GRAVITY = tf.constant(9.81, tf.float32, name="GRAVITY")
DRIVER_MASS = tf.constant(56.0, tf.float32, name="DRIVER_MASS")
BARE_BONE_MASS = tf.constant(27.0, tf.float32, name="BARE_BONE_MASS")
MASS = tf.add(DRIVER_MASS, BARE_BONE_MASS, name="MASS")
INERTIAL_MASS = tf.add(MASS, 4.0)

#Bearings
BEARING_CR = tf.constant(0.00008, tf.float32, name="BEARING_CR")

def get_f_bearings():
    return tf.multiply(BEARING_CR, tf.multiply(MASS, GRAVITY), name="f_bearings")
    

def get_f_slope(slope):
	return tf.multiply(tf.multiply(MASS, GRAVITY), tf.sin(slope), name="f_slope")

#Aerodynamic
T_AMB = tf.constant(273.0 + 25.0, tf.float32, name="T_AMB") # Kelvin + degree
AIR_DENSITY = tf.divide(101325.0/287.058, T_AMB, name="AIR_DENSITY")

CX = tf.constant(0.12, tf.float32, name="CX")
FRONTAL_AREA = tf.constant(0.35, tf.float32, name="FRONTAL_AREA")

def get_f_aerodynamic(v, radius):
    yaw = tf.multiply(25.0 / 0.10, radius)
    yaw_factor = tf.add(tf.multiply(3E-17, tf.pow(yaw, 5)), tf.multiply(2E-05, tf.pow(yaw, 4)))
    yaw_factor = tf.add(yaw_factor, tf.add(tf.multiply(5E-14, tf.pow(yaw, 3)), tf.multiply(0.0037, tf.pow(yaw, 2))))
    yaw_factor = tf.add(yaw_factor, tf.add(tf.multiply(2E-11, yaw), 1.0))

    f_aerodynamic = tf.multiply(0.5, tf.multiply(tf.multiply(AIR_DENSITY, tf.pow(v, 2)), tf.multiply(FRONTAL_AREA, CX)))

    return tf.multiply(f_aerodynamic, yaw_factor, name="f_aerodynamic")


#Tyres
WHEEL_BASE = tf.constant(1.60, tf.float32, name="WHEEL_BASE")       # m (passo)
VEHICLE_XG = tf.constant(0.90, tf.float32, name="VEHICLE_XG")       # m (distanza centro di massa - ruota posteriore)
VEHICLE_ZG = tf.constant(0.52, tf.float32, name="VEHICLE_ZG")       # m (distanza centro di massa - piano stradale)
VEHICLE_TRACK = tf.constant(0.52, tf.float32, name="VEHICLE_TRACK") # m (carreggiata)
TYRE_PRESSURE = tf.constant(6.00, tf.float32, name="VEHICLE_XG")    # bar (London pressure) 5.00; // bar (!!!! tra 4 e 6.5 bar)
TYRE_CR = tf.constant(0.0012, tf.float32, name="TYRE_CR")           # (Michelin official 4bar: 0.001)   // Cr a 4bar
TYRE_CORNER_CORR_FACTOR = tf.constant(1.35, tf.float32, name="TYRE_CORNER_CORR_FACTOR")  

TYRE_FRONT_CAMBER = tf.constant(0.1, tf.float32, name="TYRE_FRONT_CAMBER")  # deg (camber ruote anteriori)
TYRE_FRONT_TOE_IN = tf.constant(0.0, tf.float32, name="TYRE_FRONT_TOE_IN")  # deg (toe in ruote anteriori)
TYRE_FRONT_MISALIGNMENT = tf.constant(0.0, tf.float32, name="TYRE_FRONT_MISALIGNMENT")  #  deg (non-allineamento tra ruote anteriori e posteriori)

A30 = tf.constant(57.806, tf.float32, name="A30")      
A31 = tf.constant(15.101, tf.float32, name="A31")       
A40 = tf.constant(-0.082, tf.float32, name="A40")       
A41 = tf.constant(0.186, tf.float32, name="A41")       

def get_f_eq_components(v, radius):
    # Removed sign

    fzr = tf.multiply(MASS, tf.multiply(GRAVITY, tf.subtract(1.0, tf.divide(VEHICLE_XG, WHEEL_BASE))))

    fzi = tf.subtract(
        tf.multiply(GRAVITY, tf.divide(tf.divide(VEHICLE_XG, WHEEL_BASE), 2.0)),
        tf.multiply(tf.divide(tf.multiply(VEHICLE_ZG, tf.pow(v, 2.0)), VEHICLE_TRACK), radius)
    )
    fzi = tf.maximum(0.0, tf.multiply(MASS, fzi))

    fzo = tf.add(
        tf.multiply(GRAVITY, tf.divide(tf.divide(VEHICLE_XG, WHEEL_BASE), 2.0)),
        tf.multiply(tf.divide(tf.multiply(VEHICLE_ZG, tf.pow(v, 2.0)), VEHICLE_TRACK), radius)
    )
    fzo = tf.maximum(0.0, tf.multiply(MASS, fzo))

    car = tf.multiply(
        tf.add(A30, tf.multiply(A31, TYRE_PRESSURE)),
        tf.multiply(
            tf.sin(
                tf.multiply(2.0, tf.atan(
                    tf.divide(tf.divide(fzr, 1000.0), 
                    tf.add(A40, tf.multiply(A41, TYRE_PRESSURE)))
                ))
            ), 
            180.0 / np.pi
        )
    )

    cai = tf.multiply(
        tf.add(A30, tf.multiply(A31, TYRE_PRESSURE)),
        tf.multiply(
            tf.sin(
                tf.multiply(2.0, tf.atan(
                    tf.divide(tf.divide(fzi, 1000.0), 
                    tf.add(A40, tf.multiply(A41, TYRE_PRESSURE)))
                ))
            ), 
            180.0 / np.pi
        )
    )

    cao = tf.multiply(
        tf.add(A30, tf.multiply(A31, TYRE_PRESSURE)),
        tf.multiply(
            tf.sin(
                tf.multiply(2.0, tf.atan(
                    tf.divide(
                        tf.divide(fzo, 1000.0),
                        tf.add(A40, tf.multiply(A41, TYRE_PRESSURE))
                    )
                ))
            ), 
            180.0 / np.pi
        )
    )

    crr = tf.multiply(TYRE_CR,
        tf.add(
            tf.add(
                tf.multiply(-0.0156, tf.pow(TYRE_PRESSURE, 3.0)),
                tf.multiply(0.289, tf.pow(TYRE_PRESSURE, 2.0))
            ),
            tf.add(
                tf.multiply(-1.803, TYRE_PRESSURE),
                4.587
            )
        )
    )

    cri = cro = crr

    # Removed if
    angleo = tf.atan(
        tf.divide(
            WHEEL_BASE,
            tf.add(
                tf.divide(1.0, radius),
                tf.divide(VEHICLE_TRACK, 2.0)
            )
        )
    )

    anglei = tf.atan(
        tf.divide(
            WHEEL_BASE,
            tf.subtract(
                tf.divide(1.0, radius),
                tf.divide(VEHICLE_TRACK, 2.0)
            )
        )
    )

    angleo = tf.add(angleo, tf.divide(TYRE_FRONT_TOE_IN, 180.0 * np.pi))
    anglei = tf.subtract(anglei, tf.divide(TYRE_FRONT_TOE_IN, 180.0 * np.pi))

    # Removed if
    angleo = tf.add(angleo, tf.divide(TYRE_FRONT_MISALIGNMENT, 180.0 * np.pi))
    anglei = tf.add(anglei, tf.divide(TYRE_FRONT_MISALIGNMENT, 180.0 * np.pi))
   
    cambo = tf.multiply(tf.divide(TYRE_FRONT_CAMBER, 180.0), np.pi)
    cambi = tf.multiply(tf.divide(TYRE_FRONT_CAMBER, -180.0), np.pi)

    fxr = tf.multiply(crr, fzr)

    fxi = tf.multiply(
        tf.multiply(cri, fzi),
        tf.add(
            tf.add(
                1.0,
                tf.multiply(0.0005, tf.pow(tf.multiply(cambi, np.pi * 180.0), 3.0))
            ),
            tf.add(
                tf.multiply(-0.01, tf.pow(tf.multiply(cambi, np.pi * 180.0), 2.0)),
                tf.multiply(0.076, tf.multiply(cambi, np.pi * 180.0))
            )
        )
    )
    
    fxo = tf.multiply(
        tf.multiply(cro, fzo),
        tf.add(
            tf.add(
                1.0,
                tf.multiply(0.0005, tf.pow(tf.multiply(cambo, np.pi * 180.0), 3.0))
            ),
            tf.add(
                tf.multiply(-0.01, tf.pow(tf.multiply(cambo, np.pi * 180.0), 2.0)),
                tf.multiply(0.076, tf.multiply(cambo, np.pi * 180.0))
            )
        )
    )
    
    fc = tf.multiply(tf.pow(v, 2.0), tf.multiply(radius, MASS))

    return angleo, anglei, cambo, cambi, car, cao, cai, fxr, fxo, fxi, fc

def get_f_eq(v, radius, beta, angleo, anglei, cambo, cambi, car, cao, cai, fc, fxr, fxo, fxi):
    e = tf.cond(tf.equal(radius, 0.0), 
        true_fn=lambda: tf.zeros(beta.shape), 
        false_fn=lambda: tf.subtract(VEHICLE_XG, tf.divide(tf.sin(beta), radius))
    ) 
    
    slipr = tf.cond(tf.equal(radius, 0.0), 
        true_fn=lambda: tf.zeros(beta.shape), 
        false_fn=lambda: tf.atan(tf.divide(tf.multiply(e, radius), tf.cos(beta)))
    )

    slipi = tf.cond(tf.equal(radius, 0.0), 
        true_fn=lambda: tf.zeros(beta.shape), 
        false_fn=lambda: tf.subtract(
            anglei,
            tf.atan(
                tf.divide(
                    tf.subtract(WHEEL_BASE, e), 
                    tf.subtract(
                        tf.divide(tf.cos(beta), radius), 
                        tf.divide(VEHICLE_TRACK, 2.0)
                    ) 
                )
            )
        )
    )

    slipo = tf.cond(tf.equal(radius, 0.0), 
        true_fn=lambda: tf.zeros(beta.shape), 
        false_fn=lambda: tf.subtract(
            angleo,
            tf.atan(
                tf.divide(
                    tf.subtract(WHEEL_BASE, e), 
                    tf.add(
                        tf.divide(tf.cos(beta), radius), 
                        tf.divide(VEHICLE_TRACK, 2.0)
                    ) 
                )
            )
        )
    )
   
    fyr = tf.multiply(car, slipr)
    fyi = tf.add(tf.multiply(cai, slipi), tf.divide(tf.multiply(cambi, cai), 10.0))          
    fyo = tf.add(tf.multiply(cao, slipo), tf.divide(tf.multiply(cambo, cao), 10.0))          

    eq = tf.subtract(
            tf.add(
                tf.add(
                    tf.multiply(fc, tf.cos(beta)),
                    tf.multiply(fxo, tf.sin(angleo))
                ),
                tf.subtract(
                    tf.multiply(fxi, tf.sin(anglei)),
                    fyr
                )
            ),
            tf.add(
                tf.multiply(fyo, tf.cos(angleo)),
                tf.multiply(fyi, tf.cos(anglei)) 
            )
        )

    return eq, beta, fyo, fyi

def get_optimal_beta(v, radius, angleo, anglei, cambo, cambi, car, cao, cai, fc, fxr, fxo, fxi):
    opt_beta = tf.zeros(v.shape)

    eq = get_f_eq(v, radius, opt_beta, angleo, anglei, cambo, cambi, car, cao, cai, fc, fxr, fxo, fxi)[0]
    grad = tf.gradients(eq, opt_beta)[0]
    
    for i in range(0, 5):
        eq = get_f_eq(v, radius, opt_beta, angleo, anglei, cambo, cambi, car, cao, cai, fc, fxr, fxo, fxi)[0]
        opt_beta = tf.subtract(opt_beta, 
            tf.cond(
                tf.equal(radius, 0.0),
                true_fn= lambda: tf.zeros(v.shape),
                false_fn= lambda: tf.divide(eq, grad)
            )
        )

    return opt_beta
   

def get_f_tyres(v, radius):
    angleo, anglei, cambo, cambi, car, cao, cai, fxr, fxo, fxi, fc = get_f_eq_components(v, radius)
   
    #opt_beta = tf.zeros(v.shape)
    opt_beta = get_optimal_beta(v, radius, angleo, anglei, cambo, cambi, car, cao, cai, fc, fxr, fxo, fxi)

    feq, beta, fyo, fyi = get_f_eq(v, radius, opt_beta, angleo, anglei, cambo, cambi, car, cao, cai, fc, fxr, fxo, fxi)

    f_tractive = tf.add(
        tf.add(
            tf.add(
                fxr,
                tf.multiply(fxo, tf.cos(angleo))
            ),
            tf.add(
                tf.multiply(fxi, tf.cos(anglei)),
                tf.multiply(fyo, tf.sin(angleo))
            )
        ),
        tf.subtract(
            tf.multiply(fyi, tf.sin(anglei)),
            tf.multiply(fc, tf.sin(opt_beta))
        )
    )

    f_tractive = tf.add(
        f_tractive,
        tf.subtract(
            tf.multiply(0.00765, tf.pow(v, 2.0)),
            tf.multiply(0.0062, v)
        )
    )

    return f_tractive


WHEEL_DIAMETER = tf.constant(0.466, tf.float32, name="WHEEL_DIAMETER")
TRANSMISSION_RATEO = tf.constant(1.0 / 32.0, tf.float32, name="TRANSMISSION_RATEO")
TRANSMISSION_EFF = tf.constant(1.0 / 32.0, tf.float32, name="TRANSMISSION_EFF")
MOTOR_SC = tf.constant(25.2, tf.float32, name="MOTOR_SC")
MOTOR_STG = tf.constant(71.93, tf.float32, name="MOTOR_STG")

BATTERY_VOLTAGE = tf.constant(46.0, tf.float32, name="BATTERY_VOLTAGE")

MAX_MOTOR_POWER = tf.constant(500.0, tf.float32, name="MAX_MOTOR_POWER")
MAX_WHEEL_FORCE = tf.constant(65.0, tf.float32, name="MAX_WHEEL_FORCE")

def get_f_traction_max(v):
    wheel_speed = tf.divide(tf.multiply(2.0, v), WHEEL_DIAMETER)

    motor_speed = tf.divide(wheel_speed, TRANSMISSION_RATEO)
	
    motor_torque = tf.divide(tf.subtract(tf.multiply(BATTERY_VOLTAGE, MOTOR_SC), motor_speed), MOTOR_STG) 
    
    wheel_torque = tf.multiply(tf.divide(motor_torque, TRANSMISSION_RATEO), TRANSMISSION_EFF) 
	
    f_traction = tf.divide(wheel_torque, tf.divide(WHEEL_DIAMETER, 2.0))
	
    f_traction = tf.minimum(f_traction, tf.divide(MAX_MOTOR_POWER, v))
    f_traction = tf.minimum(f_traction, MAX_WHEEL_FORCE)
    f_traction = tf.maximum(f_traction, 0.0)

    return f_traction
