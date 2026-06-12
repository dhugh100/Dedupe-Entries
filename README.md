# Dedupe Entries

## Description

Dedupe Entries is a GTK 4 desktop application for finding and removing duplicate files.
It works by computing SHA256 hashes of files and grouping those that match.
Once duplicates are identified you can inspect them, decide which copies to keep,
and send the rest to the trash -- either by picking them yourself or by letting the
program apply a rule automatically.

## Installation

### Download from GitHub (Preferred)

Pre-built RPM packages are published with each release on the
[GitHub releases page](https://github.com/dhugh100/Dedupee/releases).
Download the `.rpm` for your distribution and install it with:

```bash
sudo dnf install ./dedupee-<version>.fc44.x86_64.rpm
```

The GitHub CI build runs inside a Fedora 44 container using `rpmbuild`.
The compiler invocation in the spec file is:

```
gcc %{optflags} -Wall $(pkg-config --cflags gtk4) -o dedupee <sources> \
    -lcrypto $(pkg-config --libs gtk4)
```

`%{optflags}` expands to the standard RPM optimization flags for the target
distribution (typically `-O2 -flto=auto -ffat-lto-objects -fexceptions
-g -grecord-gcc-switches -pipe -Wall -Werror=format-security ...`).

### Compile and link

You will need GTK 4 development headers and OpenSSL v3 or later.
On Fedora these come from the `gtk4-devel` and `openssl-devel` packages.

The application icon is stored as an XML resource file and must be compiled into
C source before the main build:

```bash
glib-compile-resources --generate-header logo.xml
glib-compile-resources --generate-source logo.xml
```

Then compile and link everything together:

```bash
gcc $(pkg-config --cflags gtk4) -Wall -Werror -o dedupee \
    lib.c work-auto.c about.c search.c main.c get-folders.c load-store.c \
    traverse.c get-hash.c get-results.c show-columns.c install-property.c \
    work-selected.c view-file.c sort-store.c filter-store.c work-trash.c \
    work-options.c logo.c -lcrypto $(pkg-config --libs gtk4)
```

The provided `doBuild.sh` script wraps this and prompts for the version number
to stamp into the binary.  It resolves `%{optflags}` at build time via
`rpm --eval '%{optflags}'` and adds `-Wall -Werror`:

```bash
gcc $(rpm --eval '%{optflags}') $(pkg-config --cflags gtk4) -Wall -Werror \
    -o dedupee <sources> -lcrypto $(pkg-config --libs gtk4)
```

Run it interactively:

```bash
./doBuild.sh
```

## Usage

### Manual Selection

Click **Get** to open a folder chooser. You can add as many directories as you like
and the scan will cover all of them together, so duplicates that span different
folders are caught the same way as duplicates within a single one.

After the scan completes, results appear in a two-column list. The left column
shows what Dedupe Entries found out about each entry:

- **Empty** -- the file has zero bytes.
- **Error [...]** -- the entry could not be read; the bracket contains the reason.
- **A 7-digit group number** -- all entries sharing the same number are exact
  duplicates (identical SHA256 hash). This is the core result you are looking for.
- **Unique** -- no other scanned file shares this file's hash.
- **Directory** -- the entry is a subdirectory, shown for context when directory
  display is enabled.

Symbolic links are never followed, so the scan only touches the files that
are actually present in the selected folders.

To act on entries, select them with Ctrl+click or Shift+click and then
right-click to bring up the action menu:

- **Trash** -- move the selected file(s) to the system trash. Works for one or more entries.
- **View** -- show the file's contents as a hex dump. Available for a single file.
- **Launch** -- open the file with its associated application. Available for a single entry.
- **Copy to Clipboard** -- put the full path(s) into the clipboard.

### Auto Selection

Click **Auto** instead of **Get** if you want Dedupe Entries to handle everything
without manual picking. It will scan the chosen directories, group the duplicates,
and for each group trash all but one file. Which file survives is controlled by
the **Auto Preserve** option (see Options below). A confirmation prompt can be
shown before anything is deleted, if you want a last look before committing.

### Menu Options

**Sort** lets you choose a primary and secondary sort column, each in ascending
or descending order, so you can arrange the list in whatever way makes it easiest
to review.

**Filter** narrows the list to entries whose result or name matches a string.
The "not" checkbox inverts the match, and multiple filters can be layered with
and/or logic. An empty filter matches everything, so clearing a filter restores
the full view.

**Search** scrolls to the next row containing a given string and highlights it.
From the search prompt you can move to the next match or right-click the
highlighted row to act on it directly.

The **hamburger menu** gives access to:

- **Options** -- persistent configuration saved to `~/.config/dedupe-entries.cfg`.

  *Entry Include* controls which kinds of entries appear in the list:
  hidden files (dot-files), subdirectories, and empty files can each be
  shown or hidden independently.

  *Result Include* lets you restrict the list to duplicates only, unique
  files only, or both.

  *Auto Preserve* determines which file survives when auto mode trashes
  a duplicate group:
  - **First Modified** -- keep the oldest file by modification time.
  - **Last Modified** -- keep the newest file by modification time.
  - **Shortest Name** -- keep the file whose full path is shortest.
  - **Longest Name** -- keep the file whose full path is longest.
  - **First Name Ascending** -- keep the alphabetically first path.
  - **First Name Descending** -- keep the alphabetically last path.

  *Trash Confirmation Prompt* can be set separately for manual and auto
  selection, so you can, for example, require confirmation for manual
  trashing while letting auto mode run silently.

- **About** -- version and license information.

## Author's Note

The early versions of this program were written to explore the Gnome GUI environment
and to wrap a bash deduplicate script.  Although initial code was all
hand-written, as AI output quality has increased, so has my use of AI.  Most recently,
AI (notably Claude) has been used to generate code, find bugs, document and package.
The efficiency of using AI is too high to ignore.

## License

This project is licensed under the GPL-3.0 license -- see the LICENSE file for details.
