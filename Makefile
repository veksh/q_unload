ifndef ORACLE_HOME
export ORACLE_HOME = /usr/lib/oracle/11.2/client64/
export PC_CONF = ${ORACLE_HOME}/lib/precomp/admin/pcscfg.cfg
else
export PC_CONF = ${ORACLE_HOME}/precomp/admin/pcscfg.cfg
endif
export LD_LIBRARY_PATH = ${ORACLE_HOME}/lib/

q2csv: q2csv.pro-c.c
	proc iname=q2csv.pro-c.c oname=q2csv.c conf=${PC_CONF}
	gcc q2csv.c -L ${ORACLE_HOME}/lib -l clntsh -o q2csv
