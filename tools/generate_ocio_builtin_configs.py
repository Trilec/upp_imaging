#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import re
from pathlib import Path


CG_FILES = [
    ("cg-config-v1.0.0_aces-v1.3_ocio-v2.1", "cg-config-v1.0.0_aces-v1.3_ocio-v2.1.ocio"),
    ("cg-config-v2.1.0_aces-v1.3_ocio-v2.3", "cg-config-v2.1.0_aces-v1.3_ocio-v2.3.ocio"),
    ("cg-config-v2.2.0_aces-v1.3_ocio-v2.4", "cg-config-v2.2.0_aces-v1.3_ocio-v2.4.ocio"),
    ("cg-config-v4.0.0_aces-v2.0_ocio-v2.5", "cg-config-v4.0.0_aces-v2.0_ocio-v2.5.ocio"),
]

STUDIO_FILES = [
    ("studio-config-v1.0.0_aces-v1.3_ocio-v2.1", "studio-config-v1.0.0_aces-v1.3_ocio-v2.1.ocio"),
    ("studio-config-v2.1.0_aces-v1.3_ocio-v2.3", "studio-config-v2.1.0_aces-v1.3_ocio-v2.3.ocio"),
    ("studio-config-v2.2.0_aces-v1.3_ocio-v2.4", "studio-config-v2.2.0_aces-v1.3_ocio-v2.4.ocio"),
    ("studio-config-v4.0.0_aces-v2.0_ocio-v2.5", "studio-config-v4.0.0_aces-v2.0_ocio-v2.5.ocio"),
]


def bytes_to_hex_blob(raw: bytes) -> str:
    values = [f"0x{b:02X}" for b in raw] + ["0x00"]
    chunks = [values[i:i + 16] for i in range(0, len(values), 16)]
    lines = [", ".join(chunk) + ("," if idx < len(chunks) - 1 else "") for idx, chunk in enumerate(chunks)]
    return ",\n     ".join(lines)


def render(template: str, mapping: dict[str, str]) -> str:
    out = template
    for key, value in mapping.items():
        out = out.replace(f"@{key}@", value)
    return out


def validate_generated(path: Path, identifiers: list[str]) -> None:
    text = path.read_text(encoding="utf-8")
    for ident in identifiers:
        match = re.search(rf"constexpr char {re.escape(ident)}\[\] = \{{(.*?)\}};", text, re.S)
        if not match:
            raise SystemExit(f"missing array: {ident} in {path}")
        body = match.group(1).strip()
        if not body.endswith("0x00"):
            raise SystemExit(f"array does not end with 0x00: {ident} in {path}")
    if len(text) < 200:
        raise SystemExit(f"generated file too small: {path}")


def build_output(root: Path) -> tuple[Path, Path]:
    src = root / "opencolorio_src"
    template_dir = src / "upstream" / "src" / "OpenColorIO" / "builtinconfigs"
    config_dir = template_dir / "configs"
    out_dir = src / "generated_include"
    out_dir.mkdir(parents=True, exist_ok=True)

    cg_template = (template_dir / "CG.cpp.in").read_text(encoding="utf-8")
    studio_template = (template_dir / "Studio.cpp.in").read_text(encoding="utf-8")

    cg_map = {}
    for key, filename in CG_FILES:
        cg_map[key] = bytes_to_hex_blob((config_dir / filename).read_bytes())

    studio_map = {}
    for key, filename in STUDIO_FILES:
        studio_map[key] = bytes_to_hex_blob((config_dir / filename).read_bytes())

    cg_out = render(cg_template, cg_map)
    studio_out = render(studio_template, studio_map)

    cg_path = out_dir / "CG.cpp"
    studio_path = out_dir / "Studio.cpp"
    cg_path.write_text(cg_out, encoding="utf-8", newline="\n")
    studio_path.write_text(studio_out, encoding="utf-8", newline="\n")

    validate_generated(cg_path, [key for key, _ in CG_FILES])
    validate_generated(studio_path, [key for key, _ in STUDIO_FILES])

    regenerated_cg = render(cg_template, cg_map)
    regenerated_studio = render(studio_template, studio_map)
    if cg_path.read_text(encoding="utf-8") != regenerated_cg:
        raise SystemExit("CG.cpp is not reproducible")
    if studio_path.read_text(encoding="utf-8") != regenerated_studio:
        raise SystemExit("Studio.cpp is not reproducible")

    return cg_path, studio_path


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate OCIO builtin config blobs")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    args = parser.parse_args()

    cg_path, studio_path = build_output(args.root)
    print(f"wrote {cg_path}")
    print(f"wrote {studio_path}")
    for path in (cg_path, studio_path):
        data = path.read_bytes()
        print(f"{path.name}: size={len(data)} sha256={hashlib.sha256(data).hexdigest().upper()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
