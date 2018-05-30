#include <stdlib.h>
#include <stdio.h>

#include "generic_vector.h"
#include "c_string.h"
#include "c_hashmap.h"

int main(int argc, char* argv[]) {
	VECTOR hVector = vector_init_default(string_assignment, string_destroy);
	if (hVector == NULL)
		exit(1);
	C_STRING arr[10];
	int i;
	for (i = 0; i < 10; i++) {
		arr[i] = string_init_c_string("test_");
		string_push_back(arr[i], (char)('0' + i));
		vector_push_back(hVector, arr[i]);
		string_destroy(&arr[i]);
	}
	for (i = 0; i < 10; i++) {
		C_STRING s = (C_STRING)vector_at(hVector, i);
		printf("%s\n", string_c_str(s));
	}
	
	vector_destroy(&hVector);
	return 0;
}