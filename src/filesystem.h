#ifndef _filesystem_h
#define _filesystem_h

#include <efi.h>
#include <efilib.h>

EFI_STATUS locateFS(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL **fs);

#endif
