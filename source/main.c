#include <nds.h>
#include <stdint.h>
#include <stdbool.h>

void __attribute__((noinline, naked, target("arm"))) test(uint32_t* vals)
{
    asm volatile (
        "sub r12, pc,#8 \n"
        "stmfd sp!, {pc} \n"
        "str pc, [sp, #-4]! \n"
        "sub sp, sp, #4 \n"
        "str pc, [sp] \n"
        "ldmfd sp! , {r1-r3} \n"
        "stmia r0, {r1-r3, r12} \n"
        "bx lr \n"
        :
    );
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();
    consoleDemoInit();
    puts("CPU STR/STM PC instruction test\n");
    uint32_t* testResults = (uint32_t*)malloc(sizeof(uint32_t)*4);
    test(testResults);

    // 0 - what is stored by str pc [sp]
    // 1 - what is stored by str pc [sp] with postdec
    // 2 - what is stored by stmfd sp!, {pc}
    // 3 - address of test() func
    uint32_t str_pc_result          = testResults[0];
    uint32_t str_pc_postdec_result  = testResults[1];
    uint32_t stmfd_pc_result        = testResults[2];
    uint32_t test_func_addr         = testResults[3];

    // these should never go negative but i'm accounting for my own stupidity :)
    int32_t str_pc_offset           = str_pc_result - (test_func_addr+16);
    int32_t str_pc_postdec_offset   = str_pc_postdec_result - (test_func_addr+8);
    int32_t stmfd_pc_offset         = stmfd_pc_result - (test_func_addr+4);

    printf(
        "str pc, [sp]\n"
        "addr: 0x%X\n"
        "result: 0x%X\n"
        "offset: %i\n\n",
        test_func_addr + 16,
        str_pc_result,
        str_pc_offset
    );

    printf(
        "str pc, [sp, #-4]!\n"
        "addr: 0x%X\n"
        "result: 0x%X\n"
        "offset: %i\n\n",
        test_func_addr + 8,
        str_pc_postdec_result,
        str_pc_postdec_offset
    );

    printf(
        "stmfd sp!, {pc}\n"
        "addr: 0x%X\n"
        "result: 0x%X\n"
        "offset: %i\n\n",
        test_func_addr + 4,
        stmfd_pc_result,
        stmfd_pc_offset
    );

    if ((str_pc_offset == str_pc_postdec_offset) & (str_pc_postdec_offset == stmfd_pc_offset))
    {
        if (str_pc_offset == 8)
            puts("fail! (inaccurate PC offset)");
        else if (str_pc_offset == 12)
            puts("pass!!");
        else
            puts("fail! (wtf PC offset)");
    }
    else
        puts("fail! (inconsistent PC offset)");

    puts("press start to exit");
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }
    return 0;
}