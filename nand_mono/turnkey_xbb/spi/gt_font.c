/**
 * Driver for GT20L16P1Y font ic.
 * 
 * I don't want to repeat this, so I just use the existing source. If I have time
 * in future, I will re-write this. Maybe not.
 */
#include "SPDA2K.H"
#include "gt_font_spi.h"

typedef enum
{
    CODE_TYPE_GB,
    CODE_TYPE_UNICODE,
} code_type_t;

//其中函数unsigned char find_zf_tab(unsigned short unicode) 参考下列代码
unsigned short unicode_zf[31] = 
{0x3000,0x3001,0x3002,0x00b7,0x02c9,0x02c7,0x00a8,0x3003,0x3005,0x2014,0xff5e,0x2016,0x2026,0x2018,0x2019,
0x201c,0x201d,0x3014,0x3015,0x3008,0x3009,0x300a,0x300b,0x300c,0x300d,0x300e,0x300f,0x3016,0x3017,0x3010,0x3011
};
unsigned char find_zf_tab(unsigned short unicode)
{
	unsigned char i;
	for( i=0; i < 31; i++ )
	{
		if( unicode == unicode_zf[i] )
			break;
	}
	if(i==31)
		return(0);
	return(i);		
}

//---------------------------------------------------------------------------------
// 函数： unsigned long unicode_muti_addr( unsigned short unicode)
// 说明：
// 包括 拉丁文、希腊文、基里尔文、希伯来文、泰文
// BaseAdd：说明本套字库在字库芯片中的起始字节地址。
// unicode：表示unicode内码（16bits）
// Address：表示汉字点阵在芯片中的字节地址。
//计算方法：
unsigned long unicode_muti_addr( unsigned short unicode)
{
	unsigned long  BaseAdd = 0;
    unsigned long Address;

	if( unicode >= 0x20 && unicode <= 0x7f )//latin 1
		Address = 16*(unicode-0x20)+ BaseAdd;
	else if( unicode >= 0xa0 && unicode <= 0xff )//latin 2
		Address = 16*(96+unicode-0xa0)+ BaseAdd;
	else if( unicode >= 0x100 && unicode <= 0x17f )//latin 3
		Address = 16*(96+96+unicode-0x100)+ BaseAdd;
	else if( unicode >= 0x1a0 && unicode <= 0x1cf )//latin 4
		Address = 16*(128+96+96+unicode-0x1a0)+ BaseAdd;
	else if( unicode >= 0x1f0 && unicode <= 0x1ff )//latin 5
		Address = 16*(48+128+96+96+unicode-0x1f0)+ BaseAdd;
	else if( unicode >= 0x210 && unicode <= 0x21f )//latin 6
		Address = 16*(16+48+128+96+96+unicode-0x210)+ BaseAdd;
	else if( unicode >= 0x1ea0 && unicode <= 0x1eff )//latin 7
		Address = 16*(16+16+48+128+96+96+unicode-0x1ea0)+ BaseAdd;
	else if( unicode >= 0x370 && unicode <= 0x3cf )//greek
		Address = 16*(96+16+16+48+128+96+96+unicode-0x370)+ BaseAdd;
	else if( unicode >= 0x400 && unicode <= 0x45f )//cyrillic 1
		Address = 16*(96+96+16+16+48+128+96+96+unicode-0x400)+ BaseAdd;
	else if( unicode >= 0x490 && unicode <= 0x4ff )//cyrillic 2
		Address = 16*(96+96+96+16+16+48+128+96+96+unicode-0x490)+ BaseAdd;
	else if( unicode >= 0x590 && unicode <= 0x5ff )//hebrew
		Address = 16*(112+96+96+96+16+16+48+128+96+96+unicode-0x590)+ BaseAdd;
	else if( unicode >= 0xe00 && unicode <= 0xe7f )//thai
		Address = 16*(112+112+96+96+96+16+16+48+128+96+96+unicode-0xe00)+ BaseAdd;
	else
		Address = BaseAdd;
	return Address;
}
//---------------------------------------------------------------------------------
//函数：unsigned long unicode_alb_addr( unsigned short unicode_alb)
//说明：
//16点阵不等宽阿拉伯文系字符
//BaseAdd：说明本套字库在字库芯片中的起始字节地址。
//unicode_alb：表示unicode内码（16bits）
//Address：表示汉字点阵在芯片中的字节地址。
//计算方法：
unsigned long unicode_alb_addr( unsigned short unicode_alb)
{
	unsigned long BaseAdd= 16640;
    unsigned long Address;

	if( unicode_alb >= 0x0600 && unicode_alb <= 0x06FF )//alb 1
		Address = 34*(unicode_alb-0x0600)+ BaseAdd;
	else if( unicode_alb >= 0xfb50 && unicode_alb <= 0xfbff )//alb 2
		Address = 34*(16*16+unicode_alb-0xfb50)+ BaseAdd;
	else if( unicode_alb >= 0xfe70 && unicode_alb <= 0xfeff )//alb 3
		Address = 34*(16*11+16*16+unicode_alb-0xfe70)+ BaseAdd;
	return Address;

}
//---------------------------------------------------------------------------------
//函数：unsigned long GB2312_addr( unsigned char MSB,unsigned char LSB )
//说明：
//15X16 GB2312编码汉字字符
//ZFAdd、HZAdd：表示字符和汉字的起始地址。
//MSB、LSB：表示GB2312编码的高字节和低字节。
//GB2312tab：表示GB2312转码表起始地址。
//Address：表示汉字点阵在芯片中的字节地址。
unsigned long GB2312_addr( unsigned char MSB,unsigned char LSB )
{
	unsigned long ZFAdd = 36224;
	unsigned long HZAdd = 93452;
	unsigned long GB2312tab = 87436;
	unsigned char buffer[2];
    unsigned long Address;

	if( MSB >= 0xA1 && MSB <= 0xA5  )//字符区
	{
		if(MSB == 0xA1 && LSB >= 0xA1 && LSB <= 0xBF )
			Address = 32*(LSB-0xA1)+ ZFAdd;
		else if(MSB == 0xA3 && LSB >= 0xA1 && LSB <= 0xFE )
			Address = 32*(31+LSB-0xA1)+ ZFAdd;
		else if(MSB == 0xA4 && LSB >= 0xA1 && LSB <= 0xF3)
			Address = 32*(31+94+LSB-0xA1)+ ZFAdd;
		else if(MSB == 0xA5 && LSB >= 0xA1 && LSB <= 0xF6)
			Address = 32*(31+94+83+LSB-0xA1)+ ZFAdd;
		else
			Address = ZFAdd;
	}
	else if( (MSB >= 0xB0 && MSB <= 0xD7)&&(LSB>=0xA1 && LSB <= 0xFE) )//汉字5270
		{Address = 32*( (MSB - 0xB0) * 94 + (LSB - 0xA1)+1)+ HZAdd;dbprintf ("addr is %lx\n", Address);}
	else if( (MSB >= 0xD8 && MSB <= 0xF7)&&(LSB>=0xA1 && LSB <= 0xFE))//汉字5270~6763
	{
		Address = ((MSB - 0xD8) * 94 + (LSB - 0xA1) ) *2 + GB2312tab;
		gt_font_spi_read (buffer, Address,2 ); //从字库中读取二个字节
		Address = 32* (256*buffer[0]+buffer[1]) + HZAdd;
	}
	return Address;

}
//---------------------------------------------------------------------------------
//函数：unsigned long unicode_addr( unsigned short Unicode )
//说明：
//15X16 Unicode编码汉字字符
//ZFAdd、HZAdd：表示字符和汉字的起始地址。
//Unicode：表示汉字的unicode编码。
//Unicodetab：表示unicode转码表起始地址。
//Address：表示汉字点阵在芯片中的字节地址。
unsigned long unicode_addr( unsigned short Unicode )
{
	unsigned long ZFAdd = 36224;
	unsigned long HZAdd = 93452;
	unsigned long Unicodetab = 45632;
	unsigned char buffer[2];
    unsigned long Address;

	if(Unicode >= 0x4e00 && Unicode < 0x9fa6 )     // Unicode汉字区
	{
		Address = 2*(Unicode-0x4e00);
		Address = Address + Unicodetab;
		gt_font_spi_read (buffer, Address,2); //从字库中读取二个字节
		Address = 256*buffer[0]+buffer[1];
		Address = 32* Address + HZAdd;
	}
	else if(Unicode >= 0xff01 && Unicode <= 0xff5d )   //Unicode 字符区1
		Address = 32*(31+Unicode-0xff01)+ ZFAdd;
	else if(Unicode == 0xffe3)                       //Unicode 字符区2
		Address = 32*(31+93)+ ZFAdd;
	else if( Unicode >= 0x3041 && Unicode <= 0x3096 ) //Unicode 字符区3
		Address = 32*(31+93+1+Unicode-0x3041)+ ZFAdd;
	else if( Unicode >= 0x30a1 && Unicode <= 0x30f6 ) //Unicode 字符区4
		Address = 32*(31+93+1+83+Unicode-0x30a1)+ ZFAdd;
	else                                           //Unicode 字符区5
		Address = 32* find_zf_tab(Unicode)+ ZFAdd;
	return Address;
}





#if 0
//-------------------------------------------
//---GB2312测试方法--------------------------
//全角字符：A （0XA3，0XC1）
//汉字字符：啊（0XB0，0XA1）；齄（0XF7，0XFE）
//--------------------------------------------
#define GB2312_LEN 6
uint8 GB2312_CODE[6] = { 0xA3,0xC1,0xB0,0xA1,0xF7,0xFE };
uint8 GB2312_DOT[128*3];
//-----------------------------------------------------
void gt_font_test_gb( void )
{
	unsigned long temp_addr;
	unsigned char i;

	for( i = 0; i < GB2312_LEN; i+=2 )
	{
		temp_addr = GB2312_addr(GB2312_CODE[i],GB2312_CODE[i+1]);
        gt_font_spi_read (GB2312_DOT+i*16,temp_addr,32); //获得点阵数据
	}
}

//-----------------------------------------------------------------------------
//UNICODE3.0测试方法
//汉字部分：一（0X4E00）；龟（0X9F9F）；
//字符部分：A（0XFF21）
//-----------------------------------------------------------------------------
#define UNICODE3_LEN 6      //test vectors
unsigned char UNICODE3_CODE[6] = { 0xFF,0x21,0x4E,0x00,0x9F,0x9F };
unsigned char UNICODE3_DOT[32*3];
//由上面的测试向量产生测试点阵数据
void gt_font_test_unicode( void )
{
	unsigned long temp_addr;
  	unsigned char i;

	for( i = 0; i < UNICODE3_LEN; i+=2 )
	{
		temp_addr = unicode_addr(256*UNICODE3_CODE[i]+UNICODE3_CODE[i+1]);//16dot
	    gt_font_spi_read (UNICODE3_DOT+i*16,temp_addr,32); 		//获得点阵数据
	}
	while(1);
}
#endif



extern void dot_matrix_vtoh (uint8 *in, uint8 *out, uint8 height, uint8 width);
///-----------------------------------------------------------------------------
/// Get the dot matrix data with the GB2312 or Unicode
///
/// There is a transformation from Y to W.
///
/// @date 2012/12/27
///-----------------------------------------------------------------------------
void gt_font_get_dot (uint8 *dot, uint16 code_num, uint8 code_type)
{
    uint32 addr;
    uint8 dot_tmp[32];
 
    // Get the starting address of this character.   
    if (code_type == CODE_TYPE_GB)
    {
        addr = GB2312_addr (code_num>>8, code_num&0xFF);
        dbprintf ("high code is %bx, low code is %bx\n", code_num>>8, code_num&0xFF);
        dbprintf ("addr is %lx\n", addr);
    }
    else if (code_type == CODE_TYPE_UNICODE)
    {
        addr = unicode_addr (code_num);
    }

    // Read the dot matrix data from ic.
    gt_font_spi_read (dot_tmp, addr, 32);

    // Transform the data from Y to W, i.e., vertical negative to horizontal posi.
    dot_matrix_vtoh (dot_tmp, dot, 16, 16);    
}




#if (GT_FONT_TEST == FEATURE_ON)
void gt_font_test (void)
{
    uint8 i;
    uint32 addr;
    uint8 dot[32];

    #if 1 // test the spi bus hw timing.
    addr = GB2312_addr (0xb0, 0xa1);
    dbprintf ("high addr is %x, low addr is %x\n", addr>>16, addr&0xFFFF);
    dbprintf ("addr is %lx\n", addr);
    while (1)
    {
        gt_font_spi_read (dot, addr, 32);
    }
    #endif

}
#endif
