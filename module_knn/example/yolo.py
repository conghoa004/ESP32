# pip install ultralytics

from ultralytics import YOLO
import cv2

# ================= LOAD MODEL =================
model = YOLO("./Results/22004316.pt")   # model bạn đã train

# ================= LOAD IMAGE =================
image_path = "./Test-files/Test1.jpg"

results = model(image_path)

# ================= SHOW RESULT =================
for r in results:

    frame = r.plot()   # YOLO tự vẽ bounding box

    cv2.imshow("Result", frame)

    cv2.waitKey(0)

cv2.destroyAllWindows()