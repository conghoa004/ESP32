from ultralytics import YOLO

model = YOLO("./Results/22004316.pt")

results = model("./Test-files/Test1.jpg")

for r in results:

    for box in r.boxes:

        class_id = int(box.cls[0])
        confidence = float(box.conf[0])

        name = model.names[class_id]

        print("Detected:", name)
        print("Confidence:", confidence)