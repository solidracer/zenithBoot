#ifndef _loader_h
#define _loader_h

#include <elf.h>
#include "filesystem.h"

#define PF_WX (PF_W | PF_X)

EFI_STATUS EFIAPI isValidELF(Elf64_Ehdr *ehdr, EFI_FILE_PROTOCOL *file, BOOLEAN *value);
EFI_STATUS EFIAPI readHeaders(Elf64_Ehdr *ehdr, Elf64_Phdr **phdrs, EFI_FILE_PROTOCOL *file);

#endif
