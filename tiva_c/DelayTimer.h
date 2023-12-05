#ifndef DELAYTIMER_H_
#define DELAYTIMER_H_

extern void timerInit(void);
extern unsigned long micros(void);
extern unsigned long millis(void);
extern void delayMicroseconds(unsigned int us);
extern void delay(uint32_t milliseconds);

#endif /* DELAYTIMER_H_ */
