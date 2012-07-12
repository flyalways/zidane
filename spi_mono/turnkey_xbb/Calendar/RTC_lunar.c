//=========================V1.0==========================
// 萬年曆農曆算法：
//1、判斷當前陽曆經過的天數，如果其大於lunarcal_table[當前年][0]，則直接將其減去相差的天數
//，剩餘天數按每月陰曆的天數累計計算即可算出當前日期對應的農曆
//2、如果當前陽曆經過的天數小於基準天數，則農曆年=當前樣機年-1，然後將統計當前日期到去年陽曆1月1日的總天數，
//將總天數-基數天數，即為當前農曆年1月1日到現在經過的天數，將該天數按月減少，即可計算出當前日期對應的農曆日期
//3、表格中的第二項表示該年閏月為幾月份，為0表示該年無閏月，值得說明的是，閏月的總天數與被閏月的總天數不一定相等
//
//4、節氣計算：table為陽曆日期對應的節氣號碼，可參考Get_Solar()函數，注意：該函數傳入的參數為陽曆的年月日
//
//
//author：sunzhk    2010-10-25
//==================================================================================


#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\variables.h"
#include "RTC_lunar.h"

//---------------------
//輸入年月日，獲取對應的農曆年月日
//獲取的農曆年月日存在gw_Year_Lunar;gc_Month_Lunar;gc_Day_Lunar;中
// //當前農曆年=當前陽曆年-1；
//當前農曆天數=(前一年陽曆總天數+當前日期總天數-35)天，這樣即對應去年農曆1月1日經過的天數，
//再按農曆算這些天對應農曆日期即可
//----------------------
U8 code days_of_months2[12] =
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
 
void Get_Lunar(U16 Y,U8 M,U8 D)
{
    data	U16	days_Lunar=0;
    xdata	U8	i;
    xdata	U8	LeapMonth=0;//閏月為0xff表示該年無閏月,判斷依據為表格
	xdata	U8	tc_Year;

    gc_Month_Lunar=1;
    for(i=0;i<(M-1);i++)
    {
        days_Lunar=days_Lunar+days_of_months2[i];//計算當前月之前的總天數
    }
    days_Lunar+=D; //加上當前月經過的天數
    if(Y%4==0 && Y%100!=0 || Y%400 == 0)
    {
        if(M>2)
        {
            days_Lunar++;//閏年並且超過2月份則天數+1
        }
    }
   // dbprintf("days_Lunar=%x\n",days_Lunar);
    //當前年經歷的總天數
    
    if(days_Lunar>lunarcal_table[Y+1-1900][0])  //當前陽曆經過的天數大於基數
    {
         days_Lunar -= lunarcal_table[Y+1-1900][0];      //注表格中是從1999年開始的，故1900年的話需要+1操作
    }
    else
    {  
        days_Lunar+=1;//新年從1日開始，故該處=1;
        Y--;
        days_Lunar += 364;//加上去年的總天數
        if(Y%4==0 && Y%100!=0 || Y%400 == 0) 
        {
         //    dbprintf("Leap year+1day\n");
             days_Lunar++;
        }  
        days_Lunar -= lunarcal_table[Y+1-1900][0];//減去基準天數，注意之前有個Y--
        
        //---至此，days_Lunar為去年農曆1月1日至今經過的總天數                                                    
    }
  //   dbprintf("days_Lunar2=%x\n",days_Lunar);
    //--經過上面的計算，確保days_Lunar是超過當前農曆年與陽曆年的基數的
    //以下為計算去年農曆1月1日經過這麼些天後變為幾月幾日，需查表看每個月的天數
    gw_Year_Lunar= Y;
   
    if(lunarcal_table[Y+1-1900][1]!=0)
    {//存在閏月
        LeapMonth=lunarcal_table[Y+1-1900][1];
    }else
    {
        LeapMonth=0xff;  
    }
    for(i=1;i<14;i++)
    {  
        if(days_Lunar>30)
        {    
            days_Lunar -= 29;//
            days_Lunar -= lunarcal_table[Y+1-1900][i+1];   
            gc_Month_Lunar=i+1;
			gb_LeapMonth=0;
			if(LeapMonth==gc_Month_Lunar-1)
			{
				gb_LeapMonth=1;
			}
            if(gc_Month_Lunar>LeapMonth)//超過閏月的月份減一
            {
                gc_Month_Lunar--;
            } 
        }
		else
        {
            if(days_Lunar == 30 && lunarcal_table[Y+1-1900][i+1]==0)
            {   //該月無30日
                gc_Day_Lunar = 1;
                gc_Month_Lunar++; 
				gb_LeapMonth=0;
            }
			else
            {
                gc_Day_Lunar = days_Lunar;
            }    
            break;
        }                
    }            

    if(gc_Month_Lunar==0x0d)
    {
        gc_Month_Lunar -= 12;
        gw_Year_Lunar +=  1;
    } 

	tc_Year=(gw_Year_Lunar-1864)%60;
	gc_Tiangan=tc_Year%12;
	gc_Dizhi=tc_Year%10;
}

#if 0
//--------------------------------------------
//Func：獲取天干地支存在變量gc_Tiangan和gc_Dizhi
//input：農曆的年
//60年一個天干地支循環一次
//--------------------------------------------
void Get_Tiangan_Dizhi(U16 Y)
{
    Y=Y-1900+17;
    Y=Y%60;
    gc_Tiangan=Y%12; //天乾號碼
    gc_Dizhi = Y%10; //地支號碼
}

//--------------------------------------------
//Func:獲取節氣,節氣號存在gc_Solar中
//input 陽曆的年月日
//--------------------------------------------
void Get_Solar(U16 Y,U8 M,U8 D)
{
    gc_Solar=0xff;//表示該天無節氣信息

//0 小寒 臘月 6 清明 三月  12 小暑 六月 18 寒露 九月 
//1 大寒 臘月 7 谷雨 三月  13 大暑 六月 19 霜降 九月 　　
//2 立春 正月 8 立夏 四月  14 立秋 七月 20 立冬 十月 　　
//3 雨水 正月 9 小滿 四月  15 處暑 七月 21 小雪 十月 　　
//4 驚蟄 二月 10 芒種 五月 16 白露 八月 22 大雪 冬月 　　
//5 春分 二月 11 夏至 五月 17 秋分 八月 23 冬至 冬月
    switch(M)
    {
        case 1:   
            if(Solar_Data[Y-1900][0]==D){ gc_Solar=0;}
            if(Solar_Data[Y-1900][1]==D){ gc_Solar=1;}
            break;
        case 2:   
            if(Solar_Data[Y-1900][2]==D){ gc_Solar=2;}
            if(Solar_Data[Y-1900][3]==D){ gc_Solar=3;}
            break;
        case 3:   
            if(Solar_Data[Y-1900][4]==D){ gc_Solar=4;}
            if(Solar_Data[Y-1900][5]==D){ gc_Solar=5;}
            break;
        case 4:   
            if(Solar_Data[Y-1900][6]==D){ gc_Solar=6;}
            if(Solar_Data[Y-1900][7]==D){ gc_Solar=7;}
            break;
        case 5:   
            if(Solar_Data[Y-1900][8]==D){ gc_Solar=8;}
            if(Solar_Data[Y-1900][9]==D){ gc_Solar=9;}
            break;
        case 6:   
            if(Solar_Data[Y-1900][10]==D){ gc_Solar=10;}
            if(Solar_Data[Y-1900][11]==D){ gc_Solar=11;}
            break;
        case 7:   
            if(Solar_Data[Y-1900][12]==D){ gc_Solar=12;}
            if(Solar_Data[Y-1900][13]==D){ gc_Solar=13;}
            break;
        case 8:   
            if(Solar_Data[Y-1900][14]==D){ gc_Solar=14;}
            if(Solar_Data[Y-1900][15]==D){ gc_Solar=15;}
            break;
        case 9:   
            if(Solar_Data[Y-1900][16]==D){ gc_Solar=16;}
            if(Solar_Data[Y-1900][17]==D){ gc_Solar=17;}
            break;
        case 10:   
            if(Solar_Data[Y-1900][18]==D){ gc_Solar=18;}
            if(Solar_Data[Y-1900][19]==D){ gc_Solar=19;}
            break;
        case 11:   
            if(Solar_Data[Y-1900][20]==D){ gc_Solar=20;}
            if(Solar_Data[Y-1900][21]==D){ gc_Solar=21;}
            break;
        case 12:   
            if(Solar_Data[Y-1900][22]==D){ gc_Solar=22;}
            if(Solar_Data[Y-1900][23]==D){ gc_Solar=23;}
            break;
        default:
            break;
    }      
}
#endif


