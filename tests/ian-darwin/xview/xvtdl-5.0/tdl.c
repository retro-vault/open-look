#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
 * $Id: tdl.c,v 1.2 1992/10/06 11:36:52 jipping Exp $
 *
 * **********************************************************************
 *
 *  Main program for tdl.
 *
 *  This routine sets up parameters from the command line, parses the
 *  specified todo database, and does the tdl action requested on the 
 *  command line.
 *
 *  Copyright (c) 1993 by Mike Jipping and Hope College
 *
 *  Permission is granted to copy and distribute this file in modified or
 *  unmodified form, for noncommercial use, provided (a) this copyright notice
 *  is preserved, (b) no attempt is made to restrict redistribution of this
 *  file, and (c) this file is not distributed as part of any collection whose
 *  redistribution is restricted by a compilation copyright.
 *
 *  Revision History:
 *
 *  $Log: tdl.c,v $
 * Revision 1.2  1992/10/06  11:36:52  jipping
 * Fixes after beta test: Added usage message.
 *
 * Revision 1.1  1992/09/15  12:11:13  jipping
 * Initial revision
 *
 *
 * **********************************************************************
 */

#include "globaldefs.h"
#include "patchlevel.h"

#define ADD 0
#define LIST 1
#define PRINT 2
#define LISTCATS 3

extern FILE *yyin;
extern struct category_rec *cat_search();

int debug=FALSE, verbose=FALSE, changed=FALSE;

extern struct entry_list *entry_head, *entry_tail;
extern struct recurrence_list *rl_head, *rl_tail;
extern struct category_rec *category_head;
int log_level, log_info_level;

struct tm current, *tm, today, *localtime();
int curr_month, curr_day, curr_year;
time_t access_time, modify_time;
char fname[FILENAMESIZ];
struct category_rec *category;
extern char priority_listing[13];
char cat_requested[80];
char printer[80];

int tdlmode, datecode_request, do_all, do_deadline;
int ps, scale_factor, incl_checked;

static char *usage[] = {
   "Usage: tdl [options]",
	"   Options:",
	"      -add     use add mode",
	"      -list    go into list mode (default)",
   "      -print   use print mode",
   "  ",
   "      -all           list/print all categories together (default)",
   "      -category cat  perform checks for category \"cat\"",
   "      -date date     perform checks for \"date\" (mm/dd/yy)",  
   "      -deadline      perform deadline checking also",
   "      -f file        use \"file\" instead of \"~/.tododb\"",
   "      -include       when printing, include checked items",
	"      -order string  list the list in \"ascending\" or \"descending\" order",
   "      -printer pt    print to printer \"pt\"",
   "      -postscript    print in PostScript form",
   "      -scale factor  when in PostScript, scale by \"factor\"",
   (char *)0
};

/*
 * This routine parses the command line, looking for options.
 */

int yyparse(void);
struct category_rec * new_category(char * name, struct category_rec * aftercr, int subcat);
void add_dialog(struct category_rec * category);
void list_categories(struct category_rec * cr, int level);
void load_xdefaults(void);
void print_list(struct category_rec *category, int all, int ps, int checked, int scale, char *printer);
void propagate(void);
void refresh_db(int fromquit);
void show_list(struct category_rec * category, int datecode, int do_all);

void parse_command_line(int argc, char ** argv)
{
   int argind, m, d, y;
   register char *tcp;
	register char **u;

   argind = 1;
   while (argind < argc)
   {   
      tcp = argv[argind];
      if (*tcp == '-') {
         tcp++;
         switch(*tcp){
			   case 'a':
				   if (EQUAL(tcp, "add")) {
						tdlmode = ADD;
					} else {
						if (EQUAL(tcp, "all")) {
							category = 0;
							do_all = TRUE;
						} else {
							printf("%s is not a legal flag\n", tcp);
						}
					}
					break;

            case 'c':
				   if (EQUAL(tcp, "category")) {
						argind++;
						strcpy(cat_requested, argv[argind]);
					} else if (EQUAL(tcp, "categories")) {
						tdlmode = LISTCATS;
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 'd':
				   if (EQUAL(tcp, "date")) {
						argind++;
						sscanf(argv[argind], "%d/%d/%d", &m, &d, &y);
						curr_month = m;
						curr_day = d;
						curr_year = y+1900;
					} else if (EQUAL(tcp, "deadline")) {
						do_deadline = TRUE;
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 'f':
                argind++;
                tcp = argv[argind];
                strcpy(fname, tcp);
                break;

            case 'h':
				   if (EQUAL(tcp, "help")) {
						for (u = usage; (char *)0 != *u; ++u)
							fprintf(stderr, "%s\n", *u);
						exit(2);
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 'i':
				   if (EQUAL(tcp, "include")) {
						incl_checked = TRUE;
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 'l':
				   if (EQUAL(tcp, "list")) {
						tdlmode = LIST;
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 'o':
				   if (EQUAL(tcp, "order")) {
						tcp = argv[++argind];
						strcpy(priority_listing, tcp);
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 'p':
				   if (EQUAL(tcp, "print")) {
						tdlmode = PRINT;
				   } else if (EQUAL(tcp, "printer")) {
						tcp = argv[++argind];
						strcpy(printer, tcp);
					} else if (EQUAL(tcp, "postscript")) {
						ps = TRUE;
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            case 's':
				   if (EQUAL(tcp, "scale")) {
						tcp = argv[++argind];
						scale_factor = atoi(tcp);
					} else {
						printf("%s is not a legal flag\n", tcp);
					}
					break;

            default:
                printf("%s is not a legal flag\n", tcp);
         }
      } else {
         printf("Bad argument: %s\n", tcp);
      }
      argind++;
    }
}

/*
 * The main program.
 */

int main(int argc, char ** argv)
{
   struct timeval tv;
   struct itimerval timer;
   char timstr[16], title[20];
	char *home;
	struct category_rec *cr;

   /* set up initial parameters, the parse the command line */
	home = (char *)getenv("HOME");
	if (home == 0) {
		strcpy(fname, ".tododb");
	} else {
		strcpy(fname, getenv("HOME"));
		strcat(fname, "/.tododb");
	}

	do_all = FALSE;
	do_deadline = FALSE;
	ps = FALSE;
	incl_checked = FALSE;
	scale_factor = 100;

   gettimeofday(&tv, 0);
   tm = localtime(&tv.tv_sec);
   today = *tm;
   curr_month = today.tm_mon+1;
   curr_day = today.tm_mday;
   curr_year = today.tm_year+1900;
	tdlmode = LIST;
	cat_requested[0] = '\0';
	incl_checked = FALSE;
	scale_factor = 100;

	load_xdefaults();
	strcpy(printer, default_printer);
	ps = postscriptmode;

   parse_command_line(argc, argv);
	datecode_request = (curr_year-1990)*10000 + curr_month*100 + curr_day;
	yyin = fopen(fname, "r");

   /* parse the todo database and set up the "current" list */
   if (yyin != 0) {
		yyparse();
		propagate();
	}
	if (category_head == 0)
		category_head = (struct category_rec *)new_category("Every Day", 0, FALSE);

	entry_head = category_head->entry_head;
	entry_tail = category_head->entry_tail;
	rl_head = category_head->rl_head;
	rl_tail = category_head->rl_tail;

	if (do_all) {
		category = 0;
	} else if (strlen(cat_requested) != 0) {
		cr = cat_search(category_head, cat_requested);
		if (cr == 0) {
			fprintf(stderr, "ERROR: There is no \"%s\" in the specified database.\n",
					  cat_requested);
			exit(-2);
		} else {
			category = cr;
			entry_head = cr->entry_head;
			entry_tail = cr->entry_tail;
			rl_head = cr->rl_head;
			rl_tail = cr->rl_tail;
		}
	}

	/* Finally, to the right thang... */

	switch (tdlmode) {
   	case ADD:
		   add_dialog(category==0?category_head:category);
			refresh_db(FALSE);
		   break;

		case LIST:
			show_list(category, datecode_request, do_all);
			if (do_deadline) refresh_db(FALSE);
			break;

		case PRINT:
			print_list(category, do_all, ps, incl_checked, scale_factor, printer);
			break;

		case LISTCATS:
			list_categories(category_head, 0);
			break;
	}

	exit(0);

}
