//=========================V1.0==========================
// �U�~��A���k�G
//1�B�P�_��e����g�L���ѼơA�p�G��j��lunarcal_table[��e�~][0]�A�h�����N���h�ۮt���Ѽ�
//�A�Ѿl�Ѽƫ��C�볱�䪺�ѼƲ֭p�p��Y�i��X��e����������A��
//2�B�p�G��e����g�L���ѼƤp���ǤѼơA�h�A��~=��e�˾��~-1�A�M��N�έp��e�����h�~����1��1�骺�`�ѼơA
//�N�`�Ѽ�-��ƤѼơA�Y����e�A��~1��1���{�b�g�L���ѼơA�N�ӤѼƫ����֡A�Y�i�p��X��e����������A����
//3�B��椤���ĤG����ܸӦ~�|�묰�X����A��0��ܸӦ~�L�|��A�ȱo�������O�A�|�몺�`�ѼƻP�Q�|�몺�`�ѼƤ��@�w�۵�
//
//4�B�`��p��Gtable���������������`�𸹽X�A�i�Ѧ�Get_Solar()��ơA�`�N�G�Ө�ƶǤJ���ѼƬ����䪺�~���
//
//
//author�Gsunzhk    2010-10-25
//==================================================================================


#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\variables.h"
#include "RTC_lunar.h"

//---------------------
//��J�~���A����������A��~���
//������A��~���s�bgw_Year_Lunar;gc_Month_Lunar;gc_Day_Lunar;��
// //��e�A��~=��e����~-1�F
//��e�A��Ѽ�=(�e�@�~�����`�Ѽ�+��e����`�Ѽ�-35)�ѡA�o�˧Y�����h�~�A��1��1��g�L���ѼơA
//�A���A���o�Ǥѹ����A�����Y�i
//----------------------
U8 code days_of_months2[12] =
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
 
void Get_Lunar(U16 Y,U8 M,U8 D)
{
    data	U16	days_Lunar=0;
    xdata	U8	i;
    xdata	U8	LeapMonth=0;//�|�묰0xff��ܸӦ~�L�|��,�P�_�̾ڬ����
	xdata	U8	tc_Year;

    gc_Month_Lunar=1;
    for(i=0;i<(M-1);i++)
    {
        days_Lunar=days_Lunar+days_of_months2[i];//�p���e�뤧�e���`�Ѽ�
    }
    days_Lunar+=D; //�[�W��e��g�L���Ѽ�
    if(Y%4==0 && Y%100!=0 || Y%400 == 0)
    {
        if(M>2)
        {
            days_Lunar++;//�|�~�åB�W�L2����h�Ѽ�+1
        }
    }
   // dbprintf("days_Lunar=%x\n",days_Lunar);
    //��e�~�g�����`�Ѽ�
    
    if(days_Lunar>lunarcal_table[Y+1-1900][0])  //��e����g�L���ѼƤj����
    {
         days_Lunar -= lunarcal_table[Y+1-1900][0];      //�`��椤�O�q1999�~�}�l���A�G1900�~���ܻݭn+1�ާ@
    }
    else
    {  
        days_Lunar+=1;//�s�~�q1��}�l�A�G�ӳB=1;
        Y--;
        days_Lunar += 364;//�[�W�h�~���`�Ѽ�
        if(Y%4==0 && Y%100!=0 || Y%400 == 0) 
        {
         //    dbprintf("Leap year+1day\n");
             days_Lunar++;
        }  
        days_Lunar -= lunarcal_table[Y+1-1900][0];//��h��ǤѼơA�`�N���e����Y--
        
        //---�ܦ��Adays_Lunar���h�~�A��1��1��ܤ��g�L���`�Ѽ�                                                    
    }
  //   dbprintf("days_Lunar2=%x\n",days_Lunar);
    //--�g�L�W�����p��A�T�Odays_Lunar�O�W�L��e�A��~�P����~����ƪ�
    //�H�U���p��h�~�A��1��1��g�L�o��Ǥѫ��ܬ��X��X��A�ݬd��ݨC�Ӥ몺�Ѽ�
    gw_Year_Lunar= Y;
   
    if(lunarcal_table[Y+1-1900][1]!=0)
    {//�s�b�|��
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
            if(gc_Month_Lunar>LeapMonth)//�W�L�|�몺�����@
            {
                gc_Month_Lunar--;
            } 
        }
		else
        {
            if(days_Lunar == 30 && lunarcal_table[Y+1-1900][i+1]==0)
            {   //�Ӥ�L30��
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
//Func�G����Ѥz�a��s�b�ܶqgc_Tiangan�Mgc_Dizhi
//input�G�A�䪺�~
//60�~�@�ӤѤz�a��`���@��
//--------------------------------------------
void Get_Tiangan_Dizhi(U16 Y)
{
    Y=Y-1900+17;
    Y=Y%60;
    gc_Tiangan=Y%12; //�Ѱ����X
    gc_Dizhi = Y%10; //�a�丹�X
}

//--------------------------------------------
//Func:����`��,�`�𸹦s�bgc_Solar��
//input ���䪺�~���
//--------------------------------------------
void Get_Solar(U16 Y,U8 M,U8 D)
{
    gc_Solar=0xff;//��ܸӤѵL�`��H��

//0 �p�H þ�� 6 �M�� �T��  12 �p�� ���� 18 �H�S �E�� 
//1 �j�H þ�� 7 ���B �T��  13 �j�� ���� 19 ���� �E�� �@�@
//2 �߬K ���� 8 �߮L �|��  14 �߬� �C�� 20 �ߥV �Q�� �@�@
//3 �B�� ���� 9 �p�� �|��  15 �B�� �C�� 21 �p�� �Q�� �@�@
//4 ���h �G�� 10 �~�� ���� 16 ���S �K�� 22 �j�� �V�� �@�@
//5 �K�� �G�� 11 �L�� ���� 17 ��� �K�� 23 �V�� �V��
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


