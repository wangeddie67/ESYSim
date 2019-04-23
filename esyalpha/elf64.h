#include <stdint.h>

#define Elf64_Addr   uint64_t
#define Elf64_Off    uint64_t
#define Elf64_Half   uint16_t
#define Elf64_Word   uint32_t
#define Elf64_Sword  uint32_t
#define Elf64_Xword  uint64_t
#define Elf64_Sxword uint64_t

// e_ident[EI_CLASS = 4]
#define ELFCLASS32 1         // 32-bit objects
#define ELFCLASS64 2         // 64-bit objects

// e_ident[EI_DATA = 5]
#define ELFDATA2LSB 1        // little-endian object file data structs
#define ELFDATA2MSB 2        // big-endian object file data structs

// e_type
#define ET_NONE   0      // No file type
#define ET_REL    1      // Relocatable object file
#define ET_EXEC   2      // Executable file
#define ET_DYN    3      // Shared object file
#define ET_CORE   4      // Core file
#define ET_LOOS   0xFE00 // Environment-specific use
#define ET_HIOS   0xFEFF // Environment-specific use
#define ET_LOPROC 0xFF00 // Processor-specific use
#define ET_HIPROC 0xFFFF // Processor-specific use

// p_type
#define PT_NULL    0          // unused entry
#define PT_LOAD    1          // loadable segment
#define PT_DYNAMIC 2          // dynamic linking tables
#define PT_INTERP  3          // program interpreter path name
#define PT_NOTE    4          // note sections
#define PT_SHLIB   5          // reserved
#define PT_PHDR    6          // program header table
#define PT_LOOS    0x60000000 // environment-specific use
#define PT_HIOS    0x6FFFFFFF // environment-specific use
#define PT_LOPROC  0x70000000 // processor-specific use
#define PT_HIPROC  0x7FFFFFFF // processor-specific use

// p_flags
#define F_X         0x1        // execute permission
#define PF_W        0x2        // write permission
#define PF_R        0x4        // read permission
#define PF_MASKOS   0x00FF0000 // environment-specific use
#define PF_MASKPROC 0xFF000000 // processor-specific use

// sh_type
#define SHT_NULL     0          // unused section header
#define SHT_PROGBITS 1          // information defined by the program
#define SHT_SYMTAB   2          // a linker symbol table
#define SHT_STRTAB   3          // a string table
#define SHT_RELA     4          // “Rela” type relocation entries
#define SHT_HASH     5          // a symbol hash table
#define SHT_DYNAMIC  6          // dynamic linking tables
#define SHT_NOTE     7          // note information
#define SHT_NOBITS   8          // uninitialized space (not in file)
#define SHT_REL      9          // “Rel” type relocation entries
#define SHT_SHLIB    10         // reserved
#define SHT_DYNSYM   11         // a dynamic loader symbol table
#define SHT_LOOS     0x60000000 // environment-specific use
#define SHT_HIOS     0x6FFFFFFF // environment-specific use
#define SHT_LOPROC   0x70000000 // processor-specific use
#define SHT_HIPROC   0x7FFFFFFF // processor-specific use

typedef struct
{
    unsigned char e_ident[16]; // ELF identification
    Elf64_Half    e_type;      // Object file type
    Elf64_Half    e_machine;   // Machine type
    Elf64_Word    e_version;   // Object file version
    Elf64_Addr    e_entry;     // Entry point address
    Elf64_Off     e_phoff;     // Program header offset
    Elf64_Off     e_shoff;     // Section header offset
    Elf64_Word    e_flags;     // Processor-specific flags
    Elf64_Half    e_ehsize;    // ELF header size
    Elf64_Half    e_phentsize; // Size of program header entry
    Elf64_Half    e_phnum;     // Number of program header entries
    Elf64_Half    e_shentsize; // Size of section header entry
    Elf64_Half    e_shnum;     // Number of section header entries
    Elf64_Half    e_shstrndx;  // Section name string table index
} Elf64_Ehdr;

typedef struct
{
    Elf64_Word        sh_name;      // Section name
    Elf64_Word        sh_type;      // Section type
    Elf64_Xword       sh_flags;     // Section attributes
    Elf64_Addr        sh_addr;      // Virtual address in memory
    Elf64_Off         sh_offset;    // Offset in file
    Elf64_Xword       sh_size;      // Size of section
    Elf64_Word        sh_link;      // Link to other section
    Elf64_Word        sh_info;      // Miscellaneous information
    Elf64_Xword       sh_addralign; // Address alignment boundary
    Elf64_Xword       sh_entsize;   // Size of entries, if section has table
} Elf64_Shdr;

typedef struct
{
    Elf64_Word       st_name;  // Symbol name
    unsigned char    st_info;  // Type and Binding attributes
    unsigned char    st_other; // Reserved
    Elf64_Half       st_shndx; // Section table index
    Elf64_Addr       st_value; // Symbol value
    Elf64_Xword      st_size;  // Size of object (e.g., common)
} Elf64_Sym;

typedef struct
{
    Elf64_Addr        r_offset; // Address of reference
    Elf64_Xword       r_info;   // Symbol index and type of relocation
} Elf64_Rel;

typedef struct
{
    Elf64_Addr        r_offset; // Address of reference
    Elf64_Xword       r_info;   // Symbol index and type of relocation
    Elf64_Sxword      r_addend; // Constant part of expression
} Elf64_Rela;

typedef struct
{
    Elf64_Word       p_type;          // Type of segment
    Elf64_Word       p_flags;         // Segment attributes
    Elf64_Off        p_offset;        // Offset in file
    Elf64_Addr       p_vaddr;         // Virtual address in memory
    Elf64_Addr       p_paddr;         // Reserved
    Elf64_Xword      p_filesz;        // Size of segment in file
    Elf64_Xword      p_memsz;         // Size of segment in memory
    Elf64_Xword      p_align;         // Alignment of segment
} Elf64_Phdr;


