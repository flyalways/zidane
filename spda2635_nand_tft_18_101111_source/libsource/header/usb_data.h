unsigned char code Standard_INQUIRY_Format[] =
{
#if 0	// for USB連線不關MP tool
 //    0     1     2     3     4     5     6     7                 0x00  0x80  0x02  0x02
    0x00, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00,   // 0x80 ==> Removable  0x00==>Not Removable
 //    8     9    10    11    12    13    14    15    // 8-15   vendor identification
     'G',  'e',  'n',  'e',  'r',  'a',  'l',  'P',

 //   16    17    18    19    20    21    22    23    // 16-31  product identification
     'l',  'u',  's',  ' ',  'S',  't',  'o',  'r',  

 //   24    25    26    27    28    29    30    31
     'a',  'g',  'e',  'D',  'e',  'v',  'i',  'c', 

 //   32    33    34    35                            // product revision level
     'e',  '2',  '.',  '0',
#else // for 自動轉檔
 //    0     1     2     3     4     5     6     7                 0x00  0x80  0x02  0x02
    0x00, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00,   // 0x80 ==> Removable  0x00==>Not Removable
 //    8     9    10    11    12    13    14    15    // 8-15   vendor identification
     'S',  'u',  'n',  'P',  'l',  'u',  's',  'm',

 //   16    17    18    19    20    21    22    23    // 16-31  product identification
     'M',  ' ',  'S',  't',  'o',  'r',  'a',  'g',  

 //   24    25    26    27    28    29    30    31
     'e',  ' ',  'D',  'e',  'v',  'i',  'c',  'e', 

 //   32    33    34    35                            // product revision level
     ' ',  '2',  '.',  '0',
#endif
};
unsigned char code Standard_INQUIRY_Format_SDMMC[] =  
{
 //    0     1     2     3     4     5     6     7    // 0x00  0x80  0x02  0x02
    0x00, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00,   // 0x80 ==> Removable  0x00==>Not Removable
 //    8     9    10    11    12    13    14    15    // 8-15   vendor identification
     'U',  'S',  'B',  ' ',  '2',  '.',  '0',  ' ',
 
 //   16    17    18    19    20    21    22    23    // 16-31  product identification
     'R',  'e',  'a',  'd',  'e',  'r',  '(',  'S',
 
 //   24    25    26    27    28    29    30    31    
     'D',  '/',  'M',  'M',  'C',  ')',  ' ',  ' ',  
 
 //   32    33    34    35                            // product revision level
     '2',  '0',  '0',  '8',
};
// 標準Request Sense回應值
unsigned char code Request_Sense_Format[] =
{
 //    0     1     2     3     4     5     6     7
    0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,  //0x0C==>如填為0x00 USB HOST會不正常

 //    8     9    10    11    12    13    14    15
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

 //   16    17
    0x00, 0x00,
};



unsigned char code DeviceQualifier_DescriptorTable[10] = {
	//	Device_Qualifier descriptor(Full Speed): from 18
	0x0A,			//Size of descriptor
	0x06,			//Device Qualifier descriptor
	0x00, 0x02,		//USB specification version number ,at least 2.0
	0x00,			//device class     for Mass Storage Class
	0x00,			//device subclass  for Mass Storage Class
	0x00, 			//device protocol  for Mass Storage Class
	0x40,			//Maximum packet size for other speed==>64 Bytes
	0x01,			//Number of Other-speed Configurations
	0x00			//Reserved for future use, must be zero
};

unsigned char code Other_Speed_Configuration_DescriptorTable[39] = {
//      Configuration descriptor
	0x09,			// length
	0x07,			// OTHER_SPEED_CONFIGURATION descriptor
	0x27, 0x00,		// total length : (Configuration, Interface, Endpoint and class- or vendor-specific)
	0x01, 			// number of interface support by this configuration
	0x01,			// configuration value
	iDeviceQualifierCFGString,// String : configuration index of this confugyration
	0x80,//0xE0,	        // attributes : (bit7:1   bit6:'0' BusPWR
	                        //                        bit6:'1' SelfPWR
	                        //                        bit5:'0' RemoteWKP Not-Support
	                        //                        bit5:'1' RemoteWKP Support
	                        //               bit4/3/2/1/0 ==> reset to zero
	0x32,			// max power : 100mA  // 注意Configuration_DescriptorTable也要改
//	INTERFACE descriptor
	0x09,			// length
	0x04,			// INTERFACE descriptor
	0x00,			// interface number, zero base
	0x00, 			// alternate setting
	0x03,			// number of endpoints
	0x08,			// interface class - Mass Storage Class
	0x06,			// interface subclass - 0x06 SCSI Block command
	0x50, 		        // interface protocol - 0x50 Bulk Only protocol
	0x00,	  	        // String : interface index
//	ENDPOINT descriptor
	0x07,			// length
	0x05,			// ENDPOINT descriptor
	0x81,			// endpoint 1, read, IN:TX
	0x02, 			// Bulk type transfer
	0x40,0x00,              // Maximum packet size==> 64 Bytes  (Mass Storage Class)
	0xFF,			// interval

	0x07,			// length
	0x05,			// ENDPOINT descriptor
	0x02,			// endpoint 2, write, OUT:RX
	0x02, 			// Bulk transfer
	0x40,0x00,              // Maximum packet size==> 64 Bytes  (Mass Storage Class)
	0xFF,			// interval

	0x07,			// length
	0x05,			// ENDPOINT descriptor
	0x83,			// endpoint 2, write, OUT:RX
	0x03, 			// Interrupt
	0x02,0x00,              // Maximum packet size==> 64 Bytes  (Mass Storage Class)
	0x01,			// interval

};

unsigned char code Device_DescriptorTable[18] = {
	0x12,			// length
	0x01,			// DEVICE descriptor
	0x10, 0x01,		// bcd USB 1.1      if USB 2.0 ==> 0x00,0x02
	0x00,			// device class     for Mass Storage Class
	0x00,			// device subclass  for Mass Storage Class
	0x00,	 		// device protocol  for Mass Storage Class
	0x40,			// max packet size  for pipe 0==>64 Bytes
	0x17, 0x00,             // VID - 0x0426b    【 XX      公司的】
	0x17, 0x00,		// PID - 0x0426b
	0x00, 0x01,		// bcd device - 1.00
	0x10,			// index of string descriptor (manufacture)
	0x20,			// index of string descriptor (product)
	0x30,			// index of string descriptor (serial number)
	0x01 			// number of configurations
};
//-------------------------------------------------------------------------//
//-------------------------------------------------------------------------//
// void Configuration_DescriptorTable(void)                                //
//-------------------------------------------------------------------------//
//	Configuration Descriptor - Length 0x20 (39 bytes - 9 + 9 + 7 + 7+7)  //
//-------------------------------------------------------------------------//
unsigned char code Configuration_DescriptorTable[39] = {
//      Configuration descriptor
	0x09,			// length
	0x02,			// CONFIGURATION descriptor
	0x27, 0x00,		// total length : (Configuration, Interface, Endpoint and class- or vendor-specific)
	0x01, 			// number of interface support by this configuration
	0x01,			// configuration value
	iConfiguraiton,		// String : configuration index of this confugyration
	0x80,//0xE0,		// attributes : (bit7:1   bit6:'0' BusPWR
	                        //                        bit6:'1' SelfPWR
	                        //                        bit5:'0' RemoteWKP Not-Support
	                        //                        bit5:'1' RemoteWKP Support
	                        //               bit4/3/2/1/0 ==> reset to zero
	0x32,			// max power : 100mA
//	INTERFACE descriptor
	0x09,			// length
	0x04,			// INTERFACE descriptor
	0x00,			// interface number, zero base
	0x00, 			// alternate setting
	0x03,			// number of endpoints
	0x08,			// interface class - Mass Storage Class
	0x06,			// interface subclass - 0x06 SCSI Block command
	0x50, 		    // interface protocol - 0x50 Bulk Only protocol
	iInterface,		// String : interface index
//	ENDPOINT descriptor
	0x07,			// length
	0x05,			// ENDPOINT descriptor
	0x81,			// endpoint 1, read, IN:TX
	0x02, 			// Bulk type transfer
	0x40,0x00,      // Default Maximum packet size==> 64 Bytes  (Mass Storage Class)
	0xFF,			// interval

	0x07,			// length
	0x05,			// ENDPOINT descriptor
	0x02,			// endpoint 2, write, OUT:RX
	0x02, 			// Bulk transfer
	0x40,0x00,      // Default Maximum packet size==> 64 Bytes  (Mass Storage Class)
	0xFF,			// interval

	0x07,			// length
	0x05,			// ENDPOINT descriptor
	0x83,			// endpoint 2, write, OUT:RX
	0x03, 			// Interrupt
	0x02,0x00,      // Maximum packet size==> 64 Bytes  (Mass Storage Class)
	0x01,			// interval

};
//------------------------------------------------------//
//------------------------------------------------------//
// void iLANGID_DescriptorTable(void)                   //
//------------------------------------------------------//
//	LANGID  String Descriptor(0x00) - Length 0x04   //
//------------------------------------------------------//
unsigned char code iLANGID_DescriptorTable[4] = {
	//  LANG ID : Index:0x00//
	0x04,			// STRING size
	0x03,			// STRING descriptor
	0x09, 04 		// LANG ID
};
//--------------------------------------------------------------//
// void iManufacturer_DescriptorTable(void)                     //
//--------------------------------------------------------------//
//	Manufacturer String Descriptor(0x10) - Length 26        //
//--------------------------------------------------------------//
unsigned char code iManufacturer_DescriptorTable[26] = {
	//  iManufacturer ID : Index:0x10
	0x1A,			// STRING size      	0
	0x03,			// STRING descriptor1	1
	'P', 0,		    //			2,3
	'o', 0,		    //			4,5
	'w', 0, 		//			6,7
	'e', 0,		    //			8,9
	'r', 0, 		//			10,11
	' ', 0,		    //			12,13
	'b', 0,   		//			14,15
	'y', 0,   		//			16,17
	' ', 0,   		//			18,19
	'U', 0,   		//			20,21
	'S', 0,   		//			22,23
	'B', 0   		//			24,25
};
//------------------------------------------------------//
// void iProduct_DescriptorTable(void)                  //
//------------------------------------------------------//
//	Product String Descriptor(0x20) - Length 48     //
//------------------------------------------------------//
unsigned char code iProduct_DescriptorTable[48] = {
	//  iProduct ID : Index:0x20
	0x30,			    // STRING size      	0
	0x03,			    // STRING descriptor1	1
	'U', 0,		        //			2,3
	'S', 0,		        //			4,5
	'B', 0,		        //			6,7
	' ', 0,		        //			8,9
	'M', 0,		        //			10,11
	'a', 0,		        //			12,13
	's', 0, 		    //			14,15
	's', 0,		        //			16,17
	' ', 0, 		    //			18,19
	'S', 0,		        //			20,21
	't', 0, 	 	    //			22,23
	'o', 0,		        // 			24,25
	'r', 0,		        //			26,27
	'a', 0,		        // 			28,29
	'g', 0,		        //			30,31
	'e', 0,		        //			32,33
	' ', 0,		        //			34,35
	'D', 0,		        //			36,37
	'e', 0,		        //			38,39
	'v', 0,		        //			40,41
	'i', 0,		        //			42,43
	'c', 0,		        //			44,45
	'e', 0		        //			46,47
};
//----------------------------------------------------------//
// void iSerialNumber_DescriptorTable(void)                 //
//----------------------------------------------------------//
// SerialNumber String Descriptor(0x30) - Length 28         //
//----------------------------------------------------------//
unsigned char code iSerialNumber_DescriptorTable[28] = {
	//  iSerialNumber ID : Index:0x20
	0x1C,			// STRING size      	0
	0x03,			// STRING descriptor1	1
	'2', 0,		        //			2,3
	'0', 0,		        //			4,5
	'1', 0, 		    //			6,7
	'0', 0,		        //			8,9
	'0', 0, 		    //			10,11
	'7', 0,		        //			12,13
	'1', 0, 		    //			14,15
	'4', 0,		        // 			16,17
	'0', 0,		        //			18,19
	'0', 0,		        // 			20,21
	'0', 0,		        // 			22,23
	'0', 0,		        //			24,25
	'a', 0		        //			26,27
};
//------------------------------------------------------//
// void iConfiguraiton_DescriptorTable(void)            //
//------------------------------------------------------//
//Configuraiton String Descriptor(0x40) - Length 10     //
//------------------------------------------------------//
unsigned char code iConfiguraiton_DescriptorTable[10] = {
	//  iConfiguraitonr ID : Index:0x40
	0x0A,			// STRING size      	0
	0x03,			// STRING descriptor1	1
	'i', 0,		        //			2,3
	'C', 0,		        //			4,5
	'f', 0, 		    //			6,7
	'g', 0 		        //			8,9
};
//------------------------------------------------------//
//void iDeviceQualifierString(void)                     //
//------------------------------------------------------//
//DeviceQualifierString Descriptor(0x50) - Length 10    //
//------------------------------------------------------//
unsigned char code iDeviceQualifierString_DescriptorTable[10] = {
	//  iDeviceQualifierString ID : Index:0x50
	0x0A,			// STRING size      	0
	0x03,			// STRING descriptor1	1
	'i', 0,		        //			2,3
	'D', 0,		        //			4,5
	'Q', 0, 		    //			6,7
	'D', 0		        //			8,9
};

unsigned char code iInterface_DescriptorTable[10] = {
	//  iDeviceQualifierString ID : Index:0x60
	0x0A,			// STRING size      	0
	0x03,			// STRING descriptor1	1
	'B', 0,		        //			2,3
	'U', 0,		        //			4,5
	'L', 0, 		    //			6,7
	'K', 0		        //			8,9
};//------------------------------------------------------//
// READ TOC/PMA/APIT table				                //
//------------------------------------------------------//
//(0x43) - Length 36									//
//------------------------------------------------------//
unsigned char code TOC_PMA_ATIP_Format[] =
{
 //    0     1     2     3 
    0x00, 0x0A, 0x01, 0x01,

//    4     5     6     7    8     9    10    11 
	0x01, 0x14, 0x01, 0xA0, 0x00, 0x00, 0x02, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x03, 0x24, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x35, 0x00, 0x35, 0x12, 0x00,
};


unsigned char code CDROM_User_INQUIRY_Format[]  =
{
 //    0     1     2     3     4     5     6     7                 0x00  0x80  0x02  0x02
    0x05, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00,   // 0x80 ==> Removable  0x00==>Not Removable
 //    8     9    10    11    12    13    14    15    // 8-15   vendor identification
     'G',  'e',  'n',  'e',  'r',  'a',  'l',  'p',
 
//   16    17    18    19    20    21    22    23    // 16-31  product identification
      'l',   'u',  's',  ' ',  'C',  'D',  'R',  'O',  

 //   24    25    26    27    28    29    30    31
      'M',  ' ',  'D',   'e',  'v',  'i',  'c',  'e', 

 //   32    33    34    35                            // product revision level
      ' ',  '2',  '.',  '0',
};
