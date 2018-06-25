#include <string.h>

#include <3ds.h>

#include "stringutil.h"

bool string_is_empty(const char* str) {
    if(strlen(str) == 0) {
        return true;
    }

    const char* curr = str;
    while(*curr) {
        if(*curr != ' ') {
            return false;
        }

        curr++;
    }

    return true;
}

void string_copy(char* dst, const char* src, size_t size) {
    strncpy(dst, src, size);
    dst[size] = '\0';
}

void string_get_file_name(char* out, const char* file, u32 size) {
    const char* end = file + strlen(file);
    const char* curr = file - 1;
    while((curr = strchr(curr + 1, '.')) != NULL) {
        end = curr;
    }

    u32 terminatorPos = end - file < size - 1 ? end - file : size - 1;
    strncpy(out, file, terminatorPos);
    out[terminatorPos] = '\0';
}

void string_escape_file_name(char* out, const char* file, size_t size) {
    static const char reservedChars[] = {'<', '>', ':', '"', '/', '\\', '|', '?', '*'};

    for(u32 i = 0; i < size; i++) {
        bool reserved = false;
        for(u32 j = 0; j < sizeof(reservedChars); j++) {
            if(file[i] == reservedChars[j]) {
                reserved = true;
                break;
            }
        }

        if(reserved) {
            out[i] = '_';
        } else {
            out[i] = file[i];
        }

        if(file[i] == '\0') {
            break;
        }
    }
}

void string_get_path_file(char* out, const char* path, u32 size) {
    const char* start = NULL;
    const char* end = NULL;
    const char* curr = path - 1;
    while((curr = strchr(curr + 1, '/')) != NULL) {
        start = end != NULL ? end : path;
        end = curr;
    }

    if(end != path + strlen(path) - 1) {
        start = end;
        end = path + strlen(path);
    }

    if(end - start == 0) {
        strncpy(out, "/", size);
    } else {
        u32 terminatorPos = end - start - 1 < size - 1 ? end - start - 1 : size - 1;
        strncpy(out, start + 1, terminatorPos);
        out[terminatorPos] = '\0';
    }
}

void string_get_parent_path(char* out, const char* path, u32 size) {
    size_t pathLen = strlen(path);

    const char* start = NULL;
    const char* end = NULL;
    const char* curr = path - 1;
    while((curr = strchr(curr + 1, '/')) != NULL && (start == NULL || curr != path + pathLen - 1)) {
        start = end != NULL ? end : path;
        end = curr;
    }

    u32 terminatorPos = end - path + 1 < size - 1 ? end - path + 1 : size - 1;
    strncpy(out, path, terminatorPos);
    out[terminatorPos] = '\0';
}

static char dec2hex(u8 c) {
    if(c >= 0 && c <= 9) {
        return (char) ('0' + c);
    } else if(c >= 10 && c <= 15) {
        return (char) ('A' + (c - 10));
    } else {
        return -1;
    }
}

static u32 hex2dec(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return -1;
    }
}

void url_decode(char url[]) {
    u32 i = 0;
    u32 res_len = 0;
    size_t len = strlen(url);
    char res[1024];
    for (i = 0; i < len; ++i) {
        char c = url[i];
        if (c != '%') {
            res[res_len++] = c;
        } else {
            char c1 = url[++i];
            char c0 = url[++i];
            u32 num = 0;
            num = hex2dec(c1) * 16 + hex2dec(c0);
            res[res_len++] = num;
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}

void url_encode(char* out, const char* in, size_t size) {
    u32 pos = 0;
    size_t len = strlen(in);
    for(u32 i = 0; i < len && pos < size; i++) {
        char c = in[i];
        if(
            (c >= '0' && c <= '9') || 
            (c >= 'A' && c <= 'Z') || 
            (c >= 'a' && c <= 'z') || 
            c == '/' || 
            c == ':' || 
            c == '.' || 
            c == '-' || 
            c == '_' || 
            c == '+' || 
            c == '=' || 
            c == '?' || 
            c == '&' || 
            c == '%' || 
            c == '#' || 
            c == '@'
            ) {
            out[pos++] = c;
        } else {
            out[pos++] = '%';

            if(pos < size) {
                out[pos++] = dec2hex((u8) (((u8) c) / 16));
            }

            if(pos < size) {
                out[pos++] = dec2hex((u8) (((u8) c) % 16));
            }
        }
    }

    out[pos < size ? pos : size] = '\0';
}