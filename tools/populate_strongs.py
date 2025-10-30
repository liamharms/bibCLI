import csv
import sqlite3

# filename = input('CSV with strong\'s: ') # ./resources/strongs.csv
filename = "./resources/strongs.csv"
fields = []
rows = []

with open(filename, 'r') as csvfile:
    csvreader = csv.reader(csvfile)  # Reader object

    fields = next(csvreader)  # Read header
    for row in csvreader:     # Read rows
        rows.append(row)

    print("Total no. of rows: %d" % csvreader.line_num)  # Row count

print('Field names are: ' + ', '.join(fields))

db = sqlite3.connect('../src/db/bible.db')
cursor = db.cursor()

for row in rows:
    print(row[0])
    if (len(row) == 5):
        cursor.execute(
            'INSERT INTO strongs(id, word, definition, derivation) VALUES (?,?,?,?)',
            (row[0],row[1],row[3],row[4])
        )
    else:
      cursor.execute(
            'INSERT INTO strongs(id, word, definition) VALUES (?,?,?)',
            (row[0],row[1],row[3])
        )  

db.commit()
cursor.close()