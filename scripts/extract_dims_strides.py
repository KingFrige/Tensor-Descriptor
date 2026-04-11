#!/usr/bin/env python3
"""Extract dimensions and strides from dump_cmd.json and display as a table.

Recursively searches the entire JSON tree for any dict containing
'dimensions' or 'strides' array fields, regardless of nesting depth.
"""

import json
import sys
from pathlib import Path


def find_dims_strides(obj, path="") -> list[dict]:
    """Recursively find all dicts that have dimensions/strides arrays."""
    results = []

    if isinstance(obj, dict):
        dims = obj.get("dimensions")
        strides = obj.get("strides")

        # Both are arrays -> this is a hit
        if isinstance(dims, list) or isinstance(strides, list):
            # Try to find a name for context
            name = obj.get("name") or obj.get("op") or obj.get("type") or path or "unknown"
            data_type = obj.get("data_type", "-")
            type_size = obj.get("type_size", "-")
            results.append({
                "name": name,
                "dimensions": dims if isinstance(dims, list) else "-",
                "strides": strides if isinstance(strides, list) else "-",
                "data_type": data_type,
                "type_size": type_size,
            })

        # Keep recursing into all values
        for key, val in obj.items():
            child_path = f"{path}.{key}" if path else key
            results.extend(find_dims_strides(val, child_path))

    elif isinstance(obj, list):
        for i, item in enumerate(obj):
            child_path = f"{path}[{i}]"
            results.extend(find_dims_strides(item, child_path))

    return results


def print_table(rows: list[dict]) -> None:
    if not rows:
        print("No records with dimensions/strides found.")
        return

    headers = ["#", "Name", "Dimensions", "Strides", "Data Type", "Type Size"]
    col_widths = [len(h) for h in headers]
    for i, row in enumerate(rows):
        col_widths[0] = max(col_widths[0], len(str(i + 1)))
        col_widths[1] = max(col_widths[1], len(str(row["name"])))
        col_widths[2] = max(col_widths[2], len(str(row["dimensions"])))
        col_widths[3] = max(col_widths[3], len(str(row["strides"])))
        col_widths[4] = max(col_widths[4], len(str(row["data_type"])))
        col_widths[5] = max(col_widths[5], len(str(row["type_size"])))

    fmt = "  ".join(f"{{:<{w}}}" for w in col_widths)
    sep = "-+-".join("-" * w for w in col_widths)

    print(fmt.format(*headers))
    print(sep)
    for i, row in enumerate(rows):
        print(fmt.format(i + 1, str(row["name"]), str(row["dimensions"]), str(row["strides"]), str(row["data_type"]), str(row["type_size"])))

    print(f"\nTotal: {len(rows)} record(s)")


def main():
    json_path = sys.argv[1] if len(sys.argv) > 1 else "../docs/qwen_tensor.json"
    path = Path(json_path)
    if not path.exists():
        print(f"Error: {json_path} not found", file=sys.stderr)
        sys.exit(1)

    with open(path, "r") as f:
        data = json.load(f)

    rows = find_dims_strides(data)
    print_table(rows)


if __name__ == "__main__":
    main()
