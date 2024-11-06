```markdown
# Dedup

## Description
Dedup uses SHA256 hashes to identify and take action on duplicate files. 

## Installation
Provide instructions on how to install and set up the project, e.g.:

## Usage
### Simple Flow Example
- Start the application with the icon or from the command line.
- Click the get button to select a directory. Dedup will traverse the directory, hashing the regular files it finds, and then present the results in the left column. The right column has the name. Possible result values:
  - Empty: The file is empty.
  - Error [...]: There was an error accessing the file.
  - 7 Digit Number: The group number - all files with the same group number are duplicates, i.e., have the same SHA256 hash.
  - Unique: No other file in the selected folder(s) has the same hash.
- Select the presented files for an action with a CTRL + Left click or Shift + Left click on the row. Multiple files can be selected.
- Right-click to bring up the action screen. The screen varies depending on whether just one or more than one entries were selected. The actions are:
  - Trash: Move the file to the trash. Option presented when either a single or multiple entries are selected.
  - View: View the file in ASCII Dump format. Option presented when a single file is selected.
  - Launch: Pass a selected file name to an application. Option presented when a single file is selected.
  - Copy to Clipboard: Copies the name of the files to the clipboard. Option is presented when a single or multiple files are selected.

## Contributing
If you would like to contribute, please fork the repository and use a feature branch. Pull requests are welcome.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
```
