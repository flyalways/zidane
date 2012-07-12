#include "..\header\SPDA2K.h"
#include "..\header\variables.h"

U8 DOS_SearchFreeCluster(U8 tc_SearchMode)
{
	data	bit	tb_is_fat16;
	data	bit	tb_SectorEnd=0;
	data	U8	tc_loop1_counter;
	data	U8	tc_loop2_counter;
	data	U8	tc_temp;
	data	U8	tc_local_cluster_count_minus_1;
	data	U8	tc_log2_local_cluster_count;
	data	U16	tw_size;
	xdata	U32 tdw_end;
	xdata	U32 tdw_FreeClusterNum;
	xdata	U32 tdw_FAT_sect_address;
	xdata	U32	tdw_last_FAT_sector;
	xdata	U8	xdata	*p_buf;

	tb_is_fat16=1;
	if(gc_DOS_FileSystemType==2)
	{
		tb_is_fat16=0;
	}

	if((tc_SearchMode==0)&&gw_ClusBufValidSize>=512)
	{
		return 0;
	}

	if(gw_ClusBufValidSize<8&&gdw_FreeClusterNum[0]<(gdw_DOS_FatMaxCluster-1))
	{
		tc_SearchMode=0;
	}

	tdw_FreeClusterNum=gdw_FreeClusterNum[tc_SearchMode]+1;
	if(gc_DOS_FileSystemType==1)						// FAT16
	{
		tc_local_cluster_count_minus_1=256-1;
		tc_log2_local_cluster_count=8;
	}
	else
	{
		tc_local_cluster_count_minus_1=128-1;
		tc_log2_local_cluster_count=7;
	}

	tdw_FAT_sect_address=gdw_DOS_Fat1Addr+(tdw_FreeClusterNum>>tc_log2_local_cluster_count); // tdw_FreeClusterNum must be a multiple of 256/128.
	tdw_last_FAT_sector=gdw_DOS_Fat1Addr+((gdw_DOS_FatMaxCluster+tc_local_cluster_count_minus_1)>>tc_log2_local_cluster_count)-1;
	tdw_end=gdw_DOS_FatMaxCluster-((U16)tc_local_cluster_count_minus_1+1);

	gb_ReadWriteDataArea=1;
	while(tdw_FreeClusterNum<gdw_DOS_FatMaxCluster)
	{
		DOS_Read_LogicSector(tdw_FAT_sect_address);
		tc_temp=((U8)tdw_FreeClusterNum)&tc_local_cluster_count_minus_1; // get the index of cluster entry to search yet.

		if(tc_temp!=0) // If not start from the beginning of sector buffer.
		{ // to deal with the case that doesn't search from gc_UserDataBuf[0]
			if(tb_is_fat16)
			{
				p_buf=gc_UserDataBuf+(tc_temp*2);
			}
			else
			{
				p_buf=gc_UserDataBuf+(tc_temp*4);
			}

			if(tdw_FAT_sect_address==tdw_last_FAT_sector)
			{
				tw_size=(U16)(gdw_DOS_FatMaxCluster-tdw_FreeClusterNum);
			}
			else
			{
				tw_size=(tc_local_cluster_count_minus_1+1)-tc_temp;
			}
		}
		else
		{
			p_buf=gc_UserDataBuf;
			if(tdw_FAT_sect_address==tdw_last_FAT_sector)//(JC)tdw_FreeClusterNum is in the last FAT sector, as the same sector with gdw_DOS_FatMaxCluster
			{
				tw_size=(U16)(gdw_DOS_FatMaxCluster-tdw_FreeClusterNum);
			}
			else
			{
				tw_size=(tc_local_cluster_count_minus_1+1);
			}
		}

		if(tw_size>128)
		{
			tc_loop1_counter=128;
			tc_loop2_counter=tw_size-128;
		}
		else
		{
			tc_loop1_counter=(U8)tw_size;
			tc_loop2_counter=0;
		}

		if(tc_SearchMode==0)	// search free clusters
		{
			if(tb_is_fat16)	// FAT16
			{
				if(free_clusters_in_sector(p_buf,tc_loop1_counter,2,tdw_FreeClusterNum))
				{
					return 0;
				}
				tdw_FreeClusterNum+=tc_loop1_counter;
				if(free_clusters_in_sector(p_buf+tc_loop1_counter*2,tc_loop2_counter,2,tdw_FreeClusterNum))
				{
					return 0;
				}
				tdw_FreeClusterNum+=tc_loop2_counter;
			}
			else 	// FAT32
			{
				if(free_clusters_in_sector(p_buf,tc_loop1_counter,4,tdw_FreeClusterNum))
				{
					return 0;
				}
				tdw_FreeClusterNum+=tc_loop1_counter;
			}
		}
		else // count free clusters
		{
			tdw_FreeClusterNum+=tw_size;
			if(tb_is_fat16)
			{
				count_free_cluster_fat16(p_buf,tc_loop1_counter,tc_loop2_counter);
			}
			else
			{
				gdw_TotalFreeClusNumber+=count_free_cluster_in_fat32_sector(p_buf,tc_loop1_counter);
			}
		}

		if(((U8)tdw_FAT_sect_address&0x1F)==0&&gc_CurrentCard==2)	// return after every 32 sector analysis
		{
			break;
		}
		else if(((U8)tdw_FAT_sect_address&0x0F)==0&&gc_CurrentCard==2)
		{
			break;
		}
		tdw_FAT_sect_address++;
	}

	gdw_FreeClusterNum[tc_SearchMode]=tdw_FreeClusterNum-1;
	return 0;
}

U8 free_clusters_in_sector(U8 *p,U8 size,U8 cluster_entry_size,U32 tdw_FreeClusterNum)
{
	data	U16	i;
	data	U16	uTemp;
	data	U8  search_index;

	search_index=0;
	while(1)
	{
		if(cluster_entry_size==2)
		{
			search_index+=fat16_free_cluster_offset(p+search_index*2,size-search_index);
		}
		else
		{
			search_index+=fat32_free_cluster_offset(p+search_index*4,size-search_index);
		}

		if(search_index<size)
		{
			i=gw_ClusBufValidSize;
			uTemp=(gw_ClusBufValidPoint+i)&0x1FF;
			gdw_ClusterBuffer[uTemp]=tdw_FreeClusterNum+search_index;
			gdw_FreeClusterNum[0]=tdw_FreeClusterNum+search_index;
			i++;
			gw_ClusBufValidSize=i;
			if(i==512)
			{
				return 1;
			}
			search_index++;
			continue;
		}
		else
		{// end is reached. searched_size should equals to size
			return 0;
		}
	}
	return 0;
}

void count_free_cluster_fat16(U8 *p,U8 size1,U8 size2)
{
	gdw_TotalFreeClusNumber+=count_free_cluster_in_fat16_sector(p,size1);
	gdw_TotalFreeClusNumber+=count_free_cluster_in_fat16_sector(p+size1*2,size2);
}


