#define	HOST_BUF_SA						0xA800

#define	HOST_GETDESCRIPTOR_FAIL			0x01
#define	HOST_SETADDRESS_FAIL			0x02
#define	HOST_SCSICMD_MEDIUMNOTPRESENT	0x12
#define HOST_SCSICMD_MEDIACHANGED		0x15
#define	HOST_TEST_ERROR					0xAA
#define	HOST_TEST_ERROR1				0xAB
#define	HOST_TimeOut_ERROR				0xEE
#define	HOST_STALL						0xFA
#define	HOST_SETUPIN_ERROR				0xFC
#define	HOST_SETUPOUT_ERROR				0xFB
#define	HOST_TRANSETUP_ERROR			0xFD
#define	HOST_PHYCOUNECT_FAIL			0xFF

void	tran_setup(U8 addr);
void	delay(U16 i);
void	Port_Reset(void);
void	GetMAXLUN(void);
void	setup_in(U8 addr,U8 amount,U8 offset,U8 setup_dtg);	
void	setup_out(U8 addr,U8 endpoU16,U16 amount,U16 offset,U8 setup_dtg);
void	tran_in(U8 addr,U8 amount,U16 offset);
void	tran_out(U8 addr,U8 amount);
U8		SetAddress(void);
U8		SetConfiguration(U8 addr,U16 wValue,U16 wIndex,U16 wLength);
U8		ClearFeature(U8 addr,U16 wValue,U16 wIndex,U16 wLength);
U8		GetDescriptor(U8 addr,U16 wValue,U16 wIndex,U16 wLength);
U8		HOST_Readsector(U32 SectorAddr,U8 *buffer);


