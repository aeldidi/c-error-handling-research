import sqlite3
import pathlib
import tomllib

ROOT = pathlib.Path(__file__).parent
EXP_ROOT = ROOT / "experiments"
SCHEMA_FILE = ROOT / "schema.sql"


def main():
    out_dir = ROOT / "out"
    out_dir.mkdir(exist_ok=True)

    final_db = out_dir / "results.db"
    if final_db.exists():
        final_db.unlink()

    conn = sqlite3.connect(final_db)
    cur = conn.cursor()
    cur.executescript(SCHEMA_FILE.read_text())

    for exp_dir in sorted(EXP_ROOT.iterdir()):
        if not exp_dir.is_dir():
            continue

        exp_name = exp_dir.name
        src_db = out_dir / f"{exp_name}.db"

        print("Merging", src_db)

        cur.execute(f"ATTACH DATABASE '{src_db}' AS src")
        cur.execute(
            """
            INSERT INTO calls(
                dataset, name, filename, line, column,
                handling_type, assigned_filename,
                assigned_line, assigned_column
            )
            SELECT
                dataset, name, filename, line, column,
                handling_type, assigned_filename,
                assigned_line, assigned_column
            FROM src.calls
        """
        )
        cur.execute("DETACH DATABASE src")

    conn.commit()
    conn.close()


if __name__ == "__main__":
    main()
