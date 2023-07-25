
#include <stdio.h>
#include <string.h>
#include "base64_hww.h"


int main()
{
	char str_input[128] = {0};
	char str_output[128*2] = {0};

	scanf("%s", str_input);
	printf("str_input:%s\n", str_input);
	
	hww_base64_init(HWW_DEBUG_LEVEL_INFO);
	
	hww_base64_encryption(str_input, strlen(str_input), str_output, sizeof(str_output));
	printf("str_output:%s\n", str_output);
	return 0;
}
