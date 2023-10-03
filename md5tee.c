#include <stdio.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE (64*1024) /* 64kb */

/* must be equal to s3 multipart_chunk_size_mb, see $HOME/.s3cfg */
#define CHUNKSIZE_DEFAULT (15*1024*1024)

void bintohex(char * dst, unsigned char * src) {
	int i;
	for (i=0; i<16; i++) sprintf(dst+(i*2),"%02x",src[i]);
	dst[32] = 0;
}

int main (int argc, char ** argv) {
	MD5_CTX c, d, e;
	unsigned char md[16], *buf;
	char md5s[33], md5t[70];
	int n;
	char *name = NULL;
	ssize_t i, size = 0, chunks = 0, chunksize = 0;
	ssize_t chunksize_cfg = CHUNKSIZE_DEFAULT;
	int append = 0; // Append to file
	if (getenv("CHUNKSIZE") != NULL) {
		chunksize_cfg = atoi(getenv("CHUNKSIZE"));
	}

	if ((chunksize_cfg < 0) ||
			((chunksize_cfg % BUFSIZE) != 0)) {
		fprintf(stderr,"md5tee: ERROR: CHUNKSIZE must be multiple of %d or 0 to disable.\n",BUFSIZE);
		return 1;
	}

    // parse the command line
    while ((n = getopt(argc, argv, "avhn:")) >= 0) {
        switch (n) {
            case 'a':
            	append = 1;
            	break;
            case 'v':
            	printf("md5tee version 2.0\n");
            	break;
            case 'n':
            	name = optarg;
            	break;
            case 'h': 
            	printf("Description: pipes content from stdin to stdout and calculates its size and md5sum\n\n"
				"Usage: command1 | md5tee | command2\n"
				"Description: pipes from command1 to command2. writes md5sum to stderr\n\n"
				"Usage: command1 | md5tee filename | command2\n"
				"Description: pipes from command1 to command2. writes md5sum to filename\n"
				"Note: md5tee will create the output file, it must not exists previously\n\n"
				"Output format: MD5SUM\n"
				"OR command1 | md5tee filename -n name1 | command2\n"
				"Output format: MD5SUM\tname1\n"
				"OR command1 | md5tee filename -n name2 -a | command2\n"
				"Output format: MD5SUM1\tname1\n"
				"Output format: MD5SUM2\tname2\n"
				"Chunksize can be changed with environment variable CHUNKSIZE (in bytes)\n"
				"Default CHUNKSIZE = %i. Use 0 to disable it\n."
				"(c) 2023 https://github.com/gerbenvoshol/md5tee\n", CHUNKSIZE_DEFAULT);
            	break;
			return 0;
        }
    }

	if (argc >= 2) {
		if ((access(argv[1],F_OK) == 0) && !append) {
			fprintf(stderr,"md5tee: ERROR: output file '%s' exists. "
				"please remove it before running.\n",argv[1]);
			return 1;
		}
	}
	
	MD5_Init(&c);
	MD5_Init(&d);
	MD5_Init(&e);

	buf = malloc(BUFSIZE);

	do {
		i = read(STDIN_FILENO, buf, BUFSIZE);
		if (i > 0) {
			size += i;
			MD5_Update(&c, buf, i);
			if (chunksize_cfg != 0) {
	if (chunksize + i >= chunksize_cfg) {
		MD5_Update(&d, buf, chunksize_cfg - chunksize);
		MD5_Final(md, &d);
		MD5_Update(&e, md, 16);
		MD5_Init(&d);
		if ((chunksize + i - chunksize_cfg) > 0) {
			MD5_Update(&d, buf + chunksize_cfg - chunksize, chunksize + i - chunksize_cfg);
			chunksize = chunksize + i - chunksize_cfg;
		} else {
			chunksize = 0;
		}
		chunks++;
	} else {
		MD5_Update(&d, buf, i);
		chunksize += i;
	}
			}
			if (write(STDOUT_FILENO, buf, i) != i) {
	perror("md5tee: ERROR writing to stdout");
	return 2;
			}
		}
	} while (i > 0);

	if ((chunksize_cfg != 0) && (chunksize >= 0)) {
		chunks++;
		MD5_Final(md, &d);
		MD5_Update(&e, md, 16);
	}

	MD5_Final(md, &c);
	bintohex(md5s,md);

	if (argc == optind) {
		if (name) {
			fprintf(stderr,"%s\t%s\n", md5s, name);
		} else {
			fprintf(stderr,"%s\n", md5s);
		}
	} else {
		FILE * fh;
		fh = fopen(argv[optind],"a+");
		if (fh == NULL) {
			perror("md5tee: ERROR creating output file");
			return 3;
		}
		if (name) {
			fprintf(fh,"%s\t%s\n", md5s, name);
		} else {
			fprintf(fh,"%s\n", md5s);
		}
		fclose(fh);
	}
	return 0;
}
