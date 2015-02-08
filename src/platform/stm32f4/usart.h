#ifndef USART_H
#define USART_H 1
// 
// STM32F4xx USART�o�b�t�@����v���O����
// �Q�l�����F
//	RM0090 Reference manual - STM32F4xx advanced ARM-based 32-bit MCUs
//	PM0056 Programming manual - STM32Fxxx Cortex-M3 programming manual
//	UM1472 Users manual - STM32F4DISCOVERY discovery board
// 
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "sysclk_config.h"
#include <stdio.h>

struct USART_BUF_CTRL_t {
	uint8_t err_flag;
	uint8_t not_done;
	char tx_data;
	char rx_data;
	uint32_t rb_r, rb_w;
	uint32_t tb_r, tb_w;
	uint32_t rb_siz;
	uint32_t tb_siz;
	char rb[BUFFER_SIZE];
	char tb[BUFFER_SIZE];
} sci;

#define	UEI	NVIC_EnableIRQ(INT_pos)
#define	UDI	NVIC_DisableIRQ(INT_pos)

#ifndef	ENTER_hndl
#define	ENTER_hndl
#endif
#ifndef	EXIT_hndl
#define	EXIT_hndl
#endif

#define	clear_sr    	{SCI->SR= (USART_SR_TC | USART_SR_TXE);}

#define	cr_set_UE   	{SCI->CR1|= (uint16_t)USART_CR1_UE;} 	// USART��������
#define	cr_clear_RE 	{SCI->CR1&= (uint16_t)~USART_CR1_RE;}	// ��M���֎~����
#define	cr_set_RE   	{SCI->CR1|= (uint16_t)USART_CR1_RE;} 	// ��M��������
#define	cr_clear_TE 	{SCI->CR1&= (uint16_t)~USART_CR1_TE;}	// ���M���֎~����
#define	cr_set_TE   	{SCI->CR1|= (uint16_t)USART_CR1_TE;} 	// ���M��������

#define	cr_clear_RIE	{SCI->CR1&= (uint16_t)~USART_CR1_RXNEIE;}	// RDE�����݂��֎~����
#define	cr_set_RIE  	{SCI->CR1|= (uint16_t)USART_CR1_RXNEIE;}	// RDE�����݂�������
#define	cr_clear_ERI	{SCI->CR1&= (uint16_t)~USART_CR1_PEIE;SCI->CR3&= (uint16_t)~USART_CR3_EIE;}	// ERI�����݂��֎~����
#define	cr_set_ERI  	{SCI->CR1|= (uint16_t)USART_CR1_PEIE; SCI->CR3|= (uint16_t)USART_CR3_EIE;}	// ERI�����݂�������
#define	cr_clear_TIE	{SCI->CR1&= (uint16_t)~USART_CR1_TXEIE;}	// TDRE�����݂��֎~����
#define	cr_set_TIE  	{SCI->CR1|= (uint16_t)USART_CR1_TXEIE;}	// TDRE�����݂�������
#define	cr_clear_TEIE	{SCI->CR1&= (uint16_t)~USART_CR1_TCIE;}	// TEND�����݂��֎~����
#define	cr_set_TEIE 	{SCI->CR1|= (uint16_t)USART_CR1_TCIE;}	// TEND�����݂�������
#define	is_sr_ERRF  	(SCI->SR & (USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE))
#define	is_sr_RDRF  	(SCI->SR & USART_SR_RXNE)
#define	is_sr_TDRE  	(SCI->SR & USART_SR_TXE)
#define	is_sr_TEND  	(SCI->SR & USART_SR_TC)
#define	BUFFER_FULL 	(0xffffffff)
#define	RX_FLAG_OVFL	(0x80)

static void sci_RDR2RxBuf(void);
static void sci_TxBuf2TDR(void);
static int16_t sci_getRxBuf(void);
static int16_t sci_putTxBuf(char c);

static void sci_init(void);
static void sci_init_RxBuf(void);
static void sci_init_TxBuf(void);

// ****************************************************************************
// �����x���C���^�[�t�F�C�X�������[�`���Q
// ****************************************************************************
// SCI�̓]�����[�g�ݒ肨��я��������s���C����M���J�n����B
void SCI_init(int brr){
	SCI->GTPR= 0;
	SCI->BRR= (APB1Clock)/brr;
	sci_init();	// buffer clean and set up
	cr_set_UE;
	UEI;
}

//
// SCI�ɑ��M������1�������ڑ���(0�ő��o�����C-1���Ԃ�Ƒ��o���W�X�^�ɋ󂫂��Ȃ����s)
// return 0:seccess set TDR -1:TDR is busy
//
int16_t SCI_putc_direct(char c){// SCI�ɕ����𑗐M(�o�b�t�@�����O�𖳎����Ċ����ފO������)
	if(0==(is_sr_TDRE))return(-1);
	SCI->DR= c;
	return(0);
}

//
// SCI�ɑ��M������𒼐ڑ���(�S�đ��荞�݂���������܂ŏ�����Ԃ��Ȃ�)
//
void SCI_puts_direct(char *s){
	for(;*s;){
		while(SCI_putc_direct(*s));	// ���M�ł���܂ōĎ��s����
		s++;
	};
}

//
// SCI�̎�M�o�b�t�@�̑ؗ��o�C�g����Ԃ��B
//
uint32_t SCI_nbuf(void){
	if(sci.rb_w == sci.rb_r) return(0);
	if(BUFFER_FULL == sci.rb_w) return(sci.rb_siz);
	if(sci.rb_w <  sci.rb_r){
		return(sci.rb_siz - sci.rb_r + sci.rb_w);
	}else{
		return(sci.rb_w - sci.rb_r);
	};
}

//
// SCI�̎�M������1��������
// �擾�����f�[�^�̒l��Ԃ��B�������󂯎��܂ŏ�����Ԃ��Ȃ��B
//
int16_t SCI_getc(void){
int16_t c;
	do c=sci_getRxBuf(); while(c < 0);
	return(c);
}

//
// SCI�ɑ��M������1��������
// �������ݐ����܂ōĎ��s����B
//
void SCI_putc(char c){
	while(sci_putTxBuf(c)); // 0�ȊO���߂�ƃG���[�Ȃ̂ōēx��������
}

//
// SCI�ɑ��M������𑗂�
// �������ݕ��������Ԃ�B���M�o�b�t�@�֑S�đ��荞�݂���������܂ŏ�����Ԃ��Ȃ��B
//
uint32_t SCI_puts(char *s){
uint32_t r;
	for(r=0; *s; s++,r++){
		SCI_putc(*s);
	};
	return(r);	// �������񂾕����������^�[������B
}

//
// SCI�ɑ��M������1��������
// �������ݕ��������Ԃ�̂�0���Ԃ�ꍇ�͑���o���Ɏ��s���Ă���
//
uint32_t SCI_write(char c){
	return(0 == sci_putTxBuf(c));
	// 0�ȊO���߂�ƃG���[�Ȃ̂ŏ������ݎ��s��0�����^�[��
	// ����ɏ������߂���0���߂�̂ŏ������ݕ�������1�����^�[��
}

//
// SCI�ɑ��M������𑗂�
// �������ݕ��������Ԃ�̂ŕ��������`�F�b�N���ĕ�����̒����ɑ���Ȃ��ꍇ�͑���o���Ɏ��s���Ă���
//
uint32_t SCI_writes(char *s){
uint32_t r;
	for(r=0; *s && SCI_write(*s); s++,r++);
	return(r);	// �������񂾕����������^�[������B
}


// ****************************************************************************
// ����M�o�b�t�@�����ݒ�p�������[�`��
// ****************************************************************************
static void sci_init(void){
	cr_clear_RE;
	cr_clear_RIE;
	cr_clear_ERI;
	cr_clear_TE;
	cr_clear_TIE;
	cr_clear_TEIE;
	clear_sr;
	cr_set_UE;
	sci_init_RxBuf();
	clear_sr;
	cr_set_ERI;
	cr_set_RE;
	sci_init_TxBuf();
	clear_sr;
	cr_set_TE;
}
//
// SCI��RX��M�o�b�t�@������������B
//
static void sci_init_RxBuf(void){
	sci.rb_siz= sizeof(sci.rb);
	sci.rb_r= sci.rb_w= 0;
	sci.err_flag= 0;
	sci.rx_data= 0;

	if(sci.rb_siz){ /* is buffering control? */
		cr_set_RIE;
	};
}

//
// SCI��TX���M�o�b�t�@������������B
//
static void sci_init_TxBuf(void){
	sci.tb_siz= sizeof(sci.tb);
	sci.tb_r= sci.tb_w= 0;
	sci.not_done= 0;
	sci.tx_data= 0;
}


// ****************************************************************************
// ��O�������[�`���Q
// ****************************************************************************
volatile uint16_t dummy_buffer;
// RXI (��M������RDR�ɂ���̂Ŏ�M�o�b�t�@�Ɉڂ�)
static void hndl_RXI(void){
	sci_RDR2RxBuf();
}
// ERI (��M�G���[����M�G���[�t���O�ɃZ�b�g����)
static void hndl_ERI(void){
	sci.err_flag |= is_sr_ERRF;
	dummy_buffer= SCI->DR;
	dummy_buffer= SCI->SR;	// Error flags clear sequence
}
// TDRE ��O (TDR���󂢂��̂ő��M�o�b�t�@�Ƀf�[�^������΂���𑗂�)
static void hndl_TXI(void){
	sci_TxBuf2TDR();
}
// TEND ��O (���M�������t���O���N���A����)
static void hndl_TEI(void){
	sci.not_done = 0;
	cr_clear_TEIE;
}

void hndl_USART2(void){
	ENTER_hndl;
	if(is_sr_ERRF)hndl_ERI();
	if(is_sr_RDRF)hndl_RXI(); // not bo read occuerd error sometime.
	if(is_sr_TDRE)hndl_TXI();
	if(is_sr_TEND)hndl_TEI();
	EXIT_hndl;
}


// ****************************************************************************
// ��M�o�b�t�@���쏈�����[�`��(�����ďo��p)
// ****************************************************************************
//
// SCI��RDR����RX��M�o�b�t�@�֕����𑗂� (���M�o�b�t�@�����O���Ɍ���Ă΂��)
// ���荞�ݏ�����������Ă΂�鏈���ł��邱�Ƃɗ��ӂ��邱��
//
static void sci_RDR2RxBuf(void){
	sci.rx_data= SCI->DR;
	if(BUFFER_FULL == sci.rb_w){  // ���^���������ꍇ�̃I�[�o�t���[����
		sci.err_flag |= RX_FLAG_OVFL;
		return;
	};
	sci.rb[sci.rb_w]= sci.rx_data;
	sci.rb_w++;
	if(sci.rb_w >= sci.rb_siz)sci.rb_w=0;
	if(sci.rb_w == sci.rb_r)sci.rb_w=BUFFER_FULL;
}
//
// SCI��RX��M�o�b�t�@���當�������o��
// RX��M�o�b�t�@���琳��Ɏ��o�����Ȃ�΁C���̕�����Ԃ�
// RX��M�o�b�t�@����̏ꍇ�́C-1��Ԃ�
//
static int16_t sci_getRxBuf(void){
    printf("                   \n");
    int16_t r;
	if(0 == sci.rb_siz){  // ��o�b�t�@�����O�̏ꍇ
		if(is_sr_RDRF){ // RDR�Ƀf�[�^�������Ă���΂��̒l�����^�[������
			r= SCI->DR; return(r);
		};
		return(-1);
	};

	if(sci.rb_r == sci.rb_w) return(-1); // ��̏ꍇ

	UDI; // enter the critical section
		r= (int16_t)(sci.rb[sci.rb_r]);
		if(BUFFER_FULL == sci.rb_w) sci.rb_w= sci.rb_r; // ���^���������ꍇ
		sci.rb_r++;
		if( sci.rb_r >= sci.rb_siz )sci.rb_r= 0;
	UEI; // exit the critical section
//	if(SCI_nbuf()<(sci.rb_siz/2))SCI_set_RTS; // �o�b�t�@�������ȏ�󂢂���RTS��on
    printf("                 \n");
	return(r);
}

// ****************************************************************************
// ���M�o�b�t�@���쏈�����[�`��(�����ďo��p)
// ****************************************************************************
//
// SCI��TX���M�o�b�t�@����TDR�֕����𑗂� (���M�o�b�t�@�����O���Ɍ���Ă΂��)
// ���荞�ݏ�������������Ă΂�鏈���ł��邱�Ƃɗ��ӂ��邱��
//
static void sci_TxBuf2TDR(void){
	if(sci.tb_r == sci.tb_w){ // ���M�o�b�t�@����̏ꍇ
		cr_clear_TIE;	// TDRE�����݂��֎~���ă��^�[��
		return;
	};

	if(is_sr_TDRE){ // TDR���󂢂Ă���ꍇ�Ƀo�b�t�@����1��������
		sci.tx_data =sci.tb[sci.tb_r];
		dummy_buffer= SCI->SR;	// TDRE/TEND flags clear sequence1
		SCI->DR= sci.tx_data;	// TDRE/TEND flags clear sequence2

		sci.not_done= 1;	// ���M��
		cr_set_TEIE;	// ���M���������݂�����
		cr_set_TIE; 	// TDRE�����݂�����

		if(BUFFER_FULL == sci.tb_w) sci.tb_w= sci.tb_r; // ���^���������ꍇ�̌�n��
		sci.tb_r++;
		if( sci.tb_r >= sci.tb_siz) sci.tb_r= 0;
	};
	return;
}

//
// SCI��TX���M�o�b�t�@�֕����𑗂荞��
// TX���M�o�b�t�@�֐���ɑ��荞�߂��Ȃ��0��Ԃ�
// TX���M�o�b�t�@�I�[�o�t���[�̏ꍇ�� -1��Ԃ�
//
static int16_t sci_putTxBuf(char c){
	if(0 == sci.tb_siz){  // ��o�b�t�@�����O�̏ꍇ
		cr_clear_TIE;	// TDRE�����݂��֎~���Ă���
		if(is_sr_TDRE){ // TDR�ɋ󂫂�����΂��̂܂܃Z�b�g���Đ���I������
			SCI->DR= sci.tx_data= c;	// set SR_TXE automaticaly
			sci.not_done= 1;	// ���M��
			cr_set_TEIE;	// ���M���������݂�����
			return(0);
		};
		return(-1); // TDR�ɋ󂫂��Ȃ��ƃo�b�t�@�t���ŃG���[���^�[������
	};

	if(BUFFER_FULL == sci.tb_w) return(-1); // ���^���̏ꍇ

	UDI; // enter the critical section
		sci.tb[sci.tb_w]= c;
		sci.tb_w++;
		if(sci.tb_w >= sci.tb_siz)sci.tb_w= 0;
		if(sci.tb_r == sci.tb_w){ // ���^���ɂȂ����ꍇ
			sci.tb_w = BUFFER_FULL;
		};
		sci_TxBuf2TDR();
	UEI; // exit the critical section
	return(0);
}

//
// �����O�o�b�t�@�̃C���v�������g���@�ɂ��Ẵ���
//
#if 0

r��0�`sz-1�܂ł̒l���Ƃ�o�b�t�@�̓ǂݏo���|�C���^
w��0�`sz-1�܂ł̒l���Ƃ�o�b�t�@�ւ̏������݃|�C���^
�������A�o�b�t�@�ւ̏������݂��I�������ʃo�b�t�@�����傤�ǈ�t�ɂȂ����ꍇ�ɂ�
w��BUFFER_SIZ(sz�̎���ő�l=0xffffffff)�ƂȂ�

�o�b�t�@�ɓ����ꍇ�ɂ�
	w��MAX_BUF_SIZ�Ȃ牽������OVF�Z�b�g�݂̂Ń��^�[��
	w�̈ʒu�ɕ�����u����, w��1��������
	w==r�Ȃ��w=MAX_BUF_SIZ�Ƃ���

�o�b�t�@������o���ꍇ�ɂ�
	r==w�Ȃ牽������ �G���[�l�����^�[��
	w==MAX_BUF_SIZ�Ȃ�΁Cw=r�Ƃ��Ă���
	r�̈ʒu�̕������擾����, r��1��������
	�擾�������������^�[��

nbuf�T�j��
	r==w �Ȃ�0�����^�[��
	w==-1�Ȃ�sz�����^�[��
	r<w  �Ȃ�w-r�����^�[��
	r>w  �Ȃ�sz-r+w�����^�[��

sz��MAXINT-1�܂ł��L�� 16bit�Ȃ��65535�܂ŁB0�̓o�b�t�@�s�g�p��\���B

#endif
#endif /* USART_H */
