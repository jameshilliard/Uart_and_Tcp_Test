#include "stdafx.h"
/*
	函数功能：将小写字母转化成大写字母
	参数：s ，未知字符串指针
	返回值：无
*/
void Upper(unsigned char *s)
{
	unsigned char *p = s;

	while(*p)
	{

		if( (*p >= 'a') && (*p <= 'f') )
			*p -= 32;

		p++;
	}
}

/*
	函数功能：将字符串转化成16进制数据
	参数：str,输入的字符串
	参数：hex输出的16进制数据
	参数：maxsize,hex的最大缓冲区
	返回值：转化成功后的16进制数据的长度
*/
int CharToHex(unsigned char *str,unsigned char *hex,int maxsize)
{
	unsigned char *p = str;
	unsigned char *dst = hex;

	int  i = 0;
	//转换成大写
	Upper(str);

	while( *p )
	{

		if( (*p >= 'A') && (*p <= 'F') )
		{
			*dst = (*p - 55) << 4;
		}
		else if( (*p >= '0') && (*p <= '9') )
		{
			*dst = (*p - 48) << 4;
		}
		else
		{
			//printf("Error,there has a %c,is not a hex!\n",*p);
			break;
		}

		p++;

		if( 0x0 == *p )
		{
			i++;
			break;
		}

		if( (*p >= 'A') && (*p <= 'F') )
		{
			*dst |= (*p - 55);
		}
		else if( (*p >= '0') && (*p <= '9') )
		{
			*dst |= (*p - 48);
		}
		else
		{
			//printf("Error,there has a %c,is not a hex!\n",*p);
			break;
		}

		dst++;

		if(++i >= maxsize)
			break;

		p++;
	}
	
	return i;
}