#pragma once

#define XCGETA  (('x' << 8) | 1)
#define XCSETA  (('x' << 8) | 2)
#define XCSETAW (('x' << 8) | 3)
#define XCSETAF (('x' << 8) | 4)
#define TCSBRK  (('T' << 8) | 5)
#define TCFLSH  (('T' << 8) | 7)
#define TCXONC  (('T' << 8) | 6)

#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

#define NCCS 13

#define TCSAFLUSH  XCSETAF
#define TCSANOW    XCSETA
#define TCSADRAIN  XCSETAW
#define TCSADFLUSH XCSETAF

#define IGNBRK 000001
#define BRKINT 000002
#define IGNPAR 000004
#define INPCK  000020
#define ISTRIP 000040
#define INLCR  000100
#define IGNCR  000200
#define ICRNL  000400
#define IXON   002000
#define IXOFF  010000

#define OPOST 000001
#define OCRNL 000004
#define ONLCR 000010
#define ONOCR 000020
#define TAB3  014000

#define CLOCAL 004000
#define CREAD  000200
#define CSIZE  000060
#define CS5    0
#define CS6    020
#define CS7    040
#define CS8    060
#define CSTOPB 000100
#define HUPCL  002000
#define PARENB 000400
#define PAODD  001000

#define ECHO   0000010
#define ECHOE  0000020
#define ECHOK  0000040
#define ECHONL 0000100
#define ICANON 0000002
#define IEXTEN 0000400 /* anybody know *what* this does?! */
#define ISIG   0000001
#define NOFLSH 0000200
#define TOSTOP 0001000

#define VEOF   4 /* also VMIN -- thanks, AT&T */
#define VEOL   5 /* also VTIME -- thanks again */
#define VERASE 2
#define VINTR  0
#define VKILL  3
#define VMIN   4 /* also VEOF */
#define VQUIT  1
#define VSUSP  10
#define VTIME  5 /* also VEOL */
#define VSTART 11
#define VSTOP  12

#define B0     000000
#define B50    000001
#define B75    000002
#define B110   000003
#define B134   000004
#define B150   000005
#define B200   000006
#define B300   000007
#define B600   000010
#define B1200  000011
#define B1800  000012
#define B2400  000013
#define B4800  000014
#define B9600  000015
#define B19200 000016
#define B38400 000017

typedef unsigned char cc_t;
typedef unsigned short tcflag_t;
typedef char speed_t;

struct termios
{
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    char c_line;
    cc_t c_cc[NCCS];
    speed_t c_ispeed;
    speed_t c_ospeed;
};

#define cfgetospeed(tp)    ((tp)->c_ospeed)
#define cfgetispeed(tp)    ((tp)->c_ispeed)
#define cfsetospeed(tp, s) (((tp)->c_ospeed = (s)), 0)
#define cfsetispeed(tp, s) (((tp)->c_ispeed = (s)), 0)

#define RTSXOFF 0x0001 /* RTS flow control on input */
#define CTSXON  0x0002 /* CTS flow control on output */
#define DTRXOFF 0x0004 /* DTR flow control on input */
#define DSRXON  0x0008 /* DCD flow control on output */

#define TIOCM_LE   0x001
#define TIOCM_DTR  0x002
#define TIOCM_RTS  0x004
#define TIOCM_ST   0x008
#define TIOCM_SR   0x010
#define TIOCM_CTS  0x020
#define TIOCM_CAR  0x040
#define TIOCM_RNG  0x080
#define TIOCM_DSR  0x100
#define TIOCM_CD   TIOCM_CAR
#define TIOCM_RI   TIOCM_RNG
#define TIOCM_OUT1 0x2000
#define TIOCM_OUT2 0x4000
#define TIOCM_LOOP 0x8000
