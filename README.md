# Dedupe Entries

## Description
Dedupe Entries uses SHA256 hashes to identify and trash duplicate files. 

## Installation

### Compile and link
- Setup the GTK 4 development environment.
- Make sure openssl v3 or greater is installed.
- Download the the c source, logo.xml, and header files.
- Compile in-line logo.xml to a C string.a
  >  `` glib-compile-resources --generate-header logo.xml ``
  >   
  >  `` glib-compile-resources --generate-source logo.xml ``

- Compile C programs and link.
  >  ``gcc `pkg-config --cflags gtk4` -o dedupee lib.c work-auto.c about.c search.c main.c get-folders.c load-store.c traverse.c get-hash.c get-results.c show-columns.c install-property.c work-selected.c view-file.c sort-store.c filter-store.c work-trash.c work-options.c logo.c -lcrypto `pkg-config --libs gtk4` ``

## Usage
### Manual Selection - Flow Example
- Start the application with the icon or from the command line.
- Click the get button to select a directory. You can select multiple directories.
- Dedup will traverse the directory(s) selected, hashing the regular files it finds, and then present the results in the left column. The right column has the name. Possible result values:
  - Empty: The file is empty.
  - Error [...]: There was an error accessing the directory entry.
  - 7 Digit Number: The group number - all files with the same group number are duplicates, e.g., have the same SHA256 hash.
  - Unique: No file in the selected folder(s) shares the same hash.
- Select the presented entries for an action with a CTRL + Left click or Shift + Left click on the row. Multiple entries can be selected.
- Right-click to bring up the action screen. The action screen choices vary depending on whether just one or more than one entries were selected. The actions are:
  - Trash: Move the file to the trash. Available if single or multiple entries selected. 
  - View: View the file in ASCII Dump format. Available if a single file is selected.
  - Launch: Pass a selected file name to an application. Available if a single entry is selected.
  - Copy to Clipboard: Copies the name of the entries to the clipboard. Available if a a single or multiple entries are selected.

### Auto Selection - Flow Example
- Start the application with the icon or from the command line.
- Click the get button to select a directory. You can select multiple directories.
- The program will identify groups of duplicate files.  For each group, one file will be preserved, the others will be trashed.  The file to be preserved is determined by the configuration options.  

### Menu options
- Main Menu
  - Get.  Select the folders to search for duplicates.
  - Sort.  Select the primary and secondary sort order columns in ascending or decending order.  Apply.
  - Filter.  Enter a string to filter columns by result or name.  The 'not' option excludes.  The filters can be combined or not (and / or). Empty filters are always matches. Apply or clear.
  - Search.  Highlight a row that contains the search string.  Proceed to the next row by selecting next at the prompt, or take action with a right click.   
  - Auto.  Automatically trash all but one file in a group of duplicate files.  

  - Hamburger.  
    - Options.  
        -- The choices are saved in ~/.config/.ddup.cfg        
        
        -- Entry Include. Choose which directory entry types to include for processing.     
        
        -- Result Include. Chose which result types to include for processing.

        -- Auto Preserve. Choose which file charactertistic in a group of files duplicates preserves a file.  
        
        -- Auto Prompt. Choose whether or not to prompt for confirmation prior to trashing entries.
    
    - About.  Program information.

## License
This project is licensed under the GPL 3.0  license - see the LICENSE file for details.
# Dedupe Entries
