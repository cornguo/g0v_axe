#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct string {
    size_t len;
    char *ptr;
};

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = (char *)realloc(s->ptr, new_len + 1);
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

char *convertTable(struct string *s) {
    char *sPtr;
    int tableLen = 0;
    char *tok;
    char key[6][30] = {"", "", "", "", "", ""};
    int kCnt = 0;
    char buf[100] = "";
    char *ret = (char *)malloc(sizeof(char) * 10240);
    sPtr = strstr(s->ptr, "<table");
    tableLen = strstr(sPtr, "</table>") - sPtr + 8;
    sPtr[tableLen] = '\0';

    // process line by line
    tok = strtok(sPtr, "\n");
    strcpy(ret, "[");

    while (NULL != tok) {
        tok = strstr(tok, "<");
        switch (tok[1]) {
            case 't':
                // get keys
                if ('a' == tok[2]) {
                    while ('/' != tok[1]) {
                        if ('d' == tok[2]) {
                            tok = strstr(tok, ">") + 1;
                            tok[strstr(tok, "<") - tok] = '\0';
                            strcpy(key[kCnt++], tok);
                        }
                        tok = strtok(NULL, "\n");
                        tok = strstr(tok, "<");
                    }
                }
                if ('r' == tok[2]) {
                    strcat(buf, "{");
                    kCnt = 0;
                    while ('/' != tok[1]) {
                        if ('d' == tok[2]) {
                            tok = strstr(tok, ">") + 1;
                            tok[strstr(tok, "<") - tok] = '\0';
                            if (0 == kCnt) {
                                strcat(buf, "\"name\":\"");
                                strcat(buf, tok);
                                strcat(buf, "\",\"grades\": {");
                            } else {
                                strcat(buf, "\"");
                                strcat(buf, key[kCnt]);
                                strcat(buf, "\":");
                                strcat(buf, tok);
                                if (5 == kCnt) {
                                    strcat(buf, "}");
                                } else {
                                    strcat(buf, ",");
                                }
                            }
                            kCnt++;
                        }
                        tok = strtok(NULL, "\n");
                        tok = strstr(tok, "<");
                    }
                    strcat(buf, "}");
                    strcat(ret, buf);
                    strcat(ret, ",");
                    strcpy(buf, "");
                }
            break;
            case '/':
            break;
        }
        tok = strtok(NULL, "\n");
    }

    ret[strlen(ret) - 1] = ']';
    ret[strlen(ret)] = '\0';

    return ret;
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    struct string s;

    s.len = 0;
    s.ptr = (char *) malloc(s.len + 1);
    s.ptr[0] = '\0';

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://axe-level-1.herokuapp.com/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    printf("%s", convertTable(&s));

    return 0;
}
