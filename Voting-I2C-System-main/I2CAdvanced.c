#include <reg51.h>
#define ACK 1
#define NO_ACK 0

#define display P2 

sbit rs = P3^0;  //LCD pins
sbit rw = P3^1; 
sbit en = P3^2;  

sbit SCL= P0^4; //I2C Pins
sbit SDA= P0^5;

sbit reset_pin = P1^6; //Reset Pin

sbit start_pin = P1^0; //Start pin
sbit stop_pin = P1^5; //Stop pin

sbit candidate_1=P1^1;  //Candidates
sbit candidate_2=P1^2;  
sbit candidate_3=P1^3;  
sbit candidate_4=P1^4;  

unsigned int vote_1, vote_2, vote_3, vote_4;
unsigned char dat;

int vote_dig[2],j;
int max = 0;
int carry = 0;
int arr[4]={0};


void delay(int delay_time)  //Delay function
{
	int j,k;
	for(j=0;j<=delay_time;j++)
		for(k=0;k<=1000;k++);
}

//-------------------------EEPROM Functions---------------------------

void I2C_Start() 
{     
	 SDA = 1;  
	 delay(1); 
	 SCL = 1;  
	 delay(1); 
	 SDA = 0; 
	 delay(1); 
	 SCL = 0; 
} 
 
void I2C_Stop(void) 
{   
	 SDA = 0; 
	 delay(1); 
	 SCL = 1;
	 delay(1); 
	 SDA = 1; 
} 

 
void I2C_Clock(void) 
{ 
	 delay(1); 
	 SCL = 1;
	 delay(1);
	 SCL = 0;
}

void I2C_Write(unsigned char dat) 
{ 
	 unsigned char i; 
	
	 for (i=0;i<8;i++)
	{
		SDA = (dat & 0x80) ? 1:0;
		SCL=1;SCL=0;
		dat<<=1;
	}

	SCL = 1;
	delay(5);
	SCL = 0;

} 
 
unsigned char I2C_Read(bit ACK_Bit)
{
	unsigned int i;
	I2C_Start();
	I2C_Write(0xA1);

	SDA = 1;
	for (i=0;i<8;i++)
	{
		SCL = 1;
		dat<<= 1;
		dat = (dat | SDA);
		SCL = 0;
		delay(5);
	}

	if (ACK_Bit == 1)
		SDA = 0; // Send ACK
	else
		SDA = 1; // Send NO ACK

	delay(5);
	SCL = 1;
	delay(5);
	SCL = 0;
	I2C_Stop();
	return dat;
}

void ReadBYTE(unsigned int Addr)
{
	I2C_Start();
	I2C_Write(0xA0);
	I2C_Write((unsigned char)(Addr>>8)&0xFF);
	I2C_Write((unsigned char)Addr&0xFF);
}


void WriteBYTE(unsigned int Addr)
{
	I2C_Start();
	I2C_Write(0xA0);
	I2C_Write((unsigned char)(Addr>>8)&0xFF); // send address high
	I2C_Write((unsigned char)Addr&0xFF); // send address low
}

//-----------------------LCD Functions---------------------------

void lcd_cmd(unsigned char cmd_addr)
{
	display = cmd_addr;
	en = 1;
	rs = 0;
	rw = 0;
	delay(1);
	en = 0;
}

void lcd_data_str(char str[50])  //Display for String
{ 
	int l;
	for (l=0;str[l]!='\0';l++)
	{
		display = str[l];
		rw = 0;
		rs = 1;
		en = 1;
		delay(1);
		en = 0;
	}
}

void lcd_data_int(unsigned int vote)  //Display for numbers
{ 
	char dig_char;
	int p;
		vote_dig[1]=vote%10;
		vote_dig[0]=vote/10;
	
	for (p=0;p<=1;p++)
	{
		dig_char = vote_dig[p]+48;
		display = dig_char;
		rw = 0;
		rs = 1;
		en = 1;
		delay(1);
		en = 0;
	}
} 

//-------------------Voting Functions---------------------------

void count()  //Count Votes
{
	while (candidate_1==0 && candidate_2==0 && candidate_3==0 && candidate_4==0);
	if (candidate_1==1)
	{
		while (candidate_1 == 1);
		vote_1 = vote_1 + 1;
		WriteBYTE(0x0000);
		I2C_Write((char)vote_1);
		I2C_Stop();
		delay(5);
	}

	if (candidate_2==1)
	{
		while (candidate_2 == 1);
		vote_2 = vote_2 + 1;
		WriteBYTE(0x0002);
		I2C_Write((char)vote_2);
		I2C_Stop();
		delay(5);
	}

	if (candidate_3==1)
	{
		while (candidate_3 == 1);
		vote_3 = vote_3 + 1;
		WriteBYTE(0x0004);
		I2C_Write((char)vote_3);
		I2C_Stop();
		delay(5);
	}

	if (candidate_4==1)
	{
		while (candidate_4 == 1);
		vote_4 = vote_4 + 1;
		WriteBYTE(0x0006);
		I2C_Write((char)vote_4);
		I2C_Stop();
		delay(5);
	}
	
}	


void voting() //Voting Start
{
	lcd_cmd(0x38);
	delay(50);
	lcd_cmd(0x0E);
	delay(50);
	lcd_cmd(0x01);
	delay(50);
	
	lcd_cmd(0x82);
	delay(50);
	lcd_data_str("PRESS BUTTON");
	delay(50);
	lcd_cmd(0xC4);
	delay(50);
	lcd_data_str("TO VOTE!");
	delay(100);
	
	count();
	lcd_cmd(0x01);
	delay(50);
	lcd_cmd(0x81);
	delay(50);
	lcd_data_str("VOTE RECORDED!");
	delay(100);
}

void results()  //Vote Results
{
	int i;
	carry = 0;
	
	ReadBYTE(0x0000);
	vote_1 = I2C_Read(NO_ACK);
	
	ReadBYTE(0x0002);
	vote_2 = I2C_Read(NO_ACK);
	
	ReadBYTE(0x0004);
	vote_3 = I2C_Read(NO_ACK);
	
	ReadBYTE(0x00006);
	vote_4 = I2C_Read(NO_ACK);
	
	
	lcd_cmd(0x01);
	delay(50);
	lcd_cmd(0x81);
	delay(50);
	lcd_data_str( "VOTING ENDS!!" );
	delay(200);
	
	lcd_cmd(0x01);
	delay(50);
	lcd_cmd(0x80);
	delay(50);
	lcd_data_str("SNS");
	delay(50);
	lcd_cmd(0x84);
	delay(50);
	lcd_data_str("NAS");
	delay(50);
	lcd_cmd(0x88);
	delay(50);
	lcd_data_str("MPMC");
	delay(50);
	lcd_cmd(0x8D);
	delay(50);
	lcd_data_str("OTH");
	delay(50);

	lcd_cmd(0xC0);
	delay(100);
	lcd_data_int(vote_1);
	delay(50);

	lcd_cmd(0xC4);
	delay(50);
	lcd_data_int(vote_2);
	delay(50);

	lcd_cmd(0xC9);
	delay(50);
	lcd_data_int(vote_3);
	delay(50);

	lcd_cmd(0xCD);
	delay(50);
	lcd_data_int(vote_4);
	delay(300);

	arr[0] = vote_1;
	arr[1] = vote_2;
	arr[2] = vote_3;
	arr[3] = vote_4;

	for( i=0; i<4; i++)
	{
		if(arr[i]>=max)
		max = arr[i];
	}
	
	
	if ( (vote_1 == max) && (vote_2 != max) && (vote_3 != max) && (vote_4 != max) )
	{
		carry = 1;
		lcd_cmd(0x01);
		delay(50);
		lcd_cmd(0x83);
		delay(50);
		lcd_data_str("CONGRATS!");
		delay(50);
		lcd_cmd(0xC4);
		delay(50);
		lcd_data_str("SNS");
		delay(50);
		lcd_cmd(0x14);
		delay(50);
		lcd_data_str("WINS!");
		delay(50);
	}

	if ( (vote_2 == max) && ( vote_1 != max) && (vote_3 != max)&& (vote_4 != max) )
	{
		carry = 1;
		lcd_cmd(0x01);
		delay(50);
		lcd_cmd(0x83);
		delay(50);
		lcd_data_str("CONGRATS!");
		delay(50);
		lcd_cmd(0xC4);
		delay(50);
		lcd_data_str("NAS");
		delay(50);
		lcd_cmd(0x14);
		delay(50);
		lcd_data_str("WINS!");
		delay(50);
	}

	if ( (vote_3 == max) && ( vote_2 != max) && (vote_1 != max)&& (vote_4 != max) )
	{
		carry = 1;
		lcd_cmd(0x01);
		delay(50);
		lcd_cmd(0x83);
		delay(50);
		lcd_data_str("CONGRATS!");
		delay(50);
		lcd_cmd(0xC3);
		delay(50);
		lcd_data_str("MPMC");
		delay(50);
		lcd_cmd(0x14);
		delay(50);
		lcd_data_str("WINS!");
		delay(50);
	}

	if ( (vote_4 == max) && ( vote_2 != max) && (vote_3 != max)&& (vote_1 != max) )
	{
		carry = 1;
		lcd_cmd(0x01);
		delay(50);
		lcd_cmd(0x83);
		delay(50);
		lcd_data_str("CONGRATS!");
		delay(50);
		lcd_cmd(0xC2);
		delay(50);
		lcd_data_str("OTHERS");
		delay(50);
		lcd_cmd(0x14);
		delay(50);
		lcd_data_str("WINS!");
		delay(50);
	}

	if (carry==0)
	{
		lcd_cmd(0x01);
		delay(50);
		lcd_cmd(0x81);
		delay(50);
		lcd_data_str("CLASH");
		delay(50);
		lcd_cmd(0x14);
		delay(50);
		lcd_data_str("BETWEEN!");
		delay(50);
		
		if(vote_1 == max)
		{
		 lcd_cmd(0xC0);
		 lcd_data_str("SNS");
		 delay(50);
		}
		if(vote_2 == max)
		{
		 lcd_cmd(0xC4);
		 lcd_data_str("NAS");
		 delay(50);
		}
		if(vote_3 == max)
		{
		 lcd_cmd(0xC8);
		 lcd_data_str("MPMC");
		 delay(50);
		}
		if(vote_4 == max)
		{
		 lcd_cmd(0xCD);
		 lcd_data_str("OTH");
		 delay(50);
		}
	}
}

void reset(void)
{
	vote_1= vote_2= vote_3= vote_4= 0;
	
		WriteBYTE(0x0000);
		I2C_Write((char)vote_1);
		I2C_Stop();
		delay(5);
		
		WriteBYTE(0x0002);
		I2C_Write((char)vote_2);
		I2C_Stop();
		delay(5);
		
		WriteBYTE(0x0004);
		I2C_Write((char)vote_3);
		I2C_Stop();
		delay(5);
	
		WriteBYTE(0x0006);
		I2C_Write((char)vote_4);
		I2C_Stop();
		delay(5);
		
		lcd_cmd(0x01);
		delay(50);
		lcd_cmd(0x80);
		delay(50);
		lcd_data_str( "ALL VOTES RESET" );
		delay(50);
		lcd_cmd(0xC5);
		delay(50);
		lcd_data_str("TO 0!");
		delay(50);
}

void initial_message()
{
	lcd_cmd(0x01);
	delay(50);
	lcd_cmd(0x80);
	delay(50);
	lcd_data_str( "WAIT FOR MASTER" );
	delay(50);
	lcd_cmd(0xC4);
	delay(50);
	lcd_data_str("TO ALLOW");
	delay(100);
}

void main()
{
	start_pin = stop_pin = reset_pin = 0;
	candidate_1 = candidate_2 = candidate_3 = candidate_4 = 0;
	
	ReadBYTE(0x0000);
	vote_1 = I2C_Read(NO_ACK);
	
	ReadBYTE(0x0002);
	vote_2 = I2C_Read(NO_ACK);
	
	ReadBYTE(0x0004);
	vote_3 = I2C_Read(NO_ACK);
	
	ReadBYTE(0x00006);
	vote_4 = I2C_Read(NO_ACK);
	
	lcd_cmd(0x38);
	delay(50);
	lcd_cmd(0x0E);
	delay(50);
	lcd_cmd(0x01);
	delay(50);
	lcd_cmd(0x83);
	delay(50);
	lcd_data_str( "VOTING DAY" );
	delay(50);
	lcd_cmd(0xC4);
	delay(50);
	lcd_data_str("BEGINS!!");
	delay(200);
	
	while(1)
	{
		initial_message();
		while(start_pin == 0)
		{
			if (reset_pin==1)
			{
				reset();
				initial_message();
				continue;
			}
		 if (stop_pin == 1)
		 break;
		}
		
		/*
		if (reset_pin==1)
		{
			reset();
			break;
		}
		*/
		
		if (stop_pin == 1)
		break;
		
		voting();
	}

	while(1)
	{
		results();
	}
}