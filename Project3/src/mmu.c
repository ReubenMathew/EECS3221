#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 256
#define MAX_ADDRESSES 1000
#define TLB_SIZE 16

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

typedef struct
{
    int tag;
    int page_number;
} tlb_entry;

tlb_entry *TLB;
address *LogicalAddresses;
page *PageTable;

int pagetable_len;
int pagefault_count = 0;
int tlbhit_count = 0;

int TABLE_SIZE;
char *backingstore_filename;

int TLBQuery(int address);
void TLBInsert(int tag, int physical_page_number);
int PageTableQuery(int address, int pagetable_len);
int PageTableInsert(address *Address);
page GetPage(int index);

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
        // printf("%d %d %d\n", temp_address, temp_page_number, temp_offset);
    }
    fclose(addressFile);

    FILE *output256 = fopen("output256.csv", "w+");

    TLB = malloc(TLB_SIZE * sizeof(*TLB));

    PageTable = malloc(TABLE_SIZE * sizeof(*PageTable));
    pagetable_len = 0;
    int index, physical_addr;
    // Go through each logical address
    for (int addr_idx = 0; addr_idx < MAX_ADDRESSES; addr_idx++)
    {
        address curr_addr = LogicalAddresses[addr_idx];

        //query for address in TLB
        TLBQuery(curr_addr.page_number);
        // query for address in pagetable
        index = PageTableQuery(curr_addr.page_number, pagetable_len);
        // pagefault -> add to pagetable
        if (index == -1)
        {
            address *toInsert = &curr_addr;
            index = PageTableInsert(toInsert);
        }
        physical_addr = (index << 8) + curr_addr.offset;
        // printf("%x   \t%x   \t%x\t", index, curr_addr.offset, physical_addr);
        page temp_page = GetPage(index);
        char byte = temp_page.byte_data[curr_addr.offset];

        fprintf(output256, "%d,%d,%d\n", curr_addr.address, physical_addr, byte);
    }
    // Statistics
    float pagefault_rate = (float)pagefault_count / MAX_ADDRESSES * 100;
    fprintf(output256, "Page Faults Rate, %.2f%,\n", pagefault_rate);
    float tlbhit_rate = (float)(tlbhit_count + 1) / MAX_ADDRESSES * 100;
    fprintf(output256, "TLB Hits Rate, %.2f%,", tlbhit_rate);
    fclose(output256);

    // for (int i = 0; i < TLB_SIZE; i++)
    // {
    //     printf("%d    \t%d\n", TLB[i].tag, TLB[i].page_number);
    // }

    // for (int i = 0; i < pagetable_len; i++)
    // {
    //     // printf("Page Number: %d   \tIndex: %d   \tData:%x\n", PageTable[i].page_number, i, PageTable[i].byte_data[3]);
    // }

    return 0;
}

int insertIndex = 0;
void TLBInsert(int tag, int physical_page_number)
{
    if (insertIndex >= TLB_SIZE)
        insertIndex = 0;
    TLB[insertIndex].tag = tag;
    TLB[insertIndex].page_number = physical_page_number;
    // printf("Inserting: %d %d\n", TLB[insertIndex].tag, TLB[insertIndex].page_number);
    insertIndex++;
}

// Iterates through TLB and returns the correct physical address
// If the entry is not found in the TLB, returns -1
int TLBQuery(int page_number)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (TLB[i].tag == page_number)
        {
            tlbhit_count++;
            return TLB[i].page_number;
        }
    }
    return -1;
}

page GetPage(int index)
{
    return PageTable[index];
}

// Inserts address into page table in a FIFO fashion, returns index of newly inserted entry
// TODO: update TLB as well
int PageTableInsert(address *Address)
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
    TLBInsert(Address->page_number, pagetable_len);
    return pagetable_len++;
}

// Iterates through page table and returns the index of the page (which is the physical page number)
// If the entry is not found in the page table, returns -1 for error
int PageTableQuery(int page_number, int pagetable_len)
{
    for (int i = 0; i < pagetable_len; i++)
    {
        if (PageTable[i].page_number == page_number)
        {
            TLBInsert(page_number, i);
            return i;
        }
    }
    pagefault_count++;
    return -1;
}