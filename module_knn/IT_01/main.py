# ================= IMPORT THƯ VIỆN =================
import cv2                     # xử lý ảnh
import numpy as np             # xử lý mảng
import pickle                  # load label encoder
import mysql.connector         # kết nối MySQL
import paho.mqtt.publish as publish  # gửi MQTT
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing.image import img_to_array
from datetime import datetime


# ================= MQTT CONFIG =================
# Địa chỉ MQTT Broker
BROKER = "44.232.241.40"

# Topic ESP32 sẽ subscribe
TOPIC = "door/access"


# ================= MODEL PATH =================
# Face detector của OpenCV
PROTO_PATH = "./face_detector/deploy.prototxt.txt"
MODEL_PATH = "./face_detector/res10_300x300_ssd_iter_140000.caffemodel"

# Model AI đã train
MODEL_TRAIN = "./model/22004316.h5"

# Label encoder
PICKLE = "./model/lb.pickle"

# kích thước input của model
IMG_WIDTH = 112
IMG_HEIGHT = 112


# ================= LOAD FACE DETECTOR =================
print("[INFO] Loading Face Detector...")
net = cv2.dnn.readNetFromCaffe(PROTO_PATH, MODEL_PATH)


# ================= LOAD MODEL AI =================
print("[INFO] Loading trained model...")
model = load_model(MODEL_TRAIN)


# ================= LOAD LABEL =================
# lb.classes_ chứa danh sách tên người
lb = pickle.loads(open(PICKLE, "rb").read())


# ================= DATABASE =================
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="iot_face"
)

cursor = db.cursor()


# =========================================================
# HÀM DỰ ĐOÁN ẢNH
# =========================================================
def predict_image(image_path):

    # đọc ảnh
    image = cv2.imread(image_path)

    if image is None:
        print("Không đọc được ảnh!")
        return

    (h, w) = image.shape[:2]


    # ================= DETECT FACE =================
    blob = cv2.dnn.blobFromImage(
        cv2.resize(image, (300, 300)),
        1.0,
        (300, 300),
        (104.0, 177.0, 123.0)
    )

    net.setInput(blob)
    detections = net.forward()


    # duyệt tất cả khuôn mặt
    for i in range(detections.shape[2]):

        confidence = detections[0,0,i,2]

        # chỉ giữ khuôn mặt có độ tin cậy > 0.5
        if confidence > 0.5:

            # lấy bounding box
            box = detections[0,0,i,3:7] * np.array([w,h,w,h])
            (startX,startY,endX,endY) = box.astype("int")

            # cắt khuôn mặt
            face = image[startY:endY,startX:endX]

            if face.shape[0] < 10 or face.shape[1] < 10:
                continue


            # ================= PREPROCESS =================
            face = cv2.resize(face,(IMG_WIDTH,IMG_HEIGHT))
            face = face.astype("float")/255.0
            face = img_to_array(face)
            face = np.expand_dims(face,axis=0)


            # ================= PREDICT =================
            # model có 2 output
            (boxPreds, labelPreds) = model.predict(face, verbose=0)

            i_label = np.argmax(labelPreds, axis=1)[0]

            label = lb.classes_[i_label]
            score = labelPreds[0][i_label]

            print(f"[INFO] {label}: {score*100:.2f}%")


            # ================= CHECK TIME =================
            now = datetime.now()
            hour = now.hour

            if (7 <= hour < 11) or (13 <= hour < 17):


                # ================= CHECK DATABASE =================
                query = "SELECT * FROM users WHERE name=%s"
                cursor.execute(query,(label,))
                result = cursor.fetchone()


                if result:

                    print("Access Granted")

                    # gửi MQTT
                    message = f"OPEN:{label}"
                    publish.single(TOPIC,message,hostname=BROKER)

                else:

                    print("Access Denied")
                    publish.single(TOPIC,"DENIED",hostname=BROKER)


            else:

                print("Outside allowed time")
                publish.single(TOPIC,"TIME_DENIED",hostname=BROKER)


            # ================= HIỂN THỊ KẾT QUẢ =================
            text = f"{label} {score*100:.1f}%"

            cv2.rectangle(image,(startX,startY),(endX,endY),(0,255,0),2)

            cv2.putText(
                image,
                text,
                (startX,startY-10),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.7,
                (0,255,0),
                2
            )


    # hiển thị ảnh kết quả
    cv2.imshow("Result",image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()



# =========================================================
# CHO NGƯỜI DÙNG NHẬP ẢNH TEST
# =========================================================
while True:

    path = input("Nhập đường dẫn ảnh (exit để thoát): ")

    if path.lower() == "exit":
        break

    predict_image(path)