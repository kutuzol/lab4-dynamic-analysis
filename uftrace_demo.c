#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
static void heavy_computation(int duration) {
usleep(duration * 1000);
}
static void function_A(int count) {
for (int i = 0; i < count; i++) {
 usleep(5000);
}
}
static void function_B(void) {
puts("-> Entering function B");
heavy_computation(10);
function_A(2);
puts("<- Exiting function B");
}
int main(void) {
puts("Program started. Tracing with uftrace...");
srand((unsigned)time(NULL));
function_B();
heavy_computation(20);
puts("Program finished.");
return 0;
}
