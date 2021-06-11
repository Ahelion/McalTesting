#include <stm32f4xx.h>


/**************************************************************************************************
 * Defines
 **************************************************************************************************/

/* Chars for hexadecimal output */
#ifndef HEX_CHARS
#define HEX_CHARS                  "0123456789ABCDEF"
#endif

/* False, True defines */
#ifndef FALSE
#define FALSE (unsigned char)0
#endif

#ifndef TRUE
#define TRUE (unsigned char)1
#endif

/* Memory size define */
#define MEMORY_SIZE 32


/**************************************************************************************************
 * Data
 **************************************************************************************************/

/**************************************************************************************************
 *\brief  Registers definition
 *************************************************************************************************/
unsigned int *ctrl = ((unsigned int*)0x11840000);
unsigned int *set = ((unsigned int*)0x11840004);
unsigned int *dat = ((unsigned int*)0x11840008);
unsigned int *trg = ((unsigned int*)0x1184000C);

unsigned int *BaseAddr = ((unsigned int*)0x11840000);
/**************************************************************************************************
 * Functions implementations
 **************************************************************************************************/


/**************************************************************************************************
 * \brief Functions for usart init
 *
 * Init configuration for usart
 *
 * \param   -
 * \return  -
 **************************************************************************************************/
void usart3_init(void) {
	/* RCC initialization */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* GPIO initialization */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitTypeDef GPIO_InitStruct = { .GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11,
			.GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_OType = GPIO_OType_PP, .GPIO_PuPd = GPIO_PuPd_UP };
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* USART initialization */
	USART_InitTypeDef USART_InitStruct = { .USART_BaudRate = 9600, .USART_Mode =
			USART_Mode_Rx, // USART_Mode_Tx,
			.USART_WordLength = USART_WordLength_8b, .USART_StopBits =
					USART_StopBits_1, .USART_Parity = USART_Parity_No };

	USART_Init(USART3, &USART_InitStruct);

	USART_Cmd(USART3, ENABLE);

	USART_ClearFlag(USART3, USART_FLAG_TC);
}


/**************************************************************************************************
 * \brief
 *
 *
 *
 * \param   -
 * \return  -
 **************************************************************************************************/
char usart_getc(void) {
	//while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != SET);
	return (char) USART_ReceiveData(USART3);
}


/**************************************************************************************************
 * \brief
 *
 *
 *
 * \param   -
 * \return  -
 **************************************************************************************************/
void usart_putc(char data) {
	/* wait until TXE (Transmit Data Register Empty) flag is set */
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART3, data);
	/* wait until TC (Tranmission Complete) flag is set */
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
		;
}


/**************************************************************************************************
 * \brief Send buffer in hex format to USART3
 *
 * \param   pBuf - pointer to the buffer
 * 			length - size of buffer in bytes
 * \return  -
 **************************************************************************************************/
void usart_SendBufHex(unsigned char *pBuf, uint16_t bufsize) {
	char ch;

	while (bufsize--) {
		ch = *pBuf++;
		while (!(USART3->SR & USART_SR_TXE)); // Wait while transmit data register not empty
		USART3->DR = HEX_CHARS[(ch >> 4)   % 0x10];
		while (!(USART3->SR & USART_SR_TXE)); // Wait while transmit data register not empty
		USART3->DR = HEX_CHARS[(ch & 0x0f) % 0x10];
	}
}


/**************************************************************************************************
 * \brief Send one byte in HEX format to USART3
 *
 * \param   num - 8-bit value to send
 * \return  -
 **************************************************************************************************/
void usart_SendHex8(uint8_t num) {
	USART_SendData(USART3,HEX_CHARS[(num >> 4)   % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num & 0x0f) % 0x10]);
}

/**************************************************************************************************
 * \brief Send 4 bytes in HEX format to USART3
 *
 * \param   num - 32-bits value to send
 * \return  -
 **************************************************************************************************/
void usart_SendHex32(uint32_t num) {

	USART_SendData(USART3,HEX_CHARS[(num >> 28) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num >> 24) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num >> 20) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num >> 16) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num >> 12) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num >>  8) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num >>  4) % 0x10]);
	USART_SendData(USART3,HEX_CHARS[(num & 0x0000000f) % 0x10]);
}

/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void usart_Tx(char *string) {
	for (; *string != '\0'; string++) {
		usart_putc(*string);
	}
}


/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void mem_ReadAndPrint32(unsigned int* address)
{
	usart_SendHex32(*address);
	usart_Tx("  ");
}


/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void mem_ReadAndPrintAll(void)
{
	int i;

	usart_Tx("\r\n\r\n Read all registers: ");
	usart_Tx("\r\n   ctrl     set        dat      trg \r\n");

	for (i = 0; i < MEMORY_SIZE/sizeof(unsigned int); i++) {
		mem_ReadAndPrint32(BaseAddr + i);
	}
}


/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void mem_WriteRead32(unsigned int* addr, unsigned int value)
{
	*addr = value;

	if ( *addr == value) {
		usart_Tx("\r\n Successfully write and read register \r\n");
	} else {
		usart_Tx("\r\n Error, incorrect write and/or read register \r\n");
	}

	mem_ReadAndPrint32(addr);
}


/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void mem_Write32(unsigned int* addr, unsigned int value)
{
	*addr = value;
}

/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void mem_Write8(unsigned int* addr, unsigned char offset, unsigned char value)
{
	unsigned char sft[] = {24, 16, 8, 0};
	unsigned int mask[] = {0x00FFFFFF, 0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00};

	unsigned int u32Value = *addr & mask[offset];

	*addr =  u32Value | (((unsigned int)value << sft[offset]));
}



/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void mem_Init(void) {
	int i;
	unsigned char result = TRUE; //consider no error during writing memory

	usart_Tx("\r\n Init and check memory: ");

	for (i = 0; i < MEMORY_SIZE/sizeof(unsigned int); i++) {
		*(BaseAddr + i) = 0xffffffff;

		if (*(BaseAddr + i)  == 0xffffffff) {
			result &= TRUE;
		} else {
			result &= FALSE;
		}
	}

	if (TRUE == result) {
		usart_Tx(" memory initialized correctly.\r\n");
	} else {
		usart_Tx(" error during initialization memory. \r\n");
	}

	/*Read all registers */
	mem_ReadAndPrintAll();
}


/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void TestCase1()
{
	usart_Tx("\r\n**************************************\r\n");
	usart_Tx("         TEST CASE 1:\r\n\r\n");
	usart_Tx(" When written 0 to 'ctrl': 'set', 'dat' and 'trg' get set to 0x00. \r\n");
	usart_Tx(" When 'dat' is read: 'dat' is completly cleared(with zeros). \r\n");
	usart_Tx("==========================================\r\n");

	/* Init memory range 0x11840000 - 0x1184001F*/
	mem_Init();

	usart_Tx("\r\n\r\n Write 0x00000000 on 'ctrl' register. \r\n ");
	mem_Write32((unsigned int*) 0x11840000, 0);

	/*Read all registers */
	mem_ReadAndPrintAll();

	usart_Tx("\r\n\r\n Write 0x12345678 on 'dat' register. \r\n ");
	mem_Write32((unsigned int*) 0x11840008, 0x12345678);

	/*Read all registers */
	mem_ReadAndPrintAll();

	/*Read all registers */
	mem_ReadAndPrintAll();
	usart_Tx("\r\n**************************************\r\n");
}


/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void TestCase2()
{
	usart_Tx("\r\n**************************************\r\n");
	usart_Tx("           TEST CASE 2:\r\n\r\n");
	usart_Tx(" When written 0xAA to lower byte of 'set': 'dat' becomes all 0xAA. \r\n");
	usart_Tx("==========================================\r\n");


	usart_Tx("\r\n Write 0x12345678 on 'ctrl' register. \r\n");
	mem_Write32((unsigned int*) 0x11840000, 0x12345678);

	usart_Tx(" Write 0xbbccddee on 'set' register. \r\n");
	mem_Write32((unsigned int*) 0x11840004, 0xbbccddee);

	/*Read all registers */
	mem_ReadAndPrintAll();

	usart_Tx("\r\n\r\n Write 0xAA on lower byte of 'set' register. \r\n");
	mem_Write8((unsigned int*)0x11840004, 3, 0xAA);

	/*Read all registers */
	mem_ReadAndPrintAll();

	/*Read all registers */
	mem_ReadAndPrintAll();

	usart_Tx("\r\n**************************************\r\n");
}
/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void TestCase3()
{
	usart_Tx("\r\n**************************************\r\n");
	usart_Tx("           TEST CASE 3:\r\n\r\n");
	usart_Tx(" When 'ctrl' is not 0 and 0xFE is written to higher byte of 'trg': \r\n	- 'set' becomes 0xFF and \r\n	- 'ctrl' is copied to 'dat'. \r\n");
	usart_Tx("==========================================\r\n");

	usart_Tx(" Write 0x11223344 on 'trg' register. \r\n");
	mem_Write32((unsigned int*) 0x1184000C, 0x11223344);

	/*Read all registers */
	mem_ReadAndPrintAll();

	usart_Tx("\r\n\r\n Write 0xFE on higher byte of 'trg' register. \r\n");
	mem_Write8((unsigned int*)0x1184000C, 0, 0xFE);

	/*Read all registers */
	mem_ReadAndPrintAll();

	/*Read all registers */
	mem_ReadAndPrintAll();

	usart_Tx("\r\n**************************************\r\n");
}
/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
void TestCase4()
{

}
/**************************************************************************************************
 * \brief
 *
 * \param
 * \return  -
 **************************************************************************************************/
 void TestCase5()
 {
 }
 
int main() {
	usart3_init();

	TestCase1();
	TestCase2();
	TestCase3();
	TestCase4();
    TestCase5();
	while (1) {

	}

	return 0;
}
