/* ########################################################################

   PICsim - PIC simulator

   ########################################################################

   Copyright (c) : 2008-2015  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"picsim.h"
#include"periferic16.h"
#include"periferic16e.h"
#include"periferic16e2.h"
#include"periferic18.h"


extern int serial_open(_pic * pic);
extern int serial_close(_pic * pic);
 

extern int read_ihx(_pic * pic,const char * fname,int lrom);
extern int read_ihx_18(_pic * pic,const char * fname,int lrom);


extern void pic_decode_16(_pic * pic);
extern void pic_decode_16E(_pic * pic);
extern void pic_decode_18(_pic * pic);


unsigned short * memsets(unsigned short * mem,unsigned short value, unsigned long size)
{
   unsigned long i;
   for(i=0;i<size;i++)
      mem[i]=value;

   return mem;
}

void   
pic_erase_flash(_pic * pic)
{
   /*zerar mem�ria*/
   switch(pic->family)
   {
     case P16:
     case P16E: 
     case P16E2:    
       memsets(pic->prog,0x3FFF,pic->ROMSIZE); 
       memsets(pic->config,0x3FFF,pic->CONFIGSIZE); 
       memsets(pic->id,0x3FFF,pic->IDSIZE); 
       break;
     case P18:
       memsets(pic->prog,0xFFFF,pic->ROMSIZE); 
       memsets(pic->config,0xFFFF,pic->CONFIGSIZE); 
       memsets(pic->id,0xFFFF,pic->IDSIZE); 
       memset(pic->debugv,0,8);  
       break;
     default:
       break;
   }
}

int 
pic_init(_pic * pic, int processor, const char * fname, int lrom, float freq)
{
   int i;

   pic->print=0;
   
   pic->freq=freq;
   pic->processor=processor;
   pic->family=getfprocbynumber(processor);

   switch(pic->family)
   { 
   case P16: 
     switch(processor)
     {
       case P16F84A:
         pic->ROMSIZE=1024;          //size in WORDS
         pic->EEPROMSIZE=64;
         pic->RAMSIZE=256;           //two banks
         pic->PINCOUNT=18;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=1;
         pic->STACKSIZE=8;
         pic->CCPCOUNT=1;
         pic->ADCCOUNT=0;
         pic->WDT_MS=18;
       break;  
       case P16F628:
       case P16F628A:
         pic->ROMSIZE=2048;          //size in WORDS
         pic->EEPROMSIZE=128;
         pic->RAMSIZE=512;           //four banks
         pic->PINCOUNT=18;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=1;
         pic->STACKSIZE=8;
         pic->CCPCOUNT=1;
         pic->ADCCOUNT=0;
         pic->WDT_MS=18;
       break;  
       case P16F648A:
       case P16F648AICD:
         pic->ROMSIZE=4096;
         pic->EEPROMSIZE=256;
         pic->RAMSIZE=512;
         pic->PINCOUNT=18;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=1;
         pic->STACKSIZE=8;
         pic->CCPCOUNT=1;
         pic->ADCCOUNT=0;
         pic->WDT_MS=18;
       break;  
       case P16F877:
       case P16F877A:
         pic->ROMSIZE=8192;
         pic->EEPROMSIZE=256;
         pic->RAMSIZE=512;
         pic->PINCOUNT=40;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=1;
         pic->STACKSIZE=8;
         pic->CCPCOUNT=2;
         pic->ADCCOUNT=8;
         pic->WDT_MS=18;
       break;
       case P16F777:
         pic->ROMSIZE=8192;
         pic->EEPROMSIZE=0;
         pic->RAMSIZE=512;
         pic->PINCOUNT=40;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=2;
         pic->STACKSIZE=8;
         pic->CCPCOUNT=3;
         pic->ADCCOUNT=14;
         pic->WDT_MS=18;
       break;
       default:
         printf("unknown processor! 0x%04X\n",processor);
         return 0;
       break;
      }
     break;
     case P16E:
       switch(processor)
       {
       case P16F1619:
         pic->ROMSIZE=8192;
         pic->EEPROMSIZE=0;
         pic->RAMSIZE=4096;// 1024 32 banks !!!!
         pic->PINCOUNT=20;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=3;
         pic->STACKSIZE=16;
         pic->CCPCOUNT=2;
         pic->ADCCOUNT=12;
         pic->WDT_MS=1;
       break;
       default:
         printf("unknown processor! 0x%04X\n",processor);
         return 0;
       break;
       }
     break;
     case P16E2:
       switch(processor)
       {  
       case P16F18855:
         pic->ROMSIZE=8192;
         pic->EEPROMSIZE=256;
         pic->RAMSIZE=32768;// debug  banks !!!!
         pic->PINCOUNT=28;
         pic->IDSIZE=8;
         pic->CONFIGSIZE=5;
         pic->STACKSIZE=16;
         pic->CCPCOUNT=5;
         pic->ADCCOUNT=24;
         pic->WDT_MS=1;
       break;
       default:
         printf("unknown processor! 0x%04X\n",processor);
         return 0;
       break;
      }
     break; 
     case P18:
       switch(processor)
       {
         case P18F452:
           pic->ROMSIZE=16384; //size in WORDS
           pic->EEPROMSIZE=256;
         //  pic->RAMSIZE=1536;
           pic->RAMSIZE=4096;//for linear bank 15
           pic->STACKSIZE=31;
           pic->PINCOUNT=40;
           pic->IDSIZE=4;
           pic->CONFIGSIZE=7;
           pic->CCPCOUNT=2;
           pic->ADCCOUNT=8;
           pic->WDT_MS=18;
           break;
         case P18F4520:
           pic->ROMSIZE=16384;
           pic->EEPROMSIZE=256;
         //  pic->RAMSIZE=1536;
           pic->RAMSIZE=4096;//for linear bank 15
           pic->STACKSIZE=31;
           pic->PINCOUNT=40;
           pic->IDSIZE=4;
           pic->CONFIGSIZE=7;
           pic->CCPCOUNT=2;
           pic->ADCCOUNT=13;
           pic->WDT_MS=4;
           break;
         case P18F4620:
           pic->ROMSIZE=32768;
           pic->EEPROMSIZE=1024;
         //  pic->RAMSIZE=3968;
           pic->RAMSIZE=4096;//for linear bank 15
           pic->STACKSIZE=31;
           pic->PINCOUNT=40;
           pic->IDSIZE=4;
           pic->CONFIGSIZE=7;
           pic->CCPCOUNT=2;
           pic->ADCCOUNT=13;
           pic->WDT_MS=4;
           break;
         case P18F4550:
         case P18F45K50:             
           pic->ROMSIZE=16384;
           pic->EEPROMSIZE=256;
         //  pic->RAMSIZE=2048;
           pic->RAMSIZE=4096;//for linear bank 15
           pic->STACKSIZE=31;
           pic->PINCOUNT=40;
           pic->IDSIZE=4;
           pic->CONFIGSIZE=7;
           pic->CCPCOUNT=1;
           pic->ADCCOUNT=13;
           pic->WDT_MS=4;
           break;
         default:
           printf("unknown processor 0x%04X!\n",processor);
           return 0;
        }
        break;
     default:
       printf("unknown family %d of processor 0X%04X!\n",pic->family,processor);
       return 0;
     }

   pic->ram=calloc(pic->RAMSIZE,sizeof(char));   
   pic->prog=calloc(pic->ROMSIZE,sizeof(short));   
   pic->id=calloc(pic->IDSIZE,sizeof(short));   
   pic->config=calloc(pic->CONFIGSIZE,sizeof(short));   
   pic->stack=calloc(pic->STACKSIZE,sizeof(short));   
   pic->eeprom=calloc(pic->EEPROMSIZE,sizeof(char));   
   pic->pins=calloc(pic->PINCOUNT,sizeof(picpin));   
   pic->ccp=calloc(pic->CCPCOUNT,sizeof(char));
   pic->adc=calloc(pic->ADCCOUNT,sizeof(char));
   pic->usart=calloc(2,sizeof(char));

   pic_erase_flash(pic);

 
   if(lrom == 1)
     memset(pic->eeprom,0xFF,pic->EEPROMSIZE); 
   
   pic->sleep=0; 
   
   pic_reset(pic,1);
   
   for(i=0; i < pic->PINCOUNT; i++)
   {
     pic->pins[i].ovalue=1;  //put default pin values to 1  
   }  

   

  /*serial config*/
   switch(pic->family)
   {
     case P16:
       pic->serial_PIR1=&pic->ram[P16_PIR1];
       pic->serial_PIE1=&pic->ram[P16_PIE1];
       pic->serial_TXSTA=&pic->ram[P16_TXSTA];
       pic->serial_RCSTA=&pic->ram[P16_RCSTA];
       pic->serial_SPBRG=&pic->ram[P16_SPBRG];
       pic->serial_RCREG=&pic->ram[P16_RCREG];
       pic->serial_TXREG=&pic->ram[P16_TXREG];
       pic->serial_TXREG_ADDR=P16_TXREG;
       pic->serial_RCSTA_ADDR=P16_RCSTA;
       pic->serial_RCREG_ADDR=P16_RCREG;         
       pic->serial_TRIS_RX=&pic->ram[pic->pins[pic->usart[0]-1].port+0x80];
       pic->serial_TRIS_RX_MASK=(0x01 << pic->pins[pic->usart[0]-1].pord);
       break;  
     case P16E:
       pic->serial_PIR1=&pic->ram[P16E_PIR1];
       pic->serial_PIE1=&pic->ram[P16E_PIE1];
       pic->serial_TXSTA=&pic->ram[P16E_TX1STA];
       pic->serial_RCSTA=&pic->ram[P16E_RC1STA];
       pic->serial_SPBRG=&pic->ram[P16E_SP1BGRL];//FIXME 16bits SPBRG
       pic->serial_RCREG=&pic->ram[P16E_RC1REG];
       pic->serial_TXREG=&pic->ram[P16E_TX1REG];
       pic->serial_TXREG_ADDR=P16E_TX1REG;
       pic->serial_RCSTA_ADDR=P16E_RC1STA;
       pic->serial_RCREG_ADDR=P16E_RC1REG;       
       pic->serial_TRIS_RX=&pic->ram[pic->pins[pic->usart[0]-1].port+0x80];
       pic->serial_TRIS_RX_MASK=(0x01 << pic->pins[pic->usart[0]-1].pord);
       break;  
     case P16E2:
     /* //TODO not implemented yet   
       pic->serial_PIR1=&pic->ram[P16E2_PIR1];
       pic->serial_PIE1=&pic->ram[P16E2_PIE1];
       pic->serial_TXSTA=&pic->ram[P16E2_TXSTA];
       pic->serial_RCSTA=&pic->ram[P16E2_RCSTA];
       pic->serial_SPBRG=&pic->ram[P16E2_SPBRG];
       pic->serial_RCREG=&pic->ram[P16E2_RCREG];
       pic->serial_TXREG=&pic->ram[P16E2_TXREG];
       pic->serial_TXREG_ADDR=P16E2_TXREG;
       pic->serial_RCSTA_ADDR=P16E2_RCSTA;
       pic->serial_RCREG_ADDR=P16E2_RCREG;       
      */ 
       break;
     case P18:
       pic->serial_PIR1=&pic->ram[P18_PIR1];
       pic->serial_PIE1=&pic->ram[P18_PIE1];
       pic->serial_TXSTA=&pic->ram[P18_TXSTA];
       pic->serial_RCSTA=&pic->ram[P18_RCSTA];
       pic->serial_SPBRG=&pic->ram[P18_SPBRG];
       pic->serial_RCREG=&pic->ram[P18_RCREG];
       pic->serial_TXREG=&pic->ram[P18_TXREG];
       pic->serial_TXREG_ADDR=P18_TXREG;
       pic->serial_RCSTA_ADDR=P18_RCSTA;
       pic->serial_RCREG_ADDR=P18_RCREG;     
       pic->serial_TRIS_RX=&pic->ram[pic->pins[pic->usart[0]-1].port+0x12];
       pic->serial_TRIS_RX_MASK=(0x01 << pic->pins[pic->usart[0]-1].pord);
       break;
     default:
       break;
   }
   
   serial_open(pic);
  
   if(fname == NULL)
   {
     return 0;
   }
   else
     switch(pic->family)
     {
       case P16: 
       case P16E: 
       case P16E2:     
         return read_ihx(pic,fname,lrom);
         break; 
       case P18: 
         return read_ihx_18(pic,fname,lrom);
         break; 
       default:
         break;
     }
   
   return 0;
}

int 
pic_reset(_pic * pic, int flags)
{
  int i;

   //verify MCLRE
   if(((pic->processor==P16F628)||(pic->processor == P16F628A)||(pic->processor == P16F648A)||(pic->processor == P16F648AICD))&&(flags < 0)&&((pic->config[0] & 0x0020) == 0x0000) )  
   {
     return 0;
   }

   pic->w=0;
   pic->pc=0;
   pic->s2=0;
   pic->lram=0;
   pic->rram=0;
  
 
   pic->jpc=0xFFFFF;

   pic->t0cki_=0;  
   pic->t1cki_=1;  
 
   /*memory clear*/
   memset(pic->ram,0,pic->RAMSIZE); 
   memset(pic->stack,0,pic->STACKSIZE*2); 
   
   for(i=0; i < pic->PINCOUNT; i++)
   {
     pic->pins[i].avalue=0;
     pic->pins[i].lvalue=0;
     pic->pins[i].pord=0;
     pic->pins[i].port=0;   
     pic->pins[i].value=0;        
     pic->pins[i].ptype=PT_CMOS;
     pic->pins[i].dir=PD_IN;
   }
   
   
   switch(pic->family)
   {
   case P16: 
   switch(pic->processor)
   {
     case P16F84A:
     case P16F628:
     case P16F628A:
     case P16F648A:
     case P16F648AICD:
       pic->pins[ 0].port=P16_PORTA;pic->pins[ 0].pord=2;
       pic->pins[ 1].port=P16_PORTA;pic->pins[ 1].pord=3;
       pic->pins[ 2].port=P16_PORTA;pic->pins[ 2].pord=4;
       pic->pins[ 3].port=P16_PORTA;pic->pins[ 3].pord=5;
       pic->pins[ 4].port=0    ;pic->pins[ 4].pord=-1;
       pic->pins[ 5].port=P16_PORTB;pic->pins[ 5].pord=0;
       pic->pins[ 6].port=P16_PORTB;pic->pins[ 6].pord=1;
       pic->pins[ 7].port=P16_PORTB;pic->pins[ 7].pord=2;
       pic->pins[ 8].port=P16_PORTB;pic->pins[ 8].pord=3;
       pic->pins[ 9].port=P16_PORTB;pic->pins[ 9].pord=4;
       pic->pins[10].port=P16_PORTB;pic->pins[10].pord=5;
       pic->pins[11].port=P16_PORTB;pic->pins[11].pord=6;
       pic->pins[12].port=P16_PORTB;pic->pins[12].pord=7;
       pic->pins[13].port=0    ;pic->pins[13].pord=-1;
       pic->pins[14].port=P16_PORTA;pic->pins[14].pord=6;
       pic->pins[15].port=P16_PORTA;pic->pins[15].pord=7;
       pic->pins[16].port=P16_PORTA;pic->pins[16].pord=0;
       pic->pins[17].port=P16_PORTA;pic->pins[17].pord=1; 

       pic->ccp[0]= 9;
      
       pic->usart[0]=7;
       pic->usart[1]=8;
	
       pic->pgc=12;
       pic->pgd=13;
       
       pic->sck=0;
       pic->sdo=0;
       pic->sdi=0;

       pic->t0cki=3;
       pic->t1cki=12;

       pic->int0=6;
       pic->int1=0;
       pic->int2=0;
 
       break;
     case P16F877:
     case P16F877A:
       pic->pins[ 0].port=0   ;pic->pins[ 0].pord=-1;
       pic->pins[ 1].port=P16_PORTA;pic->pins[ 1].pord=0;
       pic->pins[ 2].port=P16_PORTA;pic->pins[ 2].pord=1;
       pic->pins[ 3].port=P16_PORTA;pic->pins[ 3].pord=2;
       pic->pins[ 4].port=P16_PORTA;pic->pins[ 4].pord=3;
       pic->pins[ 5].port=P16_PORTA;pic->pins[ 5].pord=4;
       pic->pins[ 6].port=P16_PORTA;pic->pins[ 6].pord=5;
       pic->pins[ 7].port=P16_PORTE;pic->pins[ 7].pord=0;
       pic->pins[ 8].port=P16_PORTE;pic->pins[ 8].pord=1;
       pic->pins[ 9].port=P16_PORTE;pic->pins[ 9].pord=2;
       pic->pins[10].port=0    ;pic->pins[10].pord=-1;
       pic->pins[11].port=0    ;pic->pins[11].pord=-1;
       pic->pins[12].port=0    ;pic->pins[12].pord=-1;
       pic->pins[13].port=0    ;pic->pins[13].pord=-1;
       pic->pins[14].port=P16_PORTC;pic->pins[14].pord=0;
       pic->pins[15].port=P16_PORTC;pic->pins[15].pord=1;
       pic->pins[16].port=P16_PORTC;pic->pins[16].pord=2;
       pic->pins[17].port=P16_PORTC;pic->pins[17].pord=3;
       pic->pins[18].port=P16_PORTD;pic->pins[18].pord=0;
       pic->pins[19].port=P16_PORTD;pic->pins[19].pord=1;
       pic->pins[20].port=P16_PORTD;pic->pins[20].pord=2;
       pic->pins[21].port=P16_PORTD;pic->pins[21].pord=3;
       pic->pins[22].port=P16_PORTC;pic->pins[22].pord=4;
       pic->pins[23].port=P16_PORTC;pic->pins[23].pord=5;
       pic->pins[24].port=P16_PORTC;pic->pins[24].pord=6;
       pic->pins[25].port=P16_PORTC;pic->pins[25].pord=7;
       pic->pins[26].port=P16_PORTD;pic->pins[26].pord=4;
       pic->pins[27].port=P16_PORTD;pic->pins[27].pord=5;
       pic->pins[28].port=P16_PORTD;pic->pins[28].pord=6;
       pic->pins[29].port=P16_PORTD;pic->pins[29].pord=7;
       pic->pins[30].port=0    ;pic->pins[30].pord=-1;
       pic->pins[31].port=0    ;pic->pins[31].pord=-1;
       pic->pins[32].port=P16_PORTB;pic->pins[32].pord=0;
       pic->pins[33].port=P16_PORTB;pic->pins[33].pord=1;
       pic->pins[34].port=P16_PORTB;pic->pins[34].pord=2;
       pic->pins[35].port=P16_PORTB;pic->pins[35].pord=3;
       pic->pins[36].port=P16_PORTB;pic->pins[36].pord=4;
       pic->pins[37].port=P16_PORTB;pic->pins[37].pord=5;
       pic->pins[38].port=P16_PORTB;pic->pins[38].pord=6;
       pic->pins[39].port=P16_PORTB;pic->pins[39].pord=7;
       
       pic->ccp[0]=  17;
       pic->ccp[1]=  16;

       pic->adc[0]=2;
       pic->adc[1]=3;
       pic->adc[2]=4;
       pic->adc[3]=5;
       pic->adc[4]=7;
       pic->adc[5]=8;
       pic->adc[6]=9;
       pic->adc[7]=10;
       
       pic->usart[0]=26;
       pic->usart[1]=25;
       
       pic->pgc=39;
       pic->pgd=40;

       pic->sck=18;
       pic->sdo=24;
       pic->sdi=23;
       
       pic->t0cki=6;
       pic->t1cki=15;
       
       pic->int0=33;
       pic->int1=0;
       pic->int2=0;

       break;
     case P16F777:
       pic->pins[ 0].port=0   ;pic->pins[ 0].pord=-1;
       pic->pins[ 1].port=P16_PORTA;pic->pins[ 1].pord=0;
       pic->pins[ 2].port=P16_PORTA;pic->pins[ 2].pord=1;
       pic->pins[ 3].port=P16_PORTA;pic->pins[ 3].pord=2;
       pic->pins[ 4].port=P16_PORTA;pic->pins[ 4].pord=3;
       pic->pins[ 5].port=P16_PORTA;pic->pins[ 5].pord=4;
       pic->pins[ 6].port=P16_PORTA;pic->pins[ 6].pord=5;
       pic->pins[ 7].port=P16_PORTE;pic->pins[ 7].pord=0;
       pic->pins[ 8].port=P16_PORTE;pic->pins[ 8].pord=1;
       pic->pins[ 9].port=P16_PORTE;pic->pins[ 9].pord=2;
       pic->pins[10].port=0    ;pic->pins[10].pord=-1;
       pic->pins[11].port=0    ;pic->pins[11].pord=-1;
       pic->pins[12].port=0    ;pic->pins[12].pord=-1;
       pic->pins[13].port=0    ;pic->pins[13].pord=-1;
       pic->pins[14].port=P16_PORTC;pic->pins[14].pord=0;
       pic->pins[15].port=P16_PORTC;pic->pins[15].pord=1;
       pic->pins[16].port=P16_PORTC;pic->pins[16].pord=2;
       pic->pins[17].port=P16_PORTC;pic->pins[17].pord=3;
       pic->pins[18].port=P16_PORTD;pic->pins[18].pord=0;
       pic->pins[19].port=P16_PORTD;pic->pins[19].pord=1;
       pic->pins[20].port=P16_PORTD;pic->pins[20].pord=2;
       pic->pins[21].port=P16_PORTD;pic->pins[21].pord=3;
       pic->pins[22].port=P16_PORTC;pic->pins[22].pord=4;
       pic->pins[23].port=P16_PORTC;pic->pins[23].pord=5;
       pic->pins[24].port=P16_PORTC;pic->pins[24].pord=6;
       pic->pins[25].port=P16_PORTC;pic->pins[25].pord=7;
       pic->pins[26].port=P16_PORTD;pic->pins[26].pord=4;
       pic->pins[27].port=P16_PORTD;pic->pins[27].pord=5;
       pic->pins[28].port=P16_PORTD;pic->pins[28].pord=6;
       pic->pins[29].port=P16_PORTD;pic->pins[29].pord=7;
       pic->pins[30].port=0    ;pic->pins[30].pord=-1;
       pic->pins[31].port=0    ;pic->pins[31].pord=-1;
       pic->pins[32].port=P16_PORTB;pic->pins[32].pord=0;
       pic->pins[33].port=P16_PORTB;pic->pins[33].pord=1;
       pic->pins[34].port=P16_PORTB;pic->pins[34].pord=2;
       pic->pins[35].port=P16_PORTB;pic->pins[35].pord=3;
       pic->pins[36].port=P16_PORTB;pic->pins[36].pord=4;
       pic->pins[37].port=P16_PORTB;pic->pins[37].pord=5;
       pic->pins[38].port=P16_PORTB;pic->pins[38].pord=6;
       pic->pins[39].port=P16_PORTB;pic->pins[39].pord=7;
       
       pic->ccp[0]=  17;
       pic->ccp[1]=  16;
       pic->ccp[2]=  38;

       pic->adc[0]=2;
       pic->adc[1]=3;
       pic->adc[2]=4;
       pic->adc[3]=5;
       pic->adc[4]=7;
       pic->adc[5]=8;
       pic->adc[6]=9;
       pic->adc[7]=10;
       pic->adc[8]=35;
       pic->adc[9]=36;
       pic->adc[10]=34;
       pic->adc[11]=37;
       pic->adc[12]=33;
       pic->adc[13]=38; 
       
       pic->usart[0]=26;
       pic->usart[1]=25;
       
       pic->pgc=39;
       pic->pgd=40;

       pic->sck=18;
       pic->sdo=24;
       pic->sdi=23;
       
       pic->t0cki=6;
       pic->t1cki=15;
       
       pic->int0=33;
       pic->int1=0;
       pic->int2=0;

       break;
     default:
       break;
    }
     if(abs(flags) == 1)
     {
       pic->ram[(0x0000)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0080)|(P16_STATUS & 0x007F)]=0x18;
       if(pic->processor != P16F84A)
       {
         pic->ram[(0x0100)|(P16_STATUS & 0x007F)]=0x18;
         pic->ram[(0x0180)|(P16_STATUS & 0x007F)]=0x18;
       } 
     }	

     pic->ram[P16_TRISA]=0xFF; 
     pic->ram[P16_TRISB]=0xFF; 
     pic->ram[P16_TRISC]=0xFF; 
     pic->ram[P16_TRISD]=0xFF; 
     pic->ram[P16_TRISE]=0x07; 
     pic->ram[P16_PR2]=0xFF; 
     periferic16_rst(pic);

     break;
    case P16E:  
    switch(pic->processor)
    {
     case P16F1619:
       pic->pins[ 0].port=0         ;pic->pins[ 0].pord=-1;
       pic->pins[ 1].port=P16E_PORTA;pic->pins[ 1].pord=5;
       pic->pins[ 2].port=P16E_PORTA;pic->pins[ 2].pord=4;
       pic->pins[ 3].port=P16E_PORTA;pic->pins[ 3].pord=3;
       pic->pins[ 4].port=P16E_PORTC;pic->pins[ 4].pord=5;
       pic->pins[ 5].port=P16E_PORTC;pic->pins[ 5].pord=4;
       pic->pins[ 6].port=P16E_PORTC;pic->pins[ 6].pord=3;
       pic->pins[ 7].port=P16E_PORTC;pic->pins[ 7].pord=6;
       pic->pins[ 8].port=P16E_PORTC;pic->pins[ 8].pord=7;
       pic->pins[ 9].port=P16E_PORTB;pic->pins[ 9].pord=7;
       pic->pins[10].port=P16E_PORTB;pic->pins[10].pord=6;
       pic->pins[11].port=P16E_PORTB;pic->pins[11].pord=5;
       pic->pins[12].port=P16E_PORTB;pic->pins[12].pord=4;
       pic->pins[13].port=P16E_PORTC;pic->pins[13].pord=2;
       pic->pins[14].port=P16E_PORTC;pic->pins[14].pord=1;
       pic->pins[15].port=P16E_PORTC;pic->pins[15].pord=0;
       pic->pins[16].port=P16E_PORTA;pic->pins[16].pord=2;
       pic->pins[17].port=P16E_PORTA;pic->pins[17].pord=1;
       pic->pins[18].port=P16E_PORTA;pic->pins[18].pord=0;
       pic->pins[19].port=0         ;pic->pins[19].pord=-1;

	
       pic->pgc=17;
       pic->pgd=18;
       
       pic->adc[0]=19;
       pic->adc[1]=18;
       pic->adc[2]=17;
       pic->adc[3]=3;
       pic->adc[4]=16;
       pic->adc[5]=15;
       pic->adc[6]=14;
       pic->adc[7]=7;
       pic->adc[8]=8;
       pic->adc[9]=9;
       pic->adc[10]=13;
       pic->adc[11]=12;
       
       //Default values
     
       pic->ccp[0]= 5; 
       pic->ccp[1]= 7; 
      
       pic->usart[0]=12;  
       pic->usart[1]=0;  

       pic->sck=11;
       pic->sdo=0;
       pic->sdi=13;

       pic->t0cki=17;
       pic->t1cki=2;
       
       //FIXME P16E pin interrupt support
/*
       pic->int0=6;
       pic->int1=0;
       pic->int2=0;
 */      
       
       break;
     default:
       break;
    }
    if(abs(flags) == 1)
     {
       pic->ram[(0x0000)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0080)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0100)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0180)|(P16_STATUS & 0x007F)]=0x18;
     }	

     pic->ram[P16_TRISA]=0xFF; 
     pic->ram[P16_TRISB]=0xFF; 
     pic->ram[P16_TRISC]=0xFF; 
     pic->ram[P16_TRISD]=0xFF; 
     pic->ram[P16_TRISE]=0x07; 
     pic->ram[P16_PR2]=0xFF; 
     periferic16E_rst(pic);
     break;
    case P16E2:  
    switch(pic->processor)
    {   
     case P16F18855: //QFN !!!!
       pic->pins[ 0].port=P16E2_PORTA;pic->pins[ 0].pord=2;
       pic->pins[ 1].port=P16E2_PORTA;pic->pins[ 1].pord=3;
       pic->pins[ 2].port=P16E2_PORTA;pic->pins[ 2].pord=4;
       pic->pins[ 3].port=P16E2_PORTA;pic->pins[ 3].pord=5;
       pic->pins[ 4].port=0         ;pic->pins[ 4].pord=-1;
       pic->pins[ 5].port=P16E2_PORTA;pic->pins[ 5].pord=6;
       pic->pins[ 6].port=P16E2_PORTA;pic->pins[ 6].pord=7;
       pic->pins[ 7].port=P16E2_PORTC;pic->pins[ 7].pord=0;
       pic->pins[ 8].port=P16E2_PORTC;pic->pins[ 8].pord=1;
       pic->pins[ 9].port=P16E2_PORTC;pic->pins[ 9].pord=2;
       pic->pins[10].port=P16E2_PORTC;pic->pins[10].pord=3;
       pic->pins[11].port=P16E2_PORTC;pic->pins[11].pord=4;
       pic->pins[12].port=P16E2_PORTC;pic->pins[12].pord=5;
       pic->pins[13].port=P16E2_PORTC;pic->pins[13].pord=6;
       pic->pins[14].port=P16E2_PORTC;pic->pins[14].pord=7;
       pic->pins[15].port=0         ;pic->pins[15].pord=-1;
       pic->pins[16].port=0         ;pic->pins[16].pord=-1;
       pic->pins[17].port=P16E2_PORTB;pic->pins[17].pord=0;
       pic->pins[18].port=P16E2_PORTB;pic->pins[18].pord=1;
       pic->pins[19].port=P16E2_PORTB;pic->pins[19].pord=2;
       pic->pins[20].port=P16E2_PORTB;pic->pins[20].pord=3;
       pic->pins[21].port=P16E2_PORTB;pic->pins[21].pord=4;
       pic->pins[22].port=P16E2_PORTB;pic->pins[22].pord=5;
       pic->pins[23].port=P16E2_PORTB;pic->pins[23].pord=6;
       pic->pins[24].port=P16E2_PORTB;pic->pins[24].pord=7;
       pic->pins[25].port=P16E2_PORTE;pic->pins[25].pord=3; 
       pic->pins[26].port=P16E2_PORTA;pic->pins[26].pord=0;
       pic->pins[27].port=P16E2_PORTA;pic->pins[27].pord=1;

	
       pic->pgc=17;
       pic->pgd=18;
       
       pic->adc[0]=27;
       pic->adc[1]=28;
       pic->adc[2]=1;
       pic->adc[3]=2;
       pic->adc[4]=3;
       pic->adc[5]=4;
       pic->adc[6]=7;
       pic->adc[7]=6;

       pic->adc[8]=18;
       pic->adc[9]=19;
       pic->adc[10]=20;
       pic->adc[11]=21;
       pic->adc[12]=22;
       pic->adc[13]=23;
       pic->adc[14]=24;
       pic->adc[15]=25;
       
       pic->adc[16]=8;
       pic->adc[17]=9;
       pic->adc[18]=10;
       pic->adc[19]=11;
       pic->adc[20]=12;
       pic->adc[21]=13;
       pic->adc[22]=14;
       pic->adc[23]=15;


       //Default values

       pic->ccp[0]= 10; 
       pic->ccp[1]= 9; 
       pic->ccp[2]= 23; 
       pic->ccp[3]= 18; 
       pic->ccp[4]= 3; 
      
       pic->usart[0]=15;  
       pic->usart[1]=0;  

       pic->sck=11;
       pic->sdo=0;
       pic->sdi=12;

       pic->t0cki=3;
       pic->t1cki=8;
       
       //FIXME P16E pin interrupt support
/*
       pic->int0=6;
       pic->int1=0;
       pic->int2=0;
 */      
       
       break;
     default:
       break;
    }
     if(abs(flags) == 1)
     {
       pic->ram[(0x0000)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0080)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0100)|(P16_STATUS & 0x007F)]=0x18;
       pic->ram[(0x0180)|(P16_STATUS & 0x007F)]=0x18;
     }	

     pic->ram[P16_TRISA]=0xFF; 
     pic->ram[P16_TRISB]=0xFF; 
     pic->ram[P16_TRISC]=0xFF; 
     pic->ram[P16_TRISD]=0xFF; 
     pic->ram[P16_TRISE]=0x07; 
     pic->ram[P16_PR2]=0xFF; 
     periferic16E2_rst(pic);

     break; 
    case P18:
    switch(pic->processor)
    {
     case P18F452:
       pic->pins[ 0].port=0    ;pic->pins[ 0].pord=-1;
       pic->pins[ 1].port=P18_PORTA;pic->pins[ 1].pord=0;
       pic->pins[ 2].port=P18_PORTA;pic->pins[ 2].pord=1;
       pic->pins[ 3].port=P18_PORTA;pic->pins[ 3].pord=2;
       pic->pins[ 4].port=P18_PORTA;pic->pins[ 4].pord=3;
       pic->pins[ 5].port=P18_PORTA;pic->pins[ 5].pord=4;
       pic->pins[ 6].port=P18_PORTA;pic->pins[ 6].pord=5;
       pic->pins[ 7].port=P18_PORTE;pic->pins[ 7].pord=0;
       pic->pins[ 8].port=P18_PORTE;pic->pins[ 8].pord=1;
       pic->pins[ 9].port=P18_PORTE;pic->pins[ 9].pord=2;
       pic->pins[10].port=0    ;pic->pins[10].pord=-1;
       pic->pins[11].port=0    ;pic->pins[11].pord=-1;
       pic->pins[12].port=0    ;pic->pins[12].pord=-1;
       pic->pins[13].port=P18_PORTA;pic->pins[13].pord=6;
       pic->pins[14].port=P18_PORTC;pic->pins[14].pord=0;
       pic->pins[15].port=P18_PORTC;pic->pins[15].pord=1;
       pic->pins[16].port=P18_PORTC;pic->pins[16].pord=2;
       pic->pins[17].port=P18_PORTC;pic->pins[17].pord=3;
       pic->pins[18].port=P18_PORTD;pic->pins[18].pord=0;
       pic->pins[19].port=P18_PORTD;pic->pins[19].pord=1;
       pic->pins[20].port=P18_PORTD;pic->pins[20].pord=2;
       pic->pins[21].port=P18_PORTD;pic->pins[21].pord=3;
       pic->pins[22].port=P18_PORTC;pic->pins[22].pord=4;
       pic->pins[23].port=P18_PORTC;pic->pins[23].pord=5;
       pic->pins[24].port=P18_PORTC;pic->pins[24].pord=6;
       pic->pins[25].port=P18_PORTC;pic->pins[25].pord=7;
       pic->pins[26].port=P18_PORTD;pic->pins[26].pord=4;
       pic->pins[27].port=P18_PORTD;pic->pins[27].pord=5;
       pic->pins[28].port=P18_PORTD;pic->pins[28].pord=6;
       pic->pins[29].port=P18_PORTD;pic->pins[29].pord=7;
       pic->pins[30].port=0    ;pic->pins[30].pord=-1;
       pic->pins[31].port=0    ;pic->pins[31].pord=-1;
       pic->pins[32].port=P18_PORTB;pic->pins[32].pord=0;
       pic->pins[33].port=P18_PORTB;pic->pins[33].pord=1;
       pic->pins[34].port=P18_PORTB;pic->pins[34].pord=2;
       pic->pins[35].port=P18_PORTB;pic->pins[35].pord=3;
       pic->pins[36].port=P18_PORTB;pic->pins[36].pord=4;
       pic->pins[37].port=P18_PORTB;pic->pins[37].pord=5;
       pic->pins[38].port=P18_PORTB;pic->pins[38].pord=6;
       pic->pins[39].port=P18_PORTB;pic->pins[39].pord=7;
       
       pic->ccp[0]=  17;
       pic->ccp[1]=  16;//36

       pic->adc[0]=2;
       pic->adc[1]=3;
       pic->adc[2]=4;
       pic->adc[3]=5;
       pic->adc[4]=7;
       pic->adc[5]=8;
       pic->adc[6]=9;
       pic->adc[7]=10;
       
       pic->usart[0]=26;
       pic->usart[1]=25;
       
       pic->pgc=39;
       pic->pgd=40;

       pic->sck=18;
       pic->sdo=24;
       pic->sdi=23;
       
       pic->t0cki=6;
       pic->t1cki=15;
       
       pic->int0=33;
       pic->int1=34;
       pic->int2=35;

       break;
     case P18F4520:
     case P18F4620:
       pic->pins[ 0].port=P18_PORTE;pic->pins[ 0].pord=3;
       pic->pins[ 1].port=P18_PORTA;pic->pins[ 1].pord=0;
       pic->pins[ 2].port=P18_PORTA;pic->pins[ 2].pord=1;
       pic->pins[ 3].port=P18_PORTA;pic->pins[ 3].pord=2;
       pic->pins[ 4].port=P18_PORTA;pic->pins[ 4].pord=3;
       pic->pins[ 5].port=P18_PORTA;pic->pins[ 5].pord=4;
       pic->pins[ 6].port=P18_PORTA;pic->pins[ 6].pord=5;
       pic->pins[ 7].port=P18_PORTE;pic->pins[ 7].pord=0;
       pic->pins[ 8].port=P18_PORTE;pic->pins[ 8].pord=1;
       pic->pins[ 9].port=P18_PORTE;pic->pins[ 9].pord=2;
       pic->pins[10].port=0    ;pic->pins[10].pord=-1;
       pic->pins[11].port=0    ;pic->pins[11].pord=-1;
       pic->pins[12].port=P18_PORTA;pic->pins[12].pord=7;
       pic->pins[13].port=P18_PORTA;pic->pins[13].pord=6;
       pic->pins[14].port=P18_PORTC;pic->pins[14].pord=0;
       pic->pins[15].port=P18_PORTC;pic->pins[15].pord=1;
       pic->pins[16].port=P18_PORTC;pic->pins[16].pord=2;
       pic->pins[17].port=P18_PORTC;pic->pins[17].pord=3;
       pic->pins[18].port=P18_PORTD;pic->pins[18].pord=0;
       pic->pins[19].port=P18_PORTD;pic->pins[19].pord=1;
       pic->pins[20].port=P18_PORTD;pic->pins[20].pord=2;
       pic->pins[21].port=P18_PORTD;pic->pins[21].pord=3;
       pic->pins[22].port=P18_PORTC;pic->pins[22].pord=4;
       pic->pins[23].port=P18_PORTC;pic->pins[23].pord=5;
       pic->pins[24].port=P18_PORTC;pic->pins[24].pord=6;
       pic->pins[25].port=P18_PORTC;pic->pins[25].pord=7;
       pic->pins[26].port=P18_PORTD;pic->pins[26].pord=4;
       pic->pins[27].port=P18_PORTD;pic->pins[27].pord=5;
       pic->pins[28].port=P18_PORTD;pic->pins[28].pord=6;
       pic->pins[29].port=P18_PORTD;pic->pins[29].pord=7;
       pic->pins[30].port=0    ;pic->pins[30].pord=-1;
       pic->pins[31].port=0    ;pic->pins[31].pord=-1;
       pic->pins[32].port=P18_PORTB;pic->pins[32].pord=0;
       pic->pins[33].port=P18_PORTB;pic->pins[33].pord=1;
       pic->pins[34].port=P18_PORTB;pic->pins[34].pord=2;
       pic->pins[35].port=P18_PORTB;pic->pins[35].pord=3;
       pic->pins[36].port=P18_PORTB;pic->pins[36].pord=4;
       pic->pins[37].port=P18_PORTB;pic->pins[37].pord=5;
       pic->pins[38].port=P18_PORTB;pic->pins[38].pord=6;
       pic->pins[39].port=P18_PORTB;pic->pins[39].pord=7;
       
       pic->ccp[0]=  17;
       pic->ccp[1]=  16;//36

       pic->adc[0]=2;
       pic->adc[1]=3;
       pic->adc[2]=4;
       pic->adc[3]=5;
       pic->adc[4]=7;
       pic->adc[5]=8;
       pic->adc[6]=9;
       pic->adc[7]=10;
       pic->adc[8]=35;
       pic->adc[9]=36;
       pic->adc[10]=34;
       pic->adc[11]=37;
       pic->adc[12]=33;
       
       pic->usart[0]=26;
       pic->usart[1]=25;
       
       pic->pgc=39;
       pic->pgd=40;

       pic->sck=18;
       pic->sdo=24;
       pic->sdi=23;
       
       pic->t0cki=6;
       pic->t1cki=15;
       
       pic->int0=33;
       pic->int1=34;
       pic->int2=35;

       break;
     case P18F4550:
     case P18F45K50:         
       pic->pins[ 0].port=P18_PORTE;pic->pins[ 0].pord=3;
       pic->pins[ 1].port=P18_PORTA;pic->pins[ 1].pord=0;
       pic->pins[ 2].port=P18_PORTA;pic->pins[ 2].pord=1;
       pic->pins[ 3].port=P18_PORTA;pic->pins[ 3].pord=2;
       pic->pins[ 4].port=P18_PORTA;pic->pins[ 4].pord=3;
       pic->pins[ 5].port=P18_PORTA;pic->pins[ 5].pord=4;
       pic->pins[ 6].port=P18_PORTA;pic->pins[ 6].pord=5;
       pic->pins[ 7].port=P18_PORTE;pic->pins[ 7].pord=0;
       pic->pins[ 8].port=P18_PORTE;pic->pins[ 8].pord=1;
       pic->pins[ 9].port=P18_PORTE;pic->pins[ 9].pord=2;
       pic->pins[10].port=0    ;pic->pins[10].pord=-1;
       pic->pins[11].port=0    ;pic->pins[11].pord=-1;
       pic->pins[12].port=0    ;pic->pins[12].pord=-1;
       pic->pins[13].port=P18_PORTA;pic->pins[13].pord=6;
       pic->pins[14].port=P18_PORTC;pic->pins[14].pord=0;
       pic->pins[15].port=P18_PORTC;pic->pins[15].pord=1;
       pic->pins[16].port=P18_PORTC;pic->pins[16].pord=2;
       pic->pins[17].port=0    ;pic->pins[17].pord=-1;
       pic->pins[18].port=P18_PORTD;pic->pins[18].pord=0;
       pic->pins[19].port=P18_PORTD;pic->pins[19].pord=1;
       pic->pins[20].port=P18_PORTD;pic->pins[20].pord=2;
       pic->pins[21].port=P18_PORTD;pic->pins[21].pord=3;
       pic->pins[22].port=P18_PORTC;pic->pins[22].pord=4;
       pic->pins[23].port=P18_PORTC;pic->pins[23].pord=5;
       pic->pins[24].port=P18_PORTC;pic->pins[24].pord=6;
       pic->pins[25].port=P18_PORTC;pic->pins[25].pord=7;
       pic->pins[26].port=P18_PORTD;pic->pins[26].pord=4;
       pic->pins[27].port=P18_PORTD;pic->pins[27].pord=5;
       pic->pins[28].port=P18_PORTD;pic->pins[28].pord=6;
       pic->pins[29].port=P18_PORTD;pic->pins[29].pord=7;
       pic->pins[30].port=0    ;pic->pins[30].pord=-1;
       pic->pins[31].port=0    ;pic->pins[31].pord=-1;
       pic->pins[32].port=P18_PORTB;pic->pins[32].pord=0;
       pic->pins[33].port=P18_PORTB;pic->pins[33].pord=1;
       pic->pins[34].port=P18_PORTB;pic->pins[34].pord=2;
       pic->pins[35].port=P18_PORTB;pic->pins[35].pord=3;
       pic->pins[36].port=P18_PORTB;pic->pins[36].pord=4;
       pic->pins[37].port=P18_PORTB;pic->pins[37].pord=5;
       pic->pins[38].port=P18_PORTB;pic->pins[38].pord=6;
       pic->pins[39].port=P18_PORTB;pic->pins[39].pord=7;
       
       pic->ccp[0]=  17;
       pic->ccp[1]=  16;//36

       pic->adc[0]=2;
       pic->adc[1]=3;
       pic->adc[2]=4;
       pic->adc[3]=5;
       pic->adc[4]=7;
       pic->adc[5]=8;
       pic->adc[6]=9;
       pic->adc[7]=10;

       pic->adc[8]=35;
       pic->adc[9]=36;
       pic->adc[10]=34;
       pic->adc[11]=37;
       pic->adc[12]=33;
       
       pic->usart[0]=26;
       pic->usart[1]=25;
       
       pic->pgc=39;
       pic->pgd=40;

       pic->sck=-1;
       pic->sdo=-1;
       pic->sdi=-1;
       
       pic->t0cki=6;
       pic->t1cki=15;
       
       pic->int0=33;
       pic->int1=34;
       pic->int2=35;

       pic->serialbaud=9600; 
       pic->serialexbaud=9600.0; 

       break;
     default:
       break;
    }
     if(abs(flags) == 1)
     {
       pic->ram[P18_RCON]=0x0C;
     }	

     pic->ram[P18_TRISA]=0xFF; 
     pic->ram[P18_TRISB]=0xFF; 
     pic->ram[P18_TRISC]=0xFF; 
     pic->ram[P18_TRISD]=0xFF; 
     pic->ram[P18_TRISE]=0x07; 
     pic->ram[P18_PR2]=0xFF; 
     pic->ram[P18_STKPTR]=0x00;
   
     periferic18_rst(pic);
     break;
    default:
    break;
    }
   

 //  pic->debug=0;

   return 1;
};

void
pic_step(_pic * pic)
{
  switch(pic->family)
  { 
    case P16:
      pic_decode_16(pic);
      periferic16_step_in(pic);
      periferic16_step_out(pic);
      break;
    case P16E:  
      pic_decode_16E(pic);
      periferic16E_step_in(pic);
      periferic16E_step_out(pic);
      break;  
    case P16E2:  
      pic_decode_16E(pic);
      periferic16E2_step_in(pic);
      periferic16E2_step_out(pic);
      break;    
    case P18:
      pic_decode_18(pic);
      periferic18_step_in(pic);
      periferic18_step_out(pic);
      break;
  }
}



void
pic_end(_pic * pic)
{
  if(pic->ram)free(pic->ram);
  if(pic->prog)free(pic->prog);
  if(pic->id)free(pic->id);
  if(pic->config)free(pic->config);
  if(pic->stack)free(pic->stack);
  if(pic->eeprom)free(pic->eeprom);
  if(pic->pins)free(pic->pins);
  if(pic->ccp)free(pic->ccp);
  if(pic->adc)free(pic->adc);
  if(pic->usart)free(pic->usart);

  serial_close(pic);
}

unsigned char 
pic_get_pin(_pic * pic, unsigned char pin)
{
   if((pin-1) < pic->PINCOUNT)
   {
//     if((pic->pins[(pin-1)].type == P_OC)||(pic->pins[(pin-1)].type == P_OT))
//     {
       return pic->pins[(pin-1)].value;
//     }
//     else
//     {
//       return 0;
//     } 
   }
   else
     return 0;  
};


int 
pic_set_pin(_pic * pic,unsigned char pin,unsigned char value)
{
   unsigned char val;

   if((pin-1) < pic->PINCOUNT)
   {
    if (pic->pins[(pin-1)].value == value) return 1;
        
     if((pic->pins[(pin-1)].dir == PD_IN)&&(pic->pins[(pin-1)].ptype < 3))
     {
          pic->pins[(pin-1)].value=value;
          
          if(pic->pins[(pin-1)].port)
          {
     	      val=0x01<<(pic->pins[(pin-1)].pord);
              if((pic->pins[(pin-1)].value) == 1)
                pic->ram[pic->pins[(pin-1)].port]|=val;
              else	
                pic->ram[pic->pins[(pin-1)].port]&=~val;
	     switch(pic->family)
             {
                case P16:
                case P16E: //FIXME verificar P16E set pin with ansel
                case P16E2: //FIXME verificar P16E set pin with ansel    
                  pic->ram[pic->pins[(pin-1)].port | 0x100]=pic->ram[pic->pins[(pin-1)].port]; //espelhamento bank2 = bank0
                  break;
                case P18: 
                  if((pic->pins[(pin-1)].port == P18_PORTB)&&(pic->ram[P18_DEBUG] & 0x80)) 
                  {  
                    pic->ram[P18_DEBUG]= (pic->ram[P18_DEBUG]&0xF3)|((pic->ram[P18_PORTB] &0xC0)>>4) ; //espelhamento debug
                  } 
                  break;  
             }  
	   }

       return 1; 
     }
     else
     { 
       return 0;
     } 
   } 
   else
     return 0;
};


int 
pic_set_apin(_pic * pic,unsigned char pin,float value)
{

   if((pin-1) < pic->PINCOUNT)
   {
     if (pic->pins[(pin-1)].avalue == value) return 1;

     if((pic->pins[(pin-1)].dir == PD_IN)&&(pic->pins[(pin-1)].ptype == PT_ANALOG))
     {
       pic->pins[(pin-1)].avalue=value;
       return 1; 
     }
     else
     { 
       pic->pins[(pin-1)].avalue=0;
       return 0;
     } 
   } 
   else
     return 0;
};


unsigned char 
pic_get_pin_type(_pic * pic, unsigned char pin)
{
   if((pin-1) < pic->PINCOUNT)
   {
       return pic->pins[(pin-1)].ptype;
   }
   else
     return 0;  
};

unsigned char 
pic_get_pin_dir(_pic * pic, unsigned char pin)
{
   if((pin-1) < pic->PINCOUNT)
   {
       return pic->pins[(pin-1)].dir;
   }
   else
     return 0;  
};



int 
pic_set_pin_DOV(_pic * pic,unsigned char pin,unsigned char value)
{
   if((pin-1) < pic->PINCOUNT)
   {
    if (pic->pins[(pin-1)].ovalue == value) return 1;
        
    pic->pins[(pin-1)].ovalue=value;
    return 1;        
   } 
   else
     return 0;
}

unsigned char 
pic_get_pin_DOV(_pic * pic, unsigned char pin)
{
   if((pin-1) < pic->PINCOUNT)
   {
      return pic->pins[(pin-1)].ovalue;
   }
   else
     return 0;  
}


