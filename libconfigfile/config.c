/*
 * Copyright (c) 1995-2013 Peter Simons <simons@cryp.to>
 * Copyright (c) 2000-2001 Cable & Wireless GmbH
 * Copyright (c) 1999-2000 CyberSolutions GmbH
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>

#include "../liblists/lists.h"
#include "../libtext/text.h"
#include "configfile.h"

char * loadfile(const char *);

static List Files = NULL;

/* Remove a trailing carrige return from a string */

static void
ChopCR(char * p)
{
    while (*p != '\0' && *p != '\n')
      p++;

    *p = '\0';
}

/* Turn a string into the number of an array element that matches this
   string. */

static int
FindCFEntry(struct ConfigFile cf[], const char * keyword)
{
    int  i;

    for (i = 0; (&(cf[i]))->keyword != NULL; i++) {
	if ((strcasecmp((&(cf[i]))->keyword, keyword)) == 0)
	  return i;
    }

    return -1;
}

/* ReadConfig() will read and parse a given config file and fill the
   parsed tags into the ConfigFile structure given on the command
   line. The 'ConfigFile' structure consists of three fields: The name
   of the config tag to look for, a description of how to interpret
   the parameter specified with it and a pointer to a 'void
   pointer'-type, which will contain the result when ReadConfig()
   returns.

   Valid tag types are CF_STRING (result buffer will contain a string
   pointer to the text), CF_INTEGER (parameter will be turned into an
   integer and stored into the result buffer) and CF_YES_NO
   (parameter is expected to be a 'yes', 'no', 'true' or 'false' and
   the appropriate binary setting will be stored).

   RETURNS: A return code of zero (0) indicates success, -1 indicates
   failure. In case of a failure, ReadConfig() will log an error
   description using the syslog(3) mechanism.

 */

int
ReadConfig(const char * filename, /* path to the config file to parse */
	   struct ConfigFile cf[] /* structure describing the config tags */
	   )
{
    Node         node;
    char *       file_buf;
    char *       currLine;
    char *       nextLine;
    char *       keyword;
    char *       data;
    int          rc;
    int          i;
    unsigned int numLine;

    /* Initialize list of loaded config files. */

    if (Files == NULL) {
	Files = InitList(NULL);
	if (Files == NULL) {
	    syslog(LOG_ERR, "ReadConfig: Failed to initialize my internal config file list.");
	    return -1;
	}
    }
    else {
	/* Check whether we already parsed that file. */

	node = FindNodeByKey(Files, filename);
	if (node != NULL)
	  return 0;
    }

    /* Load the file into memory. */

    file_buf = loadfile(filename);
    if (file_buf == NULL) {
	syslog(LOG_WARNING, "ReadConfig: Failed to load config file \"%s\": %s", filename, strerror(errno));
	return -1;
    }

    /* Store the buffer details in the linked list. */

    filename = strdup(filename);
    if (filename == NULL) {
	syslog(LOG_ERR, "ReadConfig: Failed to load config file \"%s\": %s", filename, strerror(errno));
	return -1;
    }
    node = AppendNode(Files, filename, file_buf);
    if (node == NULL) {
	syslog(LOG_ERR, "ReadConfig: Internal Error: Couldn't add config file data to my list.");
	return -1;
    }

    /* Parse the file line by line. */

    for (numLine = 1, nextLine = currLine = file_buf; *currLine != '\0'; currLine = nextLine, numLine++) {
	nextLine = text_find_next_line(currLine);
	ChopCR(currLine);

	/* Ignore comments or empty lines. */

	if ((text_easy_pattern_match(currLine, "^[\t ]*#|^[\t ]*$")) == TRUE)
	  continue;		/* ignore it */

	/* Line is supposed to contain a config statement, so we
           should do a consistency check first. If the line passes,
           there're no surprises when we actually parse it. */

	if ((text_easy_pattern_match(currLine, "^[[:alnum:]_-]+[[:space:]]+[^[:space:]]+.*[^[:space:]]+[[:space:]]*$")) == FALSE) {
	    syslog(LOG_WARNING, "ReadConfig: Line \"%s\" is syntactically incorrect.",
		   currLine);
	    continue;		/* ignore it */
	}

	/* Remove all unnecessary whitespace. */

	rc = text_transform_text(currLine, currLine, "^([[:alnum:]_-]+)[[:space:]]+([^[:space:]]+.*[^[:space:]]+)[[:space:]]*$", "\\1 \\2");
	if (rc != 0) {
	    syslog(LOG_WARNING, "ReadConfig: Internal error while parsing line: %d.", rc);
	    continue;		/* ignore it */
	}

	/* Locate the keyword and data part. */

	for (keyword = currLine; *currLine != ' '; currLine++)
	  ;
	*currLine++ = '\0';
	data = currLine;

	/* Find appropriate entry in the ConfigFile structure and
           check whether we know the keyword. */

	i = FindCFEntry(cf, keyword);
	if (i == -1) {
	    syslog(LOG_WARNING, "ReadConfig: Unrecognized keyword \"%s\" in file \"%s\", line %d.",
		   keyword, filename, numLine);
	    continue;		/* ignore it */
	}

	/* Determine type of data and store it appropriately. */

	switch ((&(cf[i]))->type) {
	  case CF_STRING:
	      /* Handle strings included in quotes. */

	      rc = text_transform_text(data, data, "^\"([^\"]*)\"$", "\\1");
	      if (rc != 0 && rc != TEXT_REGEX_TRANSFORM_DIDNT_MATCH) {
		  syslog(LOG_WARNING, "ReadConfig: Internal error while parsing file \"%s\", line: %d.",
		      filename, numLine);
		  continue;
	      }

	      /* Store the string. */

	      if ((&(cf[i]))->data != NULL)
		*((char **)(&(cf[i]))->data) = data;
	      break;

	  case CF_INTEGER:
	      /* Integers will have an additional consistency check to
                 warn the user about typing errors. */

	      if ((text_easy_pattern_match(data, "^[-+][[:digit:]]+$|^[[:digit:]]+$")) == FALSE) {
		  syslog(LOG_WARNING, "ReadConfig: Specified parameter \"%s\" in file \"%s\", line %d, is not a number.", data, filename, numLine);
		  continue;		/* ignore it */
	      }
	      if ((&(cf[i]))->data != NULL)
		*((int *)(&(cf[i]))->data) = atoi(data);
	      break;
	  case CF_YES_NO:
	      if ((text_easy_pattern_match(data, "^yes$|^true$|^y$")) == TRUE) {
		  if ((&(cf[i]))->data != NULL)
		    *((bool *)(&(cf[i]))->data) = TRUE;
	      } else if ((text_easy_pattern_match(data, "^no$|^false$|^n$")) == TRUE) {
		  if ((&(cf[i]))->data != NULL)
		    *((bool *)(&(cf[i]))->data) = FALSE;
	      } else {
		  syslog(LOG_WARNING, "ReadConfig: Specified parameter \"%s\" in file \"%s\", line %d, is not a yes or a no.", data, filename, numLine);
		  continue;		/* ignore it */
	      }
	      break;
	  case CF_MULTI:
	      syslog(LOG_WARNING, "ReadConfig: Method not supported at the moment.");
	      break;
	  default:
	      syslog(LOG_ERR, "ReadConfig internal error: ConfigFile structure element %d has unknown type %d.", i, (&(cf[i]))->type);
	}

    }

    return 0;
}

/* This routine must be used to return all resources to the system,
   that have been allocaged by a specific ReadConfig(3) call. After
   that, all result buffers provided by ReadConfig() will be invalid. */

void
FreeConfig(const char * filename)
{
    Node   node;

    node = FindNodeByKey(Files, filename);
    if (node == NULL)
      return;

    free(getNodeData(node));
    free(getNodeKey(node));
    RemoveNode(node);
    FreeNode(node);
}

/* This function call will free all resources for all previous
   ReadConfig(3) calls.

   BUGS: Not implemented at the moment.
*/

void
FreeAllConfigs(void)
{

}

static char *
LocateKeywordInBuffer(char * buffer, char * keyword)
{
    char *   p;

    for (p = buffer; p != NULL; ) {
	p = text_find_string(p, keyword);
	if (p != NULL) {
	    if (p == buffer || p[-1] == '\n' || p[-1] == '\r')
	      break;
	    else
	      p++;		/* Not a keyword, look again. */
	}
    }

    return p;
}

/* Load a config file and parse it for the required keyword. Then
   return the parameter. The buffer containing the parameter should be
   free()'d by the caller. #

   RETURNS: If the keyword is not found, NULL is returned and errno is
   set to 0. If an error occurs, NULL is returned also, but errno is
   set accordingly.
*/

char *
GetConfig(const char * filename, char * keyword)
{
    char *   file_buf;
    char *   result_buf;
    char *   p;
    int      rc;

    /* Load the config file. */

    file_buf = loadfile(filename);
    if (file_buf == NULL) {
	syslog(LOG_ERR, "GetConfig: Failed to load config file \"%s\": %s", filename, strerror(errno));
	return NULL;
    }

    /* Find the keyword. */

    p = LocateKeywordInBuffer(file_buf, keyword);

    if (p == NULL) {
	/* No luck, keyword doesn't exist. */
	errno = 0;
	return NULL;
    }

    /* Okay, keyword found. Now extract the parameter. */

    ChopCR(p);
    rc = text_transform_text(p, p, "^[^\t ]+[\t ]+([^\t ].*)[\t ]*$", "\\1");
    if (rc != 0) {
	syslog(LOG_WARNING, "GetConfig: Config file entry \"%s\" is corrupt.", keyword);
	errno = -1;
	return NULL;
    }
    rc = text_transform_text(p, p, "^\"([^\"]*)\"$", "\\1");
    if (rc != 0 && rc != TEXT_REGEX_TRANSFORM_DIDNT_MATCH) {
	syslog(LOG_WARNING, "GetConfig: Config file entry \"%s\" is corrupt.", keyword);
	errno = -1;
	return NULL;
    }


    /* Copy the parameter into a new buffer. */

    result_buf = strdup(p);
    if (result_buf == NULL) {
	syslog(LOG_ERR, "GetConfig: Failed to allocate %d byte of memory.", strlen(p));
	errno = ENOMEM;
	return NULL;
    }

    /* Free the used memory and return the result. */

    free(file_buf);
    return result_buf;
}

/* This routine will set a given config entry in a config file. When
   the keyword is already there, the parameter will be changed.
   Otherwise the string "keyword<tab>data" is appended at the end of
   the file.

   RETURNS: If something goes wrong, the routine returns -1 and sets
   errno. Otherwise we return 0.

   BUGS: This routine may DESTRUCT THE FILE if some I/O error occurs.
   It is the caller's responsibility to backup the file before calling
   us.
*/

int
SetConfig(const char * filename,
	  char * keyword,	/* string pointer to the tag name */
	  const char * data	/* string pointer to the tag data */
	  )
{
    struct flock  lock;
    char *        file_buf;
    int           file_len;
    char *        p;
    char *        nextLine;
    int           fd;
    int           rc;

    /* Load the config file. */

    file_buf = loadfile(filename);
    if (file_buf == NULL) {
	syslog(LOG_ERR, "SetConfig: Failed to load config file \"%s\": %s", filename, strerror(errno));
	return -1;
    }
    file_len = errno;

    /* Find the keyword. */

    p = LocateKeywordInBuffer(file_buf, keyword);

    if (p == NULL) {
	/* No luck, keyword doesn't exist. Append the data. */

	fd = open(filename, O_WRONLY | O_APPEND, 0666);
	if (fd == -1)
	  goto io_error;

	lock.l_start  = 0;
	lock.l_len    = 0;
	lock.l_type   = F_WRLCK;
	lock.l_whence = SEEK_SET;
	fcntl(fd, F_SETLKW, &lock);

	/* Does the file end in a '\n' or must we append one? */

	if (file_buf[file_len - 1] != '\n') {
	    rc = write(fd, "\n", 1);
	    if (rc == -1)
	      goto io_error;
	}

	/* Write the actual keyword and data. */

        if ((write(fd, keyword, strlen(keyword)) == -1)
	    || (write(fd, "\t", 1) == -1)
	    || (write(fd, data, strlen(data)) == -1)
	    || (write(fd, "\n", 1) == -1)) {
	    goto io_error;
	}
    }
    else {
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd == -1)
	  goto io_error;

	lock.l_start  = 0;
	lock.l_len    = 0;
	lock.l_type   = F_WRLCK;
	lock.l_whence = SEEK_SET;
	fcntl(fd, F_SETLKW, &lock);

	nextLine = text_find_next_line(p);
	*p = '\0';

	/* Write buffer back until the to-be-replaced keyword. */

	rc = write(fd, file_buf, strlen(file_buf));
	if (fd == -1)
	  goto io_error;

	/* Write the actual keyword and data. */

        if ((write(fd, keyword, strlen(keyword)) == -1)
	    || (write(fd, "\t", 1) == -1)
	    || (write(fd, data, strlen(data)) == -1)
	    || (write(fd, "\n", 1) == -1)) {
	    goto io_error;
	}

	/* Write the rest of the buffer. */

	rc = write(fd, nextLine, strlen(nextLine));
	if (fd == -1)
	  goto io_error;
    }

    close(fd);
    free(file_buf);
    return 0;

io_error:
    syslog(LOG_ERR, "SetConfig: Failed to write to file \"%s\": %s", filename, strerror(errno));
    free(file_buf);
    return -1;
}
