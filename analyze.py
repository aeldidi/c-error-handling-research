import os
import sys
import subprocess
import pathlib
import tomllib

ROOT = pathlib.Path(__file__).parent


def main():
    if len(sys.argv) != 2:
        print("Usage: analyze.py <experiment-dir>")
        sys.exit(1)

    exp_dir = pathlib.Path(sys.argv[1])
    exp_name = exp_dir.name
    exp = tomllib.loads((exp_dir / "experiment.toml").read_text())

    dataset_root = os.environ["DATASET_ROOT"]
    src = os.path.join(dataset_root, exp["dataset"])

    notable = exp_dir / "notable.json"
    benign = exp_dir / "benign.json"

    out_dir = ROOT / "out"
    out_dir.mkdir(exist_ok=True)

    db_path = out_dir / f"{exp_name}.db"

    cmd = [
        "./build/analyze-error-handling",
        src,
        "--notable",
        str(notable),
        "--benign",
        str(benign),
        "--out-db",
        str(db_path),
        *exp["flags"],
    ]

    print("Analyzing", exp_name)
    subprocess.run(cmd, check=True)


if __name__ == "__main__":
    main()
