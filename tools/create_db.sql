CREATE TABLE books (
    shorthand       TEXT PRIMARY KEY,
    title           TEXT NOT NULL,
    testament       TEXT
);

CREATE TABLE book_aliases (
    id              INTEGER PRIMARY KEY,
    alias           TEXT NOT NULL,
    book_id         INTEGER NOT NULL REFERENCES books(id)
);

CREATE TABLE versions (
    shorthand       TEXT PRIMARY KEY,
    name            TEXT NOT NULL,
    strongs         INT, -- Supports strongs. SQLite doesn't have bools ):
    apocrypha       INT  -- Same thing
);

CREATE TABLE bible_text (
    id              INTEGER PRIMARY KEY,
    version         TEXT NOT NULL REFERENCES versions(shorthand),
    book            TEXT NOT NULL REFERENCES books(shorthand),
    chapter         INTEGER NOT NULL,
    verse           INTEGER NOT NULL,
    text            TEXT NOT NULL
);

CREATE TABLE strongs (
    id              TEXT PRIMARY KEY,
    word            TEXT NOT NULL,
    transliteration TEXT,
    definition      TEXT,
    derivation      TEXT
);

CREATE TABLE strongs_mapping (
    id              INTEGER PRIMARY KEY,
    verse           INTEGER NOT NULL REFERENCES bible_text(id),
    strong          TEXT NOT NULL REFERENCES strongs(id),
    word_position   INTEGER NOT NULL
);

CREATE TABLE commentaries (
    id              INTEGER PRIMARY KEY,
    author          TEXT NOT NULL,
    title           TEXT NOT NULL
);

CREATE TABLE commentary_text (
    id              INTEGER PRIMARY KEY,
    commentary      INTEGER NOT NULL REFERENCES commentaries(id),
    verse           INTEGER NOT NULL REFERENCES bible_text(id),
    text            TEXT NOT NULL
);