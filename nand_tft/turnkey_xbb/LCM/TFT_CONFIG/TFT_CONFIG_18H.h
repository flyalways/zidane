

#define	LCMStrobe	0x33
////////////////////////////////////////////////////////////////////////////////////////////

//1.8寸横屏显示UI，各个图标的ID及显示的位置，大小

//说明：所有的文字显示（特殊字体与数字除外），均可设置选中的颜色和未选中的颜色，这些文字的背景请采用图片的方式进行增加。
//故，就目前图片来说，需要增加单独的背景颜色，需要增加相关的图片
//
//

#define	TFT_H_Size	160//屏的水平大小
#define	TFT_V_Size	128//屏的垂直大小
				 
//DIR中最大显示行数与最大字符串长度//hao.yang 090812
#define C_LCM_ROW_MAX   5// //
#define C_BUF_LENGTH	85// //
#define C_BUF_STRING	80// //

//以下为显示的类型的标志
#define	TFT_HZKDispDir		0//显示的方式为显示字符
#define	TFT_IconDispDir		1//显示的为字库中的图标
#define	TFT_JPGDispDir		2//显示的为JPEG
#define	TFT_BMPDispDir		3//显示的为BMP



//---------------------小图标ID部分----------------------
#define	ID_SelectOrNot_1		1	//一级菜单选中文件时文件前的小图标
	#define	SelectOrNot1_H_Size		20
	#define	SelectOrNot1_V_Size		16
	#define ID_SelectOrNot_1_Y_Pos	10

#define	ID_SelectOrNot_2		2	//二级菜单选中文件时文件前的小图标
	#define	SelectOrNot2_H_Size		20
	#define	SelectOrNot2_V_Size		16
    #define	Select_Offset		     1  //选中项的图标的Offset
    #define	SelectNot_Offset		 0  //未选中的图标的Offset
	#define ID_SelectOrNot_2_Y_Pos	 25
	#define ID_SelectOrNot_BaseOffset	17

#define	ID_MainMenu	        3	//主界面动画
	#define	MainMenu_H_Size	   160
	#define	MainMenu_V_Size	   104
	#define	MainMenu_X_Pos	    0
	#define	MainMenu_Y_Pos	    24
	#define	NumOfGroup			4//每个动画有四副图片
#define	ID_MainMenu_Title	    4	// 主界面的标题(Title)
	#define	MainMenu_Title_H_Size	160
	#define	MainMenu_Title_V_Size	24
	#define	MainMenu_Title_X_Pos	0
	#define	MainMenu_Title_Y_Pos	0
#define	SongIndexID		   5	//歌曲索引
	#define	SongIndex_H_Size	5
	#define	SongIndex_V_Size	5
	#define	SongIndex_X_Pos		7
	#define	SongIndex_Y_Pos		43
#define	TotalSongID		   5	// 歌曲总数
	#define	TotalSong_H_Size	5
	#define	TotalSong_V_Size	5
	#define	TotalSong_X_Pos		35
	#define	TotalSong_Y_Pos		43
#define	SongTotalTimeID	6	// 当前歌曲总时间
	#define	SongTotalTime_H_Size	5
	#define	SongTotalTime_V_Size	5
	#define	SongTotalTime_X_Pos	13
	#define	SongTotalTime_Y_Pos	102
	#define	SongTotalTimeOffset	5
#define	PlayTimeID		    7	//歌曲播放的时间进度
	#define	PlayTime_H_Size		11
	#define	PlayTime_V_Size		17
	#define	PlayTime_X_Pos		5
	#define	PlayTime_Y_Pos		50	
	#define	SecondOffset		4	// ":"的宽度
#define	BitRateID	      	8	// 比特率BitRate	
	#define	BitRate_H_Size		5
	#define	BitRate_V_Size		5
	#define	BitRate_X_Pos		108
	#define	BitRate_Y_Pos		107 
    #define BitRate_gap         0//两个显示数字之间的间隔
#define	FMAdjID			  9	  // FM Mode选择频率时的数字显示
	#define	FMAdj_H_Size		11
	#define	FMAdj_V_Size		17
	#define	FMAdj_X_Pos			54
	#define	FMAdj_Y_Pos			42
	#define	FreqAdjDotOffset	9	// "." width
#define	FM_FreqID		10	//播放界面FM的数字显示
	#define	FM_Freq_H_Size		8
	#define	FM_Freq_V_Size		12
	#define	FM_Freq_X_Pos		28
	#define	FM_Freq_Y_Pos		3
	#define	FreqDotOffset		0	// "." width
 #define	MemInfoNumID	    11     //存储容量信息 
 

#define	IRNumID			12       //遥控器点歌的显示数字
	#define	IRNum_H_Size		11
	#define	IRNum_V_Size		17
	#define	IRNum_X_Pos			54
	#define	IRNum_Y_Pos			54
#define	VelocityID		13	// JPG自动播放的速度设置
	#define	Velocity_H_Size		11
	#define	Velocity_V_Size		17
	#define	Velocity_X_Pos		70//98
	#define	Velocity_Y_Pos		80//35

#define	PlayIconID		14	//播放状态的显示 Play/Pause/Stop
    #define	PlayIcon_H_Size 	19
	#define	PlayIcon_V_Size 	20
	#define	PlayIcon_X_Pos		8
	#define	PlayIcon_Y_Pos		107
#define	PlayModeIconID	15	// 循环模式：NORMAL/Repeat-1/Repeat-All
	#define	PlayModeIcon_H_Size	14
	#define	PlayModeIcon_V_Size	13
	#define	PlayModeIcon_X_Pos	67
	#define	PlayModeIcon_Y_Pos	2    
#define PlayRatioBarID	16	//播放进度显示
	#define	PlayRatioBarNum		38
	#define	PlayRatioBar_H_Size	74
	#define	PlayRatioBar_V_Size	6
	#define	PlayRatioBar_X_Pos	73
	#define	PlayRatioBar_Y_Pos	117
#define	EQIconID		17	// EQ图标?Normal/DBB/JAZZ...
	#define	EQIcon_H_Size		17
	#define	EQIcon_V_Size		12
	#define	EQIcon_X_Pos		83
	#define	EQIcon_Y_Pos		3 
#define	EQFrameID		18    //全屏显示EQ帧
	#define	EQFrame_H_Size		160
	#define	EQFrame_V_Size		29
	#define	EQFrame_X_Pos		0
	#define	EQFrame_Y_Pos		51    
#define	FileIconID		19	//文件小图标 music/Photo/Mjpg
	#define	FileIcon_H_Size		16
	#define	FileIcon_V_Size		16
	#define	FileIcon_X_Pos		2
	#define	FileIcon_Y_Pos		27 
#define MoveiICONID      23
#define	SongTime				19
#define	MediaIconID		21	// Media类型:NAND/USB/SD
	#define MediaIcon_H_Size	19 //在音乐时该图标的位置
	#define	MediaIcon_V_Size	20
	#define	MediaIcon_X_Pos		30
	#define	MediaIcon_Y_Pos		107   

	#define MediaIcon_Mem_H_Size	30  //在存储信息查询时该图标的位置
	#define	MediaIcon_Mem_V_Size	15
	#define	MediaIcon_Mem_X_Pos		127
	#define	MediaIcon_Mem_Y_Pos		5   
#define MusicTypeID	         22	//播放文件类型:MP3/WMA/APE/MP4/JPG
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
 #define	VOLAdjID		24	//音量调整图标
	#define	VOLAdj_H_Size		115
	#define	VOLAdj_V_Size		18
	#define	VOLAdj_X_Pos		33
	#define	VOLAdj_Y_Pos		50
*/
#define	VOLAdjID	9	// 秖秸俱(NUM)
	#define	VOLAdj_H_Size		11
	#define	VOLAdj_V_Size		17
	#define	VOLAdj_X_Pos		68
	#define	VOLAdj_Y_Pos		42
#define	PauseID		       25	// MPJG pause
	#define	Pause_H_Size		45
	#define	Pause_V_Size		13
	#define	Pause_X_Pos			57
	#define	Pause_Y_Pos			57 
#define	ErrorID			  26      //错误图标
	#define	Error_H_Size		53
	#define	Error_V_Size		45
	#define	Error_X_Pos			0
	#define	Error_Y_Pos			32
/////////////////////////////////////////////
#define	ContrastNumID	  13  //对比度设置的数字图片

#define Contrast_H_Size   11
#define Contrast_V_Size   17
#define Contrast_X_Pos    57
#define Contrast_Y_Pos    57
    

  #define	MemInfoNumID	    11     //存储容量信息 
#define	MemInfoNUM_H_Size	7
#define	MemInfoNUM_V_Size	10
#define	MemInfoNUM_X_Pos    77
#define	MemInfoNUM_Y_Pos    52
#define MenInfoNUM_BaseOffset 17
///////////////////   


//---------------全屏图片的ID，一般不需要调整其显示位置---------
//图片ID列表及各个ID的显示信息
//ID = 0为公用部分的全屏的图片
#define	ID_PUBLIC_FULL	              0//全屏显示公用的ID
      #define OFFSET_USB              0

      #define OFFSET_MENU_1_BG        1 //一级菜单的背景 
      #define OFFSET_MENU_2_BG        2 //二级菜单的背景 
      #define OFFSET_FMSET_BG         3
      #define OFFSET_VOLSET_BG        4      
      #define OFFSET_EQ_FRAME         5 //全屏显示EQ的背景  
      #define OFFSET_FullLRC_BG       6
      #define OFFSET_FWVer_BG         7 //FW 版本的背景
      #define OFFSET_MemoryInfo_BG    8 //存储信息的背景
	  #define OFFSET_Floder_BG    9 	//翻文件夹 

#define	FMORSONG_BG	31  //FM或者歌曲数字选择的背景
	#define	FMORSONG_BG_H_Size	TFT_H_Size
	#define	FMORSONG_BG_V_Size	TFT_V_Size
	#define	FMORSONG_BG_X_Pos	0
	#define	FMORSONG_BG_Y_Pos	0   

#define ID_PlayBG       32  //Play的背景图片
	#define	ID_PlayBG_H_Size	TFT_H_Size
	#define	ID_PlayBG_V_Size	TFT_V_Size
	#define	ID_PlayBG_X_Pos		0
	#define	ID_PlayBG_Y_Pos		0	

#define	PowerONLogoID	33	// 开机Logo的ID
	#define	PowerONLogo_H_Size		TFT_H_Size
	#define	PowerONLogo_V_Size		TFT_V_Size
	#define	PowerONLogo_X_Pos		0
	#define	PowerONLogo_Y_Pos		0	
	#define	PowerONLogoPicNum	16//开机Logo的图片总数
	#define	PowerONLogoTimer	30//每幅图片显示的时间
#define	PowerOFFLogoID	34	// 关机Logo的ID
	#define	PowerOFFLogo_H_Size		TFT_H_Size
	#define	PowerOFFLogo_V_Size		TFT_V_Size
	#define	PowerOFFLogo_X_Pos		0
	#define	PowerOFFLogo_Y_Pos		0	
	#define	PowerOFFLogoPicNum	15//开机Logo的图片总数
	#define	PowerOFFLogoTimer	50//每幅图片显示的时间
//================================================================
#define ID_FM_ICON					35 //播放状态下的FM图标
	#define	ID_FM_ICON_H_Size		25
	#define	ID_FM_ICON_V_Size		12
	#define	ID_FM_ICON_X_Size		2
	#define	ID_FM_ICON_Y_Size		2
#define ID_VOL_ICON					36 //播放状态下的VOL图标
	#define	ID_VOL_ICON_H_Size		16
	#define	ID_VOL_ICON_V_Size		15
	#define	ID_VOL_ICON_X_Size		122
	#define	ID_VOL_ICON_Y_Size		0
#define ID_PLAYTIME_DOT					37	//当前播放时间中间的两点
	#define	ID_PLAYTIME_DOT_H_Size		11
	#define	ID_PLAYTIME_DOT_V_Size		17
	#define	ID_PLAYTIME_DOT_X_Size		33
	#define	ID_PLAYTIME_DOT_Y_Size		50
#define ID_TOTALTIME_DOT					38//总时间中间的两点
	#define	ID_TOTALTIME_DOT_H_Size		5
	#define	ID_TOTALTIME_DOT_V_Size		5
	#define	ID_TOTALTIME_DOT_X_Size		24
	#define	ID_TOTALTIME_DOT_Y_Size		102
#define ID_FM_DOT					39//播放状态下fm的点
	#define	ID_FM_DOT_H_Size		7
	#define	ID_FM_DOT_V_Size		12
	#define	ID_FM_DOT_X_Size		51
	#define	ID_FM_DOT_Y_Size		3
#define ID_TOTALNUM_XIEGANG					40//总歌曲数的斜杠
	#define	ID_XIEGANG_H_Size		5
	#define	ID_XIEGANG_V_Size		5
	#define	ID_XIEGANG_X_Size		30
	#define	ID_XIEGANG_Y_Size		43
#define ID_PingPu							41		//播放状态下频谱
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

//----------------------------各处文字相关的显示调整-----------------------
//文件名显示相关信息：字体颜色，背景颜色，起始位置，长度
#define WordColor_FileName	    0xFFFF
#define WordColorBG_FileName	0x0000//0xBF7F //文件名的背景颜色
#define Start_X_FileName	    0//63
#define Start_Y_FileName 	    70//71//
#define FileName_Width	  	    160//90//文件显示的宽度

//add folder 后文件夹名称的显示位置及显示颜色和背景颜色
#define WordColor_FolderName	    0
#define WordColorBG_FolderName	0xA714 //文件夹名的背景颜色
#define Start_X_FolderName	    4
#define Start_Y_FolderName 	    8
#define FolderName_Width	  	    155//文件显示的宽度

//LRC字符显示的颜色(背景颜色由图片决定),起始地址及显示的总宽度
#define	WordColor_LRC_Select		0x0000//选中歌词的颜色	
#define	WordColor_LRC_Select_BG     0xAF1F//
#define WordColor_LRC_NOSelect		0xFFFF//未选中歌词的颜色
#define	WordColor_LRC_NOSelect_BG   0xFBDE

//一级菜单中文字的字体颜色及选中后的颜色
//#define WordColor_MenuItem1_Select  	 0x6014
#define WordColor_MenuItem1_Select		 0xC007
#define WordColor_MenuItem1_NOSelect	 0x027F
#define WordColor_MenuItem1_BG	         0xD71E
//二级菜单的文字颜色及背景颜色
#define WordColor_MenuItem2_Title	     0x0538   //二级菜单菜单名的颜色
#define WordColor_MenuItem2_Title_BG	 0xD71E   //二级菜单菜单名BG的颜色
//#define WordColor_MenuItem2_Select	     0x6014
#define WordColor_MenuItem2_Select	     0xC007
#define WordColor_MenuItem2_NOSelect	 0x027F 
#define WordColor_MenuItem2_BG	         0x7E5F

//文件浏览中显示的字体颜色及字体的背景颜色
#define WordColor_DIR_Select	       0x0000//DIR中选择字体的颜色
#define WordColor_DIRBG_Select	       0xFFFF//DIR中选择字体的背景
#define WordColor_DIR_NOSelect	       0xFFFF//DIR中未选择字体的颜色   
#define WordColor_DIRBG_NOSelect	   0x7E5F//DIR中未选择字体的背景
#define	Start_X_DIR		20
#define	Start_Y_DIR		27
#define DIR_Length	(TFT_H_Size-Start_X_DIR-5)//离右边距5个像素点显示的长度
#define DIR_High        20

//文件浏览中两个文件名之间的间隔将随图标变换而变化
// DIR-filename display parameters


//全屏歌词的相关设置
#define FullLRC_Word1Color				0x0000//第一句歌词的颜色
#define FullLRC_Word1Color_BG			0xAF1F//第一句歌词的背景
#define FullLRC_Start1_X				8//第一句歌词的起始位置x
#define FullLRC_Start1_Y				4//第一句歌词的起始位置x
#define FullLRC_Width1					144//第一句歌词的显示宽度
#define FullLRC_High1					20//第一句歌词的显示高度	

#define FullLRC_Word2Color				0xFFFF//第二句歌词的颜色
#define FullLRC_Word2Color_BG			0xFBDE//第二句歌词的背景
#define FullLRC_Start2_X				8//第二句歌词的起始位置x
#define FullLRC_Start2_Y				68//第二句歌词的起始位置x
#define FullLRC_Width2					144//第二句歌词的显示宽度
#define FullLRC_High2					20//第二句歌词的显示高度

#define Voice_Item_WordColor            0x0000//音量调整界面显示标题颜色
#define Voice_Item_WordColor_BG			0xA713//标题背景颜色
#define Voice_Item_X			     	4//标题显示的X起始
#define Voice_Item_Y				    8//标题显示的Y起始

#define FM_ModeItem_WordColor            0x0000//音量调整界面显示标题颜色
#define FM_ModeItem_WordColor_BG			0xA714//标题背景颜色
#define FM_ModeItem_X			     	4//标题显示的X起始
#define FM_ModeItem_Y				    8//标题显示的Y起始

//#define	FW_version	101	
//#define TFT_Disp_Type    1018    //1:竖屏 0：第一份UI图片 18：1.8寸屏 
//#define	FW_Date		20090623  
#define FW_Ver_Start_X                  50
#define FW_Ver_Start_Y                  50
#define FW_Data_Start_X              50
#define FW_Data_Start_Y              80
//-------add for refresh type 0705
#define REFRESH_ALL 0x00//全部刷新，包括背景，及显示的所有菜单
#define REFRESH_NEXT 0x01//刷新next操作，刷新当前选中项目及当前项目的前一项，只刷新这两项，不刷新背景及其他未动作的项目
#define REFRESH_PREV 0x02
#define REFRESH_NOT  0xff	//不刷新---not use

