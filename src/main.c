#include "flags.h"
#include "console.h"

/* zenithBoot, a part of zenithOS. */
/* WRITTEN BY SOLIDRACER */

/* simple error handler */
#define ERRCHECK( msg, stat )               \
if (EFI_ERROR(stat)) {                      \
    Print(L"%s: %r\r\n", msg, stat);        \
    pauseConsole();                         \
    return stat;                            \
}

typedef void (*kmain_t)();

EFI_STATUS EFIAPI efi_main(EFI_HANDLE handle, EFI_SYSTEM_TABLE *sysTable) {
    InitializeLib(handle, sysTable);
    EFI_STATUS stat;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    EFI_FILE_PROTOCOL *root, *kernel;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    gST->ConOut->SetMode(gST->ConOut, 3);
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);

    Print(L"Welcome to ");
    gST->ConOut->SetAttribute(gST->ConOut, EFI_CYAN);
    Print(L"Zenith ");
    gST->ConOut->SetAttribute(gST->ConOut, EFI_RED);
    Print(L"Bootloader!\r\n\r\n");
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);

    stat = locateFS(&fs);
    ERRCHECK(L"error when locating the file system", stat);

    stat = fs->OpenVolume(fs, &root);
    ERRCHECK(L"error when opening root", stat);

    stat = root->Open(root, &kernel, KERNEL_NAME, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    ERRCHECK(L"error when opening the kernel file", stat);

    /* loading the kernel into memory */
    Elf64_Ehdr ehdr;
    Elf64_Phdr *phdrs = NULL;
    BOOLEAN iself;

    stat = isValidELF(&ehdr, kernel, &iself);
    ERRCHECK(L"error when validating elf file", stat);

    if (!iself) {
        Print(L"error when validating elf file: File is not a valid elf file\r\n");
        pauseConsole();
        return 1;
    }

    stat = readHeaders(&ehdr, &phdrs, kernel);
    ERRCHECK(L"error when reading ELF program headers", stat);

    for (UINT32 i = 0;i<ehdr.e_phnum;i++) {
        Elf64_Phdr *phdr = phdrs + i;

        if (phdr->p_type == PT_LOAD) {
            UINTN pages         = EFI_SIZE_TO_PAGES(phdr->p_memsz),
               filesize         = phdr->p_filesz;
            Elf64_Addr segment = phdr->p_paddr;
            EFI_MEMORY_TYPE memtype = EfiLoaderData;
            if (phdr->p_flags & PF_X) {
                if ((phdr->p_flags & PF_WX) == PF_WX) {
                    Print(L"WARNING: SEGMENT HAS WRITE & EXECUTE PERMISSIONS\r\n");
                    pauseConsole();
                }
                memtype = EfiLoaderCode;
            }
            stat = gBS->AllocatePages(AllocateAddress, memtype, pages, &segment);
            ERRCHECK(L"error when allocating pages for segment", stat);
            SetMem((VOID*)segment, phdr->p_memsz, 0);
            kernel->SetPosition(kernel, phdr->p_offset);
            kernel->Read(kernel, &filesize, (VOID*)segment);
            Print(L"kernel section loaded at 0x%x (%d MiB) (%d bytes loaded, %d bytes allocated)\n", 
                  segment, (UINTN)(segment) / (1024 * 1024), filesize, phdr->p_memsz);
            if (filesize < phdr->p_memsz) {
                UINTN extra = phdr->p_memsz - filesize;
                SetMem((VOID*)(segment + filesize), extra, 0);
                Print(L"zeroed %lu bytes of segment\r\n", extra);
            }
        }
    }
    Print(L"Kernel is loaded\r\n");

    FreePool(phdrs);

    kernel->Close(kernel);
    root->Close(root);

    stat = initGraphics(&gop);
    ERRCHECK(L"error when initializing graphics", stat)

    UINTN width     = gop->Mode->Info->HorizontalResolution,
         height     = gop->Mode->Info->VerticalResolution;
    UINT32 *fb      = (UINT32*)gop->Mode->FrameBufferBase;

    UINTN mapsize, mapkey, descsize;
    UINT32 descver;
    EFI_MEMORY_DESCRIPTOR *memmap = NULL;

    stat = gBS->GetMemoryMap(&mapsize, memmap, &mapkey, &descsize, &descver);

    mapsize += 2 * descsize;

    stat = gBS->AllocatePool(EfiLoaderData, mapsize, (VOID**)&memmap);

    stat = gBS->GetMemoryMap(&mapsize, memmap, &mapkey, &descsize, &descver);

    stat = gBS->ExitBootServices(handle, mapkey);
    ERRCHECK(L"failed to exit boot services", stat);

    for (UINTN y = 0; y < height; y++) {
        for (UINTN x = 0; x < width; x++) {
            fb[(y * width) + x] = 0x00FFFFFF;
        }
    }

    kmain_t main = (kmain_t)ehdr.e_entry;
    main(fb, width, height);

    __builtin_unreachable();
}
