
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

char     *Headers[] =
{
    "From",
    "Reply-To",
    "Date",
    "Subject",
    "Message-ID",
    "MIME-Version",
    "Content-Type",
    "Content-Transfer-Encoding",
    "In-Reply-To",
    "References",
    NULL
};

#define RNEWS "/usr/local/news/bin/rnews/rnews"

int 
main(int argc, char *argv[])
{
    struct Store *ST;
    char      Buffer[16384],
             *References,
             *Ptr,
             *NewReferences;
    int       Index,
              Length,
              In;
    FILE     *Pipe;

    if (argc != 4) {
	(void) fprintf(stderr, "Usage: %s group path approved.\n", argv[0]);
	return 1;
    }

    if (fgets(Buffer, sizeof(Buffer), stdin) == NULL) {
	(void) fprintf(stderr, "%s: error reading mail.\n", argv[0]);
	return 1;
    }

    if ((Pipe = popen(RNEWS, "w")) == NULL) {
	(void) fprintf(stderr, "%s: can't open pipe.\n", argv[0]);
	return 1;
    }

    References = NULL;
    do {
	if (fgets(Buffer, sizeof(Buffer), stdin) == NULL) {
	    (void) fprintf(stderr, "%s: error reading mail.\n", argv[0]);
	    return 1;
	}
	if (Ptr = strchr(Buffer, '\n'))
	    *Ptr = '\0';

	Index = 0;
	Ptr = NULL;
	while (Headers[Index]) {
	    Length = strlen(Headers[Index]);
	    if ((strncasecmp(Buffer, Headers[Index], Length) == 0) &&
		(Buffer[Length] == ':')) {
		Ptr = &Buffer[Length + 1];
		while ((*Ptr == '\t') || (*Ptr == ' '))
		    Ptr++;
		break;
	    }
	    Index++;
	}

	switch (Index) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	    (void) fprintf(Pipe, "%s: %s\n", Headers[Index], Ptr);
	    break;
	case 8:
	    if ((References == NULL) && ((Ptr = strchr(Ptr, '<')) != NULL)) {
		char     *Braket;

		if (Braket = strchr(Ptr, '>')) {
		    Braket[1] = '\0';
		    References = strdup(Ptr);
		}
	    }
	    break;
	case 9:
	    if ((NewReferences = strdup(Ptr)) == NULL)
		break;

	    if (References)
		free(References);
	    References = NewReferences;

	    while ((In = fgetc(stdin)) != EOF)
		if ((In != '\t') && (In != ' ')) {
		    (void) ungetc(In, stdin);
		    break;
		}
		else {
		    Buffer[0] = (char) In;
		    if (fgets(&Buffer[1], sizeof(Buffer) - 1, stdin) == NULL) {
			(void) fprintf(stderr, "%s: error reading mail.\n", argv[0]);
			return 1;
		    }
		    if (Ptr = strchr(Buffer, '\n'))
			*Ptr = '\0';

		    Ptr = &Buffer[1];
		    while ((*Ptr == '\t') || (*Ptr == ' '))
			Ptr++;

		    if (*Ptr)
			if (NewReferences = malloc(strlen(References) + strlen(Ptr) + 2)) {
			    (void) strcat(strcat(strcpy(NewReferences, References), " "), Ptr);
			    free(References);
			    References = NewReferences;
			}
		}
	}
    }
    while (Buffer[0]);

    if (References) {
	(void) fprintf(Pipe, "References: %s\n", References);
	free(References);
    }
    (void) fprintf(Pipe, "Newsgroups: %s\n", argv[1]);
    if (strcmp(argv[2], "none") == 0)
	(void) fputs("Path: not-for-mail\n", Pipe);
    else
	(void) fprintf(Pipe, "Path: %s!not-for-mail\n", argv[2]);
    (void) fprintf(Pipe, "Approved: %s\n", argv[3]);

    (void) fputc('\n', Pipe);
    while ((Length = fread(Buffer, 1, sizeof(Buffer), stdin)) > 0)
	(void) fwrite(Buffer, 1, Length, Pipe);

    (void) pclose(Pipe);

    return 0;
}
