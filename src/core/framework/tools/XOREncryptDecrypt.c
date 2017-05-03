//
//  XOREncryptDecrypt.c
//  XOREncryptDecrypt
//
//  Created by Stephen Gowen on 5/3/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include <stdio.h>
#include <string.h>

void encrypt_data(FILE* input_file, FILE* output_file)
{
    const char* key = "NGS";
    int key_count = 0; //Used to restart key if strlen(key) < strlen(encrypt)
    int encrypt_byte;
    
    //Loop through each byte of file until EOF
    while ((encrypt_byte = fgetc(input_file)) != EOF)
    {
        //XOR the data and write it to a file
        fputc(encrypt_byte ^ key[key_count], output_file);
        
        //Increment key_count and start over if necessary
        key_count++;
        if (key_count == strlen(key))
        {
            key_count = 0;
        }
    }
}

int main(int argc, const char * argv[])
{
    FILE *inputFile;
#ifdef _WIN32
    errno_t err;
    if((err = fopen_s(&inputFile, argv[1], "rb")) != 0)
#else
    if ((inputFile = fopen(argv[1], "rb")) == NULL)
#endif
    {
        return -1;
    }
    
    FILE *outputFile;
#ifdef _WIN32
    if((err = fopen_s(&outputFile, argv[2], "w+b")) != 0)
#else
    if ((outputFile = fopen(argv[2], "w+b")) == NULL)
#endif
    {
        return -1;
    }
    
    if (inputFile && outputFile)
    {
        encrypt_data(inputFile, outputFile);
        
        // close the files
        fclose(inputFile);
        fclose(outputFile);
    }
    
    return 0;
}
