from pysword.modules import SwordModules
import sqlite3, json, sys

# define books and chapters
filename = 'canon.json'
with open(filename, 'r') as jsonfile:
    canon = json.load(jsonfile)

books = canon['Old Testament'] | canon['Apocrypha'] | canon['New Testament']

modules = SwordModules(sys.argv[1])
version = sys.argv[1][sys.argv[1].rfind('/')+1:sys.argv[1].find('.zip')]

found_modules = modules.parse_modules()
bible = modules.get_bible_from_module(version)

db = sqlite3.connect('../src/db/bible.db')
cursor = db.cursor()

version_name = ""
try:
    version_name = found_modules[version]['description']
except KeyError:
    pass

strongs = False
try:
    if 'StrongsNumbers' in found_modules[version]['feature']:
        strongs = True
except KeyError:
    pass

# Add version to db
cursor.execute( 'INSERT INTO versions(name, shorthand, strongs, apocrypha) VALUES (?,?,?)', (version_name, version, strongs, 0) )

has_apoc = False

for book, values in books.items():
    print(f'Starting {book} ({version})')

    chapters = values['chapters']
    shorthand = values['shorthand']
    swordbook = values['pysword_name']

    for c in range(chapters):
        c+=1 # Adjust to index 1
        print(f' ⊦ Chapter {c}')

        cleanverses = bible.get_iter(books=[swordbook], chapters=[c], clean=True)
        verses = bible.get_iter(books=[swordbook], chapters=[c], clean=False)

        # Verse plaintext -- wrapped in try so it doesn't crap itself when going over apocrypha
        try:
            for vn, v in enumerate(cleanverses):
                vn+=1 # Adjust to index 1
                cursor.execute(
                    'INSERT INTO bible_text(version, book, chapter, verse, text) VALUES (?,?,?,?,?)',
                    (version,
                    shorthand,
                    c,
                    vn,
                    v)
                    )
            if (not has_apoc) and book in canon['Apocrypha']:
                has_apoc = True
        except ValueError:
            print(f'{book} {c} ({version}) does not exist.')
            break

        # Strongs mappings
        for vn, v in enumerate(verses):
            vn+=1 # Adjust to index 1

            wordcount = 0
            strongsinfo = {}
            # Split phrases for strongs in verses
            while(v.find('<w') != -1):
                sub = v[ v.find('<w') : v.find('</w') ]

                # Identify ALL strongs numbers (there may be multiple)
                strongs = []
                while(sub.find('strong:') != -1):
                    start = sub.find('strong:') + 7
                    end = start + 5
                    failedStrong = False # Sometimes the text looks like "strong:G", with an incomplete strong...
                    # Make sure Strongs number doesn't extend past its length
                    while(sub[end] not in ['0','1','2','3','4','5','6','7','8','9']):
                        end -= 1
                        if (start == end):
                            failedStrong = True
                            break
                    if not failedStrong:
                        strongs.append(sub[ start : end + 1])

                    sub = sub[ end : ]

                # Get pure phrase
                phrase = sub[ sub.find('>')+1 : ]
                wordcount += len(phrase.split())

                strongsinfo[wordcount] = strongs

                v = v[ v.find('</w')+4 : ]

            cursor.execute('SELECT id FROM bible_text WHERE verse=?', (vn,))
            verse_id = cursor.fetchone()[0]
            for key in strongsinfo:
                for strong in strongsinfo[key]:
                    cursor.execute(
                        'INSERT INTO strongs_mapping(verse, strong, word_position) VALUES (?,?,?)',
                        (verse_id,
                        strong,
                        key,
                        )
                        )
    db.commit()

if has_apoc:
    cursor.execute( 'UPDATE versions SET apocrypha=? WHERE shorthand=?', (has_apoc,version) )

db.commit()
cursor.close()