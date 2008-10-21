
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "uzlib.h"

static voidp
my_calloc (voidp opaque, unsigned items, unsigned size)
{
	return malloc (items * size);
}
static void
my_free (voidp opaque, voidp ptr)
{
	free (ptr);
}


int
main (int argc, char **argv)
{
	z_stream z;
	unsigned char buf[4096];
	unsigned char obuf[4096];
	int ret, i;

	z.zalloc = my_calloc;
	z.zfree = my_free;
	z.avail_in = 0;
	z.next_in = Z_NULL;

	if (inflateInit (&z) != Z_OK)
		return 1;

	z.next_in = buf;

	for (;;) {
		if (z.avail_in == 0)
			z.avail_in = read (STDIN_FILENO, buf, sizeof (buf));
		if (z.avail_in == -1) {
			perror ("read");
			exit (1);
		}
		if (z.avail_in == 0) {
			printf ("Early EOF\n");
			exit (1);
		}
		
		z.next_out = obuf;
		z.avail_out = sizeof (obuf);

		ret = inflate (&z, 0);

		if (ret == Z_STREAM_END)
			break;
		if (ret != Z_OK) {
			printf ("Bad\n");
			return 1;
		}
		for (i = 0; i < sizeof (buf) - z.avail_out; i++) {
			printf ("0x%02x\n", obuf[i]);
		}
	}

	for (i = 0; i < sizeof (buf) - z.avail_out; i++) {
		printf ("0x%02x\n", obuf[i]);
	}


	return 0;
}
