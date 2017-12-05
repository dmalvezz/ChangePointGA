
import tensorflow as tf
import numpy as np
import time

import londontrack17 as track
import escorpio17 as vehicle

#Model parameter
raceTime = 2340.0
raceLaps = 10
ds = 150.0
simulation_count = 1
simulation_step = int(track.LENGTH / ds)

#Model variable
x = tf.Variable(0.0, name="x", trainable=False)
v = [None] * simulation_step
f_traction = [tf.Variable(tf.zeros(simulation_count), name="f_traction")] * simulation_step

current_v = tf.placeholder(tf.float32, shape=[simulation_count])

#Track data
slope = track.get_slope(x)
radius = track.get_radius(x)

#Model forces
f_bearings = vehicle.get_f_bearings()
f_slope = vehicle.get_f_slope(slope)
f_traction_max = vehicle.get_f_traction_max(current_v)

f_res = [None] * simulation_step
f_tot = [None] * simulation_step
a = [None] * simulation_step
delta = [None] * simulation_step
dt = [None] * simulation_step
f_aerodynamic = [None] * simulation_step
f_tyres = [None] * simulation_step

v[0] = tf.constant(0.0, tf.float32, shape=[simulation_count], name="V0")
for i in range(0, simulation_step - 1):
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

    writer = tf.summary.FileWriter('logs', sess.graph)

    #init
    start_time = time.time()

   
    hndl = sess.partial_run_setup(v[1])

    sess.run(tf.global_variables_initializer())

    v_i = sess.partial_run(hndl, v[0])
    for i in range(0, 2):

        #f_min = 0
        #f_max = sess.run(f_traction_max, feed_dict={current_v: v_i})
        #tf.random_uniform(f_traction[i].shape, f_min, f_max)
        sess.run(tf.assign(f_traction[i], [10.0]))

        print("it ", i)
        print("x = ", sess.run(x))
        print("v = ", v_i)

        print("radius =", sess.run(radius))
        print("slope =", sess.run(slope))

        print("fbearing =", sess.run(f_bearings))
        print("fslope =", sess.run(f_slope))
        print("faero =", sess.run(f_aerodynamic[i]))
        print("ftyre =", sess.run(f_tyres[i]))
        print("fres =", sess.run(f_res[i]))
        print("ftot =", sess.run(f_tot[i]))
        print("a =", sess.run(a[i]))
        print("delta =", sess.run(delta[i]))
        print("dt =", sess.run(dt[i]))
        #print("beta = ", sess.run(beta))
        #print("eq = ", sess.run(f_eq, feed_dict={current_v: v_i}))

        v_i = sess.partial_run(hndl, v[i+1])
        sess.run(tf.assign_add(x, ds))
        print("vf = ", v_i)
        print("")

    print("--- %s seconds ---" % (time.time() - start_time))


    writer.close()

    tf.train.write_graph(sess.graph_def, 'models/', 'graph.pb', as_text=False)




