/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FW2H_VERSION	"1.0.0.3"

void random_string16(char *str)
{
	const char letters[] = "0123456789ABCDEF";
	int i;

	srand((unsigned int)time(NULL));
	for (i = 0; i < sizeof(letters); ++i)
		str[i] = letters[rand() % 16];
	str[16] = '\0';
}

long filesize(FILE*stream)
{
	long curpos,length;

	curpos=ftell(stream);
	fseek(stream,0L,SEEK_END);
	length=ftell(stream);
	fseek(stream,curpos,SEEK_SET);

	return length;
}

int main(int argc, char *argv[])
{
	unsigned char read_byte;
	FILE *fp, *fp_out;
	int i, file_size;
	char guard_id[17];

	printf("FW2H v%s\n", FW2H_VERSION);

    if (argc < 3) {
        fprintf(stderr, "no valid argument found\n");
        fprintf(stderr, "usage: fw2h <in-file> <out-file>\n");
        exit(1);
    }
    
	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Open input file failed.\n");
		exit(1);
	}
    
	fp_out = fopen(argv[2], "wb");
	if (fp_out == NULL) {
		fprintf(stderr, "Open output file failed.\n");
		exit(1);
	}

	file_size = filesize(fp);

	fprintf(fp_out, "/* This code was generated by fw2h utility, please don't make change to it directly.\n");
	fprintf(fp_out, " * FW2H version: %s. */\n\n", FW2H_VERSION);
	random_string16(guard_id);
	fprintf(fp_out, "#ifndef BHY_FW_%s_H\n", guard_id);
	fprintf(fp_out, "#define BHY_FW_%s_H\n\n", guard_id);
	fprintf(fp_out, "const unsigned char bhy_firmware_image[%d] = {", file_size);

	for(i = 0; i < file_size; ++i) {
		if (i % 16 == 0)
			fprintf(fp_out, "\n\t");
		else
			fprintf(fp_out, " ");

		fread(&read_byte, 1, 1, fp);

		fprintf(fp_out, "0x%02x,", read_byte);
	}

	fclose(fp);

	fprintf(fp_out, "\n};\n\n");
	fprintf(fp_out, "#endif /*~ BHY_FW_%s_H */\n", guard_id);

	fclose(fp_out);

	printf("File converted successfully!\n");

	exit(0);
}
