#ifndef BASE64_H
#define BASE64_H

/**
 * @brief 编码器
 * @param bindata [in] 源字符串
 * @param binlength [in] 源字符串长度
 * @param base64 [out] base64字符串
 * @return base64字符串
 */
char * base64_encode( const unsigned char * bindata, int binlength, char * base64 );

/**
 * @brief 解码器
 * @param base64 [in] 源字符串
 * @param bindata [out] 目的字符串
 * @return 目的字符串长度
 */
int base64_decode( const char * base64, unsigned char * bindata );

#endif // BASE64_H

