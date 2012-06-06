#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\Memalloc.h"
#include "PlayMenu.h" 
#include "..\IR\remote.h"  
#include "..\header\variables.h"
#include "..\header\host_init.h"

#include "..\UI_Display\common_UI_disp_func.c"
#include "..\LCM\TFT_display.h"		// TFT	
#include "..\LCM\TFT_config.h"		// TFT	


void PlayMenuProcess();
void PlayMenuProcess_IR();
void PlayModeMenuProcess_IR();
void LanguageMenuProcess_IR();
void EQMenuProcess_IR();
void Play_SourceDetect_Process_Mainmenu();
void IR_Service_Process_Menu_1(void);
extern	void	ir_service_menu(void);
extern void IR_Service_Process_Menu();
extern	void	TFT_ShowMenu();
xdata U8 LanguageTable[]={2,1,0,3,4,6,5,7,11,12,8,9,10,13,14,15};

extern	xdata	U8	gc_ShowMenuTimer;
extern	U8		gc_clock_mode;  //20090526 chiayen add
xdata	U8	gc_LanguageSel;
extern xdata	U16	gw_Disp_CurrentSecBak;
extern 	U8   USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern	xdata	U8	gc_MenuItemCount;
extern	data	bit	gb_TFT_refresh;
extern  xdata 	U8 gc_ContrastSaveOrNot;
extern	void TFT_Contrast_SET(U8 Contrast);
#define SetupMenuTpye 41
#define LanguageMenuTpye 42
#define MemeryTitleType 43
#define FWVerTitleType 44

#ifdef Contrast_Set
#define ContrastTitleType 45
#endif


void LanguageMenuProcess()
{
	bit	tc_Confirm=0;

	gc_PhaseInx = 0;
	gc_MenuItem=gc_LanguageSel;
	//TFT_LanguageMenu();
    Menu_Disp_Item_Other(LanguageMenuTpye,gc_MenuItem,REFRESH_ALL);
	while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_PlayMenu:
					gc_refresh_type = REFRESH_ALL;
					tc_Confirm=1;
	            break;

	            case C_PlayNext:
					gc_refresh_type = REFRESH_NEXT;
					if(gc_MenuItem==16)
					{
						gc_MenuItem=0;
					}
					else
					{
						gc_MenuItem++;
					}
					//TFT_LanguageMenu();
				    Menu_Disp_Item_Other(LanguageMenuTpye,gc_MenuItem,gc_refresh_type);
	            break;

	            case C_PlayPrev:
					gc_refresh_type = REFRESH_PREV;
					if(gc_MenuItem==0)
					{
						gc_MenuItem=16;
					}
					else
					{
						gc_MenuItem--;
					}
					//TFT_LanguageMenu();
				    Menu_Disp_Item_Other(LanguageMenuTpye,gc_MenuItem,gc_refresh_type);
	            break;

				case C_MainMenu: //Long key mode
					gc_Task_Next=C_Task_Menu;
					return;
				break;	
	        }

	  		if(tc_Confirm==1) //enter child dir or confirm selected item
			{
            /*	if(gc_MenuItem>4)      //Terry 20090518
	            {
    	          gc_MenuItem=0;
        	    }
				gc_LanguageSel=gc_MenuItem;
				gc_MenuHZK=LanguageTable[gc_MenuItem];
			//	dbprintf("gc_MenuHZK=%bx\n",gc_MenuHZK);
				USER_LogFile_ReadWrite(1);
				gc_MenuItem=0;
				TFT_SetupMenu();
				return;
			*/
				gc_LanguageSel=gc_MenuItem;
				gc_MenuHZK=gc_LanguageSel;//LanguageTable[gc_MenuItem];
			//	dbprintf("gc_MenuHZK=%bx\n",gc_MenuHZK);
				USER_LogFile_ReadWrite(1);
				gc_MenuItem=0;
				//TFT_SetupMenu();
				Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,REFRESH_ALL);
				return;
			}
		}
		gc_PhaseInx=0;		
		Polling_TaskEvents();
                 IR_Service_Process_Menu_1(); 
	        if(gc_Task_Next==C_Task_PowerOff)
		{
			return;
		}
	}	
}

void TFT_ShowContrastAdj(void)
{
    data	U8 tc_Contrast[3];
    data	U8 i;
	xdata	U8 tc_DisplayContrast;

    tc_DisplayContrast=gc_Contrast;   

    tc_Contrast[0]=tc_DisplayContrast/100;
    tc_Contrast[1]=(tc_DisplayContrast%100)/10;
	tc_Contrast[2]=tc_DisplayContrast-tc_Contrast[0]*100-tc_Contrast[1]*10;
    for(i=0;i<3;i++)
		{
        LCM_set_view(TFT_IconDispDir,VOLAdj_X_Pos+(VOLAdj_H_Size+2)*i-2,VOLAdj_Y_Pos,VOLAdj_H_Size,VOLAdj_V_Size);
        LCD_DisplayIcon(VOLAdjID,tc_Contrast[i]);
		}
	}	

void ContrastMenuProcess()
{
	bit	tc_Confirm=0;
	xdata U8 tc_Contrast;

	gc_PhaseInx = 0; 
	
	
	Menu_Disp_Item_Other(ContrastTitleType,gc_MenuItem,REFRESH_ALL);
	TFT_Contrast_SET(gc_Contrast);//sunzhk add 
	gc_Contrast_bak = gc_Contrast;
	TFT_ShowContrastAdj();
	
	while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_PlayMenu:
					gc_refresh_type = REFRESH_ALL;
					tc_Confirm=1;
	            break;

	            case C_PlayNext:
					if(gc_Contrast == 0xff)
					{
						gc_Contrast = 1;
					}
					else
					{
						gc_Contrast++;
					}
					TFT_ShowContrastAdj();
					TFT_Contrast_SET(gc_Contrast);
	            break;

	            case C_PlayPrev:
					if(gc_Contrast == 1)
					{
						gc_Contrast = 0xff;
					}
					else
					{
						gc_Contrast--;
					}
					TFT_ShowContrastAdj();
					TFT_Contrast_SET(gc_Contrast);
	            break;

				case C_MainMenu: //Long key mode
					gc_Task_Next=C_Task_Menu;
					gc_Contrast =gc_Contrast_bak;
					TFT_Contrast_SET(gc_Contrast_bak);
					 gc_ContrastSaveOrNot=1; 
                     USER_LogFile_ReadWrite(1); 

					return;
				break;	
	        }
			if(tc_Confirm==1)
			{

				TFT_Contrast_SET(gc_Contrast);
				gc_ContrastSaveOrNot=1;
				USER_LogFile_ReadWrite(1);
				
				gc_MenuItem=1;
				Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,REFRESH_ALL);
				return;
			}
		}
		gc_PhaseInx=0;		
		Polling_TaskEvents();
        IR_Service_Process_Menu_1();
		if(gc_Task_Next==C_Task_PowerOff)
		{
			return;
		}
	}
}
void count_free_cluster_fat16(U8 xdata * p, U8 size1, U8 size2)
{
	gdw_TotalFreeClusNumber += count_free_cluster_in_fat16_sector(p, size1);
	gdw_TotalFreeClusNumber += count_free_cluster_in_fat16_sector(p+size1*2, size2);
}

//! Search or count free clusters in FAT.
/*!
 * \param tc_SearchMode 
 *			- 0 is to search free clusters.
 * 			- 1 is to count number of free clusters.
 * \return Return 0 on success. Otherwise a nonzero number is returned.
 */
U8 DOS_SearchFreeCluster(U8 tc_SearchMode)
{
	U32 tdw_FreeClusterNum;
	U32 tdw_FAT_sect_address; //Ching 081111
	U32	tdw_last_FAT_sector;
	U8	tc_loop1_counter;
	U8	tc_loop2_counter;
	U8	tc_temp;
	U16	tw_size;
	U32 tdw_end;
	U8 xdata * p_buf;
	U8	tc_local_cluster_count_minus_1;
	U8	tc_log2_local_cluster_count;
	U8	tc_is_fat16 = (gc_DOS_FileSystemType == 1);
	U8  tc_status; //Ching 081111
	U32 tdw_DOS_SectorPerFAT=0; //Ching 081111

	if (tc_SearchMode==0 && gc_ClusBufValidSize >= 8)
			return 0;
	// if cluster buffer not full, search free clusters first before counting the number.
	if (gc_ClusBufValidSize < 8 && gdw_FreeClusterNum[0]<(gdw_DOS_FatMaxCluster-1))
			tc_SearchMode = 0;
	tdw_FreeClusterNum = gdw_FreeClusterNum[tc_SearchMode]+1;
	gb_ReadWriteDataArea = 1; // set read target for DOS_Read_LogicSector().
	if (gc_DOS_FileSystemType == 1) // FAT16
	{
		tc_local_cluster_count_minus_1 = 256-1;
		tc_log2_local_cluster_count = 8;
	}
	else
	{
		tc_local_cluster_count_minus_1 = 128-1;
		tc_log2_local_cluster_count = 7;
	}

	if(gdw_DOS_Fat2Addr!=gdw_DOS_Fat1Addr) tdw_DOS_SectorPerFAT=gdw_DOS_SectorPerFAT; //Ching 081111

	tdw_FAT_sect_address = gdw_DOS_Fat1Addr + (tdw_FreeClusterNum>>tc_log2_local_cluster_count); // tdw_FreeClusterNum must be a multiple of 256/128.
	tdw_last_FAT_sector = gdw_DOS_Fat1Addr + ((gdw_DOS_FatMaxCluster+tc_local_cluster_count_minus_1)>>tc_log2_local_cluster_count) - 1;
	tdw_end = gdw_DOS_FatMaxCluster-((WORD)tc_local_cluster_count_minus_1+1);
	while (tdw_FreeClusterNum < gdw_DOS_FatMaxCluster)
	{
		tc_status = DOS_Read_LogicSector(tdw_FAT_sect_address, 1);
		if(tc_status) DOS_Read_LogicSector(tdw_FAT_sect_address+tdw_DOS_SectorPerFAT, 1); //Ching 081111

		tc_temp  = ((UBYTE)tdw_FreeClusterNum) & tc_local_cluster_count_minus_1; // get the index of cluster entry to search yet.

		if (tc_temp != 0) // If not start from the beginning of sector buffer.
		{ // to deal with the case that doesn't search from gc_UserDataBuf[0]
			if (tc_is_fat16)
				p_buf = gc_UserDataBuf + (tc_temp*2);
			else
				p_buf = gc_UserDataBuf + (tc_temp*4);


			if (tdw_FAT_sect_address == tdw_last_FAT_sector)
				tw_size = (WORD)(gdw_DOS_FatMaxCluster - tdw_FreeClusterNum);
			else
				tw_size = (tc_local_cluster_count_minus_1+1)-tc_temp;
		}
		else
		{
			p_buf = gc_UserDataBuf;
			if (tdw_FAT_sect_address == tdw_last_FAT_sector)//(JC)tdw_FreeClusterNum is in the last FAT sector, as the same sector with gdw_DOS_FatMaxCluster
					tw_size = (WORD)(gdw_DOS_FatMaxCluster - tdw_FreeClusterNum);
			else
					tw_size = (tc_local_cluster_count_minus_1+1);
		}
		if (tw_size > 128)
		{
			tc_loop1_counter = 128;
			tc_loop2_counter = tw_size - 128;
		}
		else
		{
			tc_loop1_counter = (UBYTE)tw_size;
			tc_loop2_counter = 0;
		}

		if (tc_SearchMode == 0) // search free clusters
		{
			if (tc_is_fat16) // FAT16
			{
				if (free_clusters_in_sector(p_buf, tc_loop1_counter, 2, tdw_FreeClusterNum))
				{
					return 0;
				}
				tdw_FreeClusterNum += tc_loop1_counter;
				if (free_clusters_in_sector(p_buf+tc_loop1_counter*2, tc_loop2_counter, 2, tdw_FreeClusterNum))
				{
					return 0;
				}
				tdw_FreeClusterNum += tc_loop2_counter;
			}
			else // FAT32
			{
				if (free_clusters_in_sector(p_buf, tc_loop1_counter, 4, tdw_FreeClusterNum))
				{
					return 0;
				}
				tdw_FreeClusterNum += tc_loop1_counter;
			}
		}
		else // count free clusters
		{
			tdw_FreeClusterNum += tw_size;
			if (tc_is_fat16) // FAT16
			{
				count_free_cluster_fat16(p_buf, tc_loop1_counter, tc_loop2_counter);
			}
			else
			{
				gdw_TotalFreeClusNumber += count_free_cluster_in_fat32_sector(p_buf, tc_loop1_counter);
			}
		}
		if (((UBYTE)tdw_FAT_sect_address & 0x0f) == 0) // return after every 16 sector analysis
			break;
		tdw_FAT_sect_address++;
	}
	gdw_FreeClusterNum[tc_SearchMode] = tdw_FreeClusterNum-1;
	return 0;
}

UBYTE fat16_free_cluster_offset(UBYTE xdata * p, UBYTE size);
UBYTE fat32_free_cluster_offset(UBYTE xdata * p, UBYTE size);

//! \internal
/*! \internal
 *	\param	p					Buffer for FAT.
 *	\param	size				Should be equal to or less than 128.
 *	\param	cluster_entry_size 	2 for FAT16 and 4 for FAT32.
 *	\param	tdw_FreeClusterNum	The cluster entry number at the address p.
 *	\return	1 if cluster buffer full. 0 otherwise.
 */
// All size and index variables in this function are in unit of cluster_entry_size.
U8 free_clusters_in_sector(U8 xdata * p, U8 size, U8 cluster_entry_size, U32 tdw_FreeClusterNum)
{
	U8  search_index;
	search_index = 0;

	while(1)
	{
		if (cluster_entry_size == 2)
		{
			search_index += fat16_free_cluster_offset(p+search_index*cluster_entry_size, size-search_index);
		}
		else
		{
			search_index += fat32_free_cluster_offset(p+search_index*cluster_entry_size, size-search_index);
		}
		if (search_index < size)
		{
			UBYTE uTemp, i;
			//dbprintf("%lx %bx\n", tdw_FreeClusterNum, search_index);
			i = gc_ClusBufValidSize;
			uTemp = (gc_ClusBufValidPoint+i) & 0x07; // (gc_ClusBufValidPoint+gc_ClusBufValidSize)%8	
			gdw_ClusterBuffer[uTemp] = tdw_FreeClusterNum + search_index;
			gdw_FreeClusterNum[0]    = tdw_FreeClusterNum + search_index;
			i++;
			gc_ClusBufValidSize = i;
			if (i==8)
			{
				return 1;
			}
			search_index++;
			continue;
		}
		else
		{
			// end is reached. searched_size should equals to size
			return 0;
		}
	}//while(1)
	return 0;
}




void new_DOS_SearchFreeCluster_loop(void) 
{ 
	while(gdw_FreeClusterNum[1] < (gdw_DOS_FatMaxCluster - 1)) 
	{         
		DOS_SearchFreeCluster(1); 
	} 
}


U32 User_GetFreeMemorySize_REC()
{
    new_DOS_SearchFreeCluster_loop();
	return(gdw_TotalFreeClusNumber*gc_DOS_SectorPerCluster);
}


void Menu_GetDiskCap()
{
    U32 tdw_TotalFreeSectorNumber;
	U8  tc_clock_mode_backup;  //20090526 chiayen add

	tc_clock_mode_backup=gc_clock_mode;
	set_clock_mode(CLOCK_MODE_MJPEG);  //20090526 chiayen add

	if(gc_CurrentCard==2)
	{
		gdw_CARD_TotalSizeMB = (SD_Capacity.LW>>11);
		gdw_CARD_TotalSizeMB=gdw_CARD_TotalSizeMB-1-gdw_CARD_TotalSizeMB/512;		
	}
	else if(gc_CurrentCard==0)
	{
		// maxliao-20090602
		if(gbt_Support_Dummy_Capacity)
		{
			gdw_CARD_TotalSizeMB = Dummy_Capacity.LW >> 11;
		}
		else
		{
			gdw_CARD_TotalSizeMB = Capacity.LW >> 11;
		}
		gdw_CARD_TotalSizeMB=gdw_CARD_TotalSizeMB-1-gdw_CARD_TotalSizeMB/512;
	}
	else if(gc_CurrentCard==5)
	{
		gdw_CARD_TotalSizeMB=gdw_CARD_TotalSizeMB;	
	}
	
	tdw_TotalFreeSectorNumber = User_GetFreeMemorySize_REC();
	gdw_Dos_FreeMemorySizeMB = tdw_TotalFreeSectorNumber >> 11;
	set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
}


void SetupMenuProcess()
{
	bit	tc_Confirm=0;
	U8  tc_clock_mode_backup;  //20090526 chiayen add
	tc_clock_mode_backup=gc_clock_mode;

	gc_PhaseInx = 0;
	gc_MenuItem=0;
	//TFT_SetupMenu();
	Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,REFRESH_ALL);

#ifdef Contrast_Set
	while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_PlayMenu:
					gc_refresh_type = REFRESH_ALL;
					tc_Confirm=1;
	            break;

	            case C_PlayNext:
					gc_refresh_type = REFRESH_NEXT;
					if(gc_MenuItem==4)
					{
						gc_MenuItem=0;
					}
					else
					{
						gc_MenuItem++;
					}
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
	            break;

	            case C_PlayPrev:
					gc_refresh_type = REFRESH_PREV;
					if(gc_MenuItem==0)
					{
						gc_MenuItem=4;
					}
					else
					{
						gc_MenuItem--;
					}
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
	            break;

				case C_MainMenu: //Long key mode
					gc_Task_Next=C_Task_Menu;
				break;
	        }

	  		if(tc_Confirm==1) //enter child dir or confirm selected item
			{
				gc_PhaseInx=0;
				if(gc_MenuItem==0)  //language setting
				{
					LanguageMenuProcess();
					gc_MenuItem=0;
					tc_Confirm=0;
				}
				else if(gc_MenuItem==1)
				{	
					ContrastMenuProcess();
					gc_MenuItem=1;
					tc_Confirm=0;
				}
				else if(gc_MenuItem==2)  //memory info
				{
				//	TFT_MemoryInfo();	// Show Wait... 
				//	TFT_ShowMediaIcon_M();
					set_clock_mode(CLOCK_MODE_MJPEG);
					Menu_GetDiskCap();
					set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
					TFT_MemoryInfo();                 
					TFT_ShowMediaIcon_M();

					gc_ShowMenuTimer=150;
					while((gc_ShowMenuTimer!=0))
					{
						Polling_TaskEvents();
                        IR_Service_Process_Menu_1(); 
						if(gc_PhaseInx!=0)
						{
							break;
						}
					}
					gc_MenuItem=2;
					tc_Confirm=0;
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
					gc_ShowMenuTimer=0;
				}
				else if(gc_MenuItem==3)  //firmware version
				{
					//TFT_FWVERSION();
				    Menu_Disp_Item_Other(FWVerTitleType,0,REFRESH_ALL);
					gc_ShowMenuTimer=150;
					while((gc_ShowMenuTimer!=0))
					{
						Polling_TaskEvents();
                        IR_Service_Process_Menu_1(); 
						if(gc_PhaseInx!=0)
						{
							break;
						}
					}
					gc_MenuItem=3;
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
					tc_Confirm=0;
					gc_ShowMenuTimer=0;
				}
				else if(gc_MenuItem==4)  //EXIT
				{
					gc_Task_Next=C_Task_Menu;
				}
			}
	   	}
		gc_PhaseInx=0;
		gc_CurrentCard_backup=gc_CurrentCard;		
		Polling_TaskEvents();
        IR_Service_Process_Menu_1();

		Play_SourceDetect_Process_Mainmenu(); 

       	if(gc_Task_Current!=gc_Task_Next)
        {
			gc_Task_Current=gc_Task_Next;
            break;  
        }
	}
#else
	while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_PlayMenu:
					gc_refresh_type = REFRESH_ALL;
					tc_Confirm=1;
	            break;

	            case C_PlayNext:
					gc_refresh_type = REFRESH_NEXT;
					if(gc_MenuItem==3)
					{
						gc_MenuItem=0;
					}
					else
					{
						gc_MenuItem++;
					}
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
	            break;

	            case C_PlayPrev:
					gc_refresh_type = REFRESH_PREV;
					if(gc_MenuItem==0)
					{
						gc_MenuItem=3;
					}
					else
					{
						gc_MenuItem--;
					}
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
	            break;

				case C_MainMenu: //Long key mode
					gc_Task_Next=C_Task_Menu;
				break;
	        }

	  		if(tc_Confirm==1) //enter child dir or confirm selected item
			{
				gc_PhaseInx=0;
				if(gc_MenuItem==0)  //language setting
				{
					LanguageMenuProcess();
					gc_MenuItem=0;
					tc_Confirm=0;
				}
				else if(gc_MenuItem==1)  //memory info
				{
				//	TFT_MemoryInfo();	// Show Wait... 
				//	TFT_ShowMediaIcon_M();
					set_clock_mode(CLOCK_MODE_MJPEG);
					Menu_GetDiskCap();
					set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
					TFT_MemoryInfo();                 
					TFT_ShowMediaIcon_M();

					gc_ShowMenuTimer=150;
					while((gc_ShowMenuTimer!=0))
					{
						Polling_TaskEvents();
                        IR_Service_Process_Menu_1(); 
						if(gc_PhaseInx!=0)
						{
							break;
						}
					}
					gc_MenuItem=1;
					tc_Confirm=0;
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
					gc_ShowMenuTimer=0;
				}
				else if(gc_MenuItem==2)  //firmware version
				{
					//TFT_FWVERSION();
				    Menu_Disp_Item_Other(FWVerTitleType,0,REFRESH_ALL);
					gc_ShowMenuTimer=150;
					while((gc_ShowMenuTimer!=0))
					{
						Polling_TaskEvents();
                        IR_Service_Process_Menu_1(); 
						if(gc_PhaseInx!=0)
						{
							break;
						}
					}
					gc_MenuItem=2;
					//TFT_SetupMenu();
					Menu_Disp_Item_Other(SetupMenuTpye,gc_MenuItem,gc_refresh_type);
					tc_Confirm=0;
					gc_ShowMenuTimer=0;
				}
				else if(gc_MenuItem==3)  //EXIT
				{
					gc_Task_Next=C_Task_Menu;
				}
			}
	   	}
		gc_PhaseInx=0;
		gc_CurrentCard_backup=gc_CurrentCard;		
		Polling_TaskEvents();
        IR_Service_Process_Menu_1();

		Play_SourceDetect_Process_Mainmenu(); 

       	if(gc_Task_Current!=gc_Task_Next)
        {
			gc_Task_Current=gc_Task_Next;
            break;  
        }
	}	
#endif	
}


void Play_SDFlash_DosInit_Mainmenu()
{
	gb_FindFlag = 0;
	gc_PhaseInx=0;
	gw_init_needed=0xFFFF;
	if (DOS_Initialize())
	{
	}
	else
	{
		gc_ShowTimer=0;  //20090331
		gc_DispWallpaper=1;
		gc_TuneVolFreqStatus=0;
		gb_FindFlag = 0;
		gc_PhaseInx=0;
		gw_init_needed=0xFFFF;

		gc_TaskMode_BkUp=C_Task_Play;//(JC)I0304 for back to rite TASK from Dir-list
		gc_Task_Next=C_Task_Play;

		gs_System_State.c_FileHandle=0;
		gc_MenuItemCount=0;     
		gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
		DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
		gc_PhaseInx=0;
		gb_TFT_refresh=1;
		gc_DispWallpaper=0xFF;
	}
}


void Play_SourceDetect_Process_Mainmenu()
{
	gb_SD_Exist_pre=gb_SD_Exist;
	if(!SD_Detect)  //SD exist
    {
        gb_SD_Exist=1;
    }
	else
	{
		gb_SD_Exist=0;
		gb_SDNoFileflag=0;
	}
#if 1//(USB_HOST==1)
	gb_Host_Exist_pre=gb_Host_Exist;
	if(!Host_DetectDevice())
	{
		gb_Host_Exist=1;	//host_exist		
	}
	else
	{
		gb_Host_Exist=0;
		gc_HostNoFileflag=0;
	}
#endif

	if((gc_CurrentCard==0) && ((gb_Host_Exist_pre!=gb_Host_Exist)||(gb_SD_Exist_pre!=gb_SD_Exist)))
	{
		if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
		{
			gc_CurrentCard=5;
		}
		else if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
		{
			gc_CurrentCard=2;	
		}
		else
		{
			gc_CurrentCard=0;	
		}
	}
	else
	{
		if(gc_CurrentCard==2)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
				}
				else if(((gb_SD_Exist==0) || (gb_SDNoFileflag==1)) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
					if(gc_HostNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_SD_Exist==0) || (gb_SDNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //20090803 chiayen add for SD Dos initial fail
			{
				if((gb_Host_Exist==1) && (gc_HostNoFileflag==0))
				{
					gc_CurrentCard=5;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;				
			}
		}
		#if 1//(USB_HOST==1)
		if(gc_CurrentCard==5)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
				}
				else if(((gb_Host_Exist==0) || (gc_HostNoFileflag==1)) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
					if(gb_SDNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_Host_Exist==0) || (gc_HostNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //host dos initial fail  //20090803 chiayen add
			{
				if((gb_SD_Exist==1) && (gb_SDNoFileflag==0))
				{
					gc_CurrentCard=2;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;
			}
		}
		#endif
	}


    if(gc_CurrentCard_backup!=gc_CurrentCard)
    {
		gc_CurrentCard_backup=gc_CurrentCard;
		set_clock_mode(CLOCK_MODE_MP3); //20090803 chiayen add for media change

		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
            }
            else if((gb_Host_Exist==1) && (gc_HostNoFileflag==0)) //20090730 chiayen modify
            {
				DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
				if(!Host_Initial())
				{
					gc_CurrentCard = CURRENT_MEDIA_HOST;
					gb_FindFlag = 0;
            	}
	            else
	            {
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();				    
	            }
        	}
			else 
        	{
				DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit_Mainmenu();
        }
		else if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			DEVICE_REG[0x00]= 0x01;  //20090803 chiayen add			
			if(!Host_Initial())
			{
				gc_CurrentCard = CURRENT_MEDIA_HOST;
				gb_FindFlag = 0;
			}
			else if((gb_SD_Exist==1) && (gb_SDNoFileflag==0)) //20090730 chiayen add
			{
	            if(SD_Identification_Flow())
	            { 
	                gc_CardExist |=0x02;
					gc_CurrentCard=2;      
				}
				else
				{
					DEVICE_REG[0x00]= 0x01;
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();
				}
			}
			else 
        	{
				DEVICE_REG[0x00]= 0x01;
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit_Mainmenu();
		}
		else if(gc_CurrentCard==0)
        {
			DEVICE_REG[0x00]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Mainmenu();
		}

		#if 0
		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
				Play_SDFlash_DosInit_Mainmenu();      
            }
            else
            {
				XBYTE[0xB400]= 0x01;
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
				Play_SDFlash_DosInit_Mainmenu();					    
            }
        }
        else if(gc_CurrentCard==0)
        {
			XBYTE[0xB400]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Mainmenu();
        }
		else if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			if(!Host_Initial())
			{
				gc_CurrentCard = CURRENT_MEDIA_HOST;
				gb_FindFlag = 0;
			}
			else
			{
				XBYTE[0xB400]= 0x01;
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
				gc_CurrentCard=0;
			}
			Play_SDFlash_DosInit_Mainmenu();
		}
		#endif
    }
}


void MainMenuProcess()
{
	bit	tc_Confirm=0;
	ClearIRBuffer();
	gc_PhaseInx = 0;
	
	
	gc_MenuItem=gc_MenuItemCount;

	while(1)
    {
		if(gc_ShowMenuTimer==0)
		{
			gc_ShowMenuTimer=10;
			TFT_ShowMenu();
		}

		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_PlayMenu:
					tc_Confirm=1;
	            break;
	
	            case C_PlayNext:
					gc_ShowMenuTimer=0;
					if(gc_MenuItem==3)
					{
						gc_MenuItem=0;
					}
					else
					{
						gc_MenuItem++;
					}
	            break;
	
	            case C_PlayPrev:
					gc_ShowMenuTimer=0;
					if(gc_MenuItem==0)
					{
						gc_MenuItem=3;
					}
					else
					{
						gc_MenuItem--;
					}
	            break;	

	            case C_PowerOff:
					gc_Task_Next=C_Task_PowerOff;  					    	
	            break;	
	        }

	  		if(tc_Confirm==1) //enter child dir or confirm selected item
			{
				gc_MenuItemCount=gc_MenuItem;
				if(gc_MenuItem==0)
				{
					gc_ShowTimer=0;  //20090331
					gc_DispWallpaper=1;
					gc_TuneVolFreqStatus=0;
					gb_FindFlag = 0;
					gc_PhaseInx=0;
					gw_init_needed=0xFFFF;

					gc_TaskMode_BkUp=C_Task_Play;//(JC)I0304 for back to rite TASK from Dir-list
					gc_Task_Next=C_Task_Play;
				}
				else if(gc_MenuItem==1)
				{
						dbprintf("*C_Task_Mjpeg*\n");
						gb_FindFlag = 0;  //20090331
						gw_FileIndex[2]=0;
						gs_str_scroll_state.c_str_1st_char=0;
	 			    	gs_str_scroll_state.c_shift_in_1st_char=0;
						gw_init_needed=0xFFFF;
	
						gc_TaskMode_BkUp=C_Task_Mjpeg;//(JC)I0304 for back to rite TASK from Dir-list
						gc_Task_Next=C_Task_Mjpeg;
					}
				else if(gc_MenuItem==2)
				{
					gc_TaskMode_BkUp=C_Task_Jpeg;//(JC)I0304 for back to rite TASK from Dir-list					
					gw_init_needed=0xFFFF;
					gb_FindFlag = 0;
					gc_Task_Next=C_Task_Jpeg;
				}
				else if(gc_MenuItem==3)  //setting
				{
					gc_Task_Next=C_Task_Setting;
				}
			}
	   	}

		gc_PhaseInx=0;
		gc_CurrentCard_backup=gc_CurrentCard;		
		Polling_TaskEvents();
        IR_Service_Process_Menu_1(); 
		Play_SourceDetect_Process_Mainmenu();

       	if(gc_Task_Current!=gc_Task_Next)
        {
			gc_LCM_Media=0xff;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayBitRate=0xFFFF;
			gc_DisplayPlayMode=0xFF;
			gw_DispSongNum1=0xFF;
			gc_DispPlayStatus=0xFF;
			gw_DispTotalTime=0xFFFF;
			gc_DisplayVol=0xFF;
			gc_SelectVol=1;
			if((gc_TaskMode_BkUp==C_Task_Jpeg) ||(gc_TaskMode_BkUp==C_Task_Mjpeg))  //20090331
			{
				gc_PhaseInx=1;	
			}
			gc_Task_Current=gc_Task_Next;
            break;  
        }
	}	
}


void PowerOffProcess()
{
	U16 tw_temp;

	gc_PhaseInx=0;
	set_clock_mode(CLOCK_MODE_MP3);

	TFT_PowerOfflogo();
	LCD_BACKLIGHT_OFF;

	gc_VolumeMute=0;

	while(1)
	{
		if(gc_PhaseInx==C_PowerOff)		
		{		
			gc_ShowTimer=0;  //20090331			
			gc_TuneVolFreqStatus=0;			
			gb_FindFlag = 0;			
			gw_init_needed=0xFFFF;									
			gc_Task_Next=C_Task_Play;                                                                                                		
      	    gc_MenuItemCount=0; 			//add home
		}

		Polling_TaskEvents();

//=========================================================
//=========================================================

		if(gc_IRCmdStatus!=0)
		{
			tw_temp=(IR_REG[0x1b]<<8)+IR_REG[0x1a];
			
			if(tw_temp==IR_21_Key)
			{
				if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
				{
					if(IR_REG[0x1c] == 0x00)  //Power ON
					{
						if(gc_IRCmdStatus==1)
						gc_PhaseInx = C_PowerOff;
					}
				}
			}
			else if(tw_temp==IR_21_1_Key)
			{
				if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
				{
					if(IR_REG[0x1c] == 0x45)  //Power ON
					{
						if(gc_IRCmdStatus==1)
						gc_PhaseInx = C_PowerOff;
					}
				}				
			}
			gc_IRCmdStatus=0;
		}

//=========================================================

//========================================================

	
		if(gc_Task_Current!=gc_Task_Next)
		{
		gc_Vrefinit_Timer=60;
		gc_PhaseInx=0;
		gw_Disp_CurrentSecBak=0xFFFF;
		gw_Disp_CurrentSec=0;
		gc_Task_Current=gc_Task_Next;                         
		break;  
		}
	}

	TFT_PowerOnlogo();
	USER_DelayDTms(2500);
}


void IR_Service_Process_Menu_1(void)
{
    if(gc_IRCmdStatus==1)
    {
//		gc_PlayMenu_Timeout=160;
        ir_service_menu();
    }
}
