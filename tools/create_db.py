import sqlite3

db = sqlite3.connect('../src/db/bible.db')
cursor = db.cursor()

with open('create_db.sql', 'r') as f:
    sql_script = f.read()

cursor.executescript(sql_script)

db.commit()
db.close()