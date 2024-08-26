#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H
#include "types.h"
#include "port.h"
#include "gdt.h"
class InterruptManager
{
protected:
    static InterruptManager* ActiveInterruptManager;
    struct GateDecriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegement;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;

    } __attribute__((packed));
    static GateDecriptor interruptDescriptorTable[256];
    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));
    static void SetInterruptDescriptionTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t DescriptorPrivilageLevel,
        uint8_t DescriptorType);
    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;

public:
    InterruptManager(GlobalDescriptorTable *gdt);
    ~InterruptManager();
    void Activate();
    void Deactivate();
    static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp);
    uint32_t DoHandleInterrupt(uint8_t interruptNumber,uint32_t esp);

    static void InterruptIgnore();
    static void HandleInterruptRequest0x01();
    static void HandleInterruptRequest0x00();
};

#endif