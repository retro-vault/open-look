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
#include <stdlib.h>

#include <stdio.h>
#include "pan.h"

int LLM_init(struct LLM_root * root, int size)
    {
    if(root == 0)
        {
        return(0);
        }
    root->First   = 0;
    root->Last    = 0;
    root->Current = 0;
    root->Size    = size + sizeof(struct LLM_node);
    root->Init    = 1;
    return(1);
    }

char *LLM_add(struct LLM_root * root)
    {
    struct LLM_node *hold, *temp;

    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    hold = (struct LLM_node *) malloc(root->Size);
    if(hold == 0)
        {
        return(0);
        }
    if(root->First == 0)
        {
        root->First = hold;
        root->Last = hold;
        root->Current = hold;
        hold->Next = 0;
        hold->Prev = 0;
        }
    else
        {
        temp = root->Last;
        temp->Next = hold;
        hold->Next = 0;
        hold->Prev = temp;
        root->Last = hold;
        }
    return( (char *) (hold + 1));
    }

char *LLM_first(struct LLM_root * root)
    {
    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(root->First == 0)
        {
        return(0);
        }
    root->Current = root->First;
    return((char *) (root->Current + 1));
    }

char *LLM_last(struct LLM_root * root)
    {
    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(root->Last == 0)
        {
        return(0);
        }
    root->Current = root->Last;
    return((char *) (root->Current + 1));
    }

char *LLM_next(struct LLM_root * root)
    {
    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(root->First == 0)
        {
        return(0);
        }
    if(root->Current == 0)
        {
        return(0);
        }
    if(root->Current == root->Last)
        {
        return(0);
        }
    root->Current = root->Current->Next;
    return((char *) (root->Current + 1));
    }

char *LLM_previous(struct LLM_root * root)
    {
    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(root->First == 0)
        {
        return(0);
        }
    if(root->Current == 0)
        {
        return(0);
        }
    if(root->Current == root->First)
        {
        return(0);
        }
    root->Current = root->Current->Prev;
    return((char *) (root->Current + 1));
    }

int LLM_free(struct LLM_root * root)
    {
    struct LLM_node *temp, *hold;

    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    temp = root->First;
    while(temp != 0)   /* Loop until we're at the end. */
        {
        hold = temp;
        temp = temp->Next;
        (void)free((char *)hold);       /* Free up our current node. */
        }
    root->First   = 0;
    root->Last    = 0;
    root->Current = 0;
    return(1);
    }

int LLM_delete(struct LLM_root *root, void *node)
    {
    struct LLM_node *temp;

    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(node == 0)
        {
        return(0);
        }
    temp = (struct LLM_node *) node;
    temp--;

    if(temp->Prev != 0)
        temp->Prev->Next = temp->Next;
    else
        root->First = temp->Next;

    if(temp->Next != 0)
        temp->Next->Prev = temp->Prev;
    else
        root->Last = temp->Prev;

    if(temp == root->Current) root->Current = root->First;

    (void)free((char *)temp);
    return(1);
    }

char *LLM_insert(struct LLM_root * root, char * node)
    {
    struct LLM_node *temp, *hold;

    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(node == 0)
        {
        return(0);
        }
    temp = (struct LLM_node *) node;
    temp--;
    hold = (struct LLM_node *) malloc(root->Size);
    if(hold == 0)
        {
        return(0);
        }
    if(temp == root->First)
        {
        hold->Next = root->First;
        hold->Next->Prev = hold;
        root->First = hold;
        hold->Prev = 0;
        return( (char *) (hold + 1));
        }
    hold->Next = temp;
    hold->Prev = temp->Prev;
    temp->Prev = hold;
    hold->Prev->Next = hold;

    return( (char *) (hold + 1));
    }

int LLM_position(struct LLM_root * root, char * node)
    {
    struct LLM_node *temp;

    if(root == 0)
        {
        return(0);
        }
    if(node == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    temp = (struct LLM_node *) node;
    temp--;
    root->Current = temp;
    return(1);
    }

char *LLM_current(struct LLM_root * root)
    {
    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(root->Current == 0)
        {
        return(0);
        }
    return((char *) (root->Current + 1));
    }

int LLM_unlink(struct LLM_root * root, char * node)
    {
    struct LLM_node *temp;

    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(node == 0)
        {
        return(0);
        }
    temp = (struct LLM_node *) node;
    temp--;

    if(temp->Prev != 0)
        temp->Prev->Next = temp->Next;
    else
        root->First = temp->Next;

    if(temp->Next != 0)
        temp->Next->Prev = temp->Prev;
    else
        root->Last = temp->Prev;

    if(temp == root->Current) root->Current = root->First;

    return(1);
    }

int LLM_link(struct LLM_root * root, char * node, char * newnode)
    {
    struct LLM_node *temp, *hold;

    if(root == 0)
        {
        return(0);
        }
    if(root->Init != 1)
        {
        return(0);
        }
    if(newnode == 0)
        {
        return(0);
        }
    hold = (struct LLM_node *) newnode;
    hold--;
    if(node == 0 && root->First != 0) /* link at end */
        {
        temp = root->Last;
        temp->Next = hold;
        hold->Next = 0;
        hold->Prev = temp;
        root->Last = hold;
        return(1);
        }
    if(node == 0 && root->First == 0) /* first node */
        {
        root->First = hold;
        root->Last = hold;
        root->Current = hold;
        hold->Next = 0;
        hold->Prev = 0;
        return(1);
        }
    temp = (struct LLM_node *) node;
    temp--;
    if(temp == root->First)
        {
        hold->Next = root->First;
        hold->Next->Prev = hold;
        root->First = hold;
        hold->Prev = 0;
        return(1);
        }
    hold->Next = temp;
    hold->Prev = temp->Prev;
    temp->Prev = hold;
    hold->Prev->Next = hold;

    return(1);
    }
