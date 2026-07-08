#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import re
from pathlib import Path


CG_FILES = [
    ("cg-config-v1.0.0_aces-v1.3_ocio-v2.1", "CG_CONFIG_V100_ACES_V13_OCIO_V21", "cg-config-v1.0.0_aces-v1.3_ocio-v2.1.ocio"),
    ("cg-config-v2.1.0_aces-v1.3_ocio-v2.3", "CG_CONFIG_V210_ACES_V13_OCIO_V23", "cg-config-v2.1.0_aces-v1.3_ocio-v2.3.ocio"),
    ("cg-config-v2.2.0_aces-v1.3_ocio-v2.4", "CG_CONFIG_V220_ACES_V13_OCIO_V24", "cg-config-v2.2.0_aces-v1.3_ocio-v2.4.ocio"),
    ("cg-config-v4.0.0_aces-v2.0_ocio-v2.5", "CG_CONFIG_V400_ACES_V20_OCIO_V25", "cg-config-v4.0.0_aces-v2.0_ocio-v2.5.ocio"),
]

STUDIO_FILES = [
    ("studio-config-v1.0.0_aces-v1.3_ocio-v2.1", "STUDIO_CONFIG_V100_ACES_V13_OCIO_V21", "studio-config-v1.0.0_aces-v1.3_ocio-v2.1.ocio"),
    ("studio-config-v2.1.0_aces-v1.3_ocio-v2.3", "STUDIO_CONFIG_V210_ACES_V13_OCIO_V23", "studio-config-v2.1.0_aces-v1.3_ocio-v2.3.ocio"),
    ("studio-config-v2.2.0_aces-v1.3_ocio-v2.4", "STUDIO_CONFIG_V220_ACES_V13_OCIO_V24", "studio-config-v2.2.0_aces-v1.3_ocio-v2.4.ocio"),
    ("studio-config-v4.0.0_aces-v2.0_ocio-v2.5", "STUDIO_CONFIG_V400_ACES_V20_OCIO_V25", "studio-config-v4.0.0_aces-v2.0_ocio-v2.5.ocio"),
]


def bytes_to_hex_blob(raw: bytes) -> str:
    values = [f"0x{b:02X}" for b in raw] + ["0x00"]
    chunks = [values[i:i + 16] for i in range(0, len(values), 16)]
    return ",\n".join(", ".join(chunk) for chunk in chunks)


def render(template: str, mapping: dict[str, str]) -> str:
    out = template
    for key, value in mapping.items():
        out = out.replace(f"@{key}@", value)
    return out


def validate_generated(text: str, path: Path, identifiers: list[str]) -> None:
    for ident in identifiers:
        match = re.search(rf"constexpr char {re.escape(ident)}\[\] = \{{(.*?)\}};", text, re.S)
        if not match:
            raise SystemExit(f"missing array: {ident} in {path}")
        body = match.group(1).strip()
        if not body.endswith("0x00"):
            raise SystemExit(f"array does not end with 0x00: {ident} in {path}")
    if re.search(r"@[^@\n]+@", text):
        raise SystemExit(f"unreplaced template token in {path}")
    if ",," in text:
        raise SystemExit(f"double comma sequence in {path}")
    if len(text) < 200:
        raise SystemExit(f"generated file too small: {path}")


def build_output(root: Path) -> tuple[Path, Path, str, str]:
    src = root / "opencolorio_src"
    template_dir = src / "upstream" / "src" / "OpenColorIO" / "builtinconfigs"
    config_dir = template_dir / "configs"
    out_dir = src / "generated_include"
    out_dir.mkdir(parents=True, exist_ok=True)

    cg_template = (template_dir / "CG.cpp.in").read_text(encoding="utf-8")
    studio_template = (template_dir / "Studio.cpp.in").read_text(encoding="utf-8")

    cg_map = {}
    for placeholder, _, filename in CG_FILES:
        cg_map[placeholder] = bytes_to_hex_blob((config_dir / filename).read_bytes())

    studio_map = {}
    for placeholder, _, filename in STUDIO_FILES:
        studio_map[placeholder] = bytes_to_hex_blob((config_dir / filename).read_bytes())

    cg_out = render(cg_template, cg_map)
    studio_out = render(studio_template, studio_map)

    cg_path = out_dir / "CG.cpp"
    studio_path = out_dir / "Studio.cpp"
    return cg_path, studio_path, cg_out, studio_out


def report(path: Path, text: str) -> None:
    data = text.encode("utf-8")
    print(f"{path.name}: size={len(data)} sha256={hashlib.sha256(data).hexdigest().upper()}")


def write_if_needed(path: Path, text: str, check: bool) -> None:
    if check:
        return
    path.write_text(text, encoding="utf-8", newline="\n")


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate OCIO builtin config blobs")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--check", action="store_true", help="verify checked-in generated files")
    args = parser.parse_args()

    cg_path, studio_path, cg_out, studio_out = build_output(args.root)

    validate_generated(cg_out, cg_path, [identifier for _, identifier, _ in CG_FILES])
    validate_generated(studio_out, studio_path, [identifier for _, identifier, _ in STUDIO_FILES])

    if args.check:
        checked_cg = cg_path.read_text(encoding="utf-8")
        checked_studio = studio_path.read_text(encoding="utf-8")
        if checked_cg != cg_out:
            raise SystemExit("CG.cpp differs from generated output")
        if checked_studio != studio_out:
            raise SystemExit("Studio.cpp differs from generated output")
    else:
        write_if_needed(cg_path, cg_out, False)
        write_if_needed(studio_path, studio_out, False)

    report(cg_path, cg_out)
    report(studio_path, studio_out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
