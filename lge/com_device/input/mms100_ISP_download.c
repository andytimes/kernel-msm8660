//--------------------------------------------------------
//
//
//      Melfas MMS100 Series Download base v1.6 2011.07.11
//

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/irq.h>
     
#include "mms100_ISP_download.h"
//#include "MTH_LBATMAN_4G_R00_V02_bin.h"
#include "MTH_LBATMAN_4G_R00_V09_bin.h"
    
//============================================================
//
//      Include MELFAS Binary code File ( ex> MELFAS_FIRM_bin.c)
//
//      Warning!!!!
//              Please, don't add binary.c file into project
//              Just #include here !!
//
//============================================================
     
//#include "core_bin_V01.c"
//#include "public_bin_V01.c"
    UINT8 ucVerifyBuffer[MELFAS_TRANSFER_LENGTH];	//      You may melloc *ucVerifyBuffer instead of this

//---------------------------------
//      Downloading functions
//---------------------------------
static int mcsdl_download(const UINT8 * pData, const UINT16 nLength,
			  INT8 IdxNum);
 static void mcsdl_set_ready(void);
static void mcsdl_reboot_mcs(void);
 static int mcsdl_erase_flash(INT8 IdxNum);
static int mcsdl_program_flash(UINT8 * pDataOriginal, UINT16 unLength,
				INT8 IdxNum);
static void mcsdl_program_flash_part(UINT8 * pData);
 static int mcsdl_verify_flash(UINT8 * pData, UINT16 nLength, INT8 IdxNum);
 static void mcsdl_read_flash(UINT8 * pBuffer);
static int mcsdl_read_flash_from(UINT8 * pBuffer, UINT16 unStart_addr,
				  UINT16 unLength, INT8 IdxNum);
 static void mcsdl_select_isp_mode(UINT8 ucMode);
static void mcsdl_unselect_isp_mode(void);
 static void mcsdl_read_32bits(UINT8 * pData);
static void mcsdl_write_bits(UINT32 wordData, int nBits);

//static void mcsdl_scl_toggle_twice(void);
     
//---------------------------------
//      Delay functions
//---------------------------------
void mcsdl_delay(UINT32 nCount);
 void mcsdl_delay_test(INT32 nCount);
 
//---------------------------------
//      For debugging display
//---------------------------------
#if MELFAS_ENABLE_DBG_PRINT
static void mcsdl_print_result(int nRet);

#endif	/*  */
     
//----------------------------------
// Download enable command
//----------------------------------
#if MELFAS_USE_PROTOCOL_COMMAND_FOR_DOWNLOAD
void melfas_send_download_enable_command(void) 
{
	
	    // TO DO : Fill this up
}  
#endif	/*  */
     
//============================================================
//
//      Main Download furnction
//
//   1. Run mcsdl_download( pBinary[IdxNum], nBinary_length[IdxNum], IdxNum);
//       IdxNum : 0 (Master Chip Download)
//       IdxNum : 1 (2Chip Download)
//
//
//============================================================
int mms100_ISP_download_binary_data(int dl_mode) 
{
	int nRet = 0;
	int retry_cnt = 0;
	 
#if 0
#if MELFAS_USE_PROTOCOL_COMMAND_FOR_DOWNLOAD
	    melfas_send_download_enable_command();
	mcsdl_delay(MCSDL_DELAY_100US);
	
#endif	/*  */
	    MELFAS_DISABLE_BASEBAND_ISR();	// Disable Baseband touch interrupt ISR.
	MELFAS_DISABLE_WATCHDOG_TIMER_RESET();	// Disable Baseband watchdog timer
	
#endif	/*  */
	    
	    //------------------------
	    // Run Download
	    //------------------------
	    for (retry_cnt = 0; retry_cnt < 5; retry_cnt++) {
		if (dl_mode == 0x01)	//MELFAS_ISP_DOWNLOAD
			nRet =
			    mcsdl_download((const UINT8 *)MELFAS_binary,
					   (const UINT16)MELFAS_binary_nLength,
					   0);
		
		else		//MELFAS_ISC_DOWNLOAD
		{
			nRet =
			    mcsdl_download((const UINT8 *)MELFAS_binary,
					   (const UINT16)MELFAS_binary_nLength,
					   0);
		} 
#if MELFAS_2CHIP_DOWNLOAD_ENABLE
		    if (!nRet)
			 {
			if (dl_mode == 0x01)	//MELFAS_ISP_DOWNLOAD
				nRet = mcsdl_download((const UINT8 *)MELFAS_binary, (const UINT16)MELFAS_binary_nLength, 1);	// Slave Binary data download
			else	//MELFAS_ISC_DOWNLOAD
				nRet =
				    mcsdl_download((const UINT8 *)MELFAS_binary,
						   (const UINT16)
						   MELFAS_binary_nLength, 1);
			}
		
#endif	/*  */
		    if (!nRet)
			break;
	}
	 MELFAS_ROLLBACK_BASEBAND_ISR();	// Roll-back Baseband touch interrupt ISR.
	MELFAS_ROLLBACK_WATCHDOG_TIMER_RESET();	// Roll-back Baseband watchdog timer
/*
fw_error:
	if (nRet) {
		mcsdl_erase_flash(0);
		mcsdl_erase_flash(1);
	}*/ 
	    return nRet;
}

 
/*                                                                                         
                                         
 
          
          

                                  
                                 
                                                
                                                     
   
                       
                                                               
                                                        
                                                               
                                                       
                                                                            
                                              
                                        
   
                                                     

      

                                                         
                                   

           
             

                                  
                
                                  

                                                       
     
                                            
     

                                  
                    
                                  

                           

                                     

                                  
                      
                                  

                                                  

                        
     

                                            
     

                                  
                     
                                  

                           

                                                                            

                                     
     

                                            

                                                             
                          

                                            
     

                                  
               
                                  

             

       

                                            
                                          
                                   
      

                                                                                            
                                                                                      

                               
     
                                                                                                        
         
                                      
                           
                                      
                                                                                                 
         
            
         
                                          
         
     

                                                                                              
                                                                                        

                            
                            
       

                                      

 */ 
    
//------------------------------------------------------------------
//
//      Download function
//
//------------------------------------------------------------------
static int mcsdl_download(const UINT8 * pBianry, const UINT16 unLength,
			   INT8 IdxNum) 
{
	int nRet;
	 
	    //---------------------------------
	    // Check Binary Size
	    //---------------------------------
	    if (unLength > MELFAS_FIRMWARE_MAX_SIZE) {
		 nRet = MCSDL_RET_PROGRAM_SIZE_IS_WRONG;
		goto MCSDL_DOWNLOAD_FINISH;
	}
	  
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk(" - Starting download...\n");
	
#endif	/*  */
	     
	    //---------------------------------
	    // Make it ready
	    //---------------------------------
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk(" > Ready\n");
	
#endif	/*  */
	    mcsdl_set_ready();
	  
	    //---------------------------------
	    // Erase Flash
	    //---------------------------------
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk(" > Erase\n");
	
#endif	/*  */
	    nRet = mcsdl_erase_flash(IdxNum);
	 if (nRet != MCSDL_RET_SUCCESS)
		goto MCSDL_DOWNLOAD_FINISH;
	 
	    //---------------------------------
	    // Program Flash
	    //---------------------------------
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk("\n > Program   ");
	
#endif	/*  */
	    
//      if(IdxNum > 0)
//              mcsdl_set_ready();
	    nRet =
	    mcsdl_program_flash((UINT8 *) pBianry, (UINT16) unLength, IdxNum);
	if (nRet != MCSDL_RET_SUCCESS)
		goto MCSDL_DOWNLOAD_FINISH;
	 
	    //---------------------------------
	    // Verify flash
	    //---------------------------------
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk("\n > Verify    ");
	
#endif	/*  */
	    nRet =
	    mcsdl_verify_flash((UINT8 *) pBianry, (UINT16) unLength, IdxNum);
	if (nRet != MCSDL_RET_SUCCESS)
		goto MCSDL_DOWNLOAD_FINISH;
	  nRet = MCSDL_RET_SUCCESS;
  MCSDL_DOWNLOAD_FINISH: 
#if MELFAS_ENABLE_DBG_PRINT
	    mcsdl_print_result(nRet);	// Show result
#endif	/*  */
	
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk(" > Rebooting\n");
	printk(" - Fin.\n\n");
	
#endif	/*  */
	    mcsdl_reboot_mcs();
	 return nRet;
}

   
//------------------------------------------------------------------
//
//      Sub functions
//
//------------------------------------------------------------------
static int mcsdl_erase_flash(INT8 IdxNum) 
{
	int i;
	UINT8 readBuffer[32];
	int eraseCompareValue = 0xFF;
	
	    //----------------------------------------
	    //      Do erase
	    //----------------------------------------
	    if (IdxNum > 0)
		 {
		mcsdl_select_isp_mode(ISP_MODE_NEXT_CHIP_BYPASS);
		mcsdl_delay(MCSDL_DELAY_3US);
		}
	mcsdl_select_isp_mode(ISP_MODE_ERASE_FLASH);
	mcsdl_unselect_isp_mode();
	  
	    //----------------------------------------
	    //      Check 'erased well'
	    //----------------------------------------
//start ADD DELAY
	    mcsdl_read_flash_from(readBuffer, 0x0000, 16, IdxNum);
	mcsdl_read_flash_from(&readBuffer[16], 0x7FF0, 16, IdxNum);
	
	    //end ADD DELAY
	    if (IdxNum > 0)
		 {
		eraseCompareValue = 0x00;
		}
	
	    // Compare with '0xFF'
	    for (i = 0; i < 32; i++) {
		if (readBuffer[i] != eraseCompareValue)
			return MCSDL_RET_ERASE_FLASH_VERIFY_FAILED;
	}
	  return MCSDL_RET_SUCCESS;
}

  static int mcsdl_program_flash(UINT8 * pDataOriginal, UINT16 unLength,
				    INT8 IdxNum) 
{
	int i;
	 UINT8 * pData;
	UINT8 ucLength;
	 UINT16 addr;
	UINT32 header;
	 addr = 0;
	pData = pDataOriginal;
	 ucLength = MELFAS_TRANSFER_LENGTH;
	 
//kang
	    while ((addr * 4) < (int)unLength)
		 {
		if ((unLength - (addr * 4)) < MELFAS_TRANSFER_LENGTH)
			 {
			ucLength = (UINT8) (unLength - (addr * 4));
			}
		 
		    //--------------------------------------
		    //      Select ISP Mode
		    //--------------------------------------
		    
		    // start ADD DELAY
		    mcsdl_delay(MCSDL_DELAY_40US);
		
		    //end ADD DELAY
		    if (IdxNum > 0) {
			mcsdl_select_isp_mode(ISP_MODE_NEXT_CHIP_BYPASS);
			mcsdl_delay(MCSDL_DELAY_3US);
		 }
		mcsdl_select_isp_mode(ISP_MODE_SERIAL_WRITE);
		 
		    //---------------------------------------------
		    //      Header
		    //      Address[13ibts] <<1
		    //---------------------------------------------
		    header = ((addr & 0x1FFF) << 1) | 0x0;
		header = header << 14;
		 
		    //Write 18bits
		    mcsdl_write_bits(header, 18);
		
		    //start ADD DELAY
		    //end ADD DELAY
		    
		    //---------------------------------
		    //      Writing
		    //---------------------------------
		    //          addr += (UINT16)ucLength;
		    addr += 1;
		 
#if 0				// MELFAS_ENABLE_DBG_PROGRESS_PRINT
		    printk("#");
		
#endif	/*  */
		     mcsdl_program_flash_part(pData);
		 pData += ucLength;
		 
		    //---------------------------------------------
		    //      Tail
		    //---------------------------------------------
		    MCSDL_GPIO_SDA_SET_HIGH();
		
//kang
		    mcsdl_delay(MCSDL_DELAY_40US);
		 for (i = 0; i < 6; i++)
			 {
			if (i == 2)
				 {
				mcsdl_delay(MCSDL_DELAY_20US);
				}
			
			else if (i == 3)
				 {
				mcsdl_delay(MCSDL_DELAY_40US);
				}
			 MCSDL_GPIO_SCL_SET_HIGH();
			mcsdl_delay(MCSDL_DELAY_10US);
			MCSDL_GPIO_SCL_SET_LOW();
			mcsdl_delay(MCSDL_DELAY_10US);
			}
		MCSDL_GPIO_SDA_SET_LOW();
		 mcsdl_unselect_isp_mode();
		
//start ADD DELAY
		    mcsdl_delay(MCSDL_DELAY_300US);	//bomi
//end ADD DELAY
		 }
	 return MCSDL_RET_SUCCESS;
}

 static void mcsdl_program_flash_part(UINT8 * pData) 
{
	
	    //int     i;
	    UINT32 data;
	  
	    //---------------------------------
	    //      Body
	    //---------------------------------
	    data = (UINT32) pData[0] << 0;
	data |= (UINT32) pData[1] << 8;
	data |= (UINT32) pData[2] << 16;
	data |= (UINT32) pData[3] << 24;
	mcsdl_write_bits(data, 32);
  }  static int mcsdl_verify_flash(UINT8 * pDataOriginal, UINT16 unLength,
					INT8 IdxNum) 
{
	int j;
	int nRet;
	 UINT8 * pData;
	UINT8 ucLength;
	 UINT16 addr;
	UINT32 wordData;
	 addr = 0;
	pData = (UINT8 *) pDataOriginal;
	 ucLength = MELFAS_TRANSFER_LENGTH;
	 while ((addr * 4) < (int)unLength)
		 {
		if ((unLength - (addr * 4)) < MELFAS_TRANSFER_LENGTH)
			 {
			ucLength = (UINT8) (unLength - (addr * 4));
			}
		
		    // start ADD DELAY
		    mcsdl_delay(MCSDL_DELAY_40US);
		 
		    //--------------------------------------
		    //      Select ISP Mode
		    //--------------------------------------
		    if (IdxNum > 0)
			 {
			mcsdl_select_isp_mode(ISP_MODE_NEXT_CHIP_BYPASS);
			mcsdl_delay(MCSDL_DELAY_3US);
			}
		 mcsdl_select_isp_mode(ISP_MODE_SERIAL_READ);
		  
		    //---------------------------------------------
		    //      Header
		    //      Address[13ibts] <<1
		    //---------------------------------------------
		    wordData = ((addr & 0x1FFF) << 1) | 0x0;
		wordData <<= 14;
		 mcsdl_write_bits(wordData, 18);
		 addr += 1;
		 
#if 0				//MELFAS_ENABLE_DBG_PROGRESS_PRINT
		    printk("# \n");
		
#endif	/*  */
		      
		    //--------------------
		    // Read flash
		    //--------------------
		    mcsdl_read_flash(ucVerifyBuffer);
		  
		    // MCSDL_GPIO_SDA_SET_LOW(); //bomi
		    MCSDL_GPIO_SDA_SET_OUTPUT();
		MCSDL_GPIO_SDA_SET_LOW();
		 
		    //--------------------
		    // Comparing
		    //--------------------
		     if (IdxNum == 0)
			 {
			for (j = 0; j < (int)ucLength; j++)
				 {
				
#if 0				// MELFAS_ENABLE_DBG_PROGRESS_PRINT
				    printk(" %02X", ucVerifyBuffer[j]);
				
#endif	/*  */
				    if (ucVerifyBuffer[j] != pData[j])
					 {
					
#if 0				// MELFAS_ENABLE_DBG_PROGRESS_PRINT
					    printk
					    ("\n [Error] Address : 0x%04X : 0x%02X - 0x%02X\n",
					     addr, pData[j], ucVerifyBuffer[j]);
					
#endif	/*  */
					     nRet =
					    MCSDL_RET_PROGRAM_VERIFY_FAILED;
					goto MCSDL_VERIFY_FLASH_FINISH;
					 }
				}
			}
		
		else		// slave
		{
			for (j = 0; j < (int)ucLength; j++)
				 {
				
#if 0				// MELFAS_ENABLE_DBG_PROGRESS_PRINT
				    printk(" %02X", ucVerifyBuffer[j]);
				
#endif	/*  */
				    if ((0xff - ucVerifyBuffer[j]) != pData[j])
					 {
					
#if 0				//MELFAS_ENABLE_DBG_PROGRESS_PRINT
					    printk
					    ("\n [Error] Address : 0x%04X : 0x%02X - 0x%02X\n",
					     addr, pData[j], ucVerifyBuffer[j]);
					
#endif	/*  */
					     nRet =
					    MCSDL_RET_PROGRAM_VERIFY_FAILED;
					goto MCSDL_VERIFY_FLASH_FINISH;
					 }
				}
		}
		pData += ucLength;
		mcsdl_unselect_isp_mode();
		}
	 nRet = MCSDL_RET_SUCCESS;
 MCSDL_VERIFY_FLASH_FINISH: 
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk("\n");
	
#endif	/*  */
	    mcsdl_unselect_isp_mode();
	 return nRet;
}

  static void mcsdl_read_flash(UINT8 * pBuffer) 
{
	int i;
	 MCSDL_GPIO_SCL_SET_LOW();
	MCSDL_GPIO_SDA_SET_LOW();
	 mcsdl_delay(MCSDL_DELAY_40US);	//bomi
	for (i = 0; i < 6; i++)
		 {
		MCSDL_GPIO_SCL_SET_HIGH();
		mcsdl_delay(MCSDL_DELAY_10US);	//bomi
		MCSDL_GPIO_SCL_SET_LOW();
		mcsdl_delay(MCSDL_DELAY_10US);	//bomi
		}
	 mcsdl_read_32bits(pBuffer);
}

 static int mcsdl_read_flash_from(UINT8 * pBuffer, UINT16 unStart_addr,
				    UINT16 unLength, INT8 IdxNum) 
{
	int i;
	int j;
	 UINT8 ucLength;
	 UINT16 addr;
	UINT32 wordData;
	 if (unLength >= MELFAS_FIRMWARE_MAX_SIZE)
		 {
		return MCSDL_RET_PROGRAM_SIZE_IS_WRONG;
		}
	 addr = 0;
	ucLength = MELFAS_TRANSFER_LENGTH;
	 
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    printk(" %04X : ", unStart_addr);
	
#endif	/*  */
	     for (i = 0; i < (int)unLength; i += (int)ucLength)
		 {
		 addr = (UINT16) i;
		if (IdxNum > 0)
			 {
			mcsdl_select_isp_mode(ISP_MODE_NEXT_CHIP_BYPASS);
			mcsdl_delay(MCSDL_DELAY_3US);
			}
		 mcsdl_select_isp_mode(ISP_MODE_SERIAL_READ);
		wordData = (((unStart_addr + addr) & 0x1FFF) << 1) | 0x0;
		wordData <<= 14;
		 mcsdl_write_bits(wordData, 18);
		 
		    //printk("wordData =%x \n",wordData);
		    if ((unLength - addr) < MELFAS_TRANSFER_LENGTH)
			 {
			ucLength = (UINT8) (unLength - addr);
			}
		 
		    //--------------------
		    // Read flash
		    //--------------------
		    mcsdl_read_flash(&pBuffer[addr]);
		  
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
		    for (j = 0; j < (int)ucLength; j++)
			 {
			printk("%02X ", pBuffer[j]);
			} 
#endif	/*  */
		    mcsdl_unselect_isp_mode();
		 }  
#if MELFAS_ENABLE_DBG_PROGRESS_PRINT
	    //printk("\n");
#endif	/*  */
	    return MCSDL_RET_SUCCESS;
 }

  static void mcsdl_set_ready(void) 
{
	
	    //--------------------------------------------
	    // Tkey module reset
	    //--------------------------------------------
	    MCSDL_VDD_SET_LOW();	// power
	
	    //MCSDL_CE_SET_LOW();
	    MCSDL_CE_SET_OUTPUT();
	 MCSDL_SET_GPIO_I2C();
	 
	    //MCSDL_GPIO_SDA_SET_LOW(); //bomi
	    MCSDL_GPIO_SDA_SET_OUTPUT();
	MCSDL_GPIO_SDA_SET_LOW();
	 
	    //MCSDL_GPIO_SCL_SET_LOW();  //bomi
	    MCSDL_GPIO_SCL_SET_OUTPUT();
	MCSDL_GPIO_SCL_SET_LOW();
	 
	    //MCSDL_RESETB_SET_LOW();  //bomi
	    MCSDL_RESETB_SET_OUTPUT();
	MCSDL_RESETB_SET_LOW();
	 mcsdl_delay(MCSDL_DELAY_25MS);	// Delay for Stable VDD
	MCSDL_VDD_SET_HIGH();
	MCSDL_CE_SET_HIGH();
	 MCSDL_GPIO_SCL_SET_LOW();
	MCSDL_GPIO_SDA_SET_HIGH();
	 mcsdl_delay(MCSDL_DELAY_40MS);	// Delay '30 msec'
}   static void mcsdl_reboot_mcs(void) 
{
	
	    //--------------------------------------------
	    // Tkey module reset
	    //--------------------------------------------
	    MCSDL_VDD_SET_LOW();
	 MCSDL_CE_SET_LOW();
	MCSDL_CE_SET_OUTPUT();
	 MCSDL_GPIO_SDA_SET_HIGH();
	MCSDL_GPIO_SDA_SET_OUTPUT();
	 MCSDL_GPIO_SCL_SET_HIGH();
	MCSDL_GPIO_SCL_SET_OUTPUT();
	 
	    //MCSDL_SET_HW_I2C();
	    
	    //MCSDL_RESETB_SET_LOW(); //bomi
	    MCSDL_RESETB_SET_OUTPUT();
	
	    //MCSDL_RESETB_SET_LOW();
	    mcsdl_delay(MCSDL_DELAY_25MS);	// Delay for Stable VDD
	MCSDL_VDD_SET_HIGH();
	 MCSDL_RESETB_SET_HIGH();
	MCSDL_RESETB_SET_INPUT();
	MCSDL_GPIO_SCL_SET_INPUT();
	MCSDL_GPIO_SDA_SET_INPUT();
	 mcsdl_delay(MCSDL_DELAY_30MS);	// Delay '25 msec'
}   

//--------------------------------------------
//
//   Write ISP Mode entering signal
//
//--------------------------------------------
static void mcsdl_select_isp_mode(UINT8 ucMode) 
{
	int i;
	 UINT8 enteringCodeMassErase[16] = {
	0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1};
	UINT8 enteringCodeSerialWrite[16] = {
	0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1};
	UINT8 enteringCodeSerialRead[16] = {
	0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1};
	UINT8 enteringCodeNextChipBypass[16] = {
	1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1};
	 UINT8 * pCode = 0;
	  
	    //------------------------------------
	    // Entering ISP mode : Part 1
	    //------------------------------------
	    if (ucMode == ISP_MODE_ERASE_FLASH)
		pCode = enteringCodeMassErase;
	
	else if (ucMode == ISP_MODE_SERIAL_WRITE)
		pCode = enteringCodeSerialWrite;
	
	else if (ucMode == ISP_MODE_SERIAL_READ)
		pCode = enteringCodeSerialRead;
	
	else if (ucMode == ISP_MODE_NEXT_CHIP_BYPASS)
		pCode = enteringCodeNextChipBypass;
	 MCSDL_RESETB_SET_LOW();
	MCSDL_GPIO_SCL_SET_LOW();
	MCSDL_GPIO_SDA_SET_HIGH();
	for (i = 0; i < 16; i++)
		 {
		if (pCode[i] == 1)
			MCSDL_RESETB_SET_HIGH();
		
		else
			MCSDL_RESETB_SET_LOW();
		 
//start add delay for INT
		    mcsdl_delay(MCSDL_DELAY_3US);
		
//end delay for INT
		    MCSDL_GPIO_SCL_SET_HIGH();
		mcsdl_delay(MCSDL_DELAY_3US);
		MCSDL_GPIO_SCL_SET_LOW();
		mcsdl_delay(MCSDL_DELAY_3US);
		  }
	 MCSDL_RESETB_SET_LOW();
	 
	    //---------------------------------------------------
	    // Entering ISP mode : Part 2   - Only Mass Erase
	    //---------------------------------------------------
	    mcsdl_delay(MCSDL_DELAY_7US);
	 MCSDL_GPIO_SCL_SET_LOW();
	MCSDL_GPIO_SDA_SET_HIGH();
	if (ucMode == ISP_MODE_ERASE_FLASH) {
		mcsdl_delay(MCSDL_DELAY_7US);
		for (i = 0; i < 4; i++) {
			 if (i == 2)
				mcsdl_delay(MCSDL_DELAY_25MS);
			
			else if (i == 3)
				mcsdl_delay(MCSDL_DELAY_150US);	//bomi
			MCSDL_GPIO_SCL_SET_HIGH();
			mcsdl_delay(MCSDL_DELAY_3US);
			MCSDL_GPIO_SCL_SET_LOW();
			mcsdl_delay(MCSDL_DELAY_7US);
		}
	}
	MCSDL_GPIO_SDA_SET_LOW();
}

  static void mcsdl_unselect_isp_mode(void) 
{
	int i;
	 
	    // MCSDL_GPIO_SDA_SET_HIGH();
	    // MCSDL_GPIO_SDA_SET_OUTPUT();
	    MCSDL_RESETB_SET_LOW();
	 mcsdl_delay(MCSDL_DELAY_3US);
	 for (i = 0; i < 10; i++) {
		 MCSDL_GPIO_SCL_SET_HIGH();
		mcsdl_delay(MCSDL_DELAY_3US);
		MCSDL_GPIO_SCL_SET_LOW();
		mcsdl_delay(MCSDL_DELAY_3US);
	}
 }

   static void mcsdl_read_32bits(UINT8 * pData) 
{
	int i, j;
	MCSDL_GPIO_SCL_SET_LOW();
	MCSDL_GPIO_SDA_SET_LOW();
	MCSDL_GPIO_SDA_SET_INPUT();
	 for (i = 3; i >= 0; i--) {
		 pData[i] = 0;
		 for (j = 0; j < 8; j++) {
			 pData[i] <<= 1;
			 MCSDL_GPIO_SCL_SET_LOW();
			mcsdl_delay(MCSDL_DELAY_1US);
			MCSDL_GPIO_SCL_SET_HIGH();
			mcsdl_delay(MCSDL_DELAY_1US);
			 if (MCSDL_GPIO_SDA_IS_HIGH())
				pData[i] |= 0x01;
		}
	}
	 
	    //MCSDL_GPIO_SDA_SET_LOW(); //bomi
	    MCSDL_GPIO_SDA_SET_OUTPUT();
	MCSDL_GPIO_SDA_SET_LOW();
}

   static void mcsdl_write_bits(UINT32 wordData, int nBits) 
{
	int i;
	 MCSDL_GPIO_SCL_SET_LOW();
	MCSDL_GPIO_SDA_SET_LOW();
	 for (i = 0; i < nBits; i++)
		 {
		if (wordData & 0x80000000) {
			MCSDL_GPIO_SDA_SET_HIGH();
		}
		
		else {
			MCSDL_GPIO_SDA_SET_LOW();
		}
		 mcsdl_delay(MCSDL_DELAY_3US);
		 MCSDL_GPIO_SCL_SET_HIGH();
		mcsdl_delay(MCSDL_DELAY_3US);
		MCSDL_GPIO_SCL_SET_LOW();
		mcsdl_delay(MCSDL_DELAY_3US);
		 wordData <<= 1;
		}
}

 
/*
static void mcsdl_scl_toggle_twice(void)
{

	MCSDL_GPIO_SDA_SET_HIGH();
	MCSDL_GPIO_SDA_SET_OUTPUT();

	MCSDL_GPIO_SCL_SET_HIGH();	mcsdl_delay(MCSDL_DELAY_20US);
	MCSDL_GPIO_SCL_SET_LOW();	mcsdl_delay(MCSDL_DELAY_20US);

	MCSDL_GPIO_SCL_SET_HIGH();	mcsdl_delay(MCSDL_DELAY_20US);
	MCSDL_GPIO_SCL_SET_LOW();	mcsdl_delay(MCSDL_DELAY_20US);
}
*/ 
    
//============================================================
//
//      Delay Function
//
//============================================================
void mcsdl_delay(UINT32 nCount) 
{
	if (nCount > 10000)
		msleep(nCount / 1000);
	
	else
		usleep(nCount);	// Please, Use your delay function
}

   
//============================================================
//
//      Debugging print functions.
//
//============================================================
    
#ifdef MELFAS_ENABLE_DBG_PRINT
static void mcsdl_print_result(int nRet) 
{
	if (nRet == MCSDL_RET_SUCCESS)
		 {
		printk(" > MELFAS Firmware downloading SUCCESS.\n");
		}
	
	else
		 {
		printk(" > MELFAS Firmware downloading FAILED  :  ");
		switch (nRet)
			 {
		case MCSDL_RET_SUCCESS:
			printk("MCSDL_RET_SUCCESS\n");
			break;
		case MCSDL_RET_ERASE_FLASH_VERIFY_FAILED:
			printk("MCSDL_RET_ERASE_FLASH_VERIFY_FAILED\n");
			break;
		case MCSDL_RET_PROGRAM_VERIFY_FAILED:
			printk("MCSDL_RET_PROGRAM_VERIFY_FAILED\n");
			break;
		 case MCSDL_RET_PROGRAM_SIZE_IS_WRONG:
			printk("MCSDL_RET_PROGRAM_SIZE_IS_WRONG\n");
			break;
		case MCSDL_RET_VERIFY_SIZE_IS_WRONG:
			printk("MCSDL_RET_VERIFY_SIZE_IS_WRONG\n");
			break;
		case MCSDL_RET_WRONG_BINARY:
			printk("MCSDL_RET_WRONG_BINARY\n");
			break;
		 case MCSDL_RET_READING_HEXFILE_FAILED:
			printk("MCSDL_RET_READING_HEXFILE_FAILED\n");
			break;
		case MCSDL_RET_FILE_ACCESS_FAILED:
			printk("MCSDL_RET_FILE_ACCESS_FAILED\n");
			break;
		case MCSDL_RET_MELLOC_FAILED:
			printk("MCSDL_RET_MELLOC_FAILED\n");
			break;
		 case MCSDL_RET_WRONG_MODULE_REVISION:
			printk("MCSDL_RET_WRONG_MODULE_REVISION\n");
			break;
		 default:
			printk("UNKNOWN ERROR. [0x%02X].\n", nRet);
			break;
			}
		 }
 }

 
#endif	/*  */
     
#if MELFAS_ENABLE_DELAY_TEST
    
//============================================================
//
//      For initial testing of delay and gpio control
//
//      You can confirm GPIO control and delay time by calling this function.
//
//============================================================
void mcsdl_delay_test(INT32 nCount) 
{
	INT16 i;
	 MELFAS_DISABLE_BASEBAND_ISR();	// Disable Baseband touch interrupt ISR.
	MELFAS_DISABLE_WATCHDOG_TIMER_RESET();	// Disable Baseband watchdog timer
	
	    //--------------------------------
	    //      Repeating 'nCount' times
	    //--------------------------------
	    MCSDL_SET_GPIO_I2C();
	MCSDL_GPIO_SCL_SET_OUTPUT();
	MCSDL_GPIO_SDA_SET_OUTPUT();
	MCSDL_RESETB_SET_OUTPUT();
	 MCSDL_GPIO_SCL_SET_HIGH();
	 for (i = 0; i < nCount; i++)
		 {
		 
#if 1
		    MCSDL_GPIO_SCL_SET_LOW();
		
#if 0
#define MCSDL_DELAY_25MS							25000
#define MCSDL_DELAY_30MS							30000
#define MCSDL_DELAY_40MS							40000
#define MCSDL_DELAY_60MS                            			60000
		    
#define MCSDL_DELAY_100MS						  100000
#define MCSDL_DELAY_500MS						  500000
#endif	/*  */
		    printk("MCSDL_DELAY_1US\n");
		mcsdl_delay(MCSDL_DELAY_1US);
		printk("MCSDL_DELAY_3US\n");
		mcsdl_delay(MCSDL_DELAY_3US);
		printk("MCSDL_DELAY_7US\n");
		mcsdl_delay(MCSDL_DELAY_7US);
		printk("MCSDL_DELAY_10US\n");
		mcsdl_delay(MCSDL_DELAY_10US);
		printk("MCSDL_DELAY_20US\n");
		mcsdl_delay(MCSDL_DELAY_20US);
		printk("MCSDL_DELAY_40US\n");
		mcsdl_delay(MCSDL_DELAY_40US);
		 printk("MCSDL_DELAY_100US\n");
		mcsdl_delay(MCSDL_DELAY_100US);
		printk("MCSDL_DELAY_150US\n");
		mcsdl_delay(MCSDL_DELAY_150US);
		printk("MCSDL_DELAY_300US\n");
		mcsdl_delay(MCSDL_DELAY_300US);
		printk("MCSDL........................................\n");
		  MCSDL_GPIO_SCL_SET_HIGH();
		 mcsdl_delay(MCSDL_DELAY_100US);
		 
#elif 0
		    MCSDL_GPIO_SCL_SET_LOW();
		 mcsdl_delay(MCSDL_DELAY_500US);
		 MCSDL_GPIO_SCL_SET_HIGH();
		 mcsdl_delay(MCSDL_DELAY_1MS);
		 
#else	/*  */
		    MCSDL_GPIO_SCL_SET_LOW();
		 mcsdl_delay(MCSDL_DELAY_25MS);
		 TKEY_INTR_SET_LOW();
		 mcsdl_delay(MCSDL_DELAY_45MS);
		 TKEY_INTR_SET_HIGH();
		 
#endif	/*  */
		}
	 MCSDL_GPIO_SCL_SET_HIGH();
	 MELFAS_ROLLBACK_BASEBAND_ISR();	// Roll-back Baseband touch interrupt ISR.
	MELFAS_ROLLBACK_WATCHDOG_TIMER_RESET();	// Roll-back Baseband watchdog timer
}

  
#endif	/*  */
      
