# Description

Simple program to dump query results to text files, based on some fork of Tom Kyte 
original [array-flat][kyte-flat] utility. Usually runs 10 times faster than dumping
CSV with hand-crafted query from sql*plus, upto 100 times faster in extreme cases
like unloading large XML CLOBs in UTF8.

[kyte-flat]: https://asktom.oracle.com/pls/asktom/f?p=100:11:0::::P11_QUESTION_ID:459020243348

# Changes and added features
- Makefile to automate build, fixes for current Oracle version (tested on 11.2)
- user password is not printed to stderr
- options for 
    - null replacement string
    - newline replacement char
    - forced cursor sharing
- ablity to change fields separator
- optionally enclosing strings and escaping delimitier inside strings
- reading query from file
- setting dbms_application_info for long-running queries
- ability to work with CLOB fields with configurable maximum size (default 65K)

# Usage
- basic

        ./q2csv userid=user/$pass@server sqlstmt='select * from gestori.katalog where rownum < 10'  
        ./q2csv userid=user/$pass@server sqlfile=query.sql enclosure='"' replace_nl=' ' share=y > t.dat
        ./q2csv userid=user/$pass@server sqlfile=query.sql enclosure='"' encl_esc='"' \
           cli_info='uid="batch", host="here"' \ 
           mod_info='ges SPb, proc="load-table-mail"' \
           act_info='get-prog ora-ges.p'

- options
    - `userid`: credentials in `user/pass` format; pass with env var
    - `sqlstmt`: query to execute (for inline)
    - `sqlfile`: query file name (for saved)
    - `delimiter`: field separator, default "|"
    - `enclosure`: enclosing quotes for string fields, default: none
    - `encl_esc`: escape char for enclosing quotes inside string fields, default: none
    - `replace_null`: null replacement string, default "?"
    - `null_string`: empty (null) string replacement, default "" (empty)
    - `replace_nl`: newline replacement char, default: keep newlines
    - `share`: if not empty, enable forced cursor sharing for session
    - `arraysize`: fetch array size, default 10 records; set to 2-3 with large CLOBs
    - `cli_info`, `mod_info`, `act_info`: client, module and action for `dbms_application_info`
    - `max_clob`: maximum CLOB size, default 65K
- obscure options, specific for our environment
    - `pnull_string`: if string field equals hard-coded PRONULL string (currently "<$null4mail_ora$>"),
      replace its contents with this string (usually "?") to facilitate Progress replication
- to fine-tune date presentation: export env vars like

        export NLS_LANG=AMERICAN_CIS.UTF8
        export NLS_DATE_FORMAT='DD.MM.RR'
        export NLS_TIMESTAMP_FORMAT='YYYY-MM-DD"T"HH24:MI:SS.FF6'
