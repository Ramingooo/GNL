*This project has been created as part of the 42 curriculum by adelkahl.*

# Get Next Line

## Description

Reading a line from a file descriptor is way too tedious. The goal of this project is to implement a C function, `get_next_line(int fd)`, that returns a single line read from a file descriptor on each call. Repeated calls allow reading a text file or standard input one line at a time until End of File (EOF) is reached.

This project focuses on low-level file I/O (`read`), dynamic heap memory management (`malloc` and `free`), and the behavior of `static` variables in C.

### Bonus Part
The bonus implementation extends `get_next_line()` to manage multiple file descriptors simultaneously using only a single `static` variable. You can alternate calls across different open file descriptors (e.g., `fd 3`, `fd 4`, `fd 5`, and back to `fd 3`) without losing track of the reading state or mixing leftover buffers between files.

## Algorithm Justification

The implementation uses a modular, three-stage pipeline backed by a persistent `static` buffer to preserve unread bytes between function calls while reading as little data as possible:

1. **Reading and Buffering (`ft_read_to_backup`):**
   - Allocates a temporary buffer of size `BUFFER_SIZE + 1`.
   - Loops `read(fd, buf, BUFFER_SIZE)` and appends the null-terminated chunk to the static `backup` using a custom `ft_strjoin` (split with a static helper `ft_fill_join` to adhere to the 42 Norm).
   - Stops immediately as soon as a newline character (`\n`) is detected in `backup` (checked via `ft_strchr`) or when `read()` returns `0` (EOF). This avoids reading the entire file into memory at once.
   - If `read()` returns `-1` (such as an invalid descriptor or read error), both the temporary buffer and the static `backup` are freed immediately before returning `NULL`.

2. **Line Extraction (`ft_extract_line`):**
   - Verifies that `backup` is neither `NULL` nor an empty string (`!backup || !backup[0]`).
   - Measures the length from index `0` up to and including the first `\n` (or up to `\0` at EOF).
   - Allocates and returns a newly null-terminated string containing only that exact line.

3. **Leftover Preservation (`ft_update_backup`):**
   - Locates the first `\n` in `backup`. If none exists (EOF reached), it frees `backup` and returns `NULL` to prevent dangling pointers and leaks.
   - Otherwise, it uses pointer arithmetic (`backup + i + 1`) with `ft_strdup` to duplicate the remaining characters after the newline into a new heap allocation, frees the old `backup`, and updates the static pointer.

### Memory & Bonus Design Choices
- **`.bss` Initialization:** Because the `static` pointer(s) are uninitialized at declaration, they are placed in the `.bss` segment and guaranteed to start as `NULL`.
- **Leak-Safe `ft_strjoin`:** On any `malloc` failure inside `ft_strjoin`, the previously allocated `s1` (`backup`) is freed before returning `NULL` so no heap blocks are orphaned.
- **Multi-FD Array (`Bonus`):** Instead of a single `static char *backup;`, the bonus declares a single static array of pointers: `static char *backup[MAX_FD];` (with `MAX_FD` defaulted to `1024`, matching the standard Linux open-file limit). Each file descriptor indexes directly into its own isolated slot (`backup[fd]`). A strict guard (`if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)`) prevents out-of-bounds memory access before touching the array.

## Instructions

### Mandatory Part
Include the mandatory header in your source file:
```c
#include "get_next_line.h"
```

Compile with the mandatory source files (with or without `-D BUFFER_SIZE=n`; defaults to `42`):
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
```

### Bonus Part (Multiple File Descriptors)
Include the bonus header in your source file:
```c
#include "get_next_line_bonus.h"
```

Compile with the bonus source files:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c -o gnl_bonus
```

### Execution
Run the compiled binary against your test files:
```bash
./gnl
./gnl_bonus
```

## Resources

- **Manual Pages:** `man 2 read`, `man 2 open`, `man 2 close`, `man 3 malloc`, `man 3 free`.
- **Reference Material:** Jacob Sorber's C programming lectures on static variables, pointers, and memory leak debugging with Valgrind.
- **Peer & AI Usage:** Peers assisted in reviewing the multi-FD bonus approach, and AI was used strictly as an interactive learning tool to clarify low-level concepts (the `.bss` segment, kernel file offsets, unlinked files, preprocessor `-D` flags, and pointer arithmetic), practice defense questions, and structure this documentation in accordance with the 42 AI guidelines.