#include "filesystem.h"

EFI_STATUS locateFS(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL **fs) {
    EFI_GUID guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_STATUS stat;
    EFI_HANDLE *handles = NULL;
    UINTN handlecnt = 0;
    stat = gBS->LocateHandleBuffer(
        ByProtocol,
        &guid,
        NULL,
        &handlecnt,
        &handles
    );
    if (EFI_ERROR(stat)) return stat;
    for (UINTN i = 0;i<handlecnt;i++) {
        stat = gBS->HandleProtocol(
            handles[i],
            &guid,
            (VOID**)fs
        );
        if (!EFI_ERROR(stat)) break;
    }
    return stat;
}
