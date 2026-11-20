#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <time.h>
#include <unistd.h>
#define BUFFER_SIZE 50
int generate_random_length() {
return (rand() % (BUFFER_SIZE / 2)) + 10;
}
int process_data(const char* input_data) {
int length = generate_random_length();
char* dynamic_buffer = (char*)malloc(length + 1);
if (dynamic_buffer == NULL) {
 fprintf(stderr, "Memory allocation failed!\n");
 exit(EXIT_FAILURE);
}
strncpy(dynamic_buffer, input_data, length);
dynamic_buffer[length] = '\0';
printf("Processing data: %s (length %d)\n",
dynamic_buffer, length);
free(dynamic_buffer);
return length;
}
int main() {
srand(time(NULL));
puts("Complex program started.");
const char* data_source = "This is a longer string that we will process dynamically.";
int processed_len = process_data(data_source);
printf("Sleeping for 1 second after processing length %d\n", processed_len);
sleep(1);
puts("Program finished.");
exit(EXIT_SUCCESS);
}