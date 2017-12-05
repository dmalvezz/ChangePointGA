
import tensorflow as tf
import numpy as np
import time

import londontrack17 as track
import escorpio17 as vehicle

#Model parameter
raceTime = 2340.0
raceLaps = 10
ds = 15.0
simulation_count = 1
simulation_step = int(track.LENGTH / ds)

#Model variable
v = [None] * simulation_step
f_traction = [None] * (simulation_step - 1)

current_v = tf.placeholder(tf.float32, shape=[simulation_count])

f_res = [None] * simulation_step
f_tot = [None] * simulation_step
a = [None] * simulation_step
delta = [None] * simulation_step
dt = [None] * simulation_step
f_aerodynamic = [None] * simulation_step
f_tyres = [None] * simulation_step

print("Creating model graph...")

v[0] = tf.constant(0.0, tf.float32, shape=[simulation_count], name="V0")
for i in range(0, simulation_step - 1):
    f_traction[i] = tf.Variable(60.0*tf.ones(simulation_count), name="f_traction")

    #Track data
    slope = track.get_slope(float(i) * ds)
    radius = track.get_radius(float(i) * ds)

    #Model forces
    f_bearings = vehicle.get_f_bearings()
    f_slope = vehicle.get_f_slope(slope)
    f_aerodynamic[i] = vehicle.get_f_aerodynamic(v[i], radius)
    f_tyres[i] = vehicle.get_f_tyres(v[i], radius)
    f_res[i] = tf.add(
            tf.add(
                f_bearings,
                f_slope
            ),
            tf.add(
                f_aerodynamic[i],
                f_tyres[i]
            )
        )

    f_tot[i] = tf.subtract(f_traction[i], f_res[i])
    a[i] = tf.divide(f_tot[i], vehicle.INERTIAL_MASS)
    delta[i] = tf.add(tf.pow(v[i], 2.0), tf.multiply(2.0 * ds, a[i]))
    dt[i] = tf.divide(tf.subtract(tf.sqrt(delta[i]), v[i]), a[i])

    v[i + 1] = tf.add(v[i], tf.multiply(a[i], dt[i]))


#Run
with tf.Session() as sess:

    #writer = tf.summary.FileWriter('logs', sess.graph)

    #init
    print("Session init...")
    sess.run(tf.global_variables_initializer())

    #for i in range(0, simulation_step):
    #sess.run(tf.assign(f_traction[0], [65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0]))
    
    print("Creating loss function...")
    run_time = tf.zeros([simulation_count])
    dts = tf.zeros([simulation_count])
    for i in range(0, simulation_step-1):
        run_time = tf.add(run_time, dt[i])
        dts = tf.add(dts, tf.pow(np.e, -delta[i]))

        #print(sess.run(v[i]))

        #print("faero =", sess.run(f_aerodynamic[i]))
        #print("ftyre =", sess.run(f_tyres[i]))
        #print("fres =", sess.run(f_res[i]))
        #print("ftot =", sess.run(f_tot[i]))
        #print("a =", sess.run(a[i]))
        #print("delta =", sess.run(delta[i]))
        #print("dt =", sess.run(dt[i]))

        #print("")

    #print("tot time=", sess.run(run_time))

    loss = tf.pow(tf.subtract(run_time, 234.0), 2.0)
    #loss = tf.add(loss, dts)
    opt = tf.train.GradientDescentOptimizer(0.1)
    minimize = opt.minimize(loss)

    for i in range(0, 1000):
        print("iter", i)
        start_time = time.time()

        sess.run(minimize)
        err = sess.run(loss)
        dterr = sess.run(dts)

        print("loss =", err, "dterr =", dterr, "   time =", (time.time() - start_time))     

    #writer.close()

    print(sess.run(f_traction))

    #tf.train.write_graph(sess.graph_def, 'models/', 'graph.pb', as_text=False)




