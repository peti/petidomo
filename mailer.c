/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#include <text.h>
#include <petidomo.h>

#ifndef ARG_NUM_MAX
#  define ARG_NUM_MAX 4096
#endif
#ifndef ARG_MAX
#  define ARG_MAX 4096
#endif

static char *
my_strcpy(char * dst, const char * src)
{
    while((*dst++ = *src++) != '\0')
      ;
    return dst-1;
}

FILE *
OpenMailer(const char * envelope, const char * recipients[])
{
    assert(1==0);
    return NULL;
}

FILE *
vOpenMailer(const char * envelope, ...)
{
    const struct PD_Config *   MasterConfig;
    va_list                    ap;
    FILE *                     fh;
    char *                     cmdline;
    char *                     p;
    const char *               q;
    const char *               options;
    unsigned int               cmdline_len;

    MasterConfig = getMasterConfig();

    debug((DEBUG_MAILER, 2, "MTA is \"%s\".", MasterConfig->mta));
    debug((DEBUG_MAILER, 2, "MTA options are \"%s\".", MasterConfig->mta_options));
    debug((DEBUG_MAILER, 2, "Envelope is \"%s\".", envelope));

    /* Determine the length of the required buffer. */

    cmdline_len = strlen(MasterConfig->mta);
    cmdline_len += strlen(MasterConfig->mta_options);
    cmdline_len += strlen(envelope);
    va_start(ap, envelope);
    while ((q = va_arg(ap, const char *)) != NULL) {
	debug((DEBUG_MAILER, 2, "Recipient: \"%s\".", q));
	cmdline_len += strlen(q) + 1;
    }
    va_end(ap);
    cmdline = xmalloc(cmdline_len+8); /* we don't take any risks :) */
    debug((DEBUG_MAILER, 3, "Command line will be %u byte long.", cmdline_len));

    /* Copy the mta's path and name into the buffer. */

    p = my_strcpy(cmdline, MasterConfig->mta);
    *p++ = ' ';

    /* Copy the mta's options into the array, while replacing '%s'
       with the envelope. */

    for (options = MasterConfig->mta_options; *options != '\0'; ) {
	debug((DEBUG_MAILER, 4, "Parsing '%c' character.", *options));
	if (options[0] == '%' && options[1] == 's') {
	    p = my_strcpy(p, envelope);
	    *p++ = ' ';
	    options += 2;
	    break;
	}
	else {
	    debug((DEBUG_MAILER, 4, "Wrote '%c' to aray.", *options));
	    *p++ = *options++;
	}
    }
    *p++ = ' ';

    /* Append the list of recipients. */

    va_start(ap, envelope);
    while ((q = va_arg(ap, const char *)) != NULL) {
	p = my_strcpy(p, q);
	*p++ = ' ';
    }
    p[-1] = '\0';
    va_end(ap);

    debug((DEBUG_MAILER, 1, "Starting up \"%s\".", cmdline));

    fh = popen(cmdline, "w");
    if (fh == NULL)
      syslog(LOG_ERR, "Failed opening pipe to \"%s\": %m", cmdline);

    free(cmdline);
    return fh;
}


int
CloseMailer(FILE * fh)
{
    return pclose(fh);
}


static int
my_strlen(const char * p)
{
    u_int  i;
    for (i = 0; *p && !isspace((int)*p); p++)
      i++;
    return i;
}

#define MYPIPE_READ fildes[0]
#define MYPIPE_WRITE fildes[1]

int
ListMail(const char * envelope, const char * listname, const struct Mail * MailStruct)
{
    const struct PD_Config * MasterConfig = getMasterConfig();
    char **   arguments;
    u_int     arguments_num = 256;
    char      buffer[256];
    char *    listfile;
    char *    nextAddress;
    char *    currAddress;
    char *    p;
    u_int     counter;
    u_int     len;
    u_int     address_byte;
    u_int     max_address_byte;
    int       fildes[2];
    pid_t     child_pid;
    int       child_status;

    /* Initialize internal stuff. */

    arguments = xmalloc((arguments_num+1) * sizeof(char *));
    max_address_byte = ARG_MAX - strlen(envelope) - strlen(MasterConfig->mta) -
	strlen(MasterConfig->mta_options) - 8;

    /* Load the list of recipients. */

    sprintf(buffer, "lists/%s/list", listname);
    listfile = loadfile(buffer);
    if (listfile == NULL)
      return 1;

    /* Now go into delivery loop until we're finished. */

    for(counter = 0, currAddress = listfile; *currAddress != '\0'; counter = 0) {

	/* Set up the call to the MTA, including options. */

	arguments[counter++] = MasterConfig->mta;
	debug((DEBUG_MAILER, 5, "MTA is \"%s\".", arguments[0]));
	sprintf(buffer, MasterConfig->mta_options, envelope);
	debug((DEBUG_MAILER, 5, "MTA options are \"%s\".", buffer));
	for (p = buffer, arguments[counter++] = buffer; *p != '\0'; p++) {
	    debug((DEBUG_MAILER, 9, "Left to parse: \"%s\".", p));
	    if (isspace((int)*p)) {
		*p++ = '\0';
		debug((DEBUG_MAILER, 9, "Left to parse: \"%s\".", p));
		while(*p != '\0' && isspace((int)*p))
		  p++;
		debug((DEBUG_MAILER, 9, "Left to parse: \"%s\".", p));
		arguments[counter++] = p;
	    }
	}
	if (strlen(arguments[counter-1]) == 0)
	  counter--;

	/* Append as many recipients as fit. */

	for (address_byte = 0; *currAddress != '\0' ; currAddress = nextAddress) {
	    nextAddress = text_find_next_line(currAddress);
	    len = my_strlen(currAddress);
	    if (address_byte + len > max_address_byte) {
		debug((DEBUG_MAILER, 1, "Sending early, command line exceeds %d characters.", ARG_MAX));
		break;
	    }
	    if (counter > ARG_NUM_MAX) {
		debug((DEBUG_MAILER, 1, "Sending early, command line exceeds %d arguments.", ARG_NUM_MAX));
		break;
	    }
	    currAddress[len] = '\0';
	    debug((DEBUG_MAILER, 8, "Address \"%s\" is %u byte long.", currAddress, len));
	    address_byte += len;
	    arguments[counter++] = currAddress;
	    if (counter+8 >= arguments_num) {
		debug((DEBUG_MAILER, 1, "Enlarging internal array."));
		arguments_num += 256;
		arguments = realloc(arguments, (arguments_num+1) * sizeof(char *));
		if (arguments == NULL)
		  return -1;
	    }
	}

	/* Deliver the mail. */

	arguments[counter++] = NULL;
	if (pipe(fildes) == -1) {
	    syslog(LOG_ERR, "Couldn't open a pipe to my child process: %m");
	    return -1;
	}
	child_pid = fork();
	switch(child_pid) {
	  case 0:
	      /* Child */
	      close(MYPIPE_WRITE);
	      if (dup2(MYPIPE_READ, STDIN_FILENO) == -1) {
		  syslog(LOG_ERR, "Child process couldn't read from pipe: %m");
		  return -1;
	      }
	      close(MYPIPE_READ);
	      execv(MasterConfig->mta, arguments);
	      syslog(LOG_ERR, "Couldn't exec(\"%s\"): %m", MasterConfig->mta);
	      return -1;
	  case -1:
	      /* Error */
	      syslog(LOG_ERR, "Couldn't fork: %m");
	      return -1;
	  default:
	      /* everything is fine */
	      close(MYPIPE_READ);
	}
	write(MYPIPE_WRITE, MailStruct->Header, strlen(MailStruct->Header));
	write(MYPIPE_WRITE, "\n", 1);
	write(MYPIPE_WRITE, MailStruct->Body, strlen(MailStruct->Body));
	if (MailStruct->ListSignature != NULL)
	  write(MYPIPE_WRITE, MailStruct->ListSignature, strlen(MailStruct->ListSignature));
	close(MYPIPE_WRITE);
	waitpid(child_pid, &child_status, 0);
	if (!(WIFEXITED(child_status) && WEXITSTATUS(child_status) == 0)) {
	    syslog(LOG_ERR, "The executed mail agent return error %d, aborting.",
		WEXITSTATUS(child_status));
	    return -1;
	}
    }
    return 0;
}
