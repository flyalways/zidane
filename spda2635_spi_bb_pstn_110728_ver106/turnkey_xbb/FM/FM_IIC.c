
void	iicWriteByte(U8 I2cData);
void	iicAck(U8 ans);
void	iicSTART(void);
void	iicSTOP(void);
void	I2CSS_Wait(U16 tt);
void	IIC_Dealy(x_DelayD);
void	SendStr(U8 sla,U8 *s,U8 no);
void	RecStr(U8 sla,U8 *s,U8 no);
U8		iicCheckAck(void);
U8		iicReadByte_Ack(U8 SendAck);
U8		iicReadByte(void);

//delay
void IIC_Dealy(x_DelayD)
{
   gc_CL6017Cont=x_DelayD;
   while(gc_CL6017Cont);
}
//======================================================================================
//  FM5800C FUCTION
//  SPARK_ADD_090307
//======================================================================================
U8 iicCheckAck(void)
{
    data U16 errtime=0xffff;
/*
    //SET_IIC_SDA;
	CLR_IIC_SCL;    
	IIC_SDA_IN;   //add by alpah110225
    I2CSS_Wait(2);  //add by alpah110225
    SET_IIC_SCL;
   // IIC_SDA_IN;
    I2CSS_Wait(2);
*/
//SET_IIC_SDA;    
 //  SET_IIC_SCL;
    IIC_SDA_IN;
    I2CSS_Wait(2);
	SET_IIC_SCL;
//I2CSS_Wait(2);

    while(IIC_SDA_Test)
    {
        errtime--;
        if (!errtime)
        {		
         	 IIC_SDA_OUT;
            SomeNOP();
            iicSTOP();
            return 0;
        }
    }
	I2CSS_Wait(2);
    CLR_IIC_SCL;//5807必须先clr clk，再设置为dat out
    IIC_SDA_OUT;
    I2CSS_Wait(2);
    return 1;
}


void iicWriteByte(U8 I2cData)
{
    data	U8 Bits;

    for(Bits=0;Bits<8;Bits++)
    {
        if(I2cData&0x80)
        {
            SET_IIC_SDA;
        }
        else
        {
            CLR_IIC_SDA;
        }
        I2cData<<=1;
        SET_IIC_SCL;
        I2CSS_Wait(10);
        CLR_IIC_SCL;
   		I2CSS_Wait(10);     
    }
}


void iicSTART(void)
{
	SET_IIC_SCL;
	SET_IIC_SDA;
	I2CSS_Wait(30);
	CLR_IIC_SDA;
	I2CSS_Wait(30);
	CLR_IIC_SCL;
	I2CSS_Wait(30);
}


void iicSTOP(void)
{
	CLR_IIC_SDA;
	I2CSS_Wait(30);
	SET_IIC_SCL;
	I2CSS_Wait(30);
	SET_IIC_SDA;
}


void iicAck(U8 ans)
{
	IIC_SDA_OUT;
	I2CSS_Wait(2);
	if(ans)
	{
		SET_IIC_SDA;
	}
	else
	{
		CLR_IIC_SDA;
	}
	I2CSS_Wait(10);
	SET_IIC_SCL;
	I2CSS_Wait(10);
	CLR_IIC_SCL;
	I2CSS_Wait(10);
}


U8 iicReadByte(void)
{
	data	U8	I2cData;
	data	U8	Bits;

	SET_IIC_SDA;
	IIC_SDA_IN;
	//I2CSS_Wait(10);
	I2cData=0;
	for(Bits=0;Bits<8;Bits++)
	{
		I2cData<<=1;
		if(IIC_SDA_Test) I2cData|=0x01;
		SET_IIC_SCL;
		I2CSS_Wait(10);
		CLR_IIC_SCL;
		I2CSS_Wait(10);
	}
	return I2cData;
}


void I2CSS_Wait(U16 tt)
{	 
    while(tt>0)
    {
        tt--;
    }
}

void SendStr(U8 sla,U8 *s,U8 no)
{
    data	U8 i;

    XBYTE[0xB400]=0;    // FMGPIO as GPIO   
    XBYTE[0xB402]|=0x14;// FM[12]/FM[14] output 1
    XBYTE[0xB406]|=0x14;// FM[12]/FM[14] output enable
//	dbprintf("IIC S=%bx,No=%bx,*s=%bx\n",sla,no,*s);
P1|=0x18;
XBYTE[0xB102]=0x18;

//	dbprintf("send %bx,%bx\n",s[0],s[1]);

	iicSTART();
    iicWriteByte(sla);

    if(iicCheckAck()) 
    {
        for(i=0;i<no;i++)
        {
            iicWriteByte(*s);
            if(!iicCheckAck()) 
            {	
				dbprintf("send %bx=%bx err\n",i,*s);
                break;
            }
            s++;
        }
	}else
	{
		dbprintf("send sla err");
	} 
    iicSTOP();

}


void RecStr(U8 sla,U8 *s,U8 no)
{
    data	U8 i;
    
    XBYTE[0xB400]=0;    // FMGPIO as GPIO   
    XBYTE[0xB402]|=0x14;// FM[12]/FM[14] output 1
    XBYTE[0xB406]|=0x14;// FM[12]/FM[14] output enable
	P1|=0x18;
XBYTE[0xB102]=0x18;
    iicSTART();
    iicWriteByte(sla);
    if(iicCheckAck()) 
	{	    
        for(i=0;i<no-1;i++)
        {
            *s=iicReadByte();
            iicAck(0);
       		s++;
        }		
    }else
    {
		dbprintf("ack-err receive\n");
	}
    *s=iicReadByte();
    iicAck(1);
    iicSTOP();
} 
#if (defined Mode_FM_QN8035)||(defined FM_CL6017)
////////////////有的IIC用下面的数据格式读取及写数据===================
/***********************************************************************************************************
** Name: I2C_Read_nbyte                            
** Function: Read n data from a desired register    
**           through i2c bus 
** Description: Slave---------device address
**              Regis_Addr----register address
************************************************************************************************************/
void I2C_Write_nbyte(U8 Slave,U8 Regis_Addr,U8 *s,U8 no)
{
	data U8 i;

	XBYTE[0xB400]=0;    // FMGPIO as GPIO   
    XBYTE[0xB402]|=0x14;// FM[12]/FM[14] output 1
    XBYTE[0xB406]|=0x14;// FM[12]/FM[14] output enable

	//P1|=0x18;	// P1 output high to turn off transistor and to avoid input grounded.
P1|=0x18;
	XBYTE[0xB102]=0x18;

    iicSTART();
    iicWriteByte(Slave);
	if(iicCheckAck()) 
	{	  
    	iicWriteByte(Regis_Addr);
		if(iicCheckAck()) 
		{
	        for(i=0;i<no;i++)
	        {
				iicWriteByte(*s);
				if(!iicCheckAck()) 
	            {	
					dbprintf("send %bx=%bx err\n",i,*s);
	                break;
	            }
	            s++;
	        }
		}else
		{
			dbprintf("write add err\n");
		}
    }else
    {
		dbprintf("ack-err write slave\n");
	}

    iicSTOP();
}
/***********************************************************************************************************
** Name: I2C_Read_nbyte                            
** Function: Read n data from a desired register    
**           through i2c bus 
** Description: Slave---------device address
**              Regis_Addr----register address
************************************************************************************************************/
void I2C_Read_nbyte(U8 Slave,U8 Regis_Addr,U8 *s,U8 no)
{
    U8 temp;
	data U8 i;
    temp=Slave|0x01;
	XBYTE[0xB400]=0;    // FMGPIO as GPIO   
	XBYTE[0xB402]|=0x14;// FM[12]/FM[14] output 1
	XBYTE[0xB406]|=0x14;// FM[12]/FM[14] output enable
P1|=0x18;
XBYTE[0xB102]=0x18;
    iicSTART();
    iicWriteByte(Slave);
	if(iicCheckAck()) 
	{	  
    	iicWriteByte(Regis_Addr);
		if(iicCheckAck()) 
		{
			iicSTART();
			iicWriteByte(0x21);
			if(iicCheckAck()) 
			{
 				for(i=0; i<no-1; i++)
				{
	                *s = iicReadByte();
	                iicAck(0);
					s++;
	            }			
	       	 	*s = iicReadByte();
	            iicAck(1);
	        }else
			{
				dbprintf("read nbyte err\n");	
			}
		}else
		{
			dbprintf("write add err\n");
		}
    }else
    {
		dbprintf("ack-err write slave\n");
	}
    iicSTOP();
}

#endif


