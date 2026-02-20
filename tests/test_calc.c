
#include <stdio.h>
#include <stdint.h>

int main() {
    printf("--- 学习命令(索引0):\n");
    printf("帧头: 0x68\n");
    printf("长度: 1+2+1+1+1+1+1 = 8 (0x08 0x00)\n");
    printf("模块地址: 0x00\n");
    printf("功能码: 0x10\n");
    printf("数据: 0x00\n");
    printf("校验: (0x00 + 0x10 + 0x00) %% 256 = 0x10\n");
    printf("帧尾: 0x16\n\n");
    
    printf("完整帧: 68 08 00 00 10 00 10 16\n\n");

    printf("--- 应答帧示例:\n");
    printf("完整帧: 68 08 00 00 01 01 02 16\n");
    printf("校验: (0x00 + 0x01 + 0x01) %% 256 = 0x02\n\n");
    
    return 0;
}
