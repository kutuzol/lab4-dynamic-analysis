#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void process_input(const char *s) {
size_t len = strlen(s);
char *buf = malloc(32);
if (!buf) {
 fprintf(stderr, "malloc failed\n");
 return;
}
 // безопасное копирование с учётом NUL
if (len >= 32) {
 memcpy(buf, s, 31);
 buf[31] = '\0';
} else {
 memcpy(buf, s, len + 1);
}
 if (len > 10) {
 char *tmp = malloc(16);
 if (tmp) { memset(tmp, 0, 16);
 free(tmp);
 }
}
 if (s[0] == 'X' && len > 0) {
 char *p = malloc(8);
 if (p) {
 p[0] = 'A';
 printf("Before free: %c\n", p[0]);
 free(p);
 // не обращаемся к p после free
 }
}
free(buf);
}
int checksum(const char *data) {
unsigned int sum = 0; // инициализировано
for (size_t i = 0, n = strlen(data); i < n; ++i) {
 sum += (unsigned char)data[i];
}
return (int)(sum & 0xFFFF);
}

int main(int argc, char **argv) {
char buf[128] = {0};
if (argc > 1) {
 strncpy(buf, argv[1], sizeof(buf) - 1);
 buf[sizeof(buf) - 1] = '\0';
} else {
 if (fgets(buf, sizeof(buf), stdin) == NULL) {
 return 1;
 }
 buf[strcspn(buf, "\n")] = 0;
}
 process_input(buf);
int cs = checksum(buf);
printf("cs=%d\n", cs);
return 0;
}