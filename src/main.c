#define _XOPEN_SOURCE
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <locale.h>

#define WIDTH 70

wchar_t *buf;
size_t bufcap;
size_t buflen;
int curlinelen = 0;

void push(wchar_t c) {
    if (buflen == bufcap) {
        buf = realloc(buf, (2 * buflen + 1) * sizeof(wchar_t));
        bufcap += 2 * buflen;
    }
    buf[buflen] = c;
    buflen++;
    buf[buflen] = '\0';
}

void clear() {
    buf[0] = '\0';
    buflen = 0;
}

wchar_t peek(FILE *fp)
{
    wchar_t c = fgetwc(fp);
    if (c != WEOF) ungetwc(c, fp);
    return c;
}

int g_widechar;

int skipspace(FILE *fp, FILE *out)
{
    int lines = 0;
    int count  = 0;
    while (1) {
        wchar_t c = peek(fp);
        if (c == '\n') lines++;
        if (iswspace(c)) {
            fgetwc(fp);
            count++;
        }
        else break;
    }
    if (count > 0 && lines < 2 && g_widechar) {
        fputwc(' ', out);
        curlinelen++;
    }
    return lines;
}

int widechar_next;

int readword(FILE *fp, FILE *out)
{
    int len = 0;
    int lines = skipspace(fp, out);
    widechar_next = 0;
    if (lines >= 2) {
        return len;
    }
    wchar_t c = peek(fp);
    if (c == WEOF) return -1;
    if (wcwidth(c) > 1) {
        fgetwc(fp);
        push(c);
        g_widechar = 1;
        return wcwidth(c);
    }
    while(1) {
        c = peek(fp);
        if (iswspace(c) || c == WEOF) break;
        if (wcwidth(c) > 1) {
            widechar_next = -1;
            break;
        }
        len += wcwidth(c);
        fgetwc(fp);
        push(c);
    }
    g_widechar = 0;
    return len;
}

int main()
{
    setlocale(LC_ALL, "");
    buf = malloc(1025 * sizeof(wchar_t));
    buf[0] = '\0';
    bufcap = 1024;
    buflen = 0;
    FILE* fp = stdin;
    FILE* out = stdout;
    while (1) {
        int len = readword(fp, out);
        if (len < 0) {
            break;
        }
        if (len == 0) {
            fputwc('\n', out);
            fputwc('\n', out);
            curlinelen = 0;
            continue;
        }
        if (curlinelen + len < WIDTH) {
            fputws(buf, out);
            clear(buf);
            curlinelen += len;
            if (curlinelen < WIDTH && !g_widechar && !widechar_next) {
                fputwc(' ', out);
                curlinelen++;
            }
        } else {
            if (curlinelen != 0) {
                fputwc('\n', out);
            }
            curlinelen = len;
            fputws(buf, out);
            clear(buf);
        }
    }
    printf("\n");
    return 0;
}
