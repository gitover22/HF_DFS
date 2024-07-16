/*********************************************************
 *  des.h
 *  用户使用des算法头文件
 *	
 *********************************************************/
#ifndef _OPENDESS_H_
#define _OPENDESS_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 使用DES算法对输入数据进行加密。
 * @param pInData [in] 输入待加密的数据指针。
 * @param nInDataLen [in] 输入数据的长度。
 * @param pOutData [out] 输出加密后的数据指针。
 * @param pOutDataLen [out] 输出数据的长度指针。
 * @return 返回值为0表示加密成功，非0表示加密失败。
 */
int DesEnc(
		unsigned char *pInData,
		int            nInDataLen,
		unsigned char *pOutData,
		int           *pOutDataLen);

//加密等于4k的数据
int DesEnc_raw(
	unsigned char *pInData,
	int            nInDataLen,
	unsigned char *pOutData,
	int           *pOutDataLen);

/**
 * @brief 使用DES算法解密数据。
 * @param pInData [in] 指向待解密数据的指针。
 * @param nInDataLen [in] 待解密数据的长度。
 * @param pOutData [out] 指向解密后数据将被写入的指针。
 * @param pOutDataLen [out] 指向解密后数据长度的指针。
 * @return 返回0表示成功，其他值表示错误代码。
 */
int DesDec(
	   unsigned char *pInData,
	   int            nInDataLen,
	   unsigned char *pOutData,
	   int           *pOutDataLen);

//解密等于4k的数据
int DesDec_raw(
	unsigned char *pInData,
	int            nInDataLen,
	unsigned char *pOutData,
	int           *pOutDataLen);



#ifdef __cplusplus
}
#endif

#endif

