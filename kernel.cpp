
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"


// Define NULL if it's not already defined
#ifndef NULL
#define NULL ((void*)0)
#endif

// Define va_list and related macros
typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)

// Function to convert an integer to a string
void intToStr(int32_t num, char* str, int32_t base) {
    int32_t i = 0;
    bool isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int32_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';

    // Reverse the string
    int32_t start = 0;
    int32_t end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Helper function to print a single character
void printChar(char c) {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint8_t x = 0, y = 0;

    switch(c) {
        case '\n':
            x = 0;
            y++;
            break;
        default:
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | c;
            x++;
            break;
    }

    if(x >= 80) {
        x = 0;
        y++;
    }

    if(y >= 25) {
        for(y = 0; y < 25; y++)
            for(x = 0; x < 80; x++)
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
        x = 0;
        y = 0;
    }
}

// Function to print a string
void print(const char* str) {
    for(int32_t i = 0; str[i] != '\0'; ++i) {
        printChar(str[i]);
    }
}

// Function to print an integer
void print(int32_t num, int32_t base) {
    char str[32];
    intToStr(num, str, base);
    print(str);
}

// Function to print a pointer
void printPtr(void* ptr) {
    print("0x");
    print((uint32_t)ptr, 16);
}

// Variadic printf function
void printf2(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format != '\0') {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd': {
                    int32_t i = va_arg(args, int32_t);
                    print(i, 10);
                    break;
                }
                case 'x': {
                    uint32_t i = va_arg(args, uint32_t);
                    print(i, 16);
                    break;
                }
                case 'p': {
                    void* ptr = va_arg(args, void*);
                    printPtr(ptr);
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    print(s);
                    break;
                }
                case '%': {
                    printChar('%');
                    break;
                }
                default:
                    printChar(*format);
                    break;
            }
        } else {
            printChar(*format);
        }
        format++;
    }

    va_end(args);
}


void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}



typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello World! --- http://www.AlgorithMan.de");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);
    KeyboardDriver keyboard(&interrupts);
    interrupts.Activate();

    while(1);
}
