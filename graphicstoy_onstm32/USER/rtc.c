#include "rtc.h"
#include "delay.h"

static u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
}

static u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

void Counter_Init(){
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//'??PWR???
	PWR_BackupAccessCmd(ENABLE);	//'????J??????? 
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//????h???????
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ????    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//????????RCC???????????,?????????????
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0) return ;		//LSE ???????. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//????RTC???(RTCCLK),???LSE???RTC???    
		RCC_RTCCLKCmd(ENABLE);	//'??RTC??? 

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC????????(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC?????????(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC?????,24?????
    RTC_Init(&RTC_InitStructure);
 
		RTC_Set_Time(15,49,0,RTC_H12_AM);	//???????
		RTC_Set_Date(22,12,30,5);		//????????
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//????????'??????
	} 
	return ;
}