#include <stdio.h>
#include <iostream>
#include <string.h>
#include "host.h"
#include "machine.h"
using namespace std;

extern "C" void pushJump (md_addr_t pc, md_addr_t next_pc);
extern "C" void printJumpStats ();

#define TABLE_LENGTH    2048
#define SHIFT_LENGTH    0
#define ASSOC           4
class entry
{
  public:
    md_addr_t pc;
    counter_t count;
    entry *next;

    /* Constructor */
      entry ()
    {
	pc = 0;
	count = 0;
	next = NULL;
    }
    entry (md_addr_t next_pc)
    {
	pc = next_pc;
	count = 1;
	next = NULL;
    }

    /* De-Constructor */
    ~entry ()
    {
    }

    void insertJump (md_addr_t next_pc)
    {
	if (pc == next_pc)
	{
	    count++;
	    return;
	}

	if (next == NULL)
	{
	    next = new entry (next_pc);
	    return;
	}

	next->insertJump (next_pc);
    }

    entry *sortInsert (entry * new_entry)
    {
	if (new_entry->count > count)
	{
	    new_entry->next = this;
	    return new_entry;
	}

	if (next == NULL)
	{
	    next = new_entry;
	    return this;
	}

	next = next->sortInsert (new_entry);
    }

    void print_file (FILE * fp)
    {
	fprintf (fp, "\tPC: 0x%llx\tCount: %lld", pc, count);
	if (next != NULL)
	{
	    next->print_file (fp);
	}
    }
};

class hashTable
{
  public:
    entry * table[TABLE_LENGTH][ASSOC];
    md_addr_t pc[TABLE_LENGTH][ASSOC];

    /* Constructor */
      hashTable ()
    {
	int i, j;
	for (i = 0; i < TABLE_LENGTH; i++)
	{
	    for (j = 0; j < ASSOC; j++)
	    {
		table[i][j] = NULL;
		pc[i][j] = 0;
	    }
	}
    }

    /* De-Constructor */
     ~hashTable ()
    {
    }
};


hashTable jumpBuffer;

void
pushJump (md_addr_t pc, md_addr_t next_pc)
{
    int index;
    int i;

    if (pc == 0 || next_pc == 0)
	return;
    index = (pc >> SHIFT_LENGTH) % TABLE_LENGTH;

    /* Search if the entry is already present */
    for (i = 0; i < ASSOC; i++)
    {
	if (jumpBuffer.pc[index][i] == pc)
	{			/* PC is already present, now just propogate the address. */
	    jumpBuffer.table[index][i]->insertJump (next_pc);
	    return;
	}
    }

    /* Insert the store in first possible empty block */
    for (i = 0; i < ASSOC; i++)
    {
	if (jumpBuffer.pc[index][i] == 0)
	{
	    jumpBuffer.pc[index][i] = pc;
	    jumpBuffer.table[index][i] = new entry (next_pc);
	    return;
	}
    }

    /* If there is no position empty, let the jump go. */
}

void
sortJumps ()
{
    int i, j;

    for (i = 0; i < TABLE_LENGTH; i++)
    {
	for (j = 0; j < ASSOC; j++)
	{
	    if (jumpBuffer.pc[i][j] != 0)
	    {
		entry *temp;

		temp = jumpBuffer.table[i][j];
		jumpBuffer.table[i][j] = jumpBuffer.table[i][j]->next;
		temp->next = NULL;
		while (jumpBuffer.table[i][j] != NULL)
		{
		    entry *temp_next;

		    temp_next = jumpBuffer.table[i][j]->next;
		    jumpBuffer.table[i][j]->next = NULL;
		    temp = temp->sortInsert (jumpBuffer.table[i][j]);
		    jumpBuffer.table[i][j] = temp_next;
		}

		jumpBuffer.table[i][j] = temp;
	    }
	}
    }
}

void
printJumpStats ()
{
    FILE *fp;
    extern char *sim_str_dump;
    char *sim_dump = sim_str_dump;
    char strFileName[256];

    if (!sim_dump)
    {
	fprintf (stderr, "No name for dump file\n");
	exit (1);
    }

    strcpy (strFileName, sim_dump);
    strcat (strFileName, ".jpf");
    if ((fp = fopen (strFileName, "w")) == NULL)
    {
	fprintf (stderr, "Error opening file\n");
	exit (1);
    }

    /*
       sortJumps();
     */
    for (int i = 0; i < TABLE_LENGTH; i++)
    {
	for (int j = 0; j < ASSOC; j++)
	{
	    if (jumpBuffer.pc[i][j] != 0 && jumpBuffer.table[i][j] != NULL)
	    {
		fprintf (fp, "PC: 0x%llx", jumpBuffer.pc[i][j]);
		jumpBuffer.table[i][j]->print_file (fp);
		fprintf (fp, "\n");
	    }
	}
    }

    fclose (fp);
}
