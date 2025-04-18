#include "console.h"

VOID EFIAPI pauseConsole(VOID) {
    Print(L"Waiting for keyboard input before proceeding...\r\n");
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
    gST->ConIn->ReadKeyStroke(gST->ConIn, NULL);
}
