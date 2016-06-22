# Description

Simple program to dump query results to text files, based on some fork of Tom Kyte 
original [array-flat][kyte-flat] utility. 

[kyte-flat]: https://asktom.oracle.com/pls/asktom/f?p=100:11:0::::P11_QUESTION_ID:459020243348

# Changes and added features
- Makefile to automate build, fixes for current Oracle version (tested on 11.2)
- user password is not printed to stderr
- options for 
    - null replacement string
    - newline replacement char
    - forced cursor sharing
- ablity to change fields separator and optionally enclose strings
- reading query from file

# Usage
- basic

        ./q2csv userid=user/$pass@server sqlstmt='select * from gestori.katalog where rownum < 10'  
        ./q2csv userid=user/$pass@server sqlfile=query.sql enclosure='"' replace_nl=' ' share=y > t.dat

- options
    - `userid`: credentials in `user/pass` format; pass with env var
    - `sqlstmt`: query to execute (for inline)
    - `sqlfile`: query file name (for saved)
    - `delimiter`: field separator, default "|"
    - `enclosure`: enclosing quotes for string fields, default: none
    - `encl_esc`: escape char for enclosing quotes inside string fields, default: none
    - `null_string`: null replacement string, default "?"
    - `replace_nl`: newline replacement char, default: keep newlines
    - `share`: if not empty, enable forced cursor sharing for session
    - `arraysize`: fetch array size, default 10 records

# Missed features and todo
- bind vars
- date and number format fine-tuning
- setting dbms_application_info
