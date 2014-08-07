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
    char *tok;
    char key[3][30] = {"town", "village", "name"};
    int kCnt = 0;
    char buf[100] = "";
    char *ret = (char *)malloc(sizeof(char) * 10240);
    char *sPtr;
    int tableLen = 0;
    sPtr = strstr(s->ptr, "<table");
    tableLen = strstr(sPtr, "</table>") - sPtr + 8;
    sPtr[tableLen] = '\0';

    // process line by line
    tok = strtok(sPtr, "\n");
    strcpy(ret, "");

    while (NULL != tok) {
        tok = strstr(tok, "<");
        switch (tok[1]) {
            case 't':
                // ignore keys
                if ('a' == tok[2]) {
                    while ('/' != tok[1]) {
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
                            strcat(buf, "\"");
                            strcat(buf, key[kCnt]);
                            strcat(buf, "\":\"");
                            strcat(buf, tok);
                            strcat(buf, "\"");
                            if (2 != kCnt) {
                                strcat(buf, ",");
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

    ret[strlen(ret)] = '\0';

    return ret;
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist *slist = NULL;
    struct string s;
    char url[60] = "";
    char sessid[60] = "";
    char header[60] = "";
    int page = 1;
    char *parsed;

    s.len = 0;
    s.ptr = (char *) malloc(s.len + 1);
    s.ptr[0] = '\0';

    printf("[");
    for (page = 1; page <= 76; page++) {
        curl = curl_easy_init();
        if (curl) {
            if (1 == page) {
                strcpy(url, "http://axe-level-1.herokuapp.com/lv3/");
            } else {
                strcpy(url, "http://axe-level-1.herokuapp.com/lv3/?page=next");
            }
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
            if (1 == page) {
                curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            } else {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
            }
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }

            /* always cleanup */
            curl_easy_cleanup(curl);
        }
        if (1 == page) {
            parsed = strstr(s.ptr, "PHPSESSID");
            memcpy(sessid, parsed, strchr(parsed, ';') - parsed + 1);
            sessid[strlen(sessid)] = '\0';
            sprintf(header, "Cookie: %s", sessid);
            slist = curl_slist_append(slist, header);
        }
        parsed = convertTable(&s);
        if (76 == page) {
            parsed[strlen(parsed) - 1] = '\0';
        }
        printf("%s", parsed);
        free(s.ptr);
        s.len = 0;
        s.ptr = (char *) malloc(s.len + 1);
        s.ptr[0] = '\0';
    }
    curl_slist_free_all(slist);
    printf("]");

    return 0;
}
