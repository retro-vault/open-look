#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
Post A Note V3.0
Copyright (c) 1993, Jeffrey W. Bailey
All rights reserved.

Permission is granted to distribute this program in exact, complete
source form, which includes this copyright notice, as long as no fee
other than media and distribution cost is charged.

This program may not be used in whole, or in part, in any other manner
without prior written permission from the author.

This program may not be distributed in modified form without prior
written permission from the author.  In other words, patches may be
distributed, but modified source may not be distributed.

If there are any questions, comments or suggestions, the author may be
contacted at:

    jeff@rd1.interlan.com

    or

    Jeffrey Bailey
    Racal-Datacom, Inc.
    Mail Stop E-110
    1601 N. Harrison Parkway
    Sunrise, FL  33323-2899
*/
#include <string.h>

#include <stdio.h>
#include <ctype.h>

#ifdef HAS_STDLIB
#include <stdlib.h>
#else

#endif /* HAS_STDLIB */

/*
    Component structure for each token on the linked list.
*/
struct ps_component
    {
    struct ps_component *ps_prev; /* Pointer to previous node */
    struct ps_component *ps_next; /* Pointer to next node */
    char *ps_text;		  /* Pointer to token text */
    char ps_start_delim;	  /* Character delimiter for start of token */
    char ps_end_delim;		  /* Character delimiter for end of token */
    };

static struct ps_component *ps_add_node(struct ps_component *start, char *text, char start_delim, char end_delim);

/*

    Name:  parse_string()

    Calling sequence is:

		string  - pointer to string to be parsed
		delim   - pointer to string of delimiters
		d_quote - 1 or 0 allowing/disallowing double quoting
		s_quote - 1 or 0 allowing/disallowing single quoting
		esc     - 1 or 0 allowing/disallowing escaping

    Returns:  A pointer to the first node in the linked list; 0 on failure
              *OR* if the string contains no tokens.

    Note that this routine is implemented as a somewhat complex state machine.
*/

char * instr(char * string, char * spattern);

struct ps_component *parse_string(char * string, char *delim, int d_quote, int s_quote, int esc)
    {
    struct ps_component *start;
    char *point1, *point2, *t_string, *t_component;
    int  beg_escape = 0;
    int  in_escape = 0;
    int  in_d_quote = 0;
    int  in_s_quote = 0;
    int  in_component = 0;
    int  is_delim = 0;
    int  is_quote = 0;
    int  beg_component = 1;
    char temp [2];
    char t_start_delim;

    t_start_delim = 0;
    temp [1] = 0;
    start = 0;

    /* Skip leading delimiters */
    point1 = string;
    temp [0] = *point1;
    while(*point1 != 0 && instr(delim, temp) != 0)
        {
        t_start_delim = *point1;
        temp [0] = *++point1;
        }

    /* quick check for null string */
    if(!strlen(point1)) return(0);

    /* allocate temp storage */
    t_string = malloc(strlen(point1) + 1);
    if(t_string == 0) return(0);
    strcpy(t_string, point1);
    t_component = malloc(strlen(t_string) + 1);
    if(t_component == 0)
        {
        free(t_string);
        return(0);
        }
    *t_component = 0;
    point1 = t_string;
    point2 = t_component;

    /* parse the string */
    while(*point1 != 0)
        {
        /* handle the escape character logic */
        in_escape = 0;
        if(beg_escape) in_escape = 1;
        beg_escape = 0;
        is_quote = 0;

        /* is the next char a delimiter? */
        temp [0] = *point1;
        if(instr(delim, temp) != 0) is_delim = 1;
            else is_delim = 0;

        /* if an escape char and not already escaped */
        if(*point1 == '\\' && !in_escape && esc) beg_escape = 1;

        /* if a double quote, not in single quote, and not escaped */
        if(*point1 == '"' && !in_s_quote && !in_escape)
            {
            /* if not escaped, in double quote, and it is a double quote */
            if(d_quote && in_d_quote && !beg_escape)
                {
                is_quote = 1;
                in_d_quote = 0;
                *point2 = 0;
                start = ps_add_node(start, t_component,
                                                '"', *point1);
                point2 = t_component;
                }
            else
                if(d_quote && !in_d_quote) /* beginning double quote */
                    {
                    is_quote = 1;
                    in_d_quote = 1;
                    beg_component = 1;
                    }
            }
        /* if a single quote, not in double quote, and not escaped */
        if(*point1 == '\'' && !in_d_quote && !in_escape)
            {
            /* if not escaped, in single quote, and it is a single quote */
            if(s_quote && in_s_quote && !beg_escape)
                {
                is_quote = 1;
                in_s_quote = 0;
                *point2 = 0;
                start = ps_add_node(start, t_component,
                                                '\'', *point1);
                point2 = t_component;
                }
            else
                if(s_quote && !in_s_quote) /* beginning single quote */
                    {
                    is_quote = 1;
                    in_s_quote = 1;
                    beg_component = 1;
                    }
            }
        /* if delimiter, not in quotes and not escaped, or is a quote */
        if( (is_delim && !in_d_quote && !in_s_quote && !in_escape) || is_quote)
            {
            /* at the beginning of a component */
            beg_component = 1;
            in_component = 0;
            *point2 = 0;
            if(strlen(t_component)) start = ps_add_node(start, t_component,
                                            t_start_delim, *point1);
            /* store the start delimiter */
            t_start_delim = *point1;
            point2 = t_component;
            }
        else
            {
            if(beg_component) /* if start of component, mark as in comp. */
                {
                in_component = 1;
                beg_component = 0;
                }
            /* copy while in component */
            if(in_component && !beg_escape)
                {
                *point2 = *point1;
                point2++;
                }
            }
        point1++;
        }
    /* if still in component, terminate and add it to the list */
    if(in_component)
        {
        *point2 = 0;
        if(strlen(t_component)) start = ps_add_node(start, t_component,
                                        t_start_delim, *point1);
        }
    /* free temp storage */
    free(t_string);
    free(t_component);
    return(start);
    }

/*
    Name: ps_add_node()

    Description:  private routine called to build list
*/

static struct ps_component *ps_add_node(struct ps_component * start, char * text, char start_delim, char end_delim)
    {
    struct ps_component *hold, *current, *prev;

    hold = (struct ps_component *) malloc(sizeof(struct ps_component));
    if(hold == 0) return(0);
    hold->ps_text = malloc(strlen(text) + 1);
    if(hold->ps_text == 0) return(0);
    strcpy(hold->ps_text, text);
    hold->ps_start_delim = start_delim;
    hold->ps_end_delim = end_delim;
    current = start;
    prev = current;
    while(current != 0)
        {
        prev = current;
        current = current->ps_next;
        }
    if(prev == 0)
        {
        start = hold;
        hold->ps_prev = 0;
        hold->ps_next = 0;
        }
    else
        {
        prev->ps_next = hold;
        hold->ps_prev = prev;
        hold->ps_next = 0;
        }
    return(start);
    }

/*
    Name:  free_ps_list()

    Description:  Routine to free a list built by parse_string.  Pass the
                  address of the first node in the list.
*/

int free_ps_list(struct ps_component * start)
    {
    struct ps_component *current, *hold;

    current = start;
    while(current != 0)
        {
        hold = current->ps_next;
        if(current->ps_text != 0) free(current->ps_text);
        free(current);
        current = hold;
        }
    return(1);
    }
