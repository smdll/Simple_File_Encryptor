#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void error(char *argv)
{
	printf("Usage: %s [Options] arguments\n\n", argv);
	printf("\t-E PlainFile\t\t\tEncrypt a file. A \"-k\" and \"-o\" MUST EXIST!\n\n");
	printf("\t-D CipherFile\t\t\tDecrypt a file. A \"-k\" and \"-o\" MUST EXIST!\n\n");
	printf("\t-K KeyFile\t\t\tGenerate a 64bytes key file\n\n");
	printf("\t-o OutputFile\t\t\tIndicate an output\n\n");
	printf("\t-k KeyFile\t\t\tIndicate the key file to be used\n\n\n");
	printf("Example:\n");
	printf("\t%s -E Plain.txt -k key -o Cipher.bin\t\tEncrypt \"Plain.txt\" with \"key\"\n\n");
	printf("\t%s -D Cipher.bin -k key -o Plain.txt\t\tDecrypt \"Cipher.txt\" with \"key\"\n\n");
	printf("\t%s -K key\t\tGenerate a key file named \"key\", 64bytes.\n\n");
	printf("Please strictly follow the argument format above!\n\n");
}

bool cmp(char *left, const char *right)
{
	unsigned int i = 0;
	while(left[i] != '\0')
	{
		if(left[i] != right[i])
			return 0;
		i++;
	}
	if(right[i] != '\0')
		return 0;
	return 1;
}

unsigned short len(char *in)
{
	unsigned short i = 0;
	while(in[i] != '\0')
		i++;
	return i;
}

void cpy(char *target, char *destination)
{
	unsigned short i;
	for(i = 0; i < len(destination); i++)
		destination[i] = target[i];
}

/* Get file length */
long GetLength(FILE *file)
{
	fseek(file, 0L, SEEK_END);// Go to the end of the file
    long pos = ftell(file);// Get the offset from the head
    fseek(file, 0L, SEEK_SET);// Go back to the head

    return pos;
}

/* Encrypt the plain */
bool Encrypt(char *keyloc, char *inputloc, char *outputloc)
{
	long i, j = 0;

	FILE *inp = fopen(inputloc, "rb");
	if(!inp)
	{
		printf("Invalid plain file!\n\n");
		return 1;
	}
	printf("Plain file: %s\nFile size: %d\n\n", inputloc, GetLength(inp));

	FILE *key = fopen(keyloc, "rb");
	if(!key || GetLength(key) < 64)
	{
		printf("Invalid file or key file is smaller than 64bytes!\n\n");
		return 1;
	}
	printf("Key file: %s\n\n", keyloc);

	FILE *out = fopen(outputloc, "wb");
	if(!out)
	{
		printf("Invalid cipher file path!\n\n");
		return 1;
	}

	printf("Encypting plain file\n");
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

	printf("Done!\n");

	free(plain);// Free all the memories and close all the files
	free(k);
	free(cipher);
	fclose(inp);
	fclose(key);
	fclose(out);

	return 0;
}

/* Decrypt the cipher */
bool Decrypt(char *keyloc, char *inputloc, char *outputloc)
{
	long i, j = 0;

	FILE *inp = fopen(inputloc, "rb");
	if(!inp)
	{
		printf("Invalid cipher file!\n\n");
		return 1;
	}
	printf("Cipher file: %s\nFile size: %d\n\n", inputloc, GetLength(inp));

	FILE *key = fopen(keyloc, "rb");
	if(!key || GetLength(key) < 64)
	{
		printf("Invalid file or key file is smaller than 64bytes!\n\n");
		return 1;
	}
	printf("Key file: %s\n\n", keyloc);

	FILE *out = fopen(outputloc, "wb");
	if(!out)
	{
		printf("Invalid plain file path!\n\n");
		return 1;
	}

	printf("Decrypting cipher file\n");
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
	
	printf("Done!\n");

	free(plain);// Free all the memories and close all the files
	free(k);
	free(cipher);
	fclose(inp);
	fclose(key);
	fclose(out);

	return 0;
}

/* Generate a key base on a seed from current time, with a default size of 64k */
bool KeyGen(char *keyloc)
{
	unsigned char i;

	FILE *key = fopen(keyloc, "wb");
	if(!key)
	{
		printf("Invalid file path!\n");
		return 1;
	}
	printf("Generating key file\n");
	char *k = (char*) malloc(64 * sizeof(char));

	srand(time(NULL));// Use current time as seed for random number generator
	for(i = 0; i < 64; i ++)
		k[i] = rand();

	fwrite(k, sizeof(char), 64, key);

	printf("Done!\n");

	free(k);// Free the memory and close the file
	fclose(key);

	return 0;
}

int main(int argc, char *argv[])
{
	if(argc == 3)
	{
		if(cmp(argv[1], "-K"))
		{
			KeyGen(argv[2]);
		}
		else
			error(argv[0]);
	}
	else if(argc == 7)
	{
		if(cmp(argv[3], "-k"))
		{
			if(cmp(argv[5], "-o"))
			{
				if(cmp(argv[1], "-E"))
					Encrypt(argv[4], argv[2], argv[6]);
				else if(cmp(argv[1], "-D"))
					Decrypt(argv[4], argv[2], argv[6]);
			}
			else
				error(argv[0]);
		}
		else
			error(argv[0]);
	}
	else
		error(argv[0]);
	printf("Exiting...\n");
	return 0;
}
