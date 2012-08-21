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
	U16 w_FileTotalNum;
	U16 w_DirTotalNum;
	U8* pc_LongFileName;
	U8 	c_type;
	union
	{
		U32 dw_FileSize;
		U32 dw_StartCluster;
		U32 dw_BubbleFlag;
		U16 w_FileIndex;
	}Compare;	
}SearchFdb;


typedef struct
{
	U8	c_Task;
	U8	c_Phase;
	U8	c_FileHandle;
	U16	w_SampleRate;
}System_Struct;
#endif //!__FS_DTRUCT_H__

