import sqlite3, json

# define books and chapters
filename = 'canon.json'
with open(filename, 'r') as jsonfile:
    canon = json.load(jsonfile)

db = sqlite3.connect('../src/db/bible.db')
cursor = db.cursor()

for testament in canon:
    for book in canon[testament]:
        shorthand = canon[testament][book]['shorthand']
        swordbook = canon[testament][book]['pysword_name']

        cursor.execute('INSERT INTO books(shorthand, title) VALUES (?,?)', (shorthand, book))
        if (book != swordbook):
            cursor.execute('INSERT INTO book_aliases(alias, book_id) VALUES (?,?)', (swordbook,shorthand))

db.commit()
cursor.close()