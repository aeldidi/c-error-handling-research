CREATE TABLE IF NOT EXISTS calls(
    id INTEGER PRIMARY KEY,
    dataset TEXT NOT NULL,
    name TEXT NOT NULL,
    filename TEXT NOT NULL,
    line INTEGER NOT NULL,
    column INTEGER NOT NULL,
    handling_type TEXT NOT NULL,
    assigned_filename TEXT,
    assigned_line INTEGER,
    assigned_column INTEGER
);
