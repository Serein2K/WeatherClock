#ifndef __GBK_UTF8_UNICODE_H
#define __GBK_UTF8_UNICODE_H


// 将UTF8编码转换成Unicode（UCS-2LE）编码
int Utf8ToUnicode(char* pInput, char* pOutput);

// 将Unicode（UCS-2LE）编码转换成UTF8编码
int UnicodeToUtf8(char* pInput, char *pOutput);

// 将Utf8转成字符串形式
int Utf8ToStr(char* pInput, char* pOutput);

//  utf8转为Unicode，让pad可显示中文
int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

// hex型字符转hex
char *str2hex(const char *buf, char *dst);

// 简单演示
void EnDeCodeDemo(void);

#endif

