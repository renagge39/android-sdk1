import tensorflow as tf
import numpy as np
from sklearn.model_selection import train_test_split

X = np.array([[40, 200], [20, 50], [35, 180], [15, 30]])  
y = np.array([1, 0, 1, 0])  

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)

model = tf.keras.Sequential([
    tf.keras.layers.Dense(32, activation='relu'),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid')
])

model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
model.fit(X_train, y_train, epochs=100, batch_size=1)

converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

with open("plant_model.tflite", "wb") as f:
    f.write(tflite_model)
