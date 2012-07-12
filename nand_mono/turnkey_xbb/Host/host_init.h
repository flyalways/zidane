#define	REG_DMASIZE15_8		0xB303
#define HOST_BUF_SA			0xA800
#define	MAX_RETRY_COUNT		2

#define HOST_PHYCOUNECT_FAIL                   0xff
#define HOST_B601_ERROR                        0xfe
#define HOST_SUCCESS                           0x00
#define HOST_SHORTPACKET					   0x01
#define HOST_B611_ERROR                        0xfc
#define HOST_STALL							   0xfa
#define HOST_ACK_ERROR						   0xef
#define HOST_TimeOut_ERROR					   0xee
#define HOST_TEST_ERROR						   0xaa
#define HOST_TEST_ERROR1					   0xab


#define HOST_TRANSETUP_ERROR                	0xfd
#define HOST_SETUPIN_ERROR    				    0xfc
#define HOST_SETUPOUT_ERROR   				    0xfb

#define HOST_GETDESCRIPTOR_FAIL                 0x01
#define HOST_SETADDRESS_FAIL                    0x02
#define HOST_SETPORTFEATURE_FAIL                0x03
#define HOST_GETPORTSTATUS_FAIL                 0x04
#define HOST_MAXLUN_FAIL                        0x05
#define HOST_SCSICMD_INQUIRY_FAIL               0x06
#define HOST_SCSICMD_READCAPACITY_FAIL          0x07
#define HOST_SCSICMD_03_REQUESTSENSE_FAIL       0x08
#define HOST_READSECTOR_FAIL				    0x09
#define HOST_WRITESECTOR_FAIL				    0x0a
#define HOST_SCSICMD_12_INQUIRY_FAIL		    0x0b
#define HOST_SCSICMD_00_TESTUNITRDY_FAIL		0x0e
#define HOST_SCSICMD_5A_MODESENSE_FAIL		    0x13
#define HOST_SCSICMD_25_READCAPACITY_FAIL		0x0c
#define HOST_SCSICMD_23_READFORMATCAPACITY_FAIL	0x0d
#define HOST_SCSICMD_00_TESTUNITRDY_FAIL		0x0e
#define HOST_SCSICMD_LOGICAL_DRIVER_RDY_FAIL 	0x0f
#define HOST_SCSICMD_DRVNOT_RDY_FAIL 			0x10
#define HOST_SCSICMD_INVALID_COMMAND 			0x11
#define HOST_SCSICMD_NO_SENSE					0x00
#define HOST_SCSICMD_UNKNOW_SENSE				0x00
#define HOST_SCSICMD_MEDIUMNOTPRESENT			0x12
#define HOST_SCSICMD_MEDIACHANGED				0x15
//================= Hub class feature selector=====================================
#define PORT_RESET								0x04
						

extern	data	bit	gb_dtg_in;
extern	data	bit	gb_dtg_out;
extern	data	bit	gb_HostConnect;
extern	data	bit	gb_ReadWriteDataArea;

extern	xdata	U16	gw_FileTotalNumber[];

extern xdata U8 EP_OUT;
extern xdata U8 EP_IN;
extern xdata U8 DeviceAddress;
extern	xdata	U8	Max_Trans_Length;
extern data U8 gc_CurrentCard;
extern	xdata	U8	MaxLUN;
extern	xdata	U8	CBWLUN;
extern xdata U32 gdw_DeviceMaxSecNum;
extern xdata U32 gdw_SectorLengthInBytes;
//============================================================================================
void delay         (U16  i);
void Syn_Reset     (void);
void tran_setup    (U8 addr);

void	setup_in(U8 addr,U8 amount,U8 offset,U8 setup_dtg);	
void	setup_out(U8 addr,U8 endpoU16,U16 amount,U16 offset,U8 setup_dtg);
void	tran_in(U8 addr,U8 amount,U16 offset);
void	tran_out(U8 addr,U8 amount);
U8		SetAddress(void);
U8 ClearFeature  (U8 addr, U16  wValue,   U16 wIndex, U16 wLength);
U8	SCSICMD_03_REQUESTSENSE(void);
U8 HOST_Readsector(U32 SectorAddr,U8 *buffer);
void Host_Remove(void);
extern void USER_DelayDTms(U16 DT);

U8 Host_phy_Command_check(void);
U8 GetDescriptor(U8 addr,U16 wValue,U16 wIndex,U16 wLength);
void Port_Reset(void);

U8 SetConfiguration(U8 addr, U16 wValue, U16 wIndex, U16 wLength);
void	GetMAXLUN(void);
U8 GetPortStatus(U8 addr, U16 wValue, U16 wIndex, U16 wLength);
U8 GetHubDescriptor(U8 addr, U16 wValue, U16 wIndex, U16 wLength);

U8 SetPortFeature(U8 addr, U8 RequsetCode, U16 wValue, U16 wIndex, U16 wLength);

extern U8 DOS_Initialize(void);

U8	SCSICMD_12_INQUIRY(void); 
U8	SCSICMD_23_READFORMATCAPACITY(void);	
U8	SCSICMD_25_READCAPACITY(void);

//============================================================================================
extern void Port_ResetFirst(void);
extern U8 CheckUSBPlugIN(void);
extern	bit     gb_FindFlag;
extern xdata File_FCB gs_File_FCB[];
extern xdata U32 gdw_DOS_RootDirClus;
extern U8  DOS_Search_File(U8 tc_Mode, U8 tc_Type, U8 tc_PrevOrNext);

extern void Host_Remove(void);
extern U8 Host_Initial(void);
extern void Host_Initialize(void);
extern U8 Host_DetectDevice(void);
extern void System_HostDetect();


#define M_HiByteOfWord(x)       (U8) ((x) >> 8)
#define M_LoByteOfWord(x)       (U8) (x)
#define		REG_DMA_SEL			0xB301
#define		REG_DMASIZE7_0			0xB302
#define		REG_SRAMDMASRCIDX7_0		0xB112
#define		REG_SRAMDMASRCIDX15_8		0xB113
#define		REG_SRAMDMADSTIDX7_0		0xB114
#define		REG_SRAMDMADSTIDX15_8		0xB115
#define		REG_DMASTART			0xB3B0
#define		REG_DMACMP			0xB3C0

#define CURRENT_MEDIA_HOST    0x05  // chiayen 20081008 add temp
