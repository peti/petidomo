/*
 * Copyright (c) 1995-2010 Peter Simons <simons@cryp.to>
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
#include <string.h>

#include "petidomo.h"

char *
LoadFromDescriptor(int fd)
{
    char *        buffer;
    unsigned int  buffer_size;
    unsigned int  read_size;
    ssize_t       rc;

    buffer_size = 8 * 1024;
    read_size = 0;
    buffer = malloc(buffer_size);
    if (buffer == NULL) {
	syslog(LOG_ERR, "Failed to allocate %u byte of memory.", buffer_size);
	return NULL;
    }

    for (;;) {
	rc = read(fd, (buffer+read_size), (buffer_size - read_size - 1));
	if (rc == -1) {
	    syslog(LOG_ERR, "Error occured while reading file: %s", strerror(errno));
	    free(buffer);
	    return NULL;
	}
	else if (rc == 0) {	/* EOF */
	    break;
	}
	else {			/* Read succeeded normally */
	    read_size += rc;
	    if ((buffer_size - read_size) <= 1) { /* re-allocate larger buffer */
		char *   new_buffer;
		buffer_size += 4 * 1024;
		new_buffer = realloc(buffer, buffer_size);
		if (new_buffer == NULL) {
		    syslog(LOG_ERR, "Failed to allocate %u byte of memory.", buffer_size);
		    free(buffer);
		    return NULL;
		}
		else
		  buffer = new_buffer;
	    }
	}
    }
    buffer[read_size] = '\0';	/* terminate read data */
    errno = read_size;
    return buffer;
}


char *
loadfile(const char *  filename)
{
    struct flock  lock;
    char *        buffer;
    int           fd;
    int           len;
    int            rc;

    assert(filename);

    if ((fd = open(filename, O_RDONLY, 0)) == -1) {
	syslog(LOG_WARNING, "open(\"%s\", O_RDONLY): %s", filename, strerror(errno));
	return NULL;
    }
    lock.l_start  = 0;
    lock.l_len    = 0;
    lock.l_type   = F_RDLCK;
    lock.l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, &lock);
    if ((len = lseek(fd, 0, SEEK_END)) == -1) {
	syslog(LOG_WARNING, "lseek(\"%s\", SEEK_END): %s", filename, strerror(errno));
	return NULL;
    }
    if ((lseek(fd, 0, SEEK_SET) == -1)) {
	syslog(LOG_WARNING, "lseek(\"%s\", SEEK_SET): %s", filename, strerror(errno));
	return NULL;
    }
    buffer = malloc(len+1);
    if (buffer == NULL) {
	syslog(LOG_WARNING, "Failed to allocate %d byte of memory.", len+1);
	return NULL;
    }
    rc = read(fd, buffer, len);
    if (rc != len) {
	syslog(LOG_WARNING, "read(\"%s\", %d) read %d byte: %s", filename, len, rc, strerror(errno));
	return NULL;
    }
    buffer[len] = '\0';
    close(fd);
    errno = len;
    return buffer;
}


int
savefile(const char * filename, const char * buffer)
{
    struct flock  lock;
    int           fd, len;
    ssize_t       rc;

    assert(filename && buffer);

    len = strlen(buffer);
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    lock.l_start  = 0;
    lock.l_len    = 0;
    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, &lock);
    if (fd == -1) {
	syslog(LOG_ERR, "open(\"%s\"): %s", filename, strerror(errno));
	return -1;
    }
    rc = write(fd, buffer, len);
    if (rc == -1) {
	syslog(LOG_ERR, "Error occured while writing to file \"%s\": %s", filename, strerror(errno));
	close(fd);
	return -1;
    }
    close(fd);
    return 0;
}
