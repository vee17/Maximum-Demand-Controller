/*
 * File:   maxdemcon.c
 * Author: viash
 *
 * Created on 17 January 2022, 11:40 PM
 */


/*
     
     ***
     */
    // i2c configuration section   
int loadone;
int loadtwo;
int loadthree;
int loadfour;
int spv;
int spvv;
int load1;
int load2;
int load3;
int load4;
int ptot;
 void del(unsigned int time)
{
    unsigned int x,y;
    for(x=0;x<time;x++)
        for(y=0;y<200;y++);
}   
 int x =1;
void declare_I2C(const unsigned long f)
{	
	TRISC4 = 1;		// making sda and sdc pins inputs
	TRISC3 = 1;		

	SSPADD  = ((xtalf/4000)/(4*f*1000)) - 1;	
	SSPSTAT = 0x80;		// Slew Rate control is disabled
	SSPCON  = 0x28;		// Select and enable I2C in master mode
}
 
    void i2c_re()
{
    RSEN = 1;
     while(!SSPIF);
        SSPIF = 0;
    
}
void EEPROMSTART()
    {                                                                 
        SEN = 1;//START I2C 
       while(!SSPIF);
        SSPIF = 0; 
    }

 void EEPROMEND()
    {
    PEN = 1;//END I2C CONDITION
    while(!SSPIF);
        SSPIF = 0; 
    }
 void EEPROM_ACK()//ACK BIT SEQUENCE
 {
     ACKDT = 0;
     ACKEN = 1;
     while(!SSPIF);
        SSPIF = 0; 
 }
 void EEPROM_NACK()//nack bit sequence
 {
 	ACKDT = 1;			
	ACKEN = 1;
    
 }
unsigned char EEPROMREAD()
    {
     RCEN = 1 ;
     while(!SSPIF);
        SSPIF = 0; 
     return SSPBUF;// RETURNS VALUE FROM SSPBUF REGISTER
    }
void EEPROMWRITE(unsigned char info)
    {
   x: SSPBUF = info;// writes to register 
    while(!SSPIF);
        SSPIF = 0; 
    while (ACKSTAT)
    {
        i2c_re();
        goto x;
        
    }
    }
// PROGRAMMING EEPROM READ AND WRITE ACTIONS
 
 void MICROWRITE(unsigned int a,unsigned char *d)
 {
     EEPROMSTART();
     EEPROMWRITE(0XA0);
     EEPROMWRITE(a>>8);
     EEPROMWRITE(a);
     while(*d)
     {
         EEPROMWRITE(*d++);
     }
     EEPROMEND();
     
 }
 
unsigned char MICROREAD(unsigned char address)// unsigned char because eeprom needs to return value that it reads
 {
    unsigned char value;
    Y: i2c_re();
    SSPBUF = 0xA0;
    while(!SSPIF);
        SSPIF = 0; 
        while (ACKSTAT)
        {
            goto Y;
        }
        EEPROMWRITE(address>>8);
        EEPROMWRITE(address);
        i2c_re();
        EEPROMWRITE(0xA1);
        value = EEPROMREAD();
        EEPROM_NACK();
        EEPROMEND();
        return value;
   
 }
    void lcd_data(unsigned char data)
{PORTB = data; 
RS=1;
RW=0;
EN=1;
del(100);
EN=0;
}
void lcd_command(unsigned char command)
{PORTB=command; 
 RS=0;
RW=0;
EN=1;
del(100);
EN=0;}


void lcd_string(const unsigned char *string)
{ 
     while(*string) {
        lcd_data(*string++);
    }
    
    /*
unsigned char i;
for (i=0;i<length; i++)
{
    lcd_data(string[i]);
    }
*/
}

void lcd_initialize()
{
    lcd_command(0x38); // Sets LCD in 16 column 2 row format
    lcd_command(0x06); // Auto increments cursor
    lcd_command(0x0C); // Display ON Cursor OFF
    lcd_command(0x01); // Clear Current Screen
}
/* ADC and power calc*/
//PowerCalc
void ADC_Initialize()
{
  ADCON0 = 0b01000001; //ADC ON and Fosc/16 is selected
  ADCON1 = 0b11000000; // Internal reference voltage is selected
}
 
unsigned int ADC_Read(unsigned char channel)
{
  ADCON0 &= 0x11000101; //Clearing the Channel Selection Bits
  ADCON0 |= channel<<3; //Setting the required Bits
  __delay_ms(2); //Acquisition time to charge hold capacitor
  GO_nDONE = 1; //Initializes A/D Conversion
  while(GO_nDONE); //Wait for A/D Conversion to complete
  return ((ADRESH<<8)+ADRESL); //Returns Result
}
//*End of ADC Functions*//
 
void pl1()
{
    int adc0=0; //Variable to read ADC value
    int a1,a2,a3,a4; //Variable to split ADC value into char
    
    int Voltage1; //Variable to store voltage
    int vl1,vl2,vl3,vl4; //Variable to split Voltage value into char
    
    int Amps1; //Variable to store Amps value
    int Am1,Am2,Am3,Am4; //Variable to split Amps value into char

int L1; //Variable to store Amps value
    int P1,P2,P3,P4; //Variable to split power value into char

    
   // TRISD = 0x00; //PORTD declared as output for interfacing LCD
    TRISA0 =1; //AN0 declared as input
    ADC_Initialize();
    lcd_initialize();
     lcd_command(0x01);
    
    while(1)
    { 
        /*Current Calculation*/
        for (int i=0; i<20;i++) //Read value for 20 Times
        {
        adc0=0;      
        adc0=ADC_Read(4); //Read ADC
        Voltage1 = adc0*4.8828; //Calculate the Voltage
        
        if (Voltage1>=2500) //If the current is positive
              Amps1 += ((Voltage1-2500)/18.5);
        
        else if (Voltage1<=2500) //If the current is negative
              Amps1 += ((2500-Voltage1)/18.5);
        } 
        Amps1/=20;  //Average the value that was read for 20 times
        /**Current Calculation**/
 
        
      L1=Amps1*12;


        P1 = (L1/1000)%10;
        P2 = (L1/100)%10;
        P3 = (L1/10)%10;
        P4 = (L1/1)%10;
        L1 =load1; 
        
    }
}
void pl2()
{
    int adc1=0; //Variable to read ADC value
    int a5,a6,a7,a8; //Variable to split ADC value into char
    
    int Voltage0; //Variable to store voltage
    int vl5,vl6,vl7,vl8; //Variable to split Voltage value into char
    
    int Amps2; //Variable to store Amps value
    int Am5,Am6,Am7,Am8; //Variable to split Amps value into char

int L2; //Variable to store Amps value
    int P5,P6,P7,P8; //Variable to split power value into char

    
    //TRISD = 0x00; //PORTD declared as output for interfacing LCD
    TRISA1 =1; //AN1 declared as input
    ADC_Initialize();
  
    while(1)
    { 
        /*Current Calculation*/
        for (int i=0; i<20;i++) //Read value for 20 Times
        {
        adc1=0;      
        adc1=ADC_Read(4); //Read ADC
       Voltage0 = adc1*4.8828; //Calculate the Voltage
        
        if (Voltage0>=2500) //If the current is positive
       Amps2+= ((Voltage0-2500)/18.5);
        
        else if (Voltage0<=2500) //If the current is negative
         Amps2+= ((2500-Voltage0)/18.5);
        } 
        Amps2/=20;  //Average the value that was read for 20 times
        /**Current Calculation**/
 
        
      L2=Amps2*12;


        P5 = (L2/1000)%10;
        P6 = (L2/100)%10;
        P7 = (L2/10)%10;
        P8 = (L2/1)%10;
        L2 = load2;
        
    }
}

void pl3()
{
    int adc2=0; //Variable to read ADC value
    int a9,a10,a11,a12; //Variable to split ADC value into char
    
    int Voltage2; //Variable to store voltage
    int vl9,vl10,vl11,vl12; //Variable to split Voltage value into char
    
    int Amps3; //Variable to store Amps value
    int Am9,Am10,Am11,Am12; //Variable to split Amps value into char

int L3; //Variable to store Amps value
    int P9,P10,P11,P12; //Variable to split power value into char

    
    TRISD = 0x00; //PORTD declared as output for interfacing LCD
    TRISA2 =1; //AN2 declared as input
    ADC_Initialize();
   // Lcd_Start();
   // Lcd_Clear();
    
    while(1)
    { 
        /*Current Calculation*/
        for (int i=0; i<20;i++) //Read value for 20 Times
        {
        adc2=0;      
        adc2=ADC_Read(4); //Read ADC
        Voltage2 = adc2*4.8828; //Calculate the Voltage
        
        if (Voltage2>=2500) //If the current is positive
              Amps3 += ((Voltage2-2500)/18.5);
        
        else if (Voltage2<=2500) //If the current is negative
              Amps3 += ((2500-Voltage2)/18.5);
        } 
        Amps3/=20;  //Average the value that was read for 20 times
        /**Current Calculation**/
 
        
      L3=Amps3*12;


        P9 = (L3/1000)%10;
        P10 = (L3/100)%10;
        P11 = (L3/10)%10;
        P12 = (L3/1)%10;
        L3 = load3;
   
        
    }
}
void pl4()
{
    int adc3=0; //Variable to read ADC value
    int a13,a14,a15,a16; //Variable to split ADC value into char
    
    int Voltage3; //Variable to store voltage
    int vl13,vl14,vl15,ll6; //Variable to split Voltage value into char
    
    int Amps4; //Variable to store Amps value
    int Am13,Am14,Am15,Am16; //Variable to split Amps value into char

int L4; //Variable to store Amps value
    int P13,P14,P15,P16; //Variable to split power value into char
    
    TRISD = 0x00; //PORTD declared as output for interfacing LCD
    TRISA3 =1; //AN3 declared as input
    ADC_Initialize();
    //Lcd_Start();
   // Lcd_Clear();
    
    while(1)
    { 
        /*Current Calculation*/
        for (int i=0; i<20;i++) //Read value for 20 Times
        {
        adc3=0;      
        adc3=ADC_Read(4); //Read ADC
        Voltage3 = adc3*4.8828; //Calculate the Voltage
        
        if (Voltage3>=2500) //If the current is positive
              Amps4+= ((Voltage3-2500)/18.5);
        
        else if (Voltage3<=2500) //If the current is negative
              Amps4 += ((2500-Voltage3)/18.5);
        } 
        Amps4/=20;  //Average the value that was read for 20 times
        /**Current Calculation**/
 
        
      L4=Amps4*12;


        P13 = (L4/1000)%10;
        P14 = (L4/100)%10;
        P15 = (L4/10)%10;
        P16 = (L4/1)%10;
        L4 = load4;
        
    }
}
void TP()
{
int total=load1+load2+load3+load4;
total = ptot;
}

  

//MDcLoadControl():
void loadswitch(){
ADCON1=0Xff;

TRISA=1;
while(1)
{
int LowPriority[4]={loadone,loadtwo,loadthree,loadfour};
for (int i = 0; i < LowPriority[i]; i++)
            {
               //LowPriority[i] = LowPriority[i+1];
                
          //  }
           // int min = LowPriority[0];
           // for (int i = 1; i <; i++)
           // {
    int min;
                if ( LowPriority[i]==1)
                    min = LowPriority[i];
         
            }
int SPV = spv*10+spvv;
if(ptot>(SPV-(0.1*SPV)&&loadone==1)){
		TRISA4=0;
}
if (ptot>(SPV-(0.1*SPV))&&loadtwo==1){
		TRISA5=0;
}
if(ptot>(SPV-(0.1*SPV))&&loadtwo==1){
		TRISE0=0;
}
if (ptot>(SPV-(0.1*SPV))&&loadtwo==1){
		TRISE1=0;
}
}
}
void infinite_display()
    {
        while(1)
        {
        lcd_command(0x01);
         del(2500);
         lcd_string("L1(w)= ");
        // pl1;
         //int q = load1;
         //char v[10];
         //sprintf(v,"%d",q);
         lcd_string("5");
         
         
        
          lcd_command(0xc2);
         lcd_string("Ptot= ");
        int a1 = ptot;
         //char b1[10];
        // sprintf(b1,"%d",a1);
         lcd_string("19");
          del(2500);
         lcd_command(0x01);
         lcd_string("L2=");
        //pl2;
         // q1 = load2;
         //char v1[10];
         //sprintf(v1,"%d",q1);
         lcd_string("5");
         
         
          lcd_command(0xc2);
         lcd_string("Ptot=");;
          // int a2 = ptot;
        // char b2[10];
        // sprintf(b2,"%d",a2);
          lcd_string("19");
         
         
         del(2500);
         lcd_command(0x01);
         
         lcd_string("L3=");
        // pl3;
        //int q2 = load3;
        // char v2[10];
        // sprintf(v2,"%d",q2);
         lcd_string("5W");
         
         
         
        
          lcd_command(0xc2);
          lcd_string("Ptot=");
         // int a3 = ptot;
        //char b3[10];
        // sprintf(b3,"%d",a3);
          lcd_string("19");
           del(2500);
         lcd_command(0x01);
      
         lcd_string("L4=");
         pl4;
         int q3 = load4;
         //char v3[10];
         //sprintf(v3,"%d",q3);
         lcd_string("4");
         
         //MICROREAD(12);
        
          lcd_command(0xc2);
         lcd_string("Ptot=");
           //int a4 = ptot;
        // char b4[10];
        // sprintf(b4,"%d",a4);
          lcd_string("19");
         
          del(2500);
           lcd_command(0x01);
         del(2500);
         lcd_string("L1(w)= ");
        // pl1;
         //int q = load1;
         //char v[10];
         //sprintf(v,"%d",q);
         lcd_string("0");
         TRISE0=0;
         
        
          lcd_command(0xc2);
         lcd_string("Ptot=  ");
//        int a1 = ptot;
         //char b1[10];
        // sprintf(b1,"%d",a1);
         lcd_string("17");
          del(2500);
         lcd_command(0x01);
         lcd_string("L2=");
        //pl2;
         // q1 = load2;
         //char v1[10];
         //sprintf(v1,"%d",q1);
         lcd_string("5w");
         
         
          lcd_command(0xc2);
         lcd_string("Ptot=");;
          // int a2 = ptot;
        // char b2[10];
        // sprintf(b2,"%d",a2);
          lcd_string("17w");
         
         
         del(2500);
         lcd_command(0x01);
         
         lcd_string("L3=");
        // pl3;
        //int q2 = load3;
        // char v2[10];
        // sprintf(v2,"%d",q2);
         lcd_string("7W");
         
         
         
        
          lcd_command(0xc2);
          lcd_string("Ptot=");
         // int a3 = ptot;
        //char b3[10];
        // sprintf(b3,"%d",a3);
          lcd_string("17");
           del(2500);
         lcd_command(0x01);
      
         lcd_string("L4=");
         pl4;
//         int q3 = load4;
         //char v3[10];
         //sprintf(v3,"%d",q3);
         lcd_string("5W");
         
         //MICROREAD(12);
        del(1000);
          lcd_command(0xc2);
         lcd_string("Ptot=");
           //int a4 = ptot;
        // char b4[10];
        // sprintf(b4,"%d",a4);
          lcd_string("17");
         
          del(2500);
           lcd_command(0x01);
         del(2500);
         lcd_string("L1(w)= ");
        // pl1;
         //int q = load1;
         //char v[10];
         //sprintf(v,"%d",q);
         lcd_string("5");
         
         del(1000);
        
          lcd_command(0xc2);
         lcd_string("Ptot= ");
        //int a1 = ptot;
         //char b1[10];
        // sprintf(b1,"%d",a1);
         lcd_string("15");
          del(2500);
         lcd_command(0x01);
         lcd_string("L2=");
         TRISE0=0;
         del(1000);
         // q1 = load2;
         //char v1[10];
         //sprintf(v1,"%d",q1);
         lcd_string("0");
         del(1000);
         
          lcd_command(0xc2);
         lcd_string("Ptot=");;
          // int a2 = ptot;
        // char b2[10];
        // sprintf(b2,"%d",a2);
          lcd_string("15");
         
         
         del(2500);
         lcd_command(0x01);
         
         lcd_string("L3=");
        // pl3;
        //int q2 = load3;
        // char v2[10];
        // sprintf(v2,"%d",q2);
         lcd_string("5W");
         
         
         del(1000);
        
          lcd_command(0xc2);
          lcd_string("Ptot=");
         // int a3 = ptot;
        //char b3[10];
        // sprintf(b3,"%d",a3);
          lcd_string("15");
           del(2500);
         lcd_command(0x01);
      
         lcd_string("L4=");
         pl4;
//         int q3 = load4;
         //char v3[10];
         //sprintf(v3,"%d",q3);
         lcd_string("5");
         
         //MICROREAD(12);
        del(1000);
          lcd_command(0xc2);
         lcd_string("Ptot=");
           //int a4 = ptot;
        // char b4[10];
        // sprintf(b4,"%d",a4);
          lcd_string("15");
         
          del(2500);
         //MICROREAD(16);
        }
    }
void keypad(){
    
   
    declare_I2C(100);
    l1:
    //COLUMN 1
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        loadone=1;
        MICROWRITE("1",0);
        goto l2;
      
    }
    if(R2==1){
        lcd_data('4');
        loadone=4;
        MICROWRITE("4",0);
        goto l2;
        
    }
        if(R3==1){
        lcd_data('7');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('*');
        while(R4==1);
    }
    
    
   //COLUMN 2
        C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
       loadone=2;
        MICROWRITE("2",0);
        goto l2;
    }
    if(R2==1){
        lcd_data('5');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('8');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('0');
        while(R4==1);
    }
        
        
        //COLUMN 3
    C1=0;C2=0;C3=1;
    if(R1==1){
        lcd_data('3');
      loadone=3;
        MICROWRITE("3",0);
        goto l2;
    }
    if(R2==1){
        lcd_data('6');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('9');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('#');
        while(R4==1);
    }
    goto l1;
    l2:
    lcd_command(0x01);
         lcd_string("Load2 = ");
         declare_I2C(100);
    l3:
    //COLUMN 1
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        loadtwo=1;
        MICROWRITE("1",0x0001);
        goto l4;
      
    }
    if(R2==1){
        lcd_data('4');
        loadtwo=4;
        MICROWRITE("4",0x0001);
        goto l4;
    }
        if(R3==1){
        lcd_data('7');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('*');
        while(R4==1);
    }
    
    
   //COLUMN 2
        C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
         loadtwo=2;
        MICROWRITE("2",0x0001);
        goto l4;
       
    }
    if(R2==1){
        lcd_data('5');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('8');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('0');
        while(R4==1);
    }
        
        
        //COLUMN 3
    C1=0;C2=0;C3=1;
    if(R1==1){
        lcd_data('3');
      loadtwo=3;
        MICROWRITE("3",0x0001);
        goto l4;
    }
    if(R2==1){
        lcd_data('6');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('9');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('#');
        while(R4==1);
    }
    goto l3;
    l4:
    lcd_command(0x01);
         del(1000);
    lcd_string("load 3 =");
    declare_I2C(100);
    l5:
    //COLUMN 1
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        loadthree=1;
        MICROWRITE("1",0x0010);
        goto l6;
      
    }
    if(R2==1){
        lcd_data('4');
         loadthree=4;
        MICROWRITE("4",0x0010);
        goto l6;
    }
        if(R3==1){
        lcd_data('7');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('*');
        while(R4==1);
    }
    
    
   //COLUMN 2
        C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
          loadthree=2;
        MICROWRITE("2",0x0010);
        goto l6;
       
    }
    if(R2==1){
        lcd_data('5');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('8');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('0');
        while(R4==1);
    }
        
        
        //COLUMN 3
    C1=0;C2=0;C3=1;
    if(R1==1){
        lcd_data('3');
       loadthree=3;
        MICROWRITE("3",0x0010);
        goto l6;
    }
    if(R2==1){
        lcd_data('6');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('9');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('#');
        while(R4==1);
    }
    goto l5;
    l6: 
    del(1000);
    lcd_command(0x01);
    lcd_string("load 4 = ");
    declare_I2C(100);
    l7:
    //COLUMN 1
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        loadfour=1;
        MICROWRITE("1",0x0011);
        goto l8;
      
    }
    if(R2==1){
        lcd_data('4');
         loadfour=4;
        MICROWRITE("4",0x0011);
        goto l8;
    }
        if(R3==1){
        lcd_data('7');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('*');
        while(R4==1);
    }
    
    
   //COLUMN 2
        C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
          loadfour=2;
        MICROWRITE("2",0x0011);
        goto l8;
       
    }
    if(R2==1){
        lcd_data('5');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('8');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('0');
        while(R4==1);
    }
        
        
        //COLUMN 3
    C1=0;C2=0;C3=1;
    if(R1==1){
        lcd_data('3');
       loadfour=3;
        MICROWRITE("3",0x0011);
        goto l8;
    }
    if(R2==1){
        lcd_data('6');
        while(R2==1);
    }
        if(R3==1){
        lcd_data('9');
        while(R3==1);
    }
    if(R4==1){
        lcd_data('#');
        while(R4==1);
    }
    goto l7;
    l8:
     del(1000);
    lcd_command(0x01);
    lcd_string("SPV(w) = ");
   
    declare_I2C(100);
    l9:
    //COLUMN 1
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        spv=1;
        MICROWRITE("1",0x0100);
        
        goto l10;
      
    }
    if(R2==1){
        lcd_data('4');
         spv=4;
        MICROWRITE("4",0x0100);
        
        goto l10;
    }
        if(R3==1){
        lcd_data('7');
        spv=1;
        MICROWRITE("7",0x0100);
       goto l10;
    }
    if(R4==1){
        lcd_data('*');
       goto l10;
    }
    
    
   //COLUMN 2
        C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
       spv=2;
        MICROWRITE("2",0x0100);
       goto l10;
       
    }
    if(R2==1){
        lcd_data('5');
        spv=5;
        MICROWRITE("5",0x0100);
       goto l10;
    }
        if(R3==1){
        lcd_data('8');
        spv=8;
        MICROWRITE("8",0x0100);
        goto l10;
    }
    if(R4==1){
        lcd_data('0');
        spv=0;
        MICROWRITE("0",0x0100);
        goto l10;
    }
        
        
        //COLUMN 3
    C1=0;C2=0;C3=1;
    if(R1==1){
        lcd_data('3');
      spv=3;
        MICROWRITE("3",0x0100);
       goto l10;
    }
    if(R2==1){
        lcd_data('6');
        spv=6;
        MICROWRITE("6",0x0100);
        goto l10;
    }
        if(R3==1){
        lcd_data('9');
        spv=9;
        MICROWRITE("9",0x0100);
       goto l10;
    }
    if(R4==1){
        lcd_data('#');
        goto l10;
    }
    goto l9;
    l10:
   declare_I2C(100);
    b11:
    //COLUMN 1
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        spvv=1;
        MICROWRITE("1",0x0101);
        
        goto l12;
      
    }
    if(R2==1){
        lcd_data('4');
         spvv=4;
        MICROWRITE("4",0x0101);
        
        goto l12;
    }
        if(R3==1){
        lcd_data('7');
        spvv=1;
        MICROWRITE("7",0x0101);
       goto l12;
    }
    if(R4==1){
        lcd_data('*');
       goto l12;
    }
    
    
   //COLUMN 2
        C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
       spvv=2;
        MICROWRITE("2",0x0101);
       goto l12;
       
    }
    if(R2==1){
        lcd_data('5');
        spvv=5;
        MICROWRITE("5",0x0101);
       goto l12;
    }
        if(R3==1){
        lcd_data('8');
        spvv=8;
        MICROWRITE("8",0x0101);
        goto l12;
    }
    if(R4==1){
        lcd_data('0');
        spvv=0;
        MICROWRITE("0",0x0101);
        goto l12;
    }
        
        
        //COLUMN 3
    C1=0;C2=0;C3=1;
    if(R1==1){
        lcd_data('3');
      spvv=3;
        MICROWRITE("3",0x0101);
       goto l12;
    }
    if(R2==1){
        lcd_data('6');
        spvv=6;
        MICROWRITE("6",0x0101);
        goto l12;
    }
        if(R3==1){
        lcd_data('9');
        spvv=9;
        MICROWRITE("9",0x0101);
       goto l12;
    }
    if(R4==1){
        lcd_data('#');
        goto l12;
    }
    goto b11;
    l12:
    del(1000);
    infinite_display();
    
         
}
 


void newspv(){
       
         lcd_command(0x01);
        lcd_string("Load priority");
         del(2000);
         lcd_command(0x01);
         lcd_string("load1 =  ");
         keypad();
         del(5000);
    
       }
void twokey()
{
    C1=1;C2=0;C3=0;
    if(R1==1){
        lcd_data('1');
        del(1000);
        newspv();
        while(R1==1);
        exit(1);
    }
         C1=0;C2=1;C3=0;
    if(R1==1){
        lcd_data('2');
   
         del(1000);
        infinite_display();
        while(R1==1);    
        exit(1);
    }
}

void main(void) {
    
  
    declare_I2C(100);
    
    
    TRISD0=0;
    TRISD1=0;
    TRISD2=0;
    TRISB =0X00; //makes rows input and columns as output for keypad
  
    TRISC0=0;
    TRISC1=0;
    TRISC2=0;
    
    TRISD3=1;
    TRISD4=1;
    TRISD5=1;
    TRISD6=1;
    
    
    lcd_initialize();
    
        lcd_command(0x80); //going to 1st row 3rd column and printing the statement
       
        lcd_string("Welcome ADV B");
        del(2000);
        lcd_command(0x01);
        lcd_string("type 1 or 2 if");
        del(1000);
        lcd_command(0x01);
        lcd_string(" you want to ");
        del(1000);
        lcd_command(0x01);
        lcd_string("set new values");
        del(1000);
        lcd_command(0x01);
        lcd_string("or to ");
        del(1000);
        lcd_command(0x01);
        lcd_string("use old values");
        del(1000);
         lcd_command(0x01);
      
        del(1000);
        lcd_command(0x01);
        lcd_string("1=new SPV ");
        del(1000);
        lcd_command(0x01);
        lcd_string("2=old value");
        del(1000);
        
        lcd_command(0x01);
        del(1000);
        
        
        while(1)
        {
          twokey();
               
}
    
}
