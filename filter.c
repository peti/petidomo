/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1996 by CyberSolutions GmbH.
 *      All rights reserved.
 */

#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <petidomo.h>

#define READ 0
#define WRITE 1
#define IO_BLOCKSIZE (4*1024)

enum {
    STATE_HEADER,
    STATE_SEPERATOR,
    STATE_BODY,
    STATE_FINISHED
};

int
MailFilter(struct Mail * MailStruct, const char * filter)
{
    int     child_in[2];
    int     child_out[2];
    pid_t   child_pid;
    int     rc, i;
    int     write_state;
    int     byte_read, byte_written;
    char *  newmail = NULL;
    int     newmail_size = 0;
    char *  p = NULL;

    assert(MailStruct != NULL);
    assert(filter != NULL);

    debug((DEBUG_FILTER, 2, "Starting mail filter \"%s\".", filter));

    /* Sanity checks. */

    if (MailStruct == NULL || filter == NULL)
      return 0;

    /* Init pipes. */

    if (pipe(child_in) == -1) {
	syslog(LOG_ERR, "Couldn't open a writing-pipe to my child process: %m");
	return -1;
    }
    if (pipe(child_out) == -1) {
	syslog(LOG_ERR, "Couldn't open a read-pipe from my child process: %m");
	return -1;
    }

    /* Fork. */

    child_pid = fork();
    switch(child_pid) {
      case 0:
	  /* Child */
	  close(child_in[WRITE]);
	  close(child_out[READ]);
	  if (dup2(child_in[READ], STDIN_FILENO) == -1) {
	      syslog(LOG_ERR, "Child process couldn't read from pipe: %m");
	      return -1;
	  }
	  if (dup2(child_out[WRITE], STDOUT_FILENO) == -1) {
	      syslog(LOG_ERR, "Child process couldn't read from pipe: %m");
	      return -1;
	  }
	  close(child_in[READ]);
	  close(child_out[WRITE]);
	  debug((DEBUG_FILTER, 2, "Child process is set up. Executing filter."));
	  execl("/bin/sh", "sh", "-c", filter, NULL);
	  return -1;
      case -1:
	  /* Error */
	  close(child_in[READ]);
	  close(child_in[WRITE]);
	  close(child_out[READ]);
	  close(child_out[WRITE]);
	  syslog(LOG_ERR, "Couldn't fork: %m");
	  return -1;
      default:
	  /* everything is fine */
	  close(child_in[READ]);
	  close(child_out[WRITE]);
    }

    /* Switch the pipes into non-blocking mode. */

    rc = fcntl(child_in[WRITE], F_GETFL, 0);
    if (rc == -1) {
	syslog(LOG_ERR, "Couldn't get flags from write-pipe descriptor: %m");
	goto error_exit;
    }
    rc |= O_NONBLOCK;
    rc = fcntl(child_in[WRITE], F_SETFL, rc);
    if (rc == -1) {
	syslog(LOG_ERR, "Couldn't set flags for write-pipe descriptor: %m");
	goto error_exit;
    }
    rc = fcntl(child_out[READ], F_GETFL, 0);
    if (rc == -1) {
	syslog(LOG_ERR, "Couldn't get flags from write-pipe descriptor: %m");
	goto error_exit;
    }
    rc |= O_NONBLOCK;
    rc = fcntl(child_out[READ], F_SETFL, rc);
    if (rc == -1) {
	syslog(LOG_ERR, "Couldn't set flags for write-pipe descriptor: %m");
	goto error_exit;
    }
    debug((DEBUG_FILTER, 4, "Pipes are in non-blocking mode now."));

    /* Now write the mail into the pipe and read the result from the
       child. This has to happen parallely or we risk that the child
       hangs with a blocking i/o. */

    write_state = STATE_HEADER;
    byte_read = 0;
    byte_written = 0;

    for (;;) {
	/* Write to the pipe. */

	switch (write_state) {
	  case STATE_HEADER:
	      p = MailStruct->Header;
	      break;
	  case STATE_SEPERATOR:
	      p = "\n";
	      break;
	  case STATE_BODY:
	      p = MailStruct->Body;
	      break;
	  case STATE_FINISHED:
	      p = NULL;
	      break;
	}
	if (p != NULL) {
	    rc = write(child_in[WRITE], p  + byte_written,
		       (strlen(p + byte_written) > IO_BLOCKSIZE) ?
		       IO_BLOCKSIZE : strlen(p + byte_written));
	    debug((DEBUG_FILTER, 4, "Write returned '%d'.", rc));
	    if (rc >= 0) {
		byte_written += rc;
		if (p[byte_written] == '\0') {
		    debug((DEBUG_FILTER, 2, "New write state"));
		    write_state++; /* new state */
		    byte_written = 0;
		    if (write_state == STATE_FINISHED)
		      close(child_in[WRITE]);
		}
	    }
	    else if (errno != EAGAIN) {
		syslog(LOG_ERR, "Writing to the filter process failed: %m");
		goto error_exit;
	    }
	}

	/* Read from the pipe. */

	if ((newmail_size - byte_read) <= (IO_BLOCKSIZE)) {
	    newmail_size += 10*1024;
	    debug((DEBUG_FILTER, 4, "Allocting new read buffer: %d byte", newmail_size));
	    newmail = realloc(newmail, newmail_size);
	    if (newmail == NULL) {
		syslog(LOG_ERR, "Failed to allocate %d byte of memory: %m", newmail_size);
		goto error_exit;
	    }
	}
	rc = read(child_out[READ], newmail + byte_read, IO_BLOCKSIZE);
	debug((DEBUG_FILTER, 4, "Read returned '%d'.", rc));
	if (rc > 0)
	  byte_read += rc;
	else if (rc == 0) {
	    close(child_out[READ]);
	    break;		/* we are finished */
	}
	else if (errno != EAGAIN) {
	    syslog(LOG_ERR, "Reading from filter process failed: %m");
	    goto error_exit;
	}
	else {
	    debug((DEBUG_FILTER, 4, "read would block"));
	}
    }
    newmail[byte_read] = '\0';

    /* Parse mail and put it into the structure. */

    for (i = 0; newmail[i] != '\0'; i++) {
	if (i > 0 && newmail[i-1] == '\n' && newmail[i] == '\n') {
	    free(MailStruct->Header);
	    MailStruct->Header = newmail;
	    newmail[i] = '\0';
	    MailStruct->Body = newmail+i+1;
	    break;
	}
    }
    debug((DEBUG_FILTER, 5, "New header is:\n%s", MailStruct->Header));
    debug((DEBUG_FILTER, 5, "New body is:\n%s", MailStruct->Body));

    /* Get returncode. */

    waitpid(child_pid, &rc, 0);
    if (!WIFEXITED(rc))
      return -1;

    debug((DEBUG_FILTER, 4, "Filter \"%s\" returned %d.", filter, WEXITSTATUS(rc)));
    return WEXITSTATUS(rc);

    return 0;

error_exit:
    close(child_in[WRITE]);
    close(child_out[READ]);
    kill(child_pid, SIGTERM);
    return -1;
}
