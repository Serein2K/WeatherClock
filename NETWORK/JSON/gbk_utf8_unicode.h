#ifndef __GBK_UTF8_UNICODE_H
#define __GBK_UTF8_UNICODE_H


// ��UTF8����ת����Unicode��UCS-2LE������
int Utf8ToUnicode(char* pInput, char* pOutput);

// ��Unicode��UCS-2LE������ת����UTF8����
int UnicodeToUtf8(char* pInput, char *pOutput);

// ��Utf8ת���ַ�����ʽ
int Utf8ToStr(char* pInput, char* pOutput);

//  utf8תΪUnicode����pad����ʾ����
int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

// hex���ַ�תhex
char *str2hex(const char *buf, char *dst);

// ����ʾ
void EnDeCodeDemo(void);

#endif

