#include "elfloader.h"

EFI_STATUS isValidELF(Elf64_Ehdr *ehdr, EFI_FILE_PROTOCOL *file, BOOLEAN *value) {
    EFI_STATUS stat;
    UINTN size = sizeof(Elf64_Ehdr);
    stat = file->Read(file, &size, ehdr);

    if (EFI_ERROR(stat))
        return stat;
    
    if (CompareMem(ehdr->e_ident, ELFMAG, SELFMAG))
        *value = FALSE;
    else *value = TRUE;

    return stat;
}

EFI_STATUS readHeaders(Elf64_Ehdr *ehdr, Elf64_Phdr **phdrs, EFI_FILE_PROTOCOL *file) {
    EFI_STATUS stat;
    stat = file->SetPosition(file, ehdr->e_phoff);
    if (EFI_ERROR(stat)) return stat;

    UINTN size = sizeof(Elf64_Phdr) * ehdr->e_phnum;
    *phdrs = AllocatePool(size);
    if (!*phdrs) return EFI_OUT_OF_RESOURCES;

    stat = file->Read(file, &size, *phdrs);
    if (EFI_ERROR(stat)) return stat;

    return stat;
}
