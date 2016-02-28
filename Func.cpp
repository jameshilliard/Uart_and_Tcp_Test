#include "stdafx.h"
/*
	�������ܣ���Сд��ĸת���ɴ�д��ĸ
	������s ��δ֪�ַ���ָ��
	����ֵ����
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
	�������ܣ����ַ���ת����16��������
	������str,������ַ���
	������hex�����16��������
	������maxsize,hex����󻺳���
	����ֵ��ת���ɹ����16�������ݵĳ���
*/
int CharToHex(unsigned char *str,unsigned char *hex,int maxsize)
{
	unsigned char *p = str;
	unsigned char *dst = hex;

	int  i = 0;
	//ת���ɴ�д
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