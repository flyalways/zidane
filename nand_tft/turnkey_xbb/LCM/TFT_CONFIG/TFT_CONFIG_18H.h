

#define	LCMStrobe	0x33
////////////////////////////////////////////////////////////////////////////////////////////

//1.8�������ʾUI������ͼ���ID����ʾ��λ�ã���С

//˵�������е�������ʾ���������������ֳ��⣩����������ѡ�е���ɫ��δѡ�е���ɫ����Щ���ֵı��������ͼƬ�ķ�ʽ�������ӡ�
//�ʣ���ĿǰͼƬ��˵����Ҫ���ӵ����ı�����ɫ����Ҫ������ص�ͼƬ
//
//

#define	TFT_H_Size	160//����ˮƽ��С
#define	TFT_V_Size	128//���Ĵ�ֱ��С
				 
//DIR�������ʾ����������ַ�������//hao.yang 090812
#define C_LCM_ROW_MAX   5// //
#define C_BUF_LENGTH	85// //
#define C_BUF_STRING	80// //

//����Ϊ��ʾ�����͵ı�־
#define	TFT_HZKDispDir		0//��ʾ�ķ�ʽΪ��ʾ�ַ�
#define	TFT_IconDispDir		1//��ʾ��Ϊ�ֿ��е�ͼ��
#define	TFT_JPGDispDir		2//��ʾ��ΪJPEG
#define	TFT_BMPDispDir		3//��ʾ��ΪBMP



//---------------------Сͼ��ID����----------------------
#define	ID_SelectOrNot_1		1	//һ���˵�ѡ���ļ�ʱ�ļ�ǰ��Сͼ��
	#define	SelectOrNot1_H_Size		20
	#define	SelectOrNot1_V_Size		16
	#define ID_SelectOrNot_1_Y_Pos	10

#define	ID_SelectOrNot_2		2	//�����˵�ѡ���ļ�ʱ�ļ�ǰ��Сͼ��
	#define	SelectOrNot2_H_Size		20
	#define	SelectOrNot2_V_Size		16
    #define	Select_Offset		     1  //ѡ�����ͼ���Offset
    #define	SelectNot_Offset		 0  //δѡ�е�ͼ���Offset
	#define ID_SelectOrNot_2_Y_Pos	 25
	#define ID_SelectOrNot_BaseOffset	17

#define	ID_MainMenu	        3	//�����涯��
	#define	MainMenu_H_Size	   160
	#define	MainMenu_V_Size	   104
	#define	MainMenu_X_Pos	    0
	#define	MainMenu_Y_Pos	    24
	#define	NumOfGroup			4//ÿ���������ĸ�ͼƬ
#define	ID_MainMenu_Title	    4	// ������ı���(Title)
	#define	MainMenu_Title_H_Size	160
	#define	MainMenu_Title_V_Size	24
	#define	MainMenu_Title_X_Pos	0
	#define	MainMenu_Title_Y_Pos	0
#define	SongIndexID		   5	//��������
	#define	SongIndex_H_Size	5
	#define	SongIndex_V_Size	5
	#define	SongIndex_X_Pos		7
	#define	SongIndex_Y_Pos		43
#define	TotalSongID		   5	// ��������
	#define	TotalSong_H_Size	5
	#define	TotalSong_V_Size	5
	#define	TotalSong_X_Pos		35
	#define	TotalSong_Y_Pos		43
#define	SongTotalTimeID	6	// ��ǰ������ʱ��
	#define	SongTotalTime_H_Size	5
	#define	SongTotalTime_V_Size	5
	#define	SongTotalTime_X_Pos	13
	#define	SongTotalTime_Y_Pos	102
	#define	SongTotalTimeOffset	5
#define	PlayTimeID		    7	//�������ŵ�ʱ�����
	#define	PlayTime_H_Size		11
	#define	PlayTime_V_Size		17
	#define	PlayTime_X_Pos		5
	#define	PlayTime_Y_Pos		50	
	#define	SecondOffset		4	// ":"�Ŀ��
#define	BitRateID	      	8	// ������BitRate	
	#define	BitRate_H_Size		5
	#define	BitRate_V_Size		5
	#define	BitRate_X_Pos		108
	#define	BitRate_Y_Pos		107 
    #define BitRate_gap         0//������ʾ����֮��ļ��
#define	FMAdjID			  9	  // FM Modeѡ��Ƶ��ʱ��������ʾ
	#define	FMAdj_H_Size		11
	#define	FMAdj_V_Size		17
	#define	FMAdj_X_Pos			54
	#define	FMAdj_Y_Pos			42
	#define	FreqAdjDotOffset	9	// "." width
#define	FM_FreqID		10	//���Ž���FM��������ʾ
	#define	FM_Freq_H_Size		8
	#define	FM_Freq_V_Size		12
	#define	FM_Freq_X_Pos		28
	#define	FM_Freq_Y_Pos		3
	#define	FreqDotOffset		0	// "." width
 #define	MemInfoNumID	    11     //�洢������Ϣ 
 

#define	IRNumID			12       //ң����������ʾ����
	#define	IRNum_H_Size		11
	#define	IRNum_V_Size		17
	#define	IRNum_X_Pos			54
	#define	IRNum_Y_Pos			54
#define	VelocityID		13	// JPG�Զ����ŵ��ٶ�����
	#define	Velocity_H_Size		11
	#define	Velocity_V_Size		17
	#define	Velocity_X_Pos		70//98
	#define	Velocity_Y_Pos		80//35

#define	PlayIconID		14	//����״̬����ʾ Play/Pause/Stop
    #define	PlayIcon_H_Size 	19
	#define	PlayIcon_V_Size 	20
	#define	PlayIcon_X_Pos		8
	#define	PlayIcon_Y_Pos		107
#define	PlayModeIconID	15	// ѭ��ģʽ��NORMAL/Repeat-1/Repeat-All
	#define	PlayModeIcon_H_Size	14
	#define	PlayModeIcon_V_Size	13
	#define	PlayModeIcon_X_Pos	67
	#define	PlayModeIcon_Y_Pos	2    
#define PlayRatioBarID	16	//���Ž�����ʾ
	#define	PlayRatioBarNum		38
	#define	PlayRatioBar_H_Size	74
	#define	PlayRatioBar_V_Size	6
	#define	PlayRatioBar_X_Pos	73
	#define	PlayRatioBar_Y_Pos	117
#define	EQIconID		17	// EQͼ��?Normal/DBB/JAZZ...
	#define	EQIcon_H_Size		17
	#define	EQIcon_V_Size		12
	#define	EQIcon_X_Pos		83
	#define	EQIcon_Y_Pos		3 
#define	EQFrameID		18    //ȫ����ʾEQ֡
	#define	EQFrame_H_Size		160
	#define	EQFrame_V_Size		29
	#define	EQFrame_X_Pos		0
	#define	EQFrame_Y_Pos		51    
#define	FileIconID		19	//�ļ�Сͼ�� music/Photo/Mjpg
	#define	FileIcon_H_Size		16
	#define	FileIcon_V_Size		16
	#define	FileIcon_X_Pos		2
	#define	FileIcon_Y_Pos		27 
#define MoveiICONID      23
#define	SongTime				19
#define	MediaIconID		21	// Media����:NAND/USB/SD
	#define MediaIcon_H_Size	19 //������ʱ��ͼ���λ��
	#define	MediaIcon_V_Size	20
	#define	MediaIcon_X_Pos		30
	#define	MediaIcon_Y_Pos		107   

	#define MediaIcon_Mem_H_Size	30  //�ڴ洢��Ϣ��ѯʱ��ͼ���λ��
	#define	MediaIcon_Mem_V_Size	15
	#define	MediaIcon_Mem_X_Pos		127
	#define	MediaIcon_Mem_Y_Pos		5   
#define MusicTypeID	         22	//�����ļ�����:MP3/WMA/APE/MP4/JPG
	#define	MusicType_H_Size	19
	#define	MusicType_V_Size	16
	#define	MusicType_X_Pos		102
	#define	MusicType_Y_Pos		0
/*
#define	VOLAdj1ID		     23	 
	#define	VOLAdj1_H_Size		10
	#define	VOLAdj1_V_Size		18
	#define	VOLAdj1_X_Pos		38
	#define	VOLAdj1_Y_Pos		50
	#define	VOLstage			2	
 #define	VOLAdjID		24	//��������ͼ��
	#define	VOLAdj_H_Size		115
	#define	VOLAdj_V_Size		18
	#define	VOLAdj_X_Pos		33
	#define	VOLAdj_Y_Pos		50
*/
#define	VOLAdjID	9	// ���q�վ�(NUM)
	#define	VOLAdj_H_Size		11
	#define	VOLAdj_V_Size		17
	#define	VOLAdj_X_Pos		68
	#define	VOLAdj_Y_Pos		42
#define	PauseID		       25	// MPJG pause
	#define	Pause_H_Size		45
	#define	Pause_V_Size		13
	#define	Pause_X_Pos			57
	#define	Pause_Y_Pos			57 
#define	ErrorID			  26      //����ͼ��
	#define	Error_H_Size		53
	#define	Error_V_Size		45
	#define	Error_X_Pos			0
	#define	Error_Y_Pos			32
/////////////////////////////////////////////
#define	ContrastNumID	  13  //�Աȶ����õ�����ͼƬ

#define Contrast_H_Size   11
#define Contrast_V_Size   17
#define Contrast_X_Pos    57
#define Contrast_Y_Pos    57
    

  #define	MemInfoNumID	    11     //�洢������Ϣ 
#define	MemInfoNUM_H_Size	7
#define	MemInfoNUM_V_Size	10
#define	MemInfoNUM_X_Pos    77
#define	MemInfoNUM_Y_Pos    52
#define MenInfoNUM_BaseOffset 17
///////////////////   


//---------------ȫ��ͼƬ��ID��һ�㲻��Ҫ��������ʾλ��---------
//ͼƬID�б�����ID����ʾ��Ϣ
//ID = 0Ϊ���ò��ֵ�ȫ����ͼƬ
#define	ID_PUBLIC_FULL	              0//ȫ����ʾ���õ�ID
      #define OFFSET_USB              0

      #define OFFSET_MENU_1_BG        1 //һ���˵��ı��� 
      #define OFFSET_MENU_2_BG        2 //�����˵��ı��� 
      #define OFFSET_FMSET_BG         3
      #define OFFSET_VOLSET_BG        4      
      #define OFFSET_EQ_FRAME         5 //ȫ����ʾEQ�ı���  
      #define OFFSET_FullLRC_BG       6
      #define OFFSET_FWVer_BG         7 //FW �汾�ı���
      #define OFFSET_MemoryInfo_BG    8 //�洢��Ϣ�ı���
	  #define OFFSET_Floder_BG    9 	//���ļ��� 

#define	FMORSONG_BG	31  //FM���߸�������ѡ��ı���
	#define	FMORSONG_BG_H_Size	TFT_H_Size
	#define	FMORSONG_BG_V_Size	TFT_V_Size
	#define	FMORSONG_BG_X_Pos	0
	#define	FMORSONG_BG_Y_Pos	0   

#define ID_PlayBG       32  //Play�ı���ͼƬ
	#define	ID_PlayBG_H_Size	TFT_H_Size
	#define	ID_PlayBG_V_Size	TFT_V_Size
	#define	ID_PlayBG_X_Pos		0
	#define	ID_PlayBG_Y_Pos		0	

#define	PowerONLogoID	33	// ����Logo��ID
	#define	PowerONLogo_H_Size		TFT_H_Size
	#define	PowerONLogo_V_Size		TFT_V_Size
	#define	PowerONLogo_X_Pos		0
	#define	PowerONLogo_Y_Pos		0	
	#define	PowerONLogoPicNum	16//����Logo��ͼƬ����
	#define	PowerONLogoTimer	30//ÿ��ͼƬ��ʾ��ʱ��
#define	PowerOFFLogoID	34	// �ػ�Logo��ID
	#define	PowerOFFLogo_H_Size		TFT_H_Size
	#define	PowerOFFLogo_V_Size		TFT_V_Size
	#define	PowerOFFLogo_X_Pos		0
	#define	PowerOFFLogo_Y_Pos		0	
	#define	PowerOFFLogoPicNum	15//����Logo��ͼƬ����
	#define	PowerOFFLogoTimer	50//ÿ��ͼƬ��ʾ��ʱ��
//================================================================
#define ID_FM_ICON					35 //����״̬�µ�FMͼ��
	#define	ID_FM_ICON_H_Size		25
	#define	ID_FM_ICON_V_Size		12
	#define	ID_FM_ICON_X_Size		2
	#define	ID_FM_ICON_Y_Size		2
#define ID_VOL_ICON					36 //����״̬�µ�VOLͼ��
	#define	ID_VOL_ICON_H_Size		16
	#define	ID_VOL_ICON_V_Size		15
	#define	ID_VOL_ICON_X_Size		122
	#define	ID_VOL_ICON_Y_Size		0
#define ID_PLAYTIME_DOT					37	//��ǰ����ʱ���м������
	#define	ID_PLAYTIME_DOT_H_Size		11
	#define	ID_PLAYTIME_DOT_V_Size		17
	#define	ID_PLAYTIME_DOT_X_Size		33
	#define	ID_PLAYTIME_DOT_Y_Size		50
#define ID_TOTALTIME_DOT					38//��ʱ���м������
	#define	ID_TOTALTIME_DOT_H_Size		5
	#define	ID_TOTALTIME_DOT_V_Size		5
	#define	ID_TOTALTIME_DOT_X_Size		24
	#define	ID_TOTALTIME_DOT_Y_Size		102
#define ID_FM_DOT					39//����״̬��fm�ĵ�
	#define	ID_FM_DOT_H_Size		7
	#define	ID_FM_DOT_V_Size		12
	#define	ID_FM_DOT_X_Size		51
	#define	ID_FM_DOT_Y_Size		3
#define ID_TOTALNUM_XIEGANG					40//�ܸ�������б��
	#define	ID_XIEGANG_H_Size		5
	#define	ID_XIEGANG_V_Size		5
	#define	ID_XIEGANG_X_Size		30
	#define	ID_XIEGANG_Y_Size		43
#define ID_PingPu							41		//����״̬��Ƶ��
	#define	ID_PINGPU_H_Size		85
	#define	ID_PINGPU_V_Size		36
	#define	ID_PINGPU_X_Size		75
	#define	ID_PINGPU_Y_Size		30
#define ID_KBPS								42
	#define	ID_KBPS_H_Size		19
	#define	ID_KBPS_V_Size		6
	#define	ID_KBPS_X_Size		125
	#define	ID_KBPS_Y_Size		106
#define ID_LYRIC							43
	#define	ID_LYRIC_H_Size		24
	#define	ID_LYRIC_V_Size		12
	#define	ID_LYRIC_X_Size		72
	#define	ID_LYRIC_Y_Size		102
#define ID_VOLNUM							44
	#define	ID_VOLNUM_H_Size		8
	#define	ID_VOLNUM_V_Size		8
	#define	ID_VOLNUM_X_Size		140
	#define	ID_VOLNUM_Y_Size		4
	#define ID_VOLNUM_SPACE			1	

//----------------------------����������ص���ʾ����-----------------------
//�ļ�����ʾ�����Ϣ��������ɫ��������ɫ����ʼλ�ã�����
#define WordColor_FileName	    0xFFFF
#define WordColorBG_FileName	0x0000//0xBF7F //�ļ����ı�����ɫ
#define Start_X_FileName	    0//63
#define Start_Y_FileName 	    70//71//
#define FileName_Width	  	    160//90//�ļ���ʾ�Ŀ��

//add folder ���ļ������Ƶ���ʾλ�ü���ʾ��ɫ�ͱ�����ɫ
#define WordColor_FolderName	    0
#define WordColorBG_FolderName	0xA714 //�ļ������ı�����ɫ
#define Start_X_FolderName	    4
#define Start_Y_FolderName 	    8
#define FolderName_Width	  	    155//�ļ���ʾ�Ŀ��

//LRC�ַ���ʾ����ɫ(������ɫ��ͼƬ����),��ʼ��ַ����ʾ���ܿ��
#define	WordColor_LRC_Select		0x0000//ѡ�и�ʵ���ɫ	
#define	WordColor_LRC_Select_BG     0xAF1F//
#define WordColor_LRC_NOSelect		0xFFFF//δѡ�и�ʵ���ɫ
#define	WordColor_LRC_NOSelect_BG   0xFBDE

//һ���˵������ֵ�������ɫ��ѡ�к����ɫ
//#define WordColor_MenuItem1_Select  	 0x6014
#define WordColor_MenuItem1_Select		 0xC007
#define WordColor_MenuItem1_NOSelect	 0x027F
#define WordColor_MenuItem1_BG	         0xD71E
//�����˵���������ɫ��������ɫ
#define WordColor_MenuItem2_Title	     0x0538   //�����˵��˵�������ɫ
#define WordColor_MenuItem2_Title_BG	 0xD71E   //�����˵��˵���BG����ɫ
//#define WordColor_MenuItem2_Select	     0x6014
#define WordColor_MenuItem2_Select	     0xC007
#define WordColor_MenuItem2_NOSelect	 0x027F 
#define WordColor_MenuItem2_BG	         0x7E5F

//�ļ��������ʾ��������ɫ������ı�����ɫ
#define WordColor_DIR_Select	       0x0000//DIR��ѡ���������ɫ
#define WordColor_DIRBG_Select	       0xFFFF//DIR��ѡ������ı���
#define WordColor_DIR_NOSelect	       0xFFFF//DIR��δѡ���������ɫ   
#define WordColor_DIRBG_NOSelect	   0x7E5F//DIR��δѡ������ı���
#define	Start_X_DIR		20
#define	Start_Y_DIR		27
#define DIR_Length	(TFT_H_Size-Start_X_DIR-5)//���ұ߾�5�����ص���ʾ�ĳ���
#define DIR_High        20

//�ļ�����������ļ���֮��ļ������ͼ��任���仯
// DIR-filename display parameters


//ȫ����ʵ��������
#define FullLRC_Word1Color				0x0000//��һ���ʵ���ɫ
#define FullLRC_Word1Color_BG			0xAF1F//��һ���ʵı���
#define FullLRC_Start1_X				8//��һ���ʵ���ʼλ��x
#define FullLRC_Start1_Y				4//��һ���ʵ���ʼλ��x
#define FullLRC_Width1					144//��һ���ʵ���ʾ���
#define FullLRC_High1					20//��һ���ʵ���ʾ�߶�	

#define FullLRC_Word2Color				0xFFFF//�ڶ����ʵ���ɫ
#define FullLRC_Word2Color_BG			0xFBDE//�ڶ����ʵı���
#define FullLRC_Start2_X				8//�ڶ����ʵ���ʼλ��x
#define FullLRC_Start2_Y				68//�ڶ����ʵ���ʼλ��x
#define FullLRC_Width2					144//�ڶ����ʵ���ʾ���
#define FullLRC_High2					20//�ڶ����ʵ���ʾ�߶�

#define Voice_Item_WordColor            0x0000//��������������ʾ������ɫ
#define Voice_Item_WordColor_BG			0xA713//���ⱳ����ɫ
#define Voice_Item_X			     	4//������ʾ��X��ʼ
#define Voice_Item_Y				    8//������ʾ��Y��ʼ

#define FM_ModeItem_WordColor            0x0000//��������������ʾ������ɫ
#define FM_ModeItem_WordColor_BG			0xA714//���ⱳ����ɫ
#define FM_ModeItem_X			     	4//������ʾ��X��ʼ
#define FM_ModeItem_Y				    8//������ʾ��Y��ʼ

//#define	FW_version	101	
//#define TFT_Disp_Type    1018    //1:���� 0����һ��UIͼƬ 18��1.8���� 
//#define	FW_Date		20090623  
#define FW_Ver_Start_X                  50
#define FW_Ver_Start_Y                  50
#define FW_Data_Start_X              50
#define FW_Data_Start_Y              80
//-------add for refresh type 0705
#define REFRESH_ALL 0x00//ȫ��ˢ�£���������������ʾ�����в˵�
#define REFRESH_NEXT 0x01//ˢ��next������ˢ�µ�ǰѡ����Ŀ����ǰ��Ŀ��ǰһ�ֻˢ���������ˢ�±���������δ��������Ŀ
#define REFRESH_PREV 0x02
#define REFRESH_NOT  0xff	//��ˢ��---not use

