#ifndef __GIF_H__
#define __GIF_H__
//////////////////////////////////////////////////////////////////////////////////	 
 
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#define GIF_FORMAT_ERR		0x27	//�榡���~
#define GIF_SIZE_ERR		0x28	//�Ϥ��ؤo���~
#define GIF_WINDOW_ERR		0x29	//�����]�w���~
#define GIF_MEM_ERR			0x11	//�O������~

//////////////////////////////////////////�ϥΪ̰t�m��//////////////////////////////////
#define GIF_USE_MALLOC		1 	//�w�q�O�_�ϥ�malloc,�o�̧ڭ̿�ܨϥ�malloc	     
//////////////////////////////////////////////END/////////////////////////////////////


#define LCD_MAX_LOG_COLORS  256 
#define MAX_NUM_LWZ_BITS 	12


#define GIF_INTRO_TERMINATOR ';'	//0X3B   GIF��󵲧���
#define GIF_INTRO_EXTENSION  '!'    //0X21
#define GIF_INTRO_IMAGE      ','	//0X2C

#define GIF_COMMENT     	0xFE
#define GIF_APPLICATION 	0xFF
#define GIF_PLAINTEXT   	0x01
#define GIF_GRAPHICCTL  	0xF9

typedef struct
{
	uint8_t    aBuffer[258];                     // Input buffer for data block 
	uint16_t 	aCode  [(1 << MAX_NUM_LWZ_BITS)]; // This array stores the LZW codes for the compressed strings 
	uint8_t    aTail_index[(1 << MAX_NUM_LWZ_BITS)]; // tail Index of the LZW code.
	uint8_t    aCodeDumpBuffer[3000];            // buffer for the index stream of Code. The higher the compression, the more bytes are needed in the buffer.
	uint8_t   	GetDone;
	uint32_t   CurBit;
	uint32_t   LastByte;
	uint8_t   	CodeSizeBit;					// initial value: (MinCodeSize) + 1 in bit
	uint8_t   	MinCodeSize;					// minimum code size (2 ~ 12)
	uint16_t   ClearCodeNr;					// (1 << MinCodeSize): clear code (a no.) in the image data
	uint16_t   EndCodeNr;						// (ClearCode + 1): block Terminator (a no.) of Image Data
	uint16_t   MaxCodeNr;			// 1+(the last code no. in the code table)
	uint16_t   MaxCodeSize;					// number of entries in the code table
	uint16_t   OldCodeNr;
}LZW_INFO;

//�޿�ù��y�z��
//__packed typedef struct
typedef struct
{
	uint16_t width;		//GIF width
	uint16_t height;		//GIF heigth
	uint8_t flag;		// [M:Cr:s:Pixel] <=>  1:3:1:3
							// Pixel (3 bits): 2^(Pixel+1) = Number of colors in Global Color Table (256 when Pixel = 7)
							// s : Sort flag
							// Cr (3 bits): color depth = (Cr+1) bits for each of RGB, i.e., 3 x (Cr+1)/8 bytes for each color 
							// M (1 bit): 0=withoout global color table; 1= with global color  table
	uint8_t bkcindex;	//�I����b�����C���������(�ȷ�s�b�����C���ɦ���)
	uint8_t pixratio;	//�Ϥ��e���� (not used if 0)
}LogicalScreenDescriptor;


//�Ϲ��y�z��
typedef struct
{
	uint16_t xoff;		//x offset: image left
	uint16_t yoff;		//y offset: image top
	uint16_t width;		//image width
	uint16_t height;		//image height
	uint8_t flag;		//�ѧO�r  1:1:1:2:3=�����C���лx(1):��´�лx(1):�O�d(2):�����C���j�p(3)
}ImageDescriptor;

//�Ϲ��y�z
typedef struct
{
	LogicalScreenDescriptor gifLSD;	//�޿�ù��y�z��
	ImageDescriptor gifIMD;	//�Ϲ��y�z��
	uint16_t colortbl[256];				//��e�ϥ��C���
	uint16_t bkpcolortbl[256];			//�ƥ��C���.��s�b�����C���ɨϥ�
	uint16_t numcolors;					//�C���j�p
	uint16_t delay;					    //����ɶ�
	LZW_INFO *lzw;					//LZW�H��
	//==>>> additional 
	void* device;			/* Pointer to I/O device identifiler for the session */	
	uint16_t*	pCurColorTbl;	/* Pointer to the current Color table, either colortble[] or bkpcolortble[] */
}gif89a;

#ifndef delay_ms
void delay_ms(uint16_t Num);
#endif

uint32_t gif_in_func(gif89a* s_gif, uint8_t* buf, uint32_t num);
void gif_draw_HLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t color);
void gif_fill_Rect(uint16_t Xpos, uint16_t Ypos, uint16_t X1, uint16_t Y1, uint16_t color);
uint8_t gif89a_prepare(gif89a* s_gif);
uint8_t gif_decomp(gif89a* s_gif, uint16_t X0, uint16_t Y0);

void gif_initlzw(gif89a* s_gif, uint8_t min_codesize);
int gif_nextColorIndex(gif89a* s_gif, uint8_t *pTmp);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
