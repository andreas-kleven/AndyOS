#pragma once

#define UTSNAMELEN 65

struct utsname
{
    char sysname[UTSNAMELEN];
    char nodename[UTSNAMELEN];
    char release[UTSNAMELEN];
    char version[UTSNAMELEN];
    char machine[UTSNAMELEN];
    char domainname[UTSNAMELEN];
};
