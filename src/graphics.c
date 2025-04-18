#include "graphics.h"

EFI_STATUS EFIAPI initGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
    EFI_STATUS stat;
    EFI_GUID guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    stat = gBS->LocateProtocol(&guid, NULL, (VOID**)gop);
    if (EFI_ERROR(stat)) return stat;

    return stat;
}
