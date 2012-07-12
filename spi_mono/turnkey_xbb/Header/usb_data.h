
U8 code DeviceQualifier_DescriptorTable[10] = 
{
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


unsigned char code iLANGID_DescriptorTable[4]=
{
	//  LANG ID : Index:0x00//
	0x04,			// STRING size
	0x03,			// STRING descriptor
	0x09, 04 		// LANG ID
};


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


U8 code iSerialNumber_DescriptorTable[28]=
{
	//  iSerialNumber ID : Index:0x20
	0x1C,			// STRING size      	0
	0x03,			// STRING descriptor1	1
	'2', 0,		        //			2,3
	'0', 0,		        //			4,5
	'1', 0, 		    //			6,7
	'0', 0,		        //			8,9
	'1', 0, 		    //			10,11
	'1', 0,		        //			12,13
	'2', 0, 		    //			14,15
	'3', 0,		        // 			16,17
	'0', 0,		        //			18,19
	'0', 0,		        // 			20,21
	'0', 0,		        // 			22,23
	'0', 0,		        //			24,25
	'a', 0		        //			26,27
};
////////////////////////////////////////////////
U8 code Device_DescriptorTable[18]=
{
	0x12,			// length
	0x01,			// DEVICE descriptor
	0x10, 0x01,		// bcd USB 1.1      if USB 2.0 ==> 0x00,0x02
	0x00,			// device class     for Mass Storage Class
	0x00,			// device subclass  for Mass Storage Class
	0x00,	 		// device protocol  for Mass Storage Class
	0x40,			// max packet size  for pipe 0==>64 Bytes
	0xFC, 0x04,     // VID - 0x0426b    ¡i XX      ¤½¥qªº¡j
	0x50, 0x23,		// PID - 0x0426b
	0x00, 0x01,		// bcd device - 1.00
	0x10,			// index of string descriptor (manufacture)
	0x20,			// index of string descriptor (product)
	0x30,			// index of string descriptor (serial number)
	0x01 			// number of configurations
};

U8 code Configuration_DescriptorTable[39]=
{
	//	Configuration descriptor
	0x09,			// length
	0x02,			// CONFIGURATION descriptor
	0x27, 0x00,		// total length : (Configuration, Interface, Endpoint and class- or vendor-specific)
	0x01, 			// number of interface support by this configuration
	0x01,			// configuration value
	0x40,			// String : configuration index of this confugyration
	0x80,
	50,				// max power : 100mA
	//	INTERFACE descriptor
	0x09,			// length
	0x04,			// INTERFACE descriptor
	0x00,			// interface number, zero base
	0x00, 			// alternate setting
	0x03,			// number of endpoints
	0x08,			// interface class - Mass Storage Class
	0x06,			// interface subclass - 0x06 SCSI Block command
	0x50, 		    // interface protocol - 0x50 Bulk Only protocol
	0x60,			// String : interface index
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

