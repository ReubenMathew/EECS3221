#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

typedef struct
{
    int page_number;
    char *byte_data;
} page;

typedef struct
{
    int address;
    int page_number;
    int offset;
} address;

address *LogicalAddresses;
page *PageTable;

int PageExists(int page_number, int pagetable_length)
{
    for (int j = 0; j < pagetable_length; j++)
    {
        if (page_number == PageTable[j].page_number)
        {
            return j;
        }
    }
    return -1;
}

int main(int argc, char **argv)
{
    if (argc < 3)
        return 1;

    LogicalAddresses = malloc(sizeof(*LogicalAddresses) * 1024);
    int logical_addresses_idx = 0;

    int TABLE_SIZE = atoi(argv[1]);
    char *backingstore_filename = argv[2];
    char *addresses_filename = argv[3];

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

        LogicalAddresses[logical_addresses_idx].address = address;
        LogicalAddresses[logical_addresses_idx].page_number = page_number;
        LogicalAddresses[logical_addresses_idx].offset = offset;
        logical_addresses_idx++;
    }
    fclose(addressFile);

    PageTable = malloc(sizeof(page) * TABLE_SIZE);
    int PageTable_size = 0;

    FILE *binstore_file = fopen(backingstore_filename, "rb");
    char c;

    for (int i = 0; i < logical_addresses_idx; i++)
    {
        int page_number = PageExists(LogicalAddresses[i].page_number, PageTable_size);
        if (page_number == -1)
        {
            printf("Page Number: %d not found...", LogicalAddresses[i].page_number);
            // does not find page
            int page_offset = (LogicalAddresses[i].page_number * 256);

            /* Seek to the beginning of the file */
            fseek(binstore_file, page_offset, SEEK_SET);
            // printf("%0x\n", ftell(binstore_file));

            PageTable[i].page_number = LogicalAddresses[i].page_number;
            PageTable[i].byte_data = malloc(sizeof(char) * 256);

            
            for (int j = 0, max = TABLE_SIZE; j < max && (c = getc(binstore_file)) != EOF; j++)
                PageTable[i].byte_data[j] = c;

            page_number = PageTable_size;
            printf(" inserting at %d\n", page_number);
            PageTable_size++;
        }
        else
        {
            printf("Page Number: %d found at %d\n", LogicalAddresses[i].page_number, page_number);
        }
    }
    fclose(binstore_file);

    // for (int i = 0; i < TABLE_SIZE; i++)
    // {
    //     if (PageTable[i].page_number == 2)
    //         printf("Page Number:%d    \t Data:%x\n", PageTable[i].page_number, (u_int)PageTable[i].byte_data[3]);
    // }

    free(PageTable);

    return 0;
}