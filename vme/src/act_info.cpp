/*
 $Author: All $
 $RCSfile: act_info.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "files.h"
#include "skills.h"
#include "db.h"
#include "spells.h"
#include "vmelimits.h"
#include "affect.h"
#include "utility.h"
#include "trie.h"
#include "textutil.h"
#include "money.h"
#include "protocol.h"
#include "constants.h"
#include "common.h"
#include "guild.h"

/* extern variables */

extern class descriptor_data *descriptor_list;
extern class unit_data *unit_list;

static void add_to_string(char **buf, int *size, int len, const char *str)
{
    if (*buf == NULL)
    {
        CREATE(*buf, char, *size);
        **buf = '\0';
    }
    else if (*size < len + 1)
    {
        *size *= 2;
        RECREATE(*buf, char, *size);
    }

    strcat(*buf, str);
}

/* also used in "corpses" wizard-command */
char *in_string(class unit_data *ch, class unit_data *u)
{
    static char in_str[512];
    char *tmp = in_str;

    while ((u = UNIT_IN(u)))
        if (IS_ROOM(u))
        {
            sprintf(tmp, "<a href='#' cmd='goto #'>%s@%s</a>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
            return in_str;
        }
        else
        {
            sprintf(tmp, "%s/", UNIT_SEE_NAME(ch, u));
            TAIL(tmp);
        }

    /*  error(HERE, "Something that is UNIT_IN, not in a room!");*/
    return (NULL);
}

void player_where(class unit_data *ch, char *arg)
{
    char buf[160];
    class descriptor_data *d;
    int any = FALSE;

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->character && (d->character != ch) && UNIT_IN(d->character) && descriptor_is_playing(d) && (str_is_empty(arg) || !str_ccmp(arg, UNIT_NAME(d->character))) && CHAR_LEVEL(ch) >= UNIT_MINV(d->character) && d->original == NULL && CHAR_CAN_SEE(ch, d->character) &&
            unit_zone(ch) == unit_zone(d->character))
        {
            sprintf(buf, "%-30s at %s<br/>", UNIT_NAME(d->character),
                    TITLENAME(unit_room(d->character)));
            send_to_char(buf, ch);
            any = TRUE;
        }
    }

    if (!any)
    {
        if (str_is_empty(arg))
            send_to_char("No other visible players in this area.<br/>", ch);
        else
            send_to_char("No such player found in this area.<br/>", ch);
    }
}

void do_where(class unit_data *ch, char *aaa, const struct command_info *cmd)
{
    char *buf = NULL, buf1[MAX_STRING_LENGTH], buf2[512];
    register class unit_data *i;
    class descriptor_data *d;
    int len, cur_size = 2048;
    char *arg = (char *)aaa;

    if (IS_MORTAL(ch))
    {
        player_where(ch, arg);
        return;
    }

    if (str_is_empty(arg))
    {
        strcpy(buf1, "<u>Players</u><br/>");
        len = strlen(buf1);
        add_to_string(&buf, &cur_size, len, buf1);

        for (d = descriptor_list; d; d = d->next)
            if (d->character && UNIT_IN(d->character) && descriptor_is_playing(d) && CHAR_LEVEL(ch) >= UNIT_MINV(d->character) && (d->original == NULL || CHAR_LEVEL(ch) >= UNIT_MINV(d->original)))
            {
                if (d->original) /* If switched */
                    sprintf(buf2, " In body of %s", UNIT_NAME(d->character));
                else
                    buf2[0] = '\0';

                sprintf(buf1, "%-20s - %s [%s]%s<br/>",
                        UNIT_NAME(CHAR_ORIGINAL(d->character)),
                        UNIT_SEE_TITLE(ch, UNIT_IN(d->character)),
                        in_string(ch, d->character), buf2);
                len += strlen(buf1);
                add_to_string(&buf, &cur_size, len, buf1);
            }
    }
    else /* Arg was not empty */
    {
        len = 0;
        add_to_string(&buf, &cur_size, len, "");

        for (i = unit_list; i; i = i->gnext)
            if (UNIT_IN(i) && UNIT_NAMES(i).IsName(arg) && CHAR_LEVEL(ch) >= UNIT_MINV(i))
            {

                sprintf(buf1, "%-30s - %s [%s]<br/>",
                        TITLENAME(i),
                        UNIT_SEE_TITLE(ch, UNIT_IN(i)),
                        (!in_string(ch, i) ? "MENU" : in_string(ch, i)));

                len += strlen(buf1);

                add_to_string(&buf, &cur_size, len, buf1);
            }
    }

    if (*buf == '\0')
        send_to_char("Couldn't find any such thing.<br/>", ch);
    else
        page_string(CHAR_DESCRIPTOR(ch), buf);

    FREE(buf);
}
