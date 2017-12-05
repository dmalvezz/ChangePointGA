import tensorflow as tf
import numpy as np

data_size = 21600
batch_size = 1000
batch_count = int(data_size / batch_size)
variable_count = 5
epoch_count = 1000

learning_rate = 1.5
k = 0.7
beta = 0.01

global_step = tf.Variable(0, trainable=False)
x = tf.placeholder('float', [batch_size, variable_count])
y = tf.placeholder('float')

bias = tf.get_variable("bias", [1], dtype=tf.float32)
weigth = tf.get_variable("weigth", [variable_count, 1], dtype=tf.float32)

def create_model():    
    return tf.add(bias, tf.matmul(x, weigth))

def load_train_data(x_file, y_file, batch_index):

    features = np.genfromtxt(
        x_file, 
        delimiter=",", 
        skip_header=batch_index * batch_size, 
        dtype=float, 
        max_rows=batch_size,
        usecols = range(0, variable_count)
        )

    beta = np.genfromtxt(
        y_file, 
        delimiter=",", 
        skip_header=batch_index * batch_size, 
        dtype=float, 
        max_rows=batch_size
        )

    return features, beta

model = create_model()
regularizer = tf.nn.l2_loss(weigth)

loss = tf.reduce_mean(tf.pow(tf.subtract(y, model), 2.0))
loss = tf.reduce_mean(loss + beta * regularizer)

learning_rate = tf.train.inverse_time_decay(learning_rate, global_step, 25, k)
learning_step = tf.train.AdagradOptimizer(learning_rate).minimize(loss, global_step=global_step)


with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    
    for i in range(0, epoch_count):
        for batch_index in range(0, batch_count):
            x_value, y_value = load_train_data("exp4/x.txt", "exp4/beta.txt", batch_index)
            _, err, lr = sess.run([learning_step, loss, learning_rate], feed_dict={x: x_value, y: y_value}) 
        
        print("Epoch", i, "  loss", err, "   lr", lr)


    err = 0
    for i in range(0, int(data_size / batch_size)):
        x_value, y_value = load_train_data("exp4/x.txt", "exp4/beta.txt", i)
        de = sess.run(loss, feed_dict={x: x_value, y: y_value}) 
        err += de

    print("Total error", err / int(data_size / batch_size))

    print("bias", sess.run(bias))
    print("weigth", sess.run(weigth))

