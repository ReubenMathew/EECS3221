#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

typedef struct
{
    int page_number;
    char *byte_data;
} page;

int main(int argc, char **argv)
{
    int **logical_addresses = malloc(sizeof(*logical_addresses) * 1024);
    int logical_addresses_idx = 0;

    if (argc < 3)
        return 1;

    int ADDRESS_SIZE = atoi(argv[1]);
    char *backingstore_filename = argv[2];
    char *addresses_filename = argv[3];

    page *PageTable = malloc(sizeof(page) * ADDRESS_SIZE);

    FILE *addressFile = fopen(addresses_filename, "r");
    char *temp;
    char buffer[BUFFER_SIZE];

    u_int16_t address;
    u_int8_t page_number;
    u_int8_t offset;

    while (fgets(buffer, BUFFER_SIZE, addressFile) != NULL)
    {
        temp = strdup(buffer);
        address = atoi(temp);
        // bit-masking to get page_number and page offset
        page_number = (address >> 8) & 0xff;
        offset = address & 0xff;

        logical_addresses[logical_addresses_idx] = malloc(sizeof(int) * 3);
        logical_addresses[logical_addresses_idx][0] = address;
        logical_addresses[logical_addresses_idx][1] = page_number;
        logical_addresses[logical_addresses_idx][2] = offset;
        logical_addresses_idx++;
    }
    fclose(addressFile);

    for (int i = 0; i < logical_addresses_idx; i++)
    {
        printf("Logical Address: %d \t Page Number: %d \t Page Offset: %d\n", logical_addresses[i][0], logical_addresses[i][1], logical_addresses[i][2]);
    }

    FILE *binstore_file = fopen(backingstore_filename, "rb");
    char *temp;
    char buffer[ADDRESS_SIZE];
    for (int i = 0; i < logical_addresses_idx; i++)
    {

    }


    // DEBUG OUTPUT
    // printf("%d %s %s\n", ADDRESS_SIZE, backingstore_filename, addresses_filename);

    return 0;
}