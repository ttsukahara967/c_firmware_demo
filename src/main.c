/* No OS and no C library: everything below talks to the hardware directly. */

#include <stdint.h>

#include "semihosting.h"

/* SysTick and CPUID registers, common to all Cortex-M cores */
#define SYST_CSR (*(volatile uint32_t *)0xE000E010) /* Control and status */
#define SYST_RVR (*(volatile uint32_t *)0xE000E014) /* Reload value */
#define SYST_CVR (*(volatile uint32_t *)0xE000E018) /* Current value */
#define CPUID (*(volatile const uint32_t *)0xE000ED00)

#define SYST_CSR_ENABLE (1u << 0)
#define SYST_CSR_CLKSOURCE (1u << 2) /* 1 = core clock */
#define SYST_CSR_COUNTFLAG (1u << 16) /* Set on wrap, cleared on read */

/* No printf without a C library, so format numbers by hand */
static char *fmt_u32(char *p, uint32_t v)
{
    char tmp[10];
    int n = 0;
    do {
        tmp[n++] = (char)('0' + v % 10);
        v /= 10;
    } while (v != 0);
    while (n > 0) {
        *p++ = tmp[--n];
    }
    return p;
}

static char *fmt_hex32(char *p, uint32_t v)
{
    *p++ = '0';
    *p++ = 'x';
    for (int shift = 28; shift >= 0; shift -= 4) {
        *p++ = "0123456789abcdef"[(v >> shift) & 0xF];
    }
    return p;
}

static char *copy(char *p, const char *s)
{
    while (*s != '\0') {
        *p++ = *s++;
    }
    return p;
}

int main(void)
{
    char buf[32];
    char *p;

    sh_puts("Hello from bare-metal C!\n");

    SYST_RVR = 12000000 / 10; /* ~100 ms at 12 MHz */
    SYST_CVR = 0;
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_ENABLE;

    for (uint32_t tick = 1; tick <= 5; tick++) {
        /* Busy-wait until the counter wraps */
        while ((SYST_CSR & SYST_CSR_COUNTFLAG) == 0) {
        }
        p = copy(buf, "SysTick #");
        p = fmt_u32(p, tick);
        *p++ = '\n';
        *p = '\0';
        sh_puts(buf);
    }

    p = copy(buf, "CPUID = ");
    p = fmt_hex32(p, CPUID);
    *p++ = '\n';
    *p = '\0';
    sh_puts(buf);

    sh_exit_success();
    for (;;) {
    }
}
