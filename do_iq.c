
本实例仅仅考虑AD9361的数字范围变换。
AD9361的总线宽度为12位，因此需要考虑移位，具体的来说：
Buffer Bit	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
AD9361 Bit	11	10	 9  8	  7	  6	  5	4	3	2	1	0	X	X	X	X

此时，每个数据表示为16位，从而得出实际16位有效值，即I,Q有效值各位4字节大小。


代码：
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

int main (int argc , char* argv[])
{
        FILE *file;
	      //f：点数   d:0表示4字节，1表示8字节(默认)   
        int i, c, f = 10, j, d = 1;
        unsigned int *buf;
      	//幅度
        double ampl;
	      //实部 虚部
        short ipart, qpart;

        while ((c = getopt (argc, argv, "f:a:s")) != -1)
                switch (c) {
                case 'f':
                        f = atoi(optarg);
                        break;
                case 'a':
                        ampl = atof(optarg);
                        break;
                case 's':
                        d = 0;
                        break;
                default:
                        return 0;
        }
	      //buf开辟空间(每个单元8字节)
        buf = malloc(f * (d ? 8 : 4));
      	//幅度范围[0,1]
        if (ampl > 1.0)
                ampl = 1.0;
        else if (ampl < 0.0)
                ampl = 0.0;

        /* AD9361 12-bit MSB aligned [(2^(12-1) - 1) * 16]
	       * AD9361值范围(总线12bit)：-2048~2047  本示例模拟-2047~2047
	       * 实虚部范围(16bit): 实部16bit,虚部16bit
         */

        ampl = ampl * 2047;

        printf("32-bit Word: I : Q\n");

        for (i = 0, j = 0; i < (f); i++) {
                ipart = ampl * sin(2 * M_PI * (double)i / (double)(f));
                qpart = ampl * cos(2 * M_PI * (double)i / (double)(f));

                printf("0x%.8X : %d : %d\n", (ipart << 16) | (qpart & 0xFFFF),  ipart, qpart);

                buf[j++] = (ipart << 16) | (qpart & 0xFFFF);

                if (d) /* Second Channel */
                        buf[j++] = (ipart << 16) | (qpart & 0xFFFF);

        }

        free(buf);

        exit(EXIT_SUCCESS);
}
