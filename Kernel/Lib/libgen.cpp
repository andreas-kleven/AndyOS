#include <libgen.h>
#include <string.h>

const char *basename(char *path)
{
    char *p;
    if (path == NULL || *path == '\0')
        return ".";
    p = path + strlen(path) - 1;
    while (*p == '/')
    {
        if (p == path)
            return path;
        *p-- = '\0';
    }
    while (p >= path && *p != '/')
        p--;
    return p + 1;
}

const char *dirname(char *path)
{
    char *p;
    if (path == NULL || *path == '\0')
        return ".";
    p = path + strlen(path) - 1;
    while (*p == '/')
    {
        if (p == path)
            return path;
        *p-- = '\0';
    }
    while (p >= path && *p != '/')
        p--;
    return p < path ? "." : p == path ? "/" : (*p = '\0', path);
}

// https://stackoverflow.com/questions/28659344/alternative-to-realpath-to-resolve-and-in-a-path#answer-30361721
int pathcanon(const char *srcpath, char *dstpath, size_t sz)
{
    size_t plen = strlen(srcpath) + 1, chk;
    char wtmp[plen], *tokv[plen], *s, *tok, *sav;
    int i, ti, relpath;

    relpath = (*srcpath == '/') ? 0 : 1;

    /* make a local copy of srcpath so strtok(3) won't mangle it */

    ti = 0;
    (void)strcpy(wtmp, srcpath);

    tok = strtok_r(wtmp, "/", &sav);
    while (tok != NULL)
    {
        if (strcmp(tok, "..") == 0)
        {
            if (ti > 0)
            {
                ti--;
            }
        }
        else if (strcmp(tok, ".") != 0)
        {
            tokv[ti++] = tok;
        }
        tok = strtok_r(NULL, "/", &sav);
    }

    chk = 0;
    s = dstpath;

    /*
     * Construct canonicalized result, checking for room as we
     * go. Running out of space leaves dstpath unusable: written
     * to and *not* cleanly NUL-terminated.
     */
    for (i = 0; i < ti; i++)
    {
        size_t l = strlen(tokv[i]);

        if (i > 0 || !relpath)
        {
            if (++chk >= sz)
                return -1;
            *s++ = '/';
        }

        chk += l;
        if (chk >= sz)
            return -1;

        strcpy(s, tokv[i]);
        s += l;
    }

    if (s == dstpath)
    {
        if (++chk >= sz)
            return -1;
        *s++ = relpath ? '.' : '/';
    }
    *s = '\0';

    return 0;
}