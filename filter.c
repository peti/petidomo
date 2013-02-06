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

#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "petidomo.h"

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

    /* Sanity checks. */

    if (MailStruct == NULL || filter == NULL)
      return 0;

    /* Init pipes. */

    if (pipe(child_in) == -1) {
        syslog(LOG_ERR, "Couldn't open a writing-pipe to my child process: %s", strerror(errno));
        return -1;
    }
    if (pipe(child_out) == -1) {
        syslog(LOG_ERR, "Couldn't open a read-pipe from my child process: %s", strerror(errno));
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
              syslog(LOG_ERR, "Child process couldn't read from pipe: %s", strerror(errno));
              return -1;
          }
          if (dup2(child_out[WRITE], STDOUT_FILENO) == -1) {
              syslog(LOG_ERR, "Child process couldn't read from pipe: %s", strerror(errno));
              return -1;
          }
          close(child_in[READ]);
          close(child_out[WRITE]);
          execl("/bin/sh", "sh", "-c", filter, NULL);
          return -1;
      case -1:
          /* Error */
          close(child_in[READ]);
          close(child_in[WRITE]);
          close(child_out[READ]);
          close(child_out[WRITE]);
          syslog(LOG_ERR, "Couldn't fork: %s", strerror(errno));
          return -1;
      default:
          /* everything is fine */
          close(child_in[READ]);
          close(child_out[WRITE]);
    }

    /* Switch the pipes into non-blocking mode. */

    rc = fcntl(child_in[WRITE], F_GETFL, 0);
    if (rc == -1) {
        syslog(LOG_ERR, "Couldn't get flags from write-pipe descriptor: %s", strerror(errno));
        goto error_exit;
    }
    rc |= O_NONBLOCK;
    rc = fcntl(child_in[WRITE], F_SETFL, rc);
    if (rc == -1) {
        syslog(LOG_ERR, "Couldn't set flags for write-pipe descriptor: %s", strerror(errno));
        goto error_exit;
    }
    rc = fcntl(child_out[READ], F_GETFL, 0);
    if (rc == -1) {
        syslog(LOG_ERR, "Couldn't get flags from write-pipe descriptor: %s", strerror(errno));
        goto error_exit;
    }
    rc |= O_NONBLOCK;
    rc = fcntl(child_out[READ], F_SETFL, rc);
    if (rc == -1) {
        syslog(LOG_ERR, "Couldn't set flags for write-pipe descriptor: %s", strerror(errno));
        goto error_exit;
    }

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
            if (rc >= 0) {
                byte_written += rc;
                if (p[byte_written] == '\0') {
                    write_state++; /* new state */
                    byte_written = 0;
                    if (write_state == STATE_FINISHED)
                      close(child_in[WRITE]);
                }
            }
            else if (errno != EAGAIN) {
                syslog(LOG_ERR, "Writing to the filter process failed: %s", strerror(errno));
                goto error_exit;
            }
        }

        /* Read from the pipe. */

        if ((newmail_size - byte_read) <= (IO_BLOCKSIZE)) {
            newmail_size += 10*1024;
            newmail = realloc(newmail, newmail_size);
            if (newmail == NULL) {
                syslog(LOG_ERR, "Failed to allocate %d byte of memory: %s", newmail_size, strerror(errno));
                goto error_exit;
            }
        }
        rc = read(child_out[READ], newmail + byte_read, IO_BLOCKSIZE);
        if (rc > 0)
          byte_read += rc;
        else if (rc == 0) {
            close(child_out[READ]);
            break;              /* we are finished */
        }
        else if (errno != EAGAIN) {
            syslog(LOG_ERR, "Reading from filter process failed: %s", strerror(errno));
            goto error_exit;
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

    /* Get returncode. */

    waitpid(child_pid, &rc, 0);
    if (!WIFEXITED(rc))
      return -1;

    return WEXITSTATUS(rc);

error_exit:
    close(child_in[WRITE]);
    close(child_out[READ]);
    kill(child_pid, SIGTERM);
    return -1;
}
