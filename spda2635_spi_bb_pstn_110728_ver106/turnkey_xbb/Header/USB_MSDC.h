U8 code Standard_INQUIRY_Format_SDMMC[] =  
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
     '2',  '0',  '1',  '0',
};


