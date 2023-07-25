
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "base64_hww.h"

#define HWW_BASE64_TABLE_LEN		(64)
#define HWW_BINARY_LEN				(8)
#define HWW_BASE64_DATE_LEN_MAX 	(128)
#define HWW_BASE64_PRIMARY_INTERVAL	(24)
#define HWW_BASE64_SECOND_INTERVAL	(6)
#define	HWW_BASE64_PAD_CHAR			'='

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define HWW_INFO(info,...)  \
do{ \
    if(g_debug_level>=HWW_DEBUG_LEVEL_INFO){\
        printf("Info %s,%s,%d:"info"",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)
 
 
#define HWW_HINT(info,...)  \
do{ \
    if(g_debug_level>=HWW_DEBUG_LEVEL_HINT){\
        printf("Debug %s,%s,%d:"info"",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)


#define HWW_ERR(info,...)  \
do{ \
    if(g_debug_level>=HWW_DEBUG_LEVEL_ERR){\
        printf("Error %s,%s,%d:"info"",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

char base64_table[HWW_BASE64_TABLE_LEN] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

HWW_DEBUG_LEVEL_E g_debug_level = HWW_DEBUG_LEVEL_ERR;


static void hww_print_binary_data(char *str_binary, int len_binary, const char *fun, int line)
{
	if(NULL == str_binary) return;
	
	int i =0;

	if (g_debug_level > HWW_DEBUG_LEVEL_ERR){
		printf("[%s:%d]--------------PRINT BEGIN-----------------\n", fun, line);
		printf("[%s:%d]--------------data len:%d-----------------\n", fun, line, len_binary);
		printf("[%s:%d]--------------data-----------------\n", fun, line);
		for (i = 0; i < len_binary; i++)
			printf("%d ", str_binary[i]);
		printf("\n[%s:%d]--------------PRINT END-----------------\n", fun, line);
	}

	return;
}

//56->00111000
static void hww_convert_ASCII_to_binary(char c_ASCII, char *str_binary)
{
	if (NULL == str_binary) return;

	char c = c_ASCII;
	char array[HWW_BINARY_LEN] = {0};
	char i = 0;
	
	for (i = 0; i < HWW_BINARY_LEN; i++){
		if (0 == c) break;
		array[HWW_BINARY_LEN - i - 1] = c%2;
		c = c/2;
	}

	memcpy(str_binary, array, HWW_BINARY_LEN);

	return;
}

static void hww_fill_0_by_bit(char *str_binary, int *p_len_binary)
{
	if (NULL == str_binary || NULL == p_len_binary) return;
	
	int len_binary = *p_len_binary;
	int num_group = 0;
	int num_binary_miss = 0;
	int i =0;
	
	num_group = (int)ceil(len_binary*1.0/HWW_BASE64_SECOND_INTERVAL);
	HWW_INFO("num_group:%d\n", num_group);

	//2.1 补0
	num_binary_miss = num_group*HWW_BASE64_SECOND_INTERVAL - len_binary;
	HWW_INFO("num_binary_miss:%d\n", num_binary_miss);

	for (i = 0; i < num_binary_miss; i++)
		str_binary[len_binary+i] = 0;

	*p_len_binary += num_binary_miss;
	hww_print_binary_data(str_binary, *p_len_binary, __FUNCTION__, __LINE__);
	
	
	return;
}

static int hww_cal_equal_symbol_count(int len_binary)
{
	int num_group = 0;
	int num_binary_miss = 0;
	int num_equal_symbol = 0;
	
	num_group = (int)ceil(len_binary*1.0/HWW_BASE64_PRIMARY_INTERVAL);
	HWW_INFO("num_group:%d\n", num_group);

	//3.1 缺0数量
	num_binary_miss = num_group*HWW_BASE64_PRIMARY_INTERVAL - len_binary;
	HWW_INFO("num_binary_miss:%d\n", num_binary_miss);

	//3.2 缺#数量
	num_equal_symbol = num_binary_miss/HWW_BASE64_SECOND_INTERVAL;
	HWW_INFO("num_equal_symbol:%d\n", num_equal_symbol);

	return num_equal_symbol;
}

static void hww_convert_binary_to_decimalism(char *str_input, int len_input, char *out_put, int *p_len_output)
{
	if (NULL == str_input || NULL == out_put || NULL == p_len_output) return;

	int i = 0, j = 0, k = 0;
	char str_decimalism[HWW_BASE64_DATE_LEN_MAX*2] = {0};
	char decimalism = 0;
	char str_binary[HWW_BASE64_SECOND_INTERVAL] = {0};
	int group = len_input/HWW_BASE64_SECOND_INTERVAL;	

	for (i = 0; i < group; i++){
		memset(str_binary, 0, sizeof(str_binary));
		memcpy(str_binary, &str_input[i*HWW_BASE64_SECOND_INTERVAL], HWW_BASE64_SECOND_INTERVAL);

		decimalism = 0;
		for (j = 0; j < HWW_BASE64_SECOND_INTERVAL; j++){	
			decimalism += str_binary[HWW_BASE64_SECOND_INTERVAL-j-1]*pow(2, j);
		}
		str_decimalism[i] = decimalism;
	}

	*p_len_output = MIN(group, *p_len_output);
	memcpy(out_put, str_decimalism, *p_len_output);
	
	return;
}


static void hww_search_from_base64_table(char *str_input, int len_input)
{
	int i = 0;

	for(i = 0; i < len_input; i++){
		str_input[i] = base64_table[str_input[i]];
	}
	
	return;
}

int hww_base64_encryption(char *str_input, int len_input, char *out_put, int len_output)
{
	if (NULL == str_input || 0 == len_input) return -1;
	if (NULL == out_put || 0 == len_output) return -1;

	int i = 0;
	char str_binary[HWW_BASE64_DATE_LEN_MAX*8] = {0};
	char str_decimalism[HWW_BASE64_DATE_LEN_MAX*2] = {0};
	int num_equal_symbol = 0;
	int len_binary = 0, len_decimalism = 0;

	if (len_input > HWW_BASE64_DATE_LEN_MAX){
		HWW_ERR("len(%d) of base64 date is out of range(%d)\n", len_input, HWW_BASE64_DATE_LEN_MAX);
		return -1;
	}

	hww_print_binary_data(str_input, len_input, __FUNCTION__, __LINE__);
	
	//1.转化成二进制
	for (i = 0; i < len_input; i++){
		hww_convert_ASCII_to_binary(str_input[i], &str_binary[i*HWW_BINARY_LEN]);
		len_binary += HWW_BINARY_LEN;
	}

	//2.按照6位二进制数据进行分组，向上取整， 不足补0
	hww_fill_0_by_bit(str_binary, &len_binary);

	//3.统计补=号个数
	num_equal_symbol = hww_cal_equal_symbol_count(len_binary);

	//4.按位转化为十进制
	len_decimalism = sizeof(str_decimalism);
	hww_convert_binary_to_decimalism(str_binary, len_binary, str_decimalism, &len_decimalism);

	//5.查表
	hww_search_from_base64_table(str_decimalism, len_decimalism);

	//5.1 补=
	//len_binary = strlen(str_decimalism);
	if (num_equal_symbol){
		for(i = 0; i < num_equal_symbol; i++){
			str_decimalism[len_decimalism+i] = HWW_BASE64_PAD_CHAR;
		}
	}
	
	len_decimalism += num_equal_symbol;

	HWW_INFO("str_decimalism:%s\n", str_decimalism);

	memcpy(out_put, str_decimalism, MIN(len_decimalism, len_output));

	return 0;
}

int hww_base64_decryption(char *str_input, int len_input, char *out_put, int len_output)
{
	if (NULL == str_input || 0 == len_input) return -1;
	if (NULL == out_put || 0 == len_output) return -1;

	
	if (len_input > HWW_BASE64_DATE_LEN_MAX){
		HWW_ERR("len(%d) of base64 date is out of range(%d)\n", len_input, HWW_BASE64_DATE_LEN_MAX);
		return -1;
	}

	
	return 0;
}

void hww_base64_init(HWW_DEBUG_LEVEL_E debug_level)
{
	if (debug_level < HWW_DEBUG_LEVEL_ERR || debug_level > HWW_DEBUG_LEVEL_HINT) return;

	g_debug_level = debug_level;

	return;
}
