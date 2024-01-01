#! /usr/bin/env python3
import os
import subprocess
from datetime import datetime

DEFAULT_OUTPUT_DIR = os.getenv('SCAN_DIR')

if DEFAULT_OUTPUT_DIR is None:
    print("Please set the env variable \"DEFAULT_OUTPUT_DIR\" first")
    raise SystemExit()

def initialize_scanner():
    output = subprocess.check_output(["scanimage", "-L"]).decode("utf-8")
    print(output)

def scan_document(output_dir, color_mode, filename):
    now = datetime.now()
    timestamp = now.strftime("%Y-%m-%dT%H-%M-%S")
    if not filename:
        filename = f"scan_{timestamp}.png"

    filename = os.path.join(output_dir, filename)

    if os.path.exists(filename):
        print(f"Error: File {filename} already exists. Aborting scan.")
        return

    command = ["scanimage", "--format=png"]
    if color_mode:
        command.append("--mode=Color")

    with open(filename, "wb") as file:
        subprocess.run(command, stdout=file)

    print(f"Scan saved as {filename}")

def main():
    import argparse
    parser = argparse.ArgumentParser(description="Scanner Automation Script")
    parser.add_argument("-i", "--initialize", action="store_true", help="Initialize the scanner")
    parser.add_argument("-o", "--output-dir", default=DEFAULT_OUTPUT_DIR, help="Output directory for scanned files")
    parser.add_argument("-c", "--color", action="store_true", help="Scan in color")
    parser.add_argument("-f", "--filename", help="Specify the filename")

    args = parser.parse_args()

    if args.initialize:
        initialize_scanner()
        return

    scan_document(args.output_dir, args.color, args.filename)

if __name__ == "__main__":
    main()
