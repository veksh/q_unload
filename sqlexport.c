
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[13];
};
static struct sqlcxp sqlfpn =
{
    12,
    "sqlexport.pc"
};


static unsigned int sqlctx = 330811;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[4];
   unsigned long  sqhstl[4];
            int   sqhsts[4];
            short *sqindv[4];
            int   sqinds[4];
   unsigned long  sqharm[4];
   unsigned long  *sqharc[4];
   unsigned short  sqadto[4];
   unsigned short  sqtdso[4];
} sqlstm = {12,4};

/* SQLLIB Prototypes */
extern sqlcxt (/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlcx2t(/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlbuft(/*_ void **, char * _*/);
extern sqlgs2t(/*_ void **, char * _*/);
extern sqlorat(/*_ void **, unsigned int *, void * _*/);

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem(/*_ unsigned char *, signed int * _*/);

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4130,871,0,0,
5,0,0,1,0,0,32,106,0,0,0,0,0,1,0,
20,0,0,2,0,0,32,125,0,0,0,0,0,1,0,
35,0,0,3,0,0,17,136,0,0,1,1,0,2,7,68,66,95,78,65,77,69,1,5,0,0,
61,0,0,3,0,0,11,151,0,0,1,1,0,2,7,68,66,95,78,65,77,69,1,32,0,0,
87,0,0,3,0,0,15,183,0,0,0,0,0,2,7,68,66,95,78,65,77,69,
109,0,0,4,0,0,32,186,0,0,0,0,0,1,0,
124,0,0,0,0,0,27,438,0,0,4,4,0,2,7,68,66,95,78,65,77,69,1,9,0,0,1,9,0,0,1,9,0,
0,1,10,0,0,
162,0,0,0,0,0,27,442,0,0,4,4,0,2,7,68,66,95,78,65,77,69,1,9,0,0,1,9,0,0,1,10,0,
0,1,10,0,0,
200,0,0,3,0,0,19,661,0,0,1,1,0,2,7,68,66,95,78,65,77,69,3,32,0,0,
226,0,0,3,0,0,20,753,0,0,1,1,0,2,7,68,66,95,78,65,77,69,3,32,0,0,
252,0,0,3,0,0,14,877,0,0,1,0,0,2,7,68,66,95,78,65,77,69,2,32,0,0,
278,0,0,7,0,0,31,942,0,0,0,0,0,1,0,
};


/*******************************************************************

SQLEXPORT - modified by Stephen Turner, May 1998

This program executes a user-specified SELECT statement against an
Oracle database and places the results in a file. 

You can enter multi-line statements.  The limit is 8192 characters.
This sample program only processes up to MAX_ITEMS bind variables and
MAX_ITEMS select-list items.
*******************************************************************/

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

/* Maximum number of select-list items or bind variables. */
#define MAX_ITEMS         200

/* Maximum lengths of the _names_ of the
   select-list items or indicator variables. */
#define MAX_VNAME_LEN     30
#define MAX_INAME_LEN     30

#define kDefLen            1024
#define kSqlInxBufSize     8192
#define kUserLen           30
#define kPasswdLen         30
#define kHostLen           100

#ifndef NULL
#define NULL  0
#endif

FILE *OUTFILE;
    
void parse_arguments(int, char *[]);
void  parse_logon_argument(char []);

char *dml_commands[] = {"SELECT", "select", "INSERT", "insert",
                        "UPDATE", "update", "DELETE", "delete"};

char *usage_message = "Usage: sqlexport username/password[@hostname] -f queryfile -o outfile -q SQLquery [-s FieldSeparator] [-e EndOfLineCharacter]";

/* EXEC SQL BEGIN DECLARE SECTION; */ 

    char    sql_statement[kSqlInxBufSize];
    /* EXEC SQL VAR sql_statement IS STRING(kSqlInxBufSize); */ 

    /* VARCHAR    db_string[50]; */ 
struct { unsigned short len; unsigned char arr[50]; } db_string;

    /* VARCHAR   username[128]; */ 
struct { unsigned short len; unsigned char arr[128]; } username;

    /* VARCHAR   password[32]; */ 
struct { unsigned short len; unsigned char arr[32]; } password;

/* EXEC SQL END DECLARE SECTION; */ 

 
/* EXEC SQL DECLARE db_name DATABASE; */ 

/* EXEC SQL INCLUDE sqlca;
 */ 
/*
 * $Header: sqlca.h 24-apr-2003.12:50:58 mkandarp Exp $ sqlca.h 
 */

/* Copyright (c) 1985, 2003, Oracle Corporation.  All rights reserved.  */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */
/* EXEC SQL INCLUDE sqlda;
 */ 
/*
 * $Header: sqlda.h 08-may-2007.05:58:33 ardesai Exp $ sqlda.h 
 */

/***************************************************************
*      The SQLDA descriptor definition                         *
*--------------------------------------------------------------*
*      VAX/3B Version                                          *
*                                                              *
* Copyright (c) 1987, 2007, Oracle. All rights reserved.  *
***************************************************************/


/* NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************
*/

/*  MODIFIED
    ardesai    05/08/07  - Bug[6037057] Undef Y
    apopat     05/08/02  - [2362423] MVS PE to make lines shorter than 79
    apopat     07/31/99 -  [707588] TAB to blanks for OCCS
    lvbcheng   10/27/98 -  change long to int for sqlda
    lvbcheng   08/15/97 -  Move sqlda protos to sqlcpr.h
    lvbcheng   06/25/97 -  Move sqlda protos to this file
    jbasu      01/29/95 -  correct typo
    jbasu      01/27/95 -  correct comment - ub2->sb2
    jbasu      12/12/94 - Bug 217878: note this is an SOSD file
    Morse      12/01/87 - undef L and S for v6 include files
    Richey     07/13/87 - change int defs to long 
    Clare      09/13/84 - Port: Ch types to match SQLLIB structs
    Clare      10/02/86 - Add ifndef SQLDA
*/

#ifndef SQLDA_
#define SQLDA_ 1
 
#ifdef T
# undef T
#endif
#ifdef F
# undef F
#endif

#ifdef S
# undef S
#endif
#ifdef L
# undef L
#endif

#ifdef Y
 # undef Y
#endif
 
struct SQLDA {
  /* ub4    */ int        N; /* Descriptor size in number of entries        */
  /* text** */ char     **V; /* Ptr to Arr of addresses of main variables   */
  /* ub4*   */ int       *L; /* Ptr to Arr of lengths of buffers            */
  /* sb2*   */ short     *T; /* Ptr to Arr of types of buffers              */
  /* sb2**  */ short    **I; /* Ptr to Arr of addresses of indicator vars   */
  /* sb4    */ int        F; /* Number of variables found by DESCRIBE       */
  /* text** */ char     **S; /* Ptr to Arr of variable name pointers        */
  /* ub2*   */ short     *M; /* Ptr to Arr of max lengths of var. names     */
  /* ub2*   */ short     *C; /* Ptr to Arr of current lengths of var. names */
  /* text** */ char     **X; /* Ptr to Arr of ind. var. name pointers       */
  /* ub2*   */ short     *Y; /* Ptr to Arr of max lengths of ind. var. names*/
  /* ub2*   */ short     *Z; /* Ptr to Arr of cur lengths of ind. var. names*/
  };
 
typedef struct SQLDA SQLDA;
 
#endif

/* ----------------- */
/* defines for sqlda */
/* ----------------- */

#define SQLSQLDAAlloc(arg1, arg2, arg3, arg4) sqlaldt(arg1, arg2, arg3, arg4) 

#define SQLSQLDAFree(arg1, arg2) sqlclut(arg1, arg2) 





SQLDA *bind_dp;
SQLDA *select_dp;

extern SQLDA *sqlald();
extern void sqlnul();
char rtrim(char s[], int len);

/* Define a buffer to hold longjmp state info. */
jmp_buf jmp_continue;

/* A global flag for the error routine. */
int parse_flag = 0;

char def_separator='\t'; /* [-s] default separator */
char def_eol='\n';       /* [-e] default line terminator */
char def_queryfile[kDefLen]=""; /* -o default output file name */
char def_outfile[kDefLen]=""; /* -o default output file name */
char def_user[kUserLen]=""; /* [-u] default username */
char def_passwd[kPasswdLen]=""; /* [-p] default password */
char def_host[kHostLen]=""; /* -h default host name */

int check_sql_statement(char sql_statement[]);
char getTranChar(char c);

main(int argc, char *argv[])
{

    int oracle_connect();
    int alloc_descriptors();
    int get_sql_statement();
    int set_bind_variables();
    int process_select_list();

    int i;

    strcpy(sql_statement, "");

    parse_arguments(argc, argv);

    /* Connect to the database. */
    if (oracle_connect() != 0)
        exit(1);

    /* Allocate memory for the select and bind descriptors. */
    if (alloc_descriptors(MAX_ITEMS, MAX_VNAME_LEN, MAX_INAME_LEN) != 0)
    {
	/* EXEC SQL WHENEVER SQLERROR CONTINUE; */ 

    	/* this is just an export tool */
    	/* EXEC SQL ROLLBACK WORK RELEASE; */ 

{
     struct sqlexd sqlstm;
     sqlstm.sqlvsn = 12;
     sqlstm.arrsiz = 0;
     sqlstm.sqladtp = &sqladt;
     sqlstm.sqltdsp = &sqltds;
     sqlstm.iters = (unsigned int  )1;
     sqlstm.offset = (unsigned int  )5;
     sqlstm.cud = sqlcud0;
     sqlstm.sqlest = (unsigned char  *)&sqlca;
     sqlstm.sqlety = (unsigned short)4352;
     sqlstm.occurs = (unsigned int  )0;
     sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



        exit(1);
    }

    /* Process SQL statements. */
    i = setjmp(jmp_continue);
    
    /* Get the statement.  Break on "exit". */
    /*
      if (get_sql_statement() != 0)
      break;
      */
    
    /* parsing error occurs and errors were printed by sql_error() */
    if (parse_flag == 1 && i == 1)  /* i=1 is returned from longjmp() */
      {
	/* EXEC SQL WHENEVER SQLERROR CONTINUE; */ 

	/* this is just an export tool */
	/* EXEC SQL ROLLBACK WORK RELEASE; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 0;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )20;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)4352;
 sqlstm.occurs = (unsigned int  )0;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	
	exit(1);
      }
    
    /* Prepare the statement and declare a cursor. */
    /* EXEC SQL WHENEVER SQLERROR DO sql_error(); */ 

    
    parse_flag = 1;     /* Set a flag for sql_error(). */
    /* EXEC SQL  AT db_name 
      DECLARE S STATEMENT; */ 

    /* EXEC SQL PREPARE S FROM :sql_statement; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 1;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.stmt = "";
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )35;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlstm.sqhstv[0] = (unsigned char  *)sql_statement;
    sqlstm.sqhstl[0] = (unsigned long )8192;
    sqlstm.sqhsts[0] = (         int  )0;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqinds[0] = (         int  )0;
    sqlstm.sqharm[0] = (unsigned long )0;
    sqlstm.sqadto[0] = (unsigned short )0;
    sqlstm.sqtdso[0] = (unsigned short )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqphss = sqlstm.sqhsts;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqpins = sqlstm.sqinds;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlstm.sqpadto = sqlstm.sqadto;
    sqlstm.sqptdso = sqlstm.sqtdso;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
    if (sqlca.sqlcode < 0) sql_error();
}


    parse_flag = 0;     /* Unset the flag. */
    
    /* EXEC SQL DECLARE C CURSOR FOR S; */ 

    
    /* Set the bind variables for any placeholders in the
       SQL statement. */
    set_bind_variables();
    
    /* Open the cursor and execute the statement.
     * If the statement is not a query (SELECT), the
     * statement processing is completed after the
     * OPEN.
     */
    
    /* EXEC SQL OPEN C USING DESCRIPTOR bind_dp; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 1;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.stmt = "";
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )61;
    sqlstm.selerr = (unsigned short)1;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlstm.sqcmod = (unsigned int )0;
    sqlstm.sqhstv[0] = (unsigned char  *)bind_dp;
    sqlstm.sqhstl[0] = (unsigned long )0;
    sqlstm.sqhsts[0] = (         int  )0;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqinds[0] = (         int  )0;
    sqlstm.sqharm[0] = (unsigned long )0;
    sqlstm.sqadto[0] = (unsigned short )0;
    sqlstm.sqtdso[0] = (unsigned short )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqphss = sqlstm.sqhsts;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqpins = sqlstm.sqinds;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlstm.sqpadto = sqlstm.sqadto;
    sqlstm.sqptdso = sqlstm.sqtdso;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
    if (sqlca.sqlcode < 0) sql_error();
}


    
    /* Call the function that processes the select-list.
     * If the statement is not a query, this function
     * just returns, doing nothing.
     */
    process_select_list();
    
    /* Tell user how many rows processed. */
    printf("\n\n%d row%c processed.\n", sqlca.sqlerrd[2],
	   sqlca.sqlerrd[2] == 1 ? '\0' : 's');
    
    /* When done, free the memory allocated for
       pointers in the bind and select descriptors. */
    for (i = 0; i < MAX_ITEMS; i++)
      {    
        if (bind_dp->V[i] != (char *) 0)
	  free(bind_dp->V[i]);
        free(bind_dp->I[i]);   /* MAX_ITEMS were allocated. */
        if (select_dp->V[i] != (char *) 0)
	  free(select_dp->V[i]);
        free(select_dp->I[i]); /* MAX_ITEMS were allocated. */
      }
    
    fclose(OUTFILE);	
    
    /* Free space used by the descriptors themselves. */
    sqlclu(bind_dp);
    sqlclu(select_dp);
    
    /* EXEC SQL WHENEVER SQLERROR CONTINUE; */ 

    /* Close the cursor. */
    /* EXEC SQL CLOSE C; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 1;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )87;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



    /* this is just an export tool */
    /* EXEC SQL ROLLBACK WORK RELEASE; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 1;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )109;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}



    /* EXEC SQL WHENEVER SQLERROR DO sql_error(); */
    return;
}


void  parse_arguments(int argc, char *argv[]) {
  int i, n;
  int inc=1;
  int option_f=0, option_q=0;
  
  if (argc==1)
    {
      printf("%s\n", usage_message);
      exit(0);
    }

  /* First argument is expected to be Oracle user & possibly password and/or
     hostname */

  parse_logon_argument(argv[1]);


  /* Getting user input values */
  for (i=0; i<argc; i += inc)
    {
      inc = 1;
      if (argv[i][0]=='-')
	{
	  if (argc<=(i+1))
	    {
	      printf("%s\n", usage_message);
	      exit(1);
	    }
	  inc = 2;
	  switch (argv[i][1])
	    {
	    case 's':
	      def_separator=getTranChar(argv[i+1][0]);
	      break;
	    case 'e':
	      def_eol=getTranChar(argv[i+1][0]);
	      break;
	    case 'f':
	      if (option_q==0)
		{
		  option_f=1;
	       	  strcpy(def_queryfile, argv[i+1]);
	       	  break;
		}
	      else
		{
		  printf("-f and -q cannot be used at the same time.\n");
		  exit(0);
		}
	    case 'o':
	      strcpy(def_outfile, argv[i+1]);
	      break;
	    case 'u':
	      strcpy(def_user, argv[i+1]);
	      break;
	    case 'p':
	      strcpy(def_passwd, argv[i+1]);
	      break;
	    case 'h':
	      strcpy(def_host, argv[i+1]);
	      break;
	    case 'q':
	      if (option_f==0)
		{
		  option_q=1;
		  strcpy(sql_statement, argv[i+1]);
	       	  break;
		}
	      else
		{
		  printf("-f and -q cannot be used at the same time.\n");
		  exit(0);
		}
	    default:
	      inc = 1;
	      break;
	    }
	}
    }
  
    for(;;)
    {
      if (strlen(def_user)==0)
      {
        printf("\nusername: ");
        fgets((char *) username.arr, sizeof username.arr, stdin);
        fflush(stdin);
        username.arr[strlen((char *) username.arr)-1] = '\0';
      }
      else
        strcpy(username.arr, def_user);
      if ((username.len=strlen((char *) username.arr))!=0)
          break;
    }
      
    for (;;)
    {
      if (strlen(def_passwd)==0)
      {
        printf("password: ");
        fgets((char *) password.arr, sizeof password.arr, stdin);
        fflush(stdin);
        password.arr[strlen((char *) password.arr) - 1] = '\0'; 
        /* 
        n = getpswd((char *) password.arr, sizeof password.arr); 
        */
    
        password.arr[n]='\0';
        password.len=n;

	printf("\n");
      }
      else
        strcpy(password.arr, def_passwd);
      if ((password.len=strlen((char *) password.arr))!=0)
	   break;
    }

    if (strlen(sql_statement)==0)
    {
     if (strlen(def_queryfile)>0)
     {
	FILE *QUERYFILE;
        long read_size=0;

	if ((QUERYFILE=fopen(def_queryfile,"r"))!=NULL)
	{
	   if (fgets(sql_statement, kSqlInxBufSize, QUERYFILE)==NULL)
	       printf("Cannot read query file %s\n", def_queryfile);
	   while (fgets(&sql_statement[strlen(sql_statement)], 
			kSqlInxBufSize, QUERYFILE));
	   fclose(QUERYFILE);
	}
	else
	    printf("Cannot open query file %s\n", def_queryfile);
     }
    }

    for (;;)
    {
      if (strlen(def_outfile)==0)
      {
        printf("Output filename: ");
        fgets((char *) def_outfile, sizeof def_outfile, stdin);
        fflush(stdin);
	def_outfile[strlen(def_outfile) - 1] = '\0';
      }
      else break;
    }

    for (;;)
    {
      if (strlen(sql_statement)==0)
      {
        printf("SQL query: ");
        fgets((char *) sql_statement, sizeof sql_statement, stdin);
        fflush(stdin);
        sql_statement[strlen(sql_statement) - 1] = '\0';
      }
      else break;
    }
    
    if (check_sql_statement(sql_statement) != 0)
    {
    	fprintf(stderr, "SQL query has to begin with \"select\" \n");
    	exit(1);
    }

    if ((OUTFILE=fopen(def_outfile, "w"))==NULL) {
      fprintf (stderr, "Cannot open output file\n");
      exit(1);
    }


}

void  parse_logon_argument(char logonStr[]) {

   char *logonstrPtr, *def_userPtr, *def_passwdPtr, *def_hostPtr;

   logonstrPtr = logonStr;
   def_userPtr = def_user;
   def_passwdPtr = def_passwd;
   def_hostPtr = def_host;

   for (; *logonstrPtr!='/' && *logonstrPtr != '@' && *logonstrPtr != '\0'; 
	logonstrPtr++) {
     *def_userPtr++ = *logonstrPtr;
   }

   *def_userPtr = '\0';

   if (*logonstrPtr == '/') {
     for (logonstrPtr++; *logonstrPtr != '@' && *logonstrPtr != '\0'; 
	  logonstrPtr++) {
       *def_passwdPtr++ = *logonstrPtr;
     }

     *def_passwdPtr = '\0';
   }

   if (*logonstrPtr == '@') {
     for (logonstrPtr++; *logonstrPtr != '\0'; logonstrPtr++) {
       *def_hostPtr++ = *logonstrPtr;
     }

     *def_hostPtr = '\0';
   }

 }

char getTranChar(char c)
{
  switch(c)
  {
  case 'n':
    return('\n');
  case 't':
    return('\t');
  case 'v':
    return('\v');
  case 'b':
    return('\b');
  case 'r':
    return('\r');
  case 'f':
    return('\f');
  case 'a':
    return('\a');
  default:
    return(c);
  }
}

oracle_connect()
{

    /* Steve Turner 5/4/98 - if no host specified, default to local db */

    /* EXEC SQL WHENEVER SQLERROR GOTO connect_error; */ 


    if (def_host) {
      strcpy(db_string.arr, def_host);
      db_string.len=strlen((char *) db_string.arr);

      /* EXEC SQL CONNECT :username IDENTIFIED BY :password
	AT db_name USING :db_string; */ 

{
      struct sqlexd sqlstm;
      sqlstm.sqlvsn = 12;
      sqlstm.arrsiz = 4;
      sqlstm.sqladtp = &sqladt;
      sqlstm.sqltdsp = &sqltds;
      sqlstm.iters = (unsigned int  )10;
      sqlstm.offset = (unsigned int  )124;
      sqlstm.cud = sqlcud0;
      sqlstm.sqlest = (unsigned char  *)&sqlca;
      sqlstm.sqlety = (unsigned short)4352;
      sqlstm.occurs = (unsigned int  )0;
      sqlstm.sqhstv[0] = (unsigned char  *)&username;
      sqlstm.sqhstl[0] = (unsigned long )130;
      sqlstm.sqhsts[0] = (         int  )130;
      sqlstm.sqindv[0] = (         short *)0;
      sqlstm.sqinds[0] = (         int  )0;
      sqlstm.sqharm[0] = (unsigned long )0;
      sqlstm.sqadto[0] = (unsigned short )0;
      sqlstm.sqtdso[0] = (unsigned short )0;
      sqlstm.sqhstv[1] = (unsigned char  *)&password;
      sqlstm.sqhstl[1] = (unsigned long )34;
      sqlstm.sqhsts[1] = (         int  )34;
      sqlstm.sqindv[1] = (         short *)0;
      sqlstm.sqinds[1] = (         int  )0;
      sqlstm.sqharm[1] = (unsigned long )0;
      sqlstm.sqadto[1] = (unsigned short )0;
      sqlstm.sqtdso[1] = (unsigned short )0;
      sqlstm.sqhstv[2] = (unsigned char  *)&db_string;
      sqlstm.sqhstl[2] = (unsigned long )52;
      sqlstm.sqhsts[2] = (         int  )52;
      sqlstm.sqindv[2] = (         short *)0;
      sqlstm.sqinds[2] = (         int  )0;
      sqlstm.sqharm[2] = (unsigned long )0;
      sqlstm.sqadto[2] = (unsigned short )0;
      sqlstm.sqtdso[2] = (unsigned short )0;
      sqlstm.sqphsv = sqlstm.sqhstv;
      sqlstm.sqphsl = sqlstm.sqhstl;
      sqlstm.sqphss = sqlstm.sqhsts;
      sqlstm.sqpind = sqlstm.sqindv;
      sqlstm.sqpins = sqlstm.sqinds;
      sqlstm.sqparm = sqlstm.sqharm;
      sqlstm.sqparc = sqlstm.sqharc;
      sqlstm.sqpadto = sqlstm.sqadto;
      sqlstm.sqptdso = sqlstm.sqtdso;
      sqlstm.sqlcmax = (unsigned int )100;
      sqlstm.sqlcmin = (unsigned int )2;
      sqlstm.sqlcincr = (unsigned int )1;
      sqlstm.sqlctimeout = (unsigned int )0;
      sqlstm.sqlcnowait = (unsigned int )0;
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
      if (sqlca.sqlcode < 0) goto connect_error;
}


    }
    else {
      /* EXEC SQL CONNECT :username IDENTIFIED BY :password AT db_name ; */ 

{
      struct sqlexd sqlstm;
      sqlstm.sqlvsn = 12;
      sqlstm.arrsiz = 4;
      sqlstm.sqladtp = &sqladt;
      sqlstm.sqltdsp = &sqltds;
      sqlstm.iters = (unsigned int  )10;
      sqlstm.offset = (unsigned int  )162;
      sqlstm.cud = sqlcud0;
      sqlstm.sqlest = (unsigned char  *)&sqlca;
      sqlstm.sqlety = (unsigned short)4352;
      sqlstm.occurs = (unsigned int  )0;
      sqlstm.sqhstv[0] = (unsigned char  *)&username;
      sqlstm.sqhstl[0] = (unsigned long )130;
      sqlstm.sqhsts[0] = (         int  )130;
      sqlstm.sqindv[0] = (         short *)0;
      sqlstm.sqinds[0] = (         int  )0;
      sqlstm.sqharm[0] = (unsigned long )0;
      sqlstm.sqadto[0] = (unsigned short )0;
      sqlstm.sqtdso[0] = (unsigned short )0;
      sqlstm.sqhstv[1] = (unsigned char  *)&password;
      sqlstm.sqhstl[1] = (unsigned long )34;
      sqlstm.sqhsts[1] = (         int  )34;
      sqlstm.sqindv[1] = (         short *)0;
      sqlstm.sqinds[1] = (         int  )0;
      sqlstm.sqharm[1] = (unsigned long )0;
      sqlstm.sqadto[1] = (unsigned short )0;
      sqlstm.sqtdso[1] = (unsigned short )0;
      sqlstm.sqphsv = sqlstm.sqhstv;
      sqlstm.sqphsl = sqlstm.sqhstl;
      sqlstm.sqphss = sqlstm.sqhsts;
      sqlstm.sqpind = sqlstm.sqindv;
      sqlstm.sqpins = sqlstm.sqinds;
      sqlstm.sqparm = sqlstm.sqharm;
      sqlstm.sqparc = sqlstm.sqharc;
      sqlstm.sqpadto = sqlstm.sqadto;
      sqlstm.sqptdso = sqlstm.sqtdso;
      sqlstm.sqlcmax = (unsigned int )100;
      sqlstm.sqlcmin = (unsigned int )2;
      sqlstm.sqlcincr = (unsigned int )1;
      sqlstm.sqlctimeout = (unsigned int )0;
      sqlstm.sqlcnowait = (unsigned int )0;
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
      if (sqlca.sqlcode < 0) goto connect_error;
}


    }

    printf("\nConnected to ORACLE as user %s.\n", username.arr);

    return 0;

connect_error:
    fprintf(stderr, "Cannot connect to ORACLE as user %s\n", username.arr);
    exit(1);
}

int check_sql_statement(char sql_statement[])
{
  char *p1, *p2, *p3, *p4, *semicolonPtr ;
  char str[]="SELECT ";

  int n;
  if (strlen(sql_statement) < strlen(str))
      return -1;

  /* trim all newline & cr from end of statement */

  n = strlen(sql_statement) - 1;

  while ( n > 0 && (sql_statement[n] == '\n' || sql_statement[n] == '\r')) {
      sql_statement[n--] = '\0';
    }

  if (strlen(sql_statement) < strlen(str))
      return -1;

  p1 = (char *) sql_statement;
  while (*p1++ == ' ');
  p1--;

  /* p1 now marks the start of the first word in the sql statement */

  p2 = p3 = p4 = p1;
  while (*p2 != ' ' && *p2 != '\0') p2++;

  /* p2 now marks the first space after the first word in the statement */

  /* check that first word is 'select' */

  while (p3 < p2)
    *p3++ = toupper(*p3);
 
  if (strstr(p1, str) != p1)
        return -1;
 
  /* We now know first word is SELECT; if necessary left trim it: */

  if (p1 != sql_statement) {
    p4 = (char *) sql_statement;
    
    *p4++ = 'S';
    *p4++ = 'E';
    *p4++ = 'L';
    *p4++ = 'E';
    *p4++ = 'C';
    *p4++ = 'T';
    
    for ( ; p4 < p2; p4++) 
      *p4 = ' ';
  }

 /* If the sql statement has a semi-colon at the end, strip it off: */

  for (semicolonPtr = sql_statement + strlen(sql_statement) - 1;
       *semicolonPtr == ' ' && semicolonPtr != sql_statement;
       semicolonPtr--)
    ;

   if (*semicolonPtr == ';')
	   *semicolonPtr = '\0';
 
  return 0;
}


/*
 *  Allocate the BIND and SELECT descriptors using sqlald().
 *  Also allocate the pointers to indicator variables
 *  in each descriptor.  The pointers to the actual bind
 *  variables and the select-list items are realloc'ed in
 *  the set_bind_variables() or process_select_list()
 *  routines.  This routine allocates 1 byte for select_dp->V[i]
 *  and bind_dp->V[i], so the realloc will work correctly.
 */

alloc_descriptors(size, max_vname_len, max_iname_len)
int size;
int max_vname_len;
int max_iname_len;
{
    int i;

    /*
     * The first sqlald parameter determines the maximum number of
     * array elements in each variable in the descriptor. In
     * other words, it determines the maximum number of bind
     * variables or select-list items in the SQL statement.
     *
     * The second parameter determines the maximum length of
     * strings used to hold the names of select-list items
     * or placeholders.  The maximum length of column 
     * names in ORACLE is 30, but you can allocate more or less
     * as needed.
     *
     * The third parameter determines the maximum length of
     * strings used to hold the names of any indicator
     * variables.  To follow ORACLE standards, the maximum
     * length of these should be 30.  But, you can allocate
     * more or less as needed.
     */

    if ((bind_dp =
            sqlald(size, max_vname_len, max_iname_len)) == (SQLDA *) 0)
    {
        fprintf(stderr,
            "Cannot allocate memory for bind descriptor.");
        return -1;  /* Have to exit in this case. */
    }

    if ((select_dp =
        sqlald (size, max_vname_len, max_iname_len)) == (SQLDA *) 0)
    {
        fprintf(stderr,
            "Cannot allocate memory for select descriptor.");
        return -1;
    }
    select_dp->N = MAX_ITEMS;

    /* Allocate the pointers to the indicator variables, and the
       actual data. */
    for (i = 0; i < MAX_ITEMS; i++) {
        bind_dp->I[i] = (short *) malloc(sizeof (short));
        select_dp->I[i] = (short *) malloc(sizeof(short));
        bind_dp->V[i] = (char *) malloc(1);
        select_dp->V[i] = (char *) malloc(1);
    }
       
    return 0;
}


get_sql_statement()
{
    char *cp, linebuf[256];
    int iter, plsql;
    int help();

    for (plsql = 0, iter = 1; ;)
    {
        if (iter == 1)
        {
            printf("\nSQL> ");
            sql_statement[0] = '\0';
        }
        
        fgets(linebuf, sizeof linebuf, stdin);
        fflush(stdin);

        cp = strrchr(linebuf, '\n');
        if (cp && cp != linebuf)
            *cp = ' ';
        else if (cp == linebuf)
            continue;

        if ((strncmp(linebuf, "EXIT", 4) == 0) ||
            (strncmp(linebuf, "exit", 4) == 0))
        {
            return -1;
        }

        else if (linebuf[0] == '?' ||
            (strncmp(linebuf, "HELP", 4) == 0) ||
            (strncmp(linebuf, "help", 4) == 0))
        {
            help();
            iter = 1;
            continue;
        }

        if (strstr(linebuf, "BEGIN") ||
            (strstr(linebuf, "begin")))
        {
            plsql = 1;
        }

        strcat(sql_statement, linebuf);

        if ((plsql && (cp = strrchr(sql_statement, '/'))) ||
            (!plsql && (cp = strrchr(sql_statement, ';'))))
        {
            *cp = '\0';
            break;
        }
        else
        {
            iter++;
            printf("%3d  ", iter);
        }
    }
    return 0;
}



set_bind_variables()
{
    int i, n;
    char bind_var[64];

    /* Describe any bind variables (input host variables) */
    /* EXEC SQL WHENEVER SQLERROR DO sql_error(); */ 


    bind_dp->N = MAX_ITEMS;  /* Initialize count of array elements. */
    /* EXEC SQL DESCRIBE BIND VARIABLES FOR S INTO bind_dp; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 4;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )200;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlstm.sqhstv[0] = (unsigned char  *)bind_dp;
    sqlstm.sqhstl[0] = (unsigned long )0;
    sqlstm.sqhsts[0] = (         int  )0;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqinds[0] = (         int  )0;
    sqlstm.sqharm[0] = (unsigned long )0;
    sqlstm.sqadto[0] = (unsigned short )0;
    sqlstm.sqtdso[0] = (unsigned short )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqphss = sqlstm.sqhsts;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqpins = sqlstm.sqinds;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlstm.sqpadto = sqlstm.sqadto;
    sqlstm.sqptdso = sqlstm.sqtdso;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
    if (sqlca.sqlcode < 0) sql_error();
}



    /* If F is negative, there were more bind variables
       than originally allocated by sqlald(). */
    if (bind_dp->F < 0)
    {
        printf ("\nToo many bind variables (%d), maximum is %d\n.",
                    -bind_dp->F, MAX_ITEMS);
        return;
    }

    /* Set the maximum number of array elements in the
       descriptor to the number found. */
    bind_dp->N = bind_dp->F;
 
    /* Get the value of each bind variable as a
     * character string.
     *   
     * C[i] contains the length of the bind variable
     *      name used in the SQL statement.
     * S[i] contains the actual name of the bind variable
     *      used in the SQL statement.
     *
     * L[i] will contain the length of the data value
     *      entered.
     *
     * V[i] will contain the address of the data value
     *      entered.
     *
     * T[i] is always set to 1 because in this sample program
     *      data values for all bind variables are entered
     *      as character strings.
     *      ORACLE converts to the table value from CHAR.
     *
     * I[i] will point to the indicator value, which is
     *      set to -1 when the bind variable value is "null".
     */
    for (i = 0; i < bind_dp->F; i++)
    {
        printf ("\nEnter value for bind variable %.*s:  ",
               (int)bind_dp->C[i], bind_dp->S[i]);
        fgets(bind_var, sizeof bind_var, stdin);

        /* Get length and remove the new line character. */
        n = strlen(bind_var) - 1;

        /* Set it in the descriptor. */
        bind_dp->L[i] = n;

        /* (re-)allocate the buffer for the value.
           sqlald() reserves a pointer location for
           V[i] but does not allocate the full space for
           the pointer. */

         bind_dp->V[i] = (char *) realloc(bind_dp->V[i],
                         (bind_dp->L[i] + 1));            

        /* And copy it in. */
        strncpy(bind_dp->V[i], bind_var, n);

        /* Set the indicator variable's value. */
        if ((strncmp(bind_dp->V[i], "NULL", 4) == 0) ||
                (strncmp(bind_dp->V[i], "null", 4) == 0))
            *bind_dp->I[i] = -1;
        else
            *bind_dp->I[i] = 0;
    
        /* Set the bind datatype to 1 for CHAR. */
        bind_dp->T[i] = 1;
    }
}



process_select_list()
{
    int i, null_ok, precision, scale;

    if ((strncmp(sql_statement, "SELECT", 6) != 0) &&
        (strncmp(sql_statement, "select", 6) != 0))
    {
        select_dp->F = 0;
        return;
    }

    /* If the SQL statement is a SELECT, describe the
        select-list items.  The DESCRIBE function returns
        their names, datatypes, lengths (including precision
        and scale), and NULL/NOT NULL statuses. */

    select_dp->N = MAX_ITEMS;
    
    /* EXEC SQL DESCRIBE SELECT LIST FOR S INTO select_dp; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 4;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )226;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlstm.sqhstv[0] = (unsigned char  *)select_dp;
    sqlstm.sqhstl[0] = (unsigned long )0;
    sqlstm.sqhsts[0] = (         int  )0;
    sqlstm.sqindv[0] = (         short *)0;
    sqlstm.sqinds[0] = (         int  )0;
    sqlstm.sqharm[0] = (unsigned long )0;
    sqlstm.sqadto[0] = (unsigned short )0;
    sqlstm.sqtdso[0] = (unsigned short )0;
    sqlstm.sqphsv = sqlstm.sqhstv;
    sqlstm.sqphsl = sqlstm.sqhstl;
    sqlstm.sqphss = sqlstm.sqhsts;
    sqlstm.sqpind = sqlstm.sqindv;
    sqlstm.sqpins = sqlstm.sqinds;
    sqlstm.sqparm = sqlstm.sqharm;
    sqlstm.sqparc = sqlstm.sqharc;
    sqlstm.sqpadto = sqlstm.sqadto;
    sqlstm.sqptdso = sqlstm.sqtdso;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
    if (sqlca.sqlcode < 0) sql_error();
}



    /* If F is negative, there were more select-list
       items than originally allocated by sqlald(). */
    if (select_dp->F < 0)
    {
        printf ("\nToo many select-list items (%d), maximum is %d\n",
                -(select_dp->F), MAX_ITEMS);
        return;
    }

    /* Set the maximum number of array elements in the
       descriptor to the number found. */
    select_dp->N = select_dp->F;

    /* Allocate storage for each select-list item.
  
       sqlprc() is used to extract precision and scale
       from the length (select_dp->L[i]).

       sqlnul() is used to reset the high-order bit of
       the datatype and to check whether the column
       is NOT NULL.

       CHAR    datatypes have length, but zero precision and
               scale.  The length is defined at CREATE time.

       NUMBER  datatypes have precision and scale only if
               defined at CREATE time.  If the column
               definition was just NUMBER, the precision
               and scale are zero, and you must allocate
               the required maximum length.

       DATE    datatypes return a length of 7 if the default
               format is used.  This should be increased to
               9 to store the actual date character string.
               If you use the TO_CHAR function, the maximum
               length could be 75, but will probably be less
               (you can see the effects of this in SQL*Plus).

       ROWID   datatype always returns a fixed length of 18 if
               coerced to CHAR.

       LONG and
       LONG RAW datatypes return a length of 0 (zero),
               so you need to set a maximum.  In this example,
               it is 240 characters.

       */
    
    printf ("\n");
    for (i = 0; i < select_dp->F; i++)
    {
        /* Turn off high-order bit of datatype (in this example,
           it does not matter if the column is NOT NULL). */
        sqlnul (&(select_dp->T[i]), &(select_dp->T[i]), &null_ok);

        switch (select_dp->T[i])
        {
            case  1 : /* CHAR datatype: no change in length
                         needed, except possibly for TO_CHAR
                         conversions (not handled here). */
                break;
            case  2 : /* NUMBER datatype: use sqlprc() to
                         extract precision and scale. */
                sqlprc (&(select_dp->L[i]), &precision, &scale);
                      /* Allow for maximum size of NUMBER. */
                if (precision == 0) precision = 40;
                      /* Also allow for decimal point and
                         possible sign. */
                if (scale > 0)
                    select_dp->L[i] = precision + 2;
                else
                    select_dp->L[i] = precision;
                break;

            case  8 : /* LONG datatype */
                select_dp->L[i] = 240;
                break;

            case 11 : /* ROWID datatype */
                select_dp->L[i] = 18;
                break;

            case 12 : /* DATE datatype */
                select_dp->L[i] = 9;
                break;
 
            case 23 : /* RAW datatype */
                break;

            case 24 : /* LONG RAW datatype */
                select_dp->L[i] = 240;
                break;
        }
        /* Allocate space for the select-list data values.
           sqlald() reserves a pointer location for
           V[i] but does not allocate the full space for
           the pointer.  */

         select_dp->V[i] = (char *) realloc(select_dp->V[i],
                                    select_dp->L[i] + 1);  
        
        /* Print column headings, right-justifying number
            column headings. */
        /*
        if (select_dp->T[i] == 2)
            printf ("%.*s ", select_dp->L[i], select_dp->S[i]);
        else
            printf ("%-.*s ", select_dp->L[i], select_dp->S[i]);
        */

        /* Coerce ALL datatypes except for LONG RAW to
           character. */
        if (select_dp->T[i] != 24)
            select_dp->T[i] = 1;
    }
    printf ("\n\n");

    /* FETCH each row selected and print the column values. */
    /* EXEC SQL WHENEVER NOT FOUND GOTO end_select_loop; */ 


    for (;;)
    {
        /* EXEC SQL FETCH C USING DESCRIPTOR select_dp; */ 

{
        struct sqlexd sqlstm;
        sqlstm.sqlvsn = 12;
        sqlstm.arrsiz = 4;
        sqlstm.sqladtp = &sqladt;
        sqlstm.sqltdsp = &sqltds;
        sqlstm.iters = (unsigned int  )1;
        sqlstm.offset = (unsigned int  )252;
        sqlstm.selerr = (unsigned short)1;
        sqlstm.cud = sqlcud0;
        sqlstm.sqlest = (unsigned char  *)&sqlca;
        sqlstm.sqlety = (unsigned short)4352;
        sqlstm.occurs = (unsigned int  )0;
        sqlstm.sqfoff = (         int )0;
        sqlstm.sqfmod = (unsigned int )2;
        sqlstm.sqhstv[0] = (unsigned char  *)select_dp;
        sqlstm.sqhstl[0] = (unsigned long )0;
        sqlstm.sqhsts[0] = (         int  )0;
        sqlstm.sqindv[0] = (         short *)0;
        sqlstm.sqinds[0] = (         int  )0;
        sqlstm.sqharm[0] = (unsigned long )0;
        sqlstm.sqadto[0] = (unsigned short )0;
        sqlstm.sqtdso[0] = (unsigned short )0;
        sqlstm.sqphsv = sqlstm.sqhstv;
        sqlstm.sqphsl = sqlstm.sqhstl;
        sqlstm.sqphss = sqlstm.sqhsts;
        sqlstm.sqpind = sqlstm.sqindv;
        sqlstm.sqpins = sqlstm.sqinds;
        sqlstm.sqparm = sqlstm.sqharm;
        sqlstm.sqparc = sqlstm.sqharc;
        sqlstm.sqpadto = sqlstm.sqadto;
        sqlstm.sqptdso = sqlstm.sqtdso;
        sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
        if (sqlca.sqlcode == 1403) goto end_select_loop;
        if (sqlca.sqlcode < 0) sql_error();
}



        /* Since each variable returned has been coerced to a
           character string, very little processing is required
           here.  This routine just prints out the values on
           the terminal. */
        for (i = 0; i < select_dp->F; i++)
        {
            if (*select_dp->I[i] >= 0)
	    {
		if (rtrim(select_dp->V[i], (int)select_dp->L[i]))
                    fprintf (OUTFILE, "%s", select_dp->V[i]);
		else
		    fprintf (OUTFILE, "%-*.*s", (int)select_dp->L[i],
                        (int)select_dp->L[i], select_dp->V[i]);
	    }
            if (i!=(select_dp->F - 1))
                fprintf(OUTFILE, "%c", def_separator);
        }
        fprintf (OUTFILE, "%c", def_eol);
    }
end_select_loop:
    return;
}

char rtrim(char s[], int len)
{
  long j=1;
  char rc=0;

  while (len != 0 && len>=j && s[len-j]==' ')
  {
     rc=1;
     s[len-j]='\0';
     j++;
  }

  return rc;
}

help()
{
    puts("\n\nEnter a SQL statement or a PL/SQL block at the SQL> prompt.");
    puts("Statements can be continued over several lines, except");
    puts("within string literals.");
    puts("Terminate a SQL statement with a semicolon.");
    puts("Terminate a PL/SQL block (which can contain embedded semicolons)");
    puts("with a slash (/).");
    puts("Typing \"exit\" (no semicolon needed) exits the program.");
    puts("You typed \"?\" or \"help\" to get this message.\n\n");
}


sql_error()
{
    int i;

    /* ORACLE error handler */
    printf ("\n\n%.70s\n",sqlca.sqlerrm.sqlerrmc);
    if (parse_flag)
        printf
        ("Above error was located at position %d in SQL statement.\n",
           sqlca.sqlerrd[4]);

    /* EXEC SQL WHENEVER SQLERROR CONTINUE; */ 

    /* EXEC SQL ROLLBACK WORK; */ 

{
    struct sqlexd sqlstm;
    sqlstm.sqlvsn = 12;
    sqlstm.arrsiz = 4;
    sqlstm.sqladtp = &sqladt;
    sqlstm.sqltdsp = &sqltds;
    sqlstm.iters = (unsigned int  )1;
    sqlstm.offset = (unsigned int  )278;
    sqlstm.cud = sqlcud0;
    sqlstm.sqlest = (unsigned char  *)&sqlca;
    sqlstm.sqlety = (unsigned short)4352;
    sqlstm.occurs = (unsigned int  )0;
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


    longjmp(jmp_continue, 1);
}
