#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Get file length */
long GetLength(FILE *file)
{
	fseek(file, 0L, SEEK_END);// Go to the end of the file
    long pos = ftell(file);// Get the offset from the head
    fseek(file, 0L, SEEK_SET);// Go back to the head

    return pos;
}

/* Encrypt the plain */
bool Encrypt()
{
	char keyloc[255], inputloc[255], outputloc[255];
	long i, j = 0;

	printf("Enter the target file's' path: ");// Get the plain file's path
	scanf("%s", inputloc);
	FILE *inp = fopen(inputloc, "rb");
	if(!inp)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}
	printf("Plain file: %s\nFile size: %d\n\n", inputloc, GetLength(inp));
tag2:

	printf("Enter the key file's' path: ");// Get the key file's path
	scanf("%s", keyloc);
	FILE *key = fopen(keyloc, "rb");
	if(!key)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}
	if(GetLength(key) < 64)
	{
		printf("Key file not long enough!\nAt least 64 is needed!\n\n");
		goto tag2;
	}
	printf("Key file: %s\n\n", keyloc);

	printf("Enter the destination file's' path: ");// Indicate a path to store the cipher file
	scanf("%s", outputloc);
	FILE *out = fopen(outputloc, "wb");
	if(!out)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}

	char *plain = (char*) malloc(GetLength(inp) * sizeof(char));// Allocate memories for plain
	fread(plain, 1, GetLength(inp), inp);// Read the plain file

	char *k = (char*) malloc(64 * sizeof(char));// Allocate memories for key
	fread(k, 1, 64, key);// Take only 64 byte from the head of the key file

	char *cipher = (char*) malloc(GetLength(inp) * sizeof(char));// Allocate memories for cipher

	for(i = 0; i <= GetLength(inp); i++)// Main algorithum
	{
		if(j > 64) j = 0;
		cipher[i] = plain[i] + k[j];
		j ++;
	}

	fwrite(cipher, sizeof(char), GetLength(inp), out);// Write into file

	printf("Done!\n\n");

	free(plain);// Free all the memories and close all the files
	free(k);
	free(cipher);
	fclose(inp);
	fclose(key);
	fclose(out);

	return 0;
}

/* Decrypt the cipher */
bool Decrypt()
{
	char keyloc[255], inputloc[255], outputloc[255];
	long i, j = 0;

	printf("Enter the target file's' path: ");// Get the cipher file's path
	scanf("%s", inputloc);
	FILE *inp = fopen(inputloc, "rb");
	if(!inp)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}
	printf("Plain file: %s\nFile size: %d\n\n", inputloc, GetLength(inp));
tag3:

	printf("Enter the key file's' path: ");// Get the key file's path
	scanf("%s", keyloc);
	FILE *key = fopen(keyloc, "rb");
	if(!key)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}
	if(GetLength(key) < 64)
	{
		printf("Key file not long enough!\nAt least 64 is needed!\n\n");
		goto tag3;
	}
	printf("Key file: %s\n\n", keyloc);

	printf("Enter the destination file's' path: ");// Indicate a path to store the plain file
	scanf("%s", outputloc);
	FILE *out = fopen(outputloc, "wb");
	if(!out)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}

	char *cipher = (char*) malloc(GetLength(inp) * sizeof(char));// Allocate memories for cipher
	fread(cipher, 1, GetLength(inp), inp);// Read the cipher file

	char *k = (char*) malloc(64 * sizeof(char));
	fread(k, 1, 64, key);// Take only 64 byte from the head of key file

	char *plain = (char*) malloc(GetLength(inp) * sizeof(char));// Allocate memories for plain

	for(i = 0; i <= GetLength(inp); i++)// Main algorithum
	{
		if(j > 64) j = 0;
		plain[i] = cipher[i] - k[j];
		j ++;
	}

	fwrite(plain, sizeof(char), GetLength(inp), out);// Write into file
	
	printf("Done!\n\n");

	free(plain);// Free all the memories and close all the files
	free(k);
	free(cipher);
	fclose(inp);
	fclose(key);
	fclose(out);

	return 0;
}

/* Generate a key base on a seed from current time, with a default size of 64k */
bool KeyGen()
{
	char keyloc[255];
	unsigned char i;

	printf("Enter the key file's' path: ");// Indicate a path to store the key file
	scanf("%s", keyloc);
	FILE *key = fopen(keyloc, "wb");
	if(!key)
	{
		printf("Invalid file path!\n\n");
		return 1;
	}
	char *k = (char*) malloc(64 * sizeof(char));

	srand(time(NULL));// Use current time as seed for random number generator
	for(i = 0; i < 64; i ++)
		k[i] = rand();

	fwrite(k, sizeof(char), 64, key);

	printf("Done!\n\n");

	free(k);// Free the memory and close the file
	fclose(key);

	return 0;
}

int main(int argc, char *argv[])
{
tag4:

	char mod;
	printf("Select a mode: 0[Exit] 1[Encrypt] 2[Decrypt] 3[KeyGen] ");
	scanf("%d", &mod);
tag1:

	if(mod == 0)
		return 0;
	else if(mod == 1)
	{
		if(Encrypt()) goto tag1;
	}
	else if(mod == 2)
	{
		if(Decrypt()) goto tag1;
	}
	else if(mod == 3)
	{
		if(KeyGen()) goto tag1;
	}
	else
	{
		printf("Invalid mode!\n\n");
		goto tag4;
	}
	goto tag4;
}
