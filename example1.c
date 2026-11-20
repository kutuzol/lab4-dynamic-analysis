#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void process_input(char *s) {
int len = strlen(s);
char buf[32] = {};
strcpy(buf, s); // потенциальный OOB
 if (len > 10) {
 // двойное освобождение
 char *tmp = malloc(16);
 free(tmp);
 free(tmp);
}
 if (s[0] == 'X') {
 char *p = malloc(8);
 free(p);
 // use-after-free
 p[0] = 'A';
 printf("After free: %c\n", p[0]);
}
}
int checksum(const char *data) {
unsigned short sum; // неинициализированная переменная
for (size_t i = 0; i < strlen(data); ++i) {
 sum += (unsigned char)data[i];
}
return sum;
}

int main(int argc, char **argv) {
char buf[128];
int n = read(0, buf, 127);
buf[n] = 0;
 printf("%s\n", buf);
process_input(buf);
int cs = checksum(buf);
printf("cs=%d\n", cs);
return 0;
}