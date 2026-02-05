#!/usr/bin/env python3
"""
Sync function declarations in a C99 single-header library.

Reads fx.h, finds function definitions (prefixed with `fn`) in the
FX_IMPLEMENTATION section, and adds missing declarations to the
// DECLARATION section.
"""

import re
import sys
from pathlib import Path


def parse_function_signature(line: str) -> str | None:
    """
    Extract function signature from a definition line starting with `fn`.
    Returns the declaration (signature + semicolon) or None if not a function.
    """
    # Match: fn <return_type> <name>(<params>) {
    # Also handles: fn <return_type>* <name>(...) for pointer returns
    pattern = r"^fn\s+(.+?)\s*\{?\s*$"
    match = re.match(pattern, line.strip())

    if not match:
        return None

    signature = match.group(1).strip()

    # Remove trailing '{' if present
    signature = signature.rstrip("{").strip()

    # Verify it looks like a function (has parentheses)
    if "(" not in signature or ")" not in signature:
        return None

    return f"fn {signature}" + ";"


def extract_function_name(declaration: str) -> str | None:
    """Extract the function name from a declaration."""
    # Match function name: word immediately before the opening paren
    # Handles pointer returns like: void* arena_alloc(...)
    pattern = r'(\w+)\s*\('
    match = re.search(pattern, declaration)
    return match.group(1) if match else None


def find_declaration_section(lines: list[str]) -> tuple[int, int]:
    """
    Find line `#define fn`, considered to be the start of the declaration section.
    Find line `#ifdef FX_IMPLEMENTATION` , considered to be the start of the definition section
    Returns (start_line, end_line) indices.
    """
    start = None
    end = None
    section_start_pattern = r"^\s*//\s*fn\s+DECLARATION"
    section_end_pattern = r"^\s*//\s*END\s+fn\s+DECLARATION"

    for i, line in enumerate(lines):
        if re.match(section_start_pattern, line.strip()):
            start = i

        if re.match(section_end_pattern, line.strip()):

            end = i - 1
            break

    if start is None or end is None:
        raise ValueError("Could not find the declaration section")
    return start, end


def find_implementation_section(lines: list[str]) -> tuple[int, int]:
    """
    Find the FX_IMPLEMENTATION section.
    Returns (start_line, end_line) indices.
    """
    start = None
    end = None
    section_start_pattern = r"^#ifdef FX_IMPLEMENTATION"
    section_end_pattern = r"^#endif\s*//\s*FX_IMPLEMENTATION"

    for i, line in enumerate(lines):
        if re.match(section_start_pattern, line.strip()):
            start = i
        if re.match(section_end_pattern, line.strip()):
            end = i
            break

    if start is None or end is None:
        raise ValueError("Could not find the end of the FX_IMPLEMENTATION section")

    return start, end


def get_existing_declarations(
    lines: list[str], decl_start: int, decl_end: int
) -> set[str]:
    """Get set of function names already declared."""
    declared = set()

    for i in range(decl_start, decl_end):
        line = lines[i].strip()
        # Skip comments, macros, empty lines
        if not line.startswith("fn"):
            continue

        name = extract_function_name(line)
        if name:
            declared.add(name)
    return declared


def get_function_definitions(
    lines: list[str], impl_start: int, impl_end: int
) -> list[tuple[str, str]]:
    """
    Get all function definitions from implementation section.
    Returns list of (function_name, declaration) tuples.
    """
    definitions = []

    for i in range(impl_start, impl_end):
        line = lines[i]

        # Look for lines starting with 'fn '
        if not line.strip().startswith("fn "):
            continue

        # Handle multi-line function signatures
        full_line = line
        j = i
        while "{" not in full_line and j < impl_end - 1:
            j += 1
            full_line += " " + lines[j].strip()

        if (declaration := parse_function_signature(full_line)) and (
            name := extract_function_name(declaration)
        ):
            definitions.append((name, declaration))

    return definitions


def sync_declarations(filepath: Path) -> tuple[str, list[str]]:
    """
    Sync declarations in the file.
    Returns (updated_content, list_of_added_declarations).
    """
    content = filepath.read_text()
    lines = content.splitlines(keepends=True)

    # Normalize line endings for processing
    lines_stripped = [line.rstrip("\n\r") for line in lines]

    decl_start, decl_end = find_declaration_section(lines_stripped)
    impl_start, impl_end = find_implementation_section(lines_stripped)

    existing = get_existing_declarations(lines_stripped, decl_start, decl_end)
    definitions = get_function_definitions(lines_stripped, impl_start, impl_end)

    # Find missing declarations
    missing = []
    for name, declaration in definitions:
        if name not in existing:
            missing.append(declaration)

    if not missing:
        return content, []

    # Find insertion point (just before the blank line or macro after declarations)
    insert_line = decl_end

    # Look backwards from decl_end to find proper insertion point
    for i in range(decl_end - 1, decl_start, -1):
        line = lines_stripped[i].strip()
        if line and not line.startswith("#") and not line.startswith("//"):
            insert_line = i + 1
            break

    # Build new content
    new_lines = lines_stripped[:insert_line]
    for decl in missing:
        new_lines.append(decl)
    new_lines.extend(lines_stripped[insert_line:])

    return "\n".join(new_lines) + "\n", missing


def main():
    if len(sys.argv) < 2:
        filepath = Path("fx.h")
    else:
        filepath = Path(sys.argv[1])

    if not filepath.exists():
        print(f"Error: {filepath} not found", file=sys.stderr)
        sys.exit(1)

    print(f"Processing: {filepath}")

    updated_content, added = sync_declarations(filepath)

    if not added:
        print("All function definitions already have declarations.")
        return

    print(f"\nAdding {len(added)} missing declaration(s):")
    for decl in added:
        print(f"  {decl}")

    # Write to output location
    output_path = Path(filepath)
    output_path.write_text(updated_content)
    print(f"\nUpdated file written to: {output_path}")


if __name__ == "__main__":
    main()
