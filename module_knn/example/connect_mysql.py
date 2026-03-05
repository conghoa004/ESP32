# pip install mysql-connector-python

import mysql.connector

# ================= CONNECT DATABASE =================
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="iot_face"
)

cursor = db.cursor()

print("Connected to database")

# Lấy toàn bộ dữ liệu (SELECT)
query = "SELECT * FROM users"

cursor.execute(query)

results = cursor.fetchall()

for row in results:
    name = row[0] # Lấy giá trị của 1 trường
    print(name)

# Thêm dữ liệu (INSERT)
sql = """
INSERT INTO users (name, role)
VALUES (%s, %s)
"""

data = ("No Parking", "admin")

cursor.execute(sql, data)

db.commit()

print("Insert successful")  