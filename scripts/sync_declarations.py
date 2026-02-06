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
    pattern = r"(\w+)\s*\("
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
            end = i
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
) -> dict[str, str]:
    """Get dict mapping function names to their full declarations."""
    declared = {}

    for i in range(decl_start, decl_end):
        line = lines[i].strip()
        # Skip comments, macros, empty lines
        if not line.startswith("fn"):
            continue

        name = extract_function_name(line)
        full_line = handle_multiline_definition(lines, i, decl_end, sentinel=";")
        if name:
            declared[name] = full_line
    return declared


def handle_multiline_definition(
    lines: list[str], start: int, end: int, sentinel: str
) -> str:
    full_line = lines[start]
    j = start
    while sentinel not in full_line and j < end - 1:
        j += 1
        full_line += " " + lines[j].strip()
    full_line = full_line.split(sentinel, maxsplit=1)[0] + sentinel
    return full_line


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
        full_line = handle_multiline_definition(lines, i, impl_end, "{")
        if (declaration := parse_function_signature(full_line)) and (
            name := extract_function_name(declaration)
        ):
            definitions.append((name, declaration))

    return definitions


def extract_return_type(declaration: str) -> str:
    """Extract the return type from a declaration."""
    pattern = r"^fn\s+(.+?)\s*\w+\s*\("
    match = re.match(pattern, declaration.rstrip(";"))
    return match.group(1).strip() if match else "void"


def generate_stub_implementation(name, declaration: str) -> list[str]:
    """
    Generate a stub implementation for a declaration.
    Returns list of lines for the stub.
    """
    signature = declaration.rstrip(";").strip()
    func_name = name
    return_type = extract_return_type(declaration)

    lines = [
        "",
        "// TODO: not implemented",
        f"fn {signature} {{",
        f'    assert(0 && "{func_name} not implemented");',
    ]

    # Add return statement based on return type
    if return_type == "void":
        pass
    elif "*" in return_type:
        lines.append("    return NULL;")
    elif return_type in ("b8", "b16", "b32", "b64", "bool"):
        lines.append("    return false;")
    elif return_type in (
        "u8",
        "u16",
        "u32",
        "u64",
        "i8",
        "i16",
        "i32",
        "i64",
        "int",
        "unsigned",
        "long",
        "size_t",
        "usize",
        "f32",
        "f64",
    ):
        lines.append("    return 0;")
    else:
        # For structs or unknown types, return zeroed value
        lines.append(f"    return ({return_type}){{0}};")

    lines.append("}")
    lines.append("")
    return lines


def sync_declarations(filepath: Path) -> tuple[str, list[str], list[str]]:
    """
    Sync declarations in the file.
    Returns tuple (updated_content, list_of_added_declarations, list_of_added_impl_stubs).
    """
    content = filepath.read_text()
    lines = content.splitlines(keepends=True)

    # Normalize line endings for processing
    lines_stripped = [line.rstrip("\n\r") for line in lines]

    decl_start, decl_end = find_declaration_section(lines_stripped)
    impl_start, impl_end = find_implementation_section(lines_stripped)

    existing_decls = get_existing_declarations(lines_stripped, decl_start, decl_end)
    existing_impls = get_function_definitions(lines_stripped, impl_start, impl_end)

    # Direction 1: Find definitions missing declarations
    missing_decls = []
    for name, declaration in existing_impls:
        if name not in existing_decls:
            missing_decls.append(declaration)

    # Direction 2: Find declarations missing implementations
    missing_impls = []
    implemented = set(name for name, _ in existing_impls)
    for name, declaration in existing_decls.items():
        if name not in implemented:
            missing_impls.append((name, declaration))

    if not missing_decls and not missing_impls:
        return content, [], []

    # Start adding lines from the bottom

    # Direction 2: Add missing implementations
    if missing_impls:
        insert_line = impl_end

        new_lines = lines_stripped[:insert_line]
        for name, declaration in missing_impls:
            stub_lines = generate_stub_implementation(name, declaration)
            new_lines.extend(stub_lines)
        new_lines.extend(lines_stripped[insert_line:])
        lines_stripped = new_lines

    # Direction 1: Add missing declarations
    if missing_decls:
        insert_line = decl_end

        # Look backwards from decl_end to find proper insertion point
        for i in range(decl_end - 1, decl_start, -1):
            line = lines_stripped[i].strip()
            if line and not line.startswith("#") and not line.startswith("//"):
                insert_line = i + 1
                break

        # Build new content
        new_lines = lines_stripped[:insert_line]
        for decl in missing_decls:
            new_lines.append(decl)
        new_lines.extend(lines_stripped[insert_line:])
        lines_stripped = new_lines

    return (
        "\n".join(lines_stripped) + "\n",
        missing_decls,
        [declaration for _, declaration in missing_impls],
    )


def main():
    if len(sys.argv) < 2:
        filepath = Path("fx.h")
    else:
        filepath = Path(sys.argv[1])

    if not filepath.exists():
        print(f"Error: {filepath} not found", file=sys.stderr)
        sys.exit(1)

    print(f"Processing: {filepath}")

    updated_content, added_decls, added_impls = sync_declarations(filepath)

    if not added_decls and not added_impls:
        print("All function definitions already have declarations.")
        return

    if added_decls:
        print(f"\nAdded {len(added_decls)} missing declaration(s):")
        for decl in added_decls:
            print(f"  {decl}")

    if added_impls:
        print(f"\nAdded {len(added_impls)} missing definitions(s):")
        for decl in added_impls:
            print(f"  {decl}")

    # Write to output location
    output_path = Path(filepath)
    output_path.write_text(updated_content)
    print(f"\nUpdated file written to: {output_path}")


if __name__ == "__main__":
    main()
