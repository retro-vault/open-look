#include <errno.h>
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
#include <stdio.h>
#include <stdlib.h>

#include "pan.h"



int LLM_link(struct LLM_root * root, char * node, char * newnode);
int LLM_unlink(struct LLM_root * root, char * node);

char *add_sorted(struct LLM_root * nr, char * title)
    {
    struct Note *np;

    np = (struct Note *)LLM_first(nr);
    if(np == 0)
        {
        return(LLM_add(nr));
        }
    while(np != 0 && strcmp(title, np->ntitle) >= 0)
        {
        np = (struct Note *)LLM_next(nr);
        }
    if(np == 0)
        {
        return(LLM_add(nr));
        }
    return(LLM_insert(nr, (char *)np));
    }

/* adjust nodes position in list since the title may have changed */
int adjust_sorted(struct Note * np)
    {
    struct LLM_root *nr;
    struct Note *tp;

    nr = &np->sp->note_rt;
    LLM_unlink(nr, (char *)np);
    tp = (struct Note *)LLM_first(nr);
    while(tp != 0 && strcmp(np->ntitle, tp->ntitle) >= 0)
        {
        tp = (struct Note *)LLM_next(nr);
        }
    LLM_link(nr, (char *)tp, (char *)np);
    }
