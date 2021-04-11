#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define MAX_ADDRESSES 1000

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
int pagetable_len;

int TABLE_SIZE;
char *backingstore_filename;

int PageTableQuery(int address, int pagetable_len);
void PageTableInsert(address *Address);

int main(int argc, char **argv)
{
    TABLE_SIZE = atoi(argv[1]);
    backingstore_filename = argv[2];
    char *addresses_filename = argv[3];

    FILE *addressFile = fopen(addresses_filename, "r");
    char *temp;
    char buffer[BUFFER_SIZE];

    u_int16_t temp_address;
    u_int8_t temp_page_number;
    u_int8_t temp_offset;

    LogicalAddresses = malloc(MAX_ADDRESSES * sizeof(*LogicalAddresses));
    int logical_addresses_idx = 0;

    while (fgets(buffer, BUFFER_SIZE, addressFile) != NULL)
    {
        temp = strdup(buffer);
        temp_address = atoi(temp);
        // bit-masking to get page_number and page offset
        temp_page_number = (temp_address >> 8) & 0xff;
        temp_offset = temp_address & 0xff;

        LogicalAddresses[logical_addresses_idx].address = temp_address;
        LogicalAddresses[logical_addresses_idx].page_number = temp_page_number;
        LogicalAddresses[logical_addresses_idx].offset = temp_offset;
        logical_addresses_idx++;
    }
    fclose(addressFile);

    // Go through each logical address
    PageTable = malloc(TABLE_SIZE * sizeof(*PageTable));
    pagetable_len = 0;
    int index;
    for (int addr_idx = 0; addr_idx < MAX_ADDRESSES; addr_idx++)
    {
        address curr_addr = LogicalAddresses[addr_idx];
        index = PageTableQuery(curr_addr.page_number, pagetable_len);
        if (index == -1)
        {
            address *toInsert = &curr_addr;
            PageTableInsert(toInsert);
        }
    }

    for (int i = 0; i < pagetable_len; i++)
    {
        printf("Page Number: %d   \tIndex: %d   \tData:%x\n", PageTable[i].page_number, i, PageTable[i].byte_data[3]);
    }

    return 0;
}

// Inserts address into page table in a FIFO fashion
void PageTableInsert(address *Address)
{
    FILE *binstore_file = fopen(backingstore_filename, "rb");
    char c;

    int page_offset = (Address->page_number * 256);
    /* Seek to the correct page */
    fseek(binstore_file, page_offset, SEEK_SET);

    PageTable[pagetable_len].page_number = Address->page_number;
    PageTable[pagetable_len].byte_data = malloc(sizeof(char) * 256);

    for (int j = 0, max = TABLE_SIZE; j < max && (c = getc(binstore_file)) != EOF; j++)
        PageTable[pagetable_len].byte_data[j] = c;

    // printf("inserting %d at %d\n", Address->page_number, pagetable_len);
    pagetable_len++;
}

// Iterates through page table and returns the index of the page (which is the physical page number)
// If the entry is not found in the page table, returns -1 for error
int PageTableQuery(int page_number, int pagetable_len)
{
    for (int i = 0; i < pagetable_len; i++)
    {
        if (PageTable[i].page_number == page_number)
        {
            return i;
        }
    }
    return -1;
}