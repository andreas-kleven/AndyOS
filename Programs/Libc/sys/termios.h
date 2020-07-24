#pragma once

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

#define ISIG 0000001   /* Enable signals.  */
#define ICANON 0000002 /* Canonical input (erase and kill processing).  */
#if defined __USE_MISC || (defined __USE_XOPEN && !defined __USE_XOPEN2K)
#define XCASE 0000004
#endif
#define ECHO 0000010   /* Enable echo.  */
#define ECHOE 0000020  /* Echo erase character as error-correcting \
              backspace.  */
#define ECHOK 0000040  /* Echo KILL.  */
#define ECHONL 0000100 /* Echo NL.  */
#define NOFLSH 0000200 /* Disable flush after interrupt or quit.  */
#define TOSTOP 0000400 /* Send SIGTTOU for background output.  */
#ifdef __USE_MISC
#define ECHOCTL 0001000 /* If ECHO is also set, terminal special characters \
               other than TAB, NL, START, and STOP are echoed as            \
               ^X, where X is the character with ASCII code 0x40            \
               greater than the special character                           \
               (not in POSIX).  */
#define ECHOPRT 0002000 /* If ICANON and ECHO are also set, characters are \
               printed as they are being erased                            \
               (not in POSIX).  */
#define ECHOKE 0004000  /* If ICANON is also set, KILL is echoed by erasing \
               each character on the line, as specified by ECHOE            \
               and ECHOPRT (not in POSIX).  */
#define FLUSHO 0010000  /* Output is being flushed.  This flag is toggled by \
               typing the DISCARD character (not in POSIX).  */
#define PENDIN 0040000  /* All characters in the input queue are reprinted \
               when the next character is read                             \
               (not in POSIX).  */
#endif
#define IEXTEN 0100000 /* Enable implementation-defined input \
              processing.  */
#ifdef __USE_MISC
#define EXTPROC 0200000
#endif

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

#define NCCS 32
struct termios
{
    tcflag_t c_iflag; /* input mode flags */
    tcflag_t c_oflag; /* output mode flags */
    tcflag_t c_cflag; /* control mode flags */
    tcflag_t c_lflag; /* local mode flags */
    cc_t c_line;      /* line discipline */
    cc_t c_cc[NCCS];  /* control characters */
    speed_t c_ispeed; /* input speed */
    speed_t c_ospeed; /* output speed */

    #define _HAVE_STRUCT_TERMIOS_C_ISPEED 1
    #define _HAVE_STRUCT_TERMIOS_C_OSPEED 1
};

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
