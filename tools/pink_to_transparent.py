#!/usr/bin/env python3
"""Convert magenta (255, 0, 255) background pixels to transparent and save as PNG."""

import argparse
import sys
from pathlib import Path

from PIL import Image


def convert_pink_to_transparent(input_path, output_path, color=(255, 0, 255)):
    """Replace all pixels matching the given color with transparent pixels."""
    img = Image.open(input_path).convert("RGBA")
    data = img.getdata()

    new_data = []
    for pixel in data:
        if pixel[:3] == color:
            new_data.append((0, 0, 0, 0))
        else:
            new_data.append(pixel)

    img.putdata(new_data)
    img.save(output_path, "PNG")


def main():
    parser = argparse.ArgumentParser(
        description="Convert magenta (255,0,255) background to transparency in BMP/PNG images."
    )
    parser.add_argument("input", nargs="+", help="Input image file(s) or directory")
    parser.add_argument("-o", "--output-dir", help="Output directory (default: same as input)")
    parser.add_argument(
        "-c", "--color", default="255,0,255",
        help="Background color to make transparent as R,G,B (default: 255,0,255)"
    )
    parser.add_argument("-r", "--recursive", action="store_true", help="Process directories recursively")

    args = parser.parse_args()

    color = tuple(int(c) for c in args.color.split(","))
    if len(color) != 3:
        print("Error: color must be R,G,B", file=sys.stderr)
        sys.exit(1)

    files = []
    for path_str in args.input:
        path = Path(path_str)
        if path.is_dir():
            pattern = "**/*" if args.recursive else "*"
            files.extend(p for p in path.glob(pattern) if p.suffix.lower() in (".bmp", ".png", ".jpg"))
        elif path.is_file():
            files.append(path)
        else:
            print(f"Warning: '{path}' not found, skipping", file=sys.stderr)

    if not files:
        print("No image files found.", file=sys.stderr)
        sys.exit(1)

    output_dir = Path(args.output_dir) if args.output_dir else None
    if output_dir:
        output_dir.mkdir(parents=True, exist_ok=True)

    for f in files:
        if output_dir:
            out = output_dir / f.with_suffix(".png").name
        else:
            out = f.with_suffix(".png")

        convert_pink_to_transparent(f, out, color)
        print(f"{f} -> {out}")


if __name__ == "__main__":
    main()
