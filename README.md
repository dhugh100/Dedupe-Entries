# Dedup

## Description
Dedup uses SHA256 hashes to identify and take action on duplicate files. 

## Installation
### RPM
- Download the RPM file.  Use your distributions package manager for RPMs to install. In fedora 41:

    >  `sudo dnf install your-rpm-location` 

### Compile and link
- Setup the GTK 4 development environment.
- Make sure openssl v3 or greater is installed.
- Download the source and header files.
- Compile and link.

  >  ``gcc `pkg-config --cflags gtk4` -o ddup  main.c get_folders.c load_entry_data.c traverse.c get_hash.c get_results.c show_columns.c install_property.c work_selected.c see_entry_data.c view_file.c sort_columns.c filter_columns.c work_trash.c work_options.c -lcrypto `pkg-config --libs gtk4` ``

## Usage
### Simple Flow Example
- Start the application with the icon or from the command line.
- Click the get button to select a directory. You can select multiple directories.
- Dedup will traverse the directory(s) selected, hashing the regular files it finds, and then present the results in the left column. The right column has the name. Possible result values:
  - Empty: The file is empty.
  - Error [...]: There was an error accessing the directory entry.
  - 7 Digit Number: The group number - all files with the same group number are duplicates, e.g., have the same SHA256 hash.
  - Unique: No other file in the selected folder(s) has the same hash.
- Select the presented entries for an action with a CTRL + Left click or Shift + Left click on the row. Multiple entries can be selected.
- Right-click to bring up the action screen. The action screen choices varies depending on whether just one or more than one entries were selected. The actions are:
  - Trash: Move the file to the trash. Available if single or multiple entries selected. 
  - View: View the file in ASCII Dump format. Available if a single file is selected.
  - Launch: Pass a selected file name to an application. Available if a single entry is selected.
  - Copy to Clipboard: Copies the name of the entries to the clipboard. Available if a a single or multiple entries are selected.


## Contributing
If you would like to contribute, please fork the repository and use a feature branch. Pull requests are welcome.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
# ddup
