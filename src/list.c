
/* vim: set et ts=3 sw=3 ft=c:
 *
 * Copyright (C) 2012 James McLaughlin et al.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "list.h"

/* This file contains private helper functions for the macros defined in list.h.
 *
 * These functions provide the actual linked list logic, keeping it out of the
 * macros (and therefore out of anything but list.o).
 */

struct list_head
{
   list_element * first, * last;
   size_t length;
};

struct list_element
{
   list_head * list;
   list_element * prev, * next;
};

static list_element * get_element (list_element * elem, size_t value_size)
{
   return (list_element *) (((char *) elem) - value_size);
}

size_t _list_length (list_head * list)
{
   return list ? list->length : 0;
}

void _list_push (list_head ** p_list, size_t value_size, void * value)
{
   if (!*p_list)
      *p_list = calloc (sizeof (list_head), 1);

   list_head * list = *p_list;

   ++ list->length;

   list_element * elem = calloc (value_size + sizeof (*elem), 1);
   memcpy (elem, value, value_size);

   elem = get_element (elem, value_size);

   elem->list = list;
   elem->prev = list->last;

   if (list->last)
   {
      list->last->next = elem;
      list->last = elem;
   }
   else
   {
      list->first = list->last = elem;
   }
}

void _list_push_front (list_head ** p_list, size_t value_size, void * value)
{
   if (!*p_list)
      *p_list = calloc (sizeof (list_head), 1);

   list_head * list = *p_list;

   ++ list->length;

   list_element * elem = calloc (value_size + sizeof (*elem), 1);
   memcpy (elem, value, value_size);

   elem = get_element (elem, value_size);

   elem->list = list;
   elem->next = list->first;

   if (list->first)
   {
      list->first->prev = elem;
      list->first = elem;
   }
   else
   {
      list->first = list->last = elem;
   }
}

list_element * _list_front (list_head * list)
{
   return list ? list->first : 0;
}

list_element * _list_back (list_head * list)
{
   return list ? list->last : 0;
}

list_element * _list_next (list_element * elem, size_t value_size)
{
   return elem ? get_element (elem, value_size)->next : 0;
}

list_element * _list_prev (list_element * elem, size_t value_size)
{
   return elem ? get_element (elem, value_size)->prev : 0;
}

void _list_remove (list_element * _elem, size_t value_size)
{
   list_element * elem = get_element (_elem, value_size);

   list_head * list = elem->list;

   -- list->length;

   if (elem->next)
      elem->next->prev = elem->prev;

   if (elem->prev)
      elem->prev->next = elem->next;

   if (elem == list->first)
      list->first = 0;

   if (elem == list->last)
      list->last = 0;

   free (_elem);
}

void _list_clear (list_head ** list, size_t value_size)
{
   if (!*list)
      return;

   list_element * elem = (*list)->first;

   while (elem)
   {
      list_element * next = get_element (elem, value_size)->next;
      free (elem);
      elem = next;
   }

   free (*list);
   *list = 0;
}

