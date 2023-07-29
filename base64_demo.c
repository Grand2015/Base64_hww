
#include <stdio.h>
#include <string.h>
#include "base64_hww.h"


int main()
{
	char str_input[128] = {0};
	char str_output[128*2] = {0};
	int type = 0;

	hww_base64_init(HWW_DEBUG_LEVEL_ERR);

	while(1){
		memset(str_input, 0, sizeof(str_input));
		memset(str_output, 0, sizeof(str_output));
		printf("PLease input operate type:1-Base64 Ecryption,2-Base64 Decryption,3-Quite.\n");
		scanf("%d", &type);

		switch (type){
			case 1: 
				printf("PLease input data which need to be ecrypted by Base64.\n");
				scanf("%s", str_input);
				printf("The input data is:%s\n", str_input);
				hww_base64_encryption(str_input, strlen(str_input), str_output, sizeof(str_output));
				printf("Base64 Ecrypetion:%s\n", str_output);
				break;
			case 2:
				printf("PLease input data which need to be dcrypted by Base64.\n");
				scanf("%s", str_input);
				printf("The input data is:%s\n", str_input);
				hww_base64_decryption(str_input, strlen(str_input), str_output, sizeof(str_output));
				printf("Base64 Dcrypetion:%s\n", str_output);
				break;
			case 3:
				printf("Say Bye-Bye.\n");
				goto END;
				break;
			default:
				printf("The type(%d) which you input has been not supported.\n", type);
				break;				
		}
	}
	END:
	return 0;
}
