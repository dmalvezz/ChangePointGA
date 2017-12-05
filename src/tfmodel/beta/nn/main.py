import tensorflow as tf
import numpy as np

data_size = 21600
batch_size = 1000
batch_count = int(data_size / batch_size)

variable_count = 2
node_count = 4
epoch_count = 1000

learning_rate = 0.035
decay_step = 1000
decay_rate = 0.9

beta = 0.01

global_step = tf.Variable(0, trainable=False)
x = tf.placeholder('float', [batch_size, variable_count])
y = tf.placeholder('float')

bias_l1 = tf.Variable(tf.random_normal([1, node_count]), dtype=tf.float32)
weigth_l1 = tf.Variable(tf.random_normal([variable_count, node_count]), dtype=tf.float32)

bias_l2 = tf.Variable(tf.random_normal([1, node_count]), dtype=tf.float32)
weigth_l2 = tf.Variable(tf.random_normal([node_count, node_count]), dtype=tf.float32)

bias_l3 = tf.Variable(tf.random_normal([1]), dtype=tf.float32)
weigth_l3 = tf.Variable(tf.random_normal([node_count, 1]), dtype=tf.float32)


def create_model():    
    l1 = tf.nn.sigmoid(tf.add(tf.matmul(x, weigth_l1), bias_l1))
    l2 = tf.nn.sigmoid(tf.add(tf.matmul(l1, weigth_l2), bias_l2))
    return tf.add(tf.matmul(l2, weigth_l3), bias_l3)


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
regularizer = tf.nn.l2_loss(weigth_l1) + tf.nn.l2_loss(bias_l1)
regularizer = regularizer + tf.nn.l2_loss(weigth_l2) + tf.nn.l2_loss(bias_l2)
regularizer = regularizer + tf.nn.l2_loss(weigth_l3) + tf.nn.l2_loss(bias_l3)

loss = tf.pow(tf.subtract(y, model), 2.0)
loss = tf.reduce_mean(loss + beta * regularizer)

#learning_rate = tf.train.exponential_decay(learning_rate, global_step, decay_step, decay_rate)
learning_step = tf.train.AdamOptimizer(learning_rate).minimize(loss, global_step=global_step)


with tf.Session() as sess:
    writer = tf.summary.FileWriter("logs", sess.graph)
    tf.summary.scalar("loss", loss)
    tf.summary.scalar("learning_rate", learning_rate)
    summary_op = tf.summary.merge_all()

    sess.run(tf.global_variables_initializer())
    
    for epoch in range(0, epoch_count):
        err = 0
        for batch_index in range(0, batch_count):
            x_value, y_value = load_train_data("exp1/x.txt", "exp1/beta1000.txt", batch_index)
            _, err, summary = sess.run([learning_step, loss, summary_op], feed_dict={x: x_value, y: y_value}) 
            
            writer.add_summary(summary, epoch * batch_count + batch_index)
            
        print("Epoch", epoch, "  loss", err)


    err = 0
    for batch_index in range(0, batch_count):
        x_value, y_value = load_train_data("exp1/x.txt", "exp1/beta1000.txt", batch_index)
        de = sess.run(loss, feed_dict={x: x_value, y: y_value}) 
        err += de

    print("Total error", err / int(data_size / batch_size))

    #print(sess.run(weigth))

