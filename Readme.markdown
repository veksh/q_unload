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

# Usage
- basic

        ./q2csv userid=user/$pass sqlstmt='select * from gestori.katalog where rownum < 10'  
        ./q2csv userid=user/$pass sqlstmt="$(cat query.sql)" enclosure='"' replace_nl=' ' share=y > t.dat

- options
    - `userid`: credentials in `user/pass` format; pass with env var
    - `stmt`: query to execute, `"$(cat file.sql)"` to read from file
    - `delimiter`: field separator, default "|"
    - `enclosure`: enclosing quotes for string fields, default: none
    - `null_string`: null replacement string, default "?"
    - `replace_nl`: newline replacement char, default: keep newlines
    - `share`: if not empty, enable forced cursor sharing for session
    - `arraysize`: fetch array size, default 10 records

# Missed features and todo
- bind vars
- date and number format fine-tuning
- reading query from file or stdin
- setting dbms_application_info
