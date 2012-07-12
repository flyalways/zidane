#ifndef __PORT_DEFINE_H__
#define __PORT_DEFINE_H__

		sbit DbgP10             = P1^0;
		sbit DbgP11             = P1^1;
		sbit DbgP12 			= P1^2;
		sbit DbgP13 			= P1^3;
		sbit DbgP14 			= P1^4;
		sbit LCM_RESET			= P1^2;
     
/*  P3  */
        //sbit COLOR_R = P3^0;      // COLOR_R
		sbit DbgP30 			= P3^0;//lijian.peng add for BackLight
		sbit DbgP31 			= P3^1;
#endif //!__PORT_DEFINE_H__
