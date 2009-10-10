#if 0
    vs_1_1
    dcl_position v0
    dcl_color v1
    dp3 r1, v0, v0
    rsq r0.x, r1.x
    mul r0.x, r0.x, c4.x
    sge r1.x, r1.x, r1.x
    add r1.x, r1.x, -c5.x
    mul r0.x, r0.x, r1.x
    add r0.x, r0.x, c5.x
    rcp r0.x, r0.x
    mov r2, v0
    mul r2.w, r2.w, r0.x
    mov oPos, r2
    mov oD0, v1

// approximately 12 instruction slots used
#endif

const DWORD g_vs11_main[] =
{
    0xfffe0101, 0x0000001f, 0x80000000, 0x900f0000, 0x0000001f, 0x8000000a, 
    0x900f0001, 0x00000008, 0x800f0001, 0x90e40000, 0x90e40000, 0x00000007, 
    0x80010000, 0x80000001, 0x00000005, 0x80010000, 0x80000000, 0xa0000004, 
    0x0000000d, 0x80010001, 0x80000001, 0x80000001, 0x00000002, 0x80010001, 
    0x80000001, 0xa1000005, 0x00000005, 0x80010000, 0x80000000, 0x80000001, 
    0x00000002, 0x80010000, 0x80000000, 0xa0000005, 0x00000006, 0x80010000, 
    0x80000000, 0x00000001, 0x800f0002, 0x90e40000, 0x00000005, 0x80080002, 
    0x80ff0002, 0x80000000, 0x00000001, 0xc00f0000, 0x80e40002, 0x00000001, 
    0xd00f0000, 0x90e40001, 0x0000ffff
};
