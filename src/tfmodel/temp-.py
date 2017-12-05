 opt_beta = tf.zeros([simulation_count])
    for i in range(0, 5):
        eq = vehicle.get_f_eq(current_v, radius, opt_beta)
        grad = tf.gradients(eq, opt_beta)[0]
        opt_beta = tf.subtract(opt_beta, tf.divide(eq, grad))

    print()

    print(sess.run(opt_beta, feed_dict={current_v: [10.0, 5.0, 2.0]}))
    print(sess.run(beta))
    print(sess.run(eq, feed_dict={current_v: [10.0, 5.0, 2.0]}))