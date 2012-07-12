/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	fs_struct.h
 *	\brief	FAT related structre declarations.
 *
 *  
 *	\author jason, trista
 */

#ifndef __FS_DTRUCT_H__
#define __FS_DTRUCT_H__

//!	FCB for files
/*!	FCB(File Control Block) is the data structure for file opening and access.
 *	It is not the same as the FCB that MS-DOS uses.
 */
typedef struct
{
	U32 dw_File_StartCluster;	//!< The start cluster number which the file begins.
	U32 dw_File_TotalSize;		//!< Size of the file in bytes.
	U32 dw_File_CurrentCluster;	//!< Current cluster number being accessed.
	U32 dw_File_DataPoint;		//!< File read/write position in bytes.
	U32 dw_FDB_StartCluster;	//!< ???
	U32 dw_LongFDB_LogAdd1;		//!< ???
	U32 dw_LongFDB_LogAdd0;		//!< ???
	U32 dw_FDB_Cluster;			//!< ???
	U32 dw_FDB_LogAdd;			//!< ???
	U16 w_FDB_Offset;			//!< ???
	U8	c_FileType;				//!< The file type determined by file name extensions. There are types of MP3(0), WMA(1), WMV(2) and WAV(3).
}File_FCB;

//!	FCB for directories
/*!	FCB(File Control Block) is the data structure for directory access.
 *	It is not the same as the FCB that MS-DOS uses.
 */
typedef struct
{
	U32 dw_File_StartCluster;	//!< The start cluster number which the directory begins.
	U32 dw_FDB_StartCluster;	//!< ???
	U32 dw_LongFDB_LogAdd1;		//!< ???
	U32 dw_LongFDB_LogAdd0;		//!< ???
	U32 dw_FDB_Cluster;			//!< ???
	U32 dw_FDB_LogAdd;			//!< ???
	U16 w_FDB_Offset;			//!< ???
}DIR_FCB;

//!	Date structure for FDB search functions.
typedef  struct
{
	U8  c_Search_Mode;
	U8  c_Search_Direction;
	U8  c_Search_Attribute;
	U8  c_EXTSelect;
	U32 dw_File_StartCluster;
	U32 dw_FDB_StartCluster;
	U32 dw_LongFDB_LogAdd1;
	U32 dw_LongFDB_LogAdd0;
	U32 dw_FDB_Cluster;  
	U32 dw_FDB_LogAdd;   
	U16 w_FDB_Offset;  
  //U8 c_File_EXTName[3];
	U16 w_FileTotalNum;
	U16 w_DirTotalNum;
	U8* pc_LongFileName;
	U8 	c_type;
	union
	{
		U32 dw_FileSize;
		U32 dw_StartCluster;
		//	U32 dw_FileNameIndex;
		U32 dw_BubbleFlag;
		//	U32 dw_CreateTime;
		U16 w_FileIndex;
	}Compare;	
}SearchFdb;

typedef struct dir_entry_s
{									// offset	size
	U8	c_occupied_flag;			// 0x00		 1
	U32	dw_File_StartCluster;		// 0x01		 4
	U32	dw_FDB_StartCluster;		// 0x05		 4
	U32	dw_LongFDB_LogAdd0;			// 0x09		 4
	U32	dw_FDB_Cluster;				// 0x0d		 4
	U32	dw_FDB_LogAdd;				// 0x11		 4
	U16	w_FDB_Offset;				// 0x15		 2
	U8	c_index_parent_folder;		// 0x17		 1
	U8	c_index_child_folder;		// 0x18		 1
	U8	c_index_prev_folder;		// 0x19		 1
	U8	c_index_next_folder;		// 0x1a		 1
	U8	c_file_num_hi;				// 0x1b 	 1
	U16	w_file_folder_num;			// 0x1c 	 2
	U8	c_file_num_lo;				// 0x1e 	 1
	U8	c_subdir_num;				// 0x1f 	 1
} dir_entry_t;

#define gs_DIRtable	((dir_entry_t xdata *)gc_DIRtable)	// victor : gs_DIRtable is a 64-entry array (2kB) for dir table operations.

#endif //!__FS_DTRUCT_H__

