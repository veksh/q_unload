export LD_LIBRARY_PATH = /apps/oracle/base/software/11.2.0.4/lib/

q2csv: q2csv.pro-c.c
	proc iname=q2csv.pro-c.c oname=q2csv.c
	gcc q2csv.c -L ${ORACLE_HOME}/lib -l clntsh -o q2csv
