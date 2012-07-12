U8 ID3_V2_Get_Idf(U32 *tpdw_DATA_LogAddr);
void ID3_Read_File(U32 tdw_FilePoint, U32 *tpdw_FileCluster, U32 *tdw_DATA_LogAddr);


U16 MP3_Bitrate_Parser();
U16 MP3_Total_Time_Parser();
void MP3_Parser_Seek_Dataptr(tc_Handle, U32 tdw_DataPoint);
U8 Next_One(U8 tc_Handle, U16 *tpw_Buf_idx, U32 tdw_fileptr);
U16	MP3_VBR_Hdr_Parser(U16 tw_Frame_Size, U32 tdw_DataPoint);

U8 WMA_ASF_Parser();
void ASF_Get_GUID(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr, U8 tc_GUID[]);
U32 ASF_Get_Obj_Size(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr);
void ASF_Flush_Byte(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr, U32 tdw_flush_byte);
U32 ASF_Get_DWord(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr);


#define ID3V2_TITLE				0x54495432
#define ID3V2_ARTIST			0x54504531
#define ID3_V2_START_PTR		0
#define ID3_V2_TAG_FOUND		1
#define ID3_V2_TAG_NOT_FOUND	0
#define ID3_V1_TAG_FOUND		1
#define ID3_V1_TAG_NOT_FOUND	0
#define ID3_FRAME_ERROR			0
#define	HDR_OBJ_GUID			0
#define	FILE_PROPS_OBJ_GUID		1
#define	STRM_PROPS_OBJ_GUID		2
#define AUDIO_MEDIA_OBJ_GUID	3
#define ASF_FORMAT_ERROR		1
#define ASF_PARSE_DONE			0	//Jimi 081112
#define	APE_PARSE_DONE			0
#define APE_PARSE_FAIL			1	//Jimi 081222
#define	MP3_HDR_PARSING_DONE	0
#define	Music_GET_FILE_LONG_NAME	0xFF
#define COMPRESSION_LEVEL_EXTRA_HIGH	4000                    //Jimi 081222 for APE

