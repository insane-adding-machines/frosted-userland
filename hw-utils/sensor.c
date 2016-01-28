#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <locale.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>
#include <ioctl.h>

struct lsm303dlhc_ctrl_reg
{
    uint8_t reg;
    uint8_t data;
}; 

struct l3gd20_ctrl_reg
{
    uint8_t reg;
    uint8_t data;
}; 


#define L3GD20_WHOAMI              0x0F
#define L3GD20_CTRL_REG1      0x20
#define L3GD20_CTRL_REG2       0x21
#define L3GD20_CTRL_REG3       0x22
#define L3GD20_CTRL_REG4       0x23
#define L3GD20_CTRL_REG5       0x24
#define L3GD20_REFERENCE       0x25
#define L3GD20_OUT_TEMP        0x26
#define L3GD20_STATUS_REG      0x27
#define L3GD20_OUT_X_L         0x28
#define L3GD20_OUT_X_H         0x29
#define L3GD20_OUT_Y_L         0x2A
#define L3GD20_OUT_Y_H         0x2B
#define L3GD20_OUT_Z_L         0x2C
#define L3GD20_OUT_Z_H         0x2D
#define L3GD20_FIFO_CTRL_REG 0x2E
#define L3GD20_FIFO_SRC_REG    0x2F
#define L3GD20_INT1_CFG        0x30
#define L3GD20_INT1_SRC        0x31
#define L3GD20_INT1_TSH_XH 0x32
#define L3GD20_INT1_TSH_XL 0x33
#define L3GD20_INT1_TSH_YH 0x34
#define L3GD20_INT1_TSH_YL 0x35
#define L3GD20_INT1_TSH_ZH 0x36
#define L3GD20_INT1_TSH_ZL 0x37
#define L3GD20_INT1_DURATION 0x38

int gyro(int argc, char *args[])
{
    int fd, i=100;
    unsigned char buffer[6];
    struct l3gd20_ctrl_reg l3gd20;

    if(argc != 3 || args[2] == NULL)
    {
        printf("Usage : gyro on|off\n\r");
        goto exit;
    }

    fd = open("/dev/l3gd20", O_RDONLY);
    if (fd < 0) {
        printf("File not found.\r\n");
        exit(-1);
    }

    if(strcasecmp(args[2], "off") == 0)
    {
        l3gd20.reg = L3GD20_CTRL_REG1;
        l3gd20.data = 0x00;
        ioctl(fd, IOCTL_L3GD20_WRITE_CTRL_REG, &l3gd20);
    }
    else
    {
        l3gd20.reg = L3GD20_WHOAMI;
        ioctl(fd, IOCTL_L3GD20_READ_CTRL_REG, &l3gd20);
        printf("WHOAMI=%02X\n\r",l3gd20.data);

        l3gd20.reg = L3GD20_CTRL_REG1;
        l3gd20.data = 0x0F;                 /*PD | Zen | Yen | Zen  */
        ioctl(fd, IOCTL_L3GD20_WRITE_CTRL_REG, &l3gd20);

        l3gd20.reg = L3GD20_CTRL_REG3;
        l3gd20.data = 0x08;
        ioctl(fd, IOCTL_L3GD20_WRITE_CTRL_REG, &l3gd20);

        while(i--) {
            read(fd, buffer ,6);
            printf("%04X %04X %04X\n\r", *((uint16_t*)buffer), *((uint16_t*)&buffer[2]), *((uint16_t*)&buffer[4]) ); 
        }
    }
    close(fd);

exit:
    exit(0);
}

#define LSM303ACC_CTRL_REG1      0x20

int acc(int argc, char *args[])
{
    int fd;
    struct lsm303dlhc_ctrl_reg lsm303dlhc;

    if(argc != 3 || args[2] == NULL)
    {
        printf("Usage : acc on|off\n\r");
        goto exit;
    }

    fd = open("/dev/lsm303acc", O_RDONLY);
    if (fd < 0) {
        printf("File not found.\r\n");
        exit(-1);
    }

    if(strcasecmp(args[2], "off") == 0)
    {
        lsm303dlhc.reg = LSM303ACC_CTRL_REG1;
        lsm303dlhc.data = 0x00;
        ioctl(fd, IOCTL_LSM303DLHC_WRITE_CTRL_REG, &lsm303dlhc);
    }
    else
    {
        lsm303dlhc.reg = LSM303ACC_CTRL_REG1;
        lsm303dlhc.data = 0x20 | 0x07;;                 /*ODR | (LP | Zen | Yen | Zen)  */
        ioctl(fd, IOCTL_LSM303DLHC_WRITE_CTRL_REG, &lsm303dlhc);

        lsm303dlhc.reg = LSM303ACC_CTRL_REG1;
        lsm303dlhc.data = 0x00;                 
        ioctl(fd, IOCTL_LSM303DLHC_READ_CTRL_REG, &lsm303dlhc);

    }
    close(fd);

exit:
    exit(0);
}

#define CRA_REG_M   0x00

int mag(int argc, char *args[])
{
    int fd;
    struct lsm303dlhc_ctrl_reg lsm303dlhc;

    if(argc != 3 || args[2] == NULL)
    {
        printf("Usage : mag on|off\n\r");
        goto exit;
    }

    fd = open("/dev/lsm303mag", O_RDONLY);
    if (fd < 0) {
        printf("File not found.\r\n");
        exit(-1);
    }

    if(strcasecmp(args[2], "off") == 0)
    {
        lsm303dlhc.reg = CRA_REG_M;
        lsm303dlhc.data = 0x00;
        ioctl(fd, IOCTL_LSM303DLHC_WRITE_CTRL_REG, &lsm303dlhc);
    }
    else
    {
        lsm303dlhc.reg = CRA_REG_M;
        lsm303dlhc.data = 0x10;
        ioctl(fd, IOCTL_LSM303DLHC_WRITE_CTRL_REG, &lsm303dlhc);

        lsm303dlhc.reg = CRA_REG_M;
        lsm303dlhc.data = 0x00;                 
        ioctl(fd, IOCTL_LSM303DLHC_READ_CTRL_REG, &lsm303dlhc);

    }
    close(fd);

exit:
    exit(0);
}

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "mag")==0)
        return mag(argc, argv);
    if (strcmp(argv[1], "gyro")==0)
        return gyro(argc, argv);
    if (strcmp(argv[1], "acc")==0)
        return acc(argc, argv);
    printf("Usage: %s [mag|gyro|acc] <cmd>\r\n");
    return 1;
}

