#ifndef main_h
#define main_h

//
// see: http://www.nongnu.org/avr-libc/user-manual/mem_sections.html
// .noinit / .init3 sections
//
// see: http://www.nongnu.org/avr-libc/user-manual/group__largedemo.html
// naked attribute
//
uint8_t my_mcusr __attribute__ ((section(".noinit")));
void read_mcusr(void) __attribute__((section(".init3"))) __attribute__((naked));

void setup_hw(void);
int main(void);

#endif
