
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"

#include "uartstdio.h"
#include "utils/cmdline.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "DelayTimer.h"


static char ReceivedData[512];
static char str[128];

bool process = true;
bool ProcessRoutine();

char *Substring(char *src, char *dst, int start, int stop);
int SearchIndexOf(char src[], char str[]);
char* itoa(int i, char b[]);
void ftoa(float f,char *buf);

void InitUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

void InitSIMUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
    UARTConfigSetExpClk(UART1_BASE, 16000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART1_BASE);

}

void SendATCommand(char *cmd)
{
    while(UARTBusy(UART1_BASE));
    while(*cmd != '\0')
    {
        UARTCharPut(UART1_BASE, *cmd++);
    }
    UARTCharPut(UART1_BASE, '\r'); //CR
    UARTCharPut(UART1_BASE, '\n'); //LF

}

int recvString(char *target, char *data, int timeout, bool check)
{
    int i=0;
    char a;
    unsigned long start = millis();

    while (millis() - start < timeout)
    {
        while(UARTCharsAvail(UART1_BASE))
        {
              a = UARTCharGet(UART1_BASE);
              if(a == '\0') continue;
              data[i]= a;
              i++;
        }

        if(check)
        {
            if (SearchIndexOf(data, target) != -1)
            {
                break;
            }
        }
    }

    return 0;
}

bool recvFind(char *target, int timeout,bool check)
{
    recvString(target, ReceivedData, timeout, check);

    if (SearchIndexOf(ReceivedData, target) != -1)
    {
        return true;
    }
    return false;
}

bool AT(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT");
    return recvFind("OK",5000, true);
}

bool noEcho(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("ATE0");
    return recvFind("OK",5000, false);
}

bool GNSSPWR(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+CGNSPWR=1");
    return recvFind("OK",5000, false);
}

bool GNSSEQ(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+CGNSSEQ=\"RMC\"");
    return recvFind("OK",5000, false);
}

bool CGNSINF(char DateTime[], char Latitude[], char Longitude[])
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+CGNSINF");

    bool ans=recvFind("OK",5000, false);

    memcpy(Latitude, ReceivedData + 35, 9);
    memcpy(DateTime, ReceivedData + 16, 14);
    memcpy(Longitude, ReceivedData + 45, 9);

    return ans;
}

bool GPRS1(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
    return recvFind("OK",5000, false);
}

bool GPRS2(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");
    return recvFind("OK",5000, false);
}

bool GPRS3(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+SAPBR=1,1");
    return recvFind("OK",5000, false);
}

bool HTTP1(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+HTTPINIT");
    return recvFind("OK",5000, false);
}

bool HTTP2(void)
{
    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+HTTPPARA=\"CID\",1");
    return recvFind("OK",5000, false);
}

bool HTTPGet(char *latitude, char *longitude)
{

    if(*(latitude)==',')
            latitude++;
    if(*(longitude)==',')
            longitude++;

    bool ans=false;

    memset(ReceivedData, 0, sizeof(ReceivedData));
    char url[150] = {0};
    sprintf(url, "AT+HTTPPARA=\"URL\",\"http://13.49.65.217/api/gps?latitude=%s&longitude=%s\"", latitude, longitude);
    SendATCommand(url);
    ans= recvFind("OK",5000, false);

    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+HTTPACTION=0");
    ans= recvFind("OK",5000, false);


    memset(ReceivedData, 0, sizeof(ReceivedData));
    SendATCommand("AT+HTTPREAD");
    ans= recvFind("OK",5000, false);

    return ans;
}


int main(void)
{
    bool TaskStatus;

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);


    timerInit();



    InitUART(); // UART0
    InitSIMUART(); // UART1



    UARTprintf("Execute!\n");

    delay(1000);

    // Sim808 Init
    TaskStatus = AT();
    delay(50);
    TaskStatus = noEcho();
    delay(50);

    // GNSS Init
    TaskStatus = GNSSPWR();
    delay(50);
    TaskStatus = GNSSEQ();
    delay(50);
    char DateTime[15] = {0};
    char Latitude[10] = {0};
    char Longitude[10] = {0};
    TaskStatus = CGNSINF(DateTime, Latitude, Longitude);
    delay(50);

    // GPRS init
    TaskStatus = GPRS1();
    delay(50);
    TaskStatus = GPRS2();
    delay(50);
    TaskStatus = GPRS3();
    delay(50);

    //HTTP init
    TaskStatus = HTTP1();
    TaskStatus = HTTP2();

    while(1){

        TaskStatus=HTTPGet( Latitude, Longitude);
        delay(3000);
        TaskStatus = CGNSINF(DateTime, Latitude, Longitude);
    }
}

int SearchIndexOf(char src[], char str[])
{
   int i, j, firstOcc;
   i = 0, j = 0;

   while (src[i] != '\0')
   {

      while (src[i] != str[0] && src[i] != '\0')
         i++;

      if (src[i] == '\0')
         return (-1);

      firstOcc = i;

      while (src[i] == str[j] && src[i] != '\0' && str[j] != '\0')
      {
         i++;
         j++;
      }

      if (str[j] == '\0')
         return (firstOcc);
      if (src[i] == '\0')
         return (-1);

      i = firstOcc + 1;
      j = 0;
   }

   return (-1);
}


