//PB FUNCTION
const uint8_t PB_pin[]={3,4};

int SolenoidPin=5;

typedef struct DataS {
  uint8_t Svo_F_PosMain[3]={10,80,169 }; // low,mid,  high // Servo Main
  uint8_t Svo_F_PosLock1[2]={20,120}; // Close,OPen // Servo Lock2
  uint8_t Svo_F_PosLock2[2]={140,40}; // Close,OPen // Servo Lock2
};DataS MyS_;

#include <Servo.h> 
const uint8_t MxD_Svo=3, Svo_Pin[MxD_Svo] = {7,6,2}; //dont use #1 array.. // main, lock1,lock2,
Servo Svo[MxD_Svo]; float Svo_SpdF[5][MxD_Svo]; //1 curpos Svo_SpdF[1][MxD_Svo]//2finalPos Svo_SpdF[2][MxD_Svo]//speed //status
uint8_t Svo_I_Pos[MxD_Svo]= {MyS_.Svo_F_PosMain[0],MyS_.Svo_F_PosLock1[0],MyS_.Svo_F_PosLock2[0]}; //initial Servo Postion << update this 

float SystemStatus=0;unsigned long OpenMS,SendDataMS,OpenDoorMS;
bool ConStatus=0;
void setup(){ Serial.begin(115200); 
    pinMode(SolenoidPin, OUTPUT);digitalWrite(SolenoidPin, LOW); 
//  Serial.println("Servo Funtion Call Svo_LoopFunction first b4 calling other Svofunction"); 
//  Serial.println("SvoNor,0,179,1   >> normal function SvoNor,index,Pos,delayONOFF  ");
//  Serial.println("SvoSpeed,0,179,100   >> speed function  SvoSpeed,index,Pos,speed  ");
}

void loop() {
//  Svo_NorFunc(0,30,1); // index// position//delay
//  Svo_Spd_Func(0,30,10); // index// position//speed.. the lower the faster
//  delay(1000);
  PB_Function(false,false,50,2000,false);//pull up//LowHigh input//Press ON//delay after use pb
  Svo_LoopFunction();

  if(Svo_SpdF[1][0]==Svo_SpdF[2][0] && SystemStatus==4){
    SystemStatus=5;
    Svo_NorFunc(1,MyS_.Svo_F_PosLock1[1],1);
    Svo_NorFunc(2,MyS_.Svo_F_PosLock2[1],1);
    OpenMS=millis()+5000;
//    Serial.println(" Servo Post OK");
    } 
  if(SystemStatus==5 && OpenMS<millis()){
    SystemStatus=6;
    returnHomeServo();
//    Serial.println("Done Returning Home");
    }  
  if(Svo_SpdF[1][0]==Svo_SpdF[2][0] && SystemStatus==6){
    SystemStatus=0;
//    Serial.println("Done Home");
    }   

  if(SendDataMS<millis() && ConStatus==1){
    Serial.println("data,"+String(SystemStatus)+",");
    SendDataMS=millis()+500;
    }    
  if(OpenDoorMS<millis()){ digitalWrite(SolenoidPin, LOW);  }
}

void returnHomeServo(){
  Svo_NorFunc(0,MyS_.Svo_F_PosMain[0],1);
  Svo_NorFunc(1,MyS_.Svo_F_PosLock1[0],1);
  Svo_NorFunc(2,MyS_.Svo_F_PosLock2[0],1);
  }
  
//PB FUNCTION
void PB_Function(bool PU_, bool LH,int Press_MS,int Delay_,bool pOut){const uint8_t I_M=sizeof(PB_pin);static unsigned long PB_MS[I_M],Rst_PB[I_M];static uint8_t PB_Stat[I_M];
  static bool init_;if(!init_){for(int i=0;i<I_M;i++){if(PU_){pinMode(PB_pin[i],INPUT_PULLUP);}else{pinMode(PB_pin[i],INPUT);}}if(PU_){}else{}
  init_=1;}for(int i=0;i<I_M;i++){if(digitalRead(PB_pin[i])==LH&&PB_Stat[i]==0&&PB_MS[i]<millis()){PB_Stat[i]=2;PB_MS[i]=millis()+Press_MS;}else if(
  digitalRead(PB_pin[i])==LH&&PB_Stat[i]>0&&PB_MS[i]<millis()){PB_Stat[i]=1;}else if(digitalRead(PB_pin[i])!=LH){PB_Stat[i]=0;PB_MS[i]=millis()+50;}}


//  for(int i=0;i<I_M;i++){if(PB_Stat[i] && Rst_PB[i]<millis()){Rst_PB[i]=millis()+Delay_;if(pOut){Serial.println("PB> "+String(i));}}}
  
  if(PB_Stat[0] && PB_Stat[1] && Rst_PB[0]<millis() && SystemStatus>0 && SystemStatus< 4){
    //put code here
    if(SystemStatus==1){Svo_NorFunc(0,MyS_.Svo_F_PosMain[0],1);}
    else if(SystemStatus==2){Svo_NorFunc(0,MyS_.Svo_F_PosMain[1],1);}
    else if(SystemStatus==3){Svo_NorFunc(0,MyS_.Svo_F_PosMain[2],1);}
    
    Rst_PB[0]=millis()+Delay_;if(pOut){Serial.println("PB 0");}
    SystemStatus=4;
//    Serial.println(" Moving Main Servo");
    }
  
  }

//SERVO FUNCTION
unsigned long MS_Spd[MxD_Svo];int Svo_CheckPos(int Pos){if(Pos>179){Pos=179;}if(Pos<0){Pos=0;} return Pos;} void Svo_NorFunc(int I_,int Pos,bool D_){Pos=Svo_CheckPos(Pos);
  int a_=abs(Pos-Svo_SpdF[1][I_]),d_=map(a_,0,60,50,2200)/10;Svo[I_].write(Pos);Svo_SpdF[2][I_]=Pos;Svo_SpdF[1][I_]=Pos;if(D_){delay(d_);}MS_Spd[I_]=millis()+1000;} 
void Svo_Init(){static bool init_=0;if(!init_){for(int i=0;i<MxD_Svo;i++){Svo[i].attach(Svo_Pin[i]);Svo_NorFunc(i,Svo_I_Pos[i],1);}init_=1;}}
void Svo_Spd_Func(int I_,int Pos,int spd){Svo_SpdF[2][I_]=Svo_CheckPos(Pos);Svo_SpdF[3][I_]=spd;Svo_SpdF[4][I_]=1;MS_Spd[I_]=millis()+Svo_SpdF[3][I_];}
void Svo_LoopFunction(){Svo_Init();static unsigned long SvoMS;static float dT[MxD_Svo];SvoMS=millis();for(int i=0;i<MxD_Svo;i++){if(i==1){continue;}if(Svo_SpdF[4][i]==1){if(Svo_SpdF[1][i]
  <Svo_SpdF[2][i]&&MS_Spd[i]<millis()){dT[i]=float(SvoMS-(MS_Spd[i]-Svo_SpdF[3][i]))/Svo_SpdF[3][i]; Svo_SpdF[1][i]=Svo_SpdF[1][i]+(dT[i]); if(Svo_SpdF[1][i]>Svo_SpdF[2][i]){Svo_SpdF[1][i]
  =Svo_SpdF[2][i];}MS_Spd[i]=millis()+Svo_SpdF[3][i];}else if(Svo_SpdF[1][i]>Svo_SpdF[2][i] && MS_Spd[i]<millis()){ dT[i]=float(SvoMS-(MS_Spd[i]-Svo_SpdF[3][i]))/Svo_SpdF[3][i]; 
  Svo_SpdF[1][i]=Svo_SpdF[1][i]-(dT[i]);if(Svo_SpdF[1][i]<Svo_SpdF[2][i]){Svo_SpdF[1][i]=Svo_SpdF[2][i];} MS_Spd[i]=millis()+Svo_SpdF[3][i];}if(Svo_SpdF[1][i]!=Svo_SpdF[2][i]){ 
  Svo[i].write((int)Svo_SpdF[1][i]);}else{Svo_SpdF[4][i]=0; Svo[i].write((int)Svo_SpdF[2][i]);delay(1);}}else if(MS_Spd[i]<millis()){Svo_NorFunc(i,Svo_SpdF[1][i],0);}}}


//void SerialCom_Rcv(Stream *s,uint8_t I_,bool pOut){if(s->available()>0){char D[64];strcpy(D,s->readStringUntil('\n').c_str());if(pOut)Serial.println(D);Split_(String(D),I_,',',false);}}
void serialEvent(){ //do not move text here
  while (Serial.available()){static String D_;char inChar = (char)Serial.read();D_ += inChar;if (inChar == '\n'){Split_(D_,0,',',false);D_ = "";}}}
void Split_(String DS,uint8_t I_,char d_,bool pOut){char S_DA[2][10][15];String DA_[10];DS.remove(DS.indexOf('\n'),1);DS.remove(DS.indexOf((char)13),1);int d_I=0;int d_I2=DS.indexOf(d_);
  while(d_I2!=-1){strcpy(S_DA[I_][d_I],DS.substring(0,d_I2).c_str());DA_[d_I]=S_DA[I_][d_I];DS=DS.substring(d_I2+1);d_I2=DS.indexOf(d_);d_I++;}DA_[d_I]=DS;strcpy(S_DA[I_][d_I],DS.c_str());
  if(pOut)for(int i=0;i<=d_I;i++){Serial.println("Data "+String(i)+":\t"+DA_[i]+" ");}
  
  if(DA_[0].equalsIgnoreCase("SvoNor")){Svo_NorFunc(DA_[1].toInt(),DA_[2].toInt(),DA_[3].toInt()); }
  if(DA_[0].equalsIgnoreCase("SvoSpeed")){Svo_Spd_Func(DA_[1].toInt(),DA_[2].toInt(),DA_[3].toInt());}
  if(DA_[0].equalsIgnoreCase("con")){Serial.println("ICRprint,"); if(ConStatus==0){returnHomeServo();ConStatus=1; }}
  if(DA_[0].equalsIgnoreCase("OpenDoor")){digitalWrite(SolenoidPin, HIGH);OpenDoorMS=millis()+10000; Serial.println("OpenDoorMS");}
  if(DA_[0].equalsIgnoreCase("reset")){SystemStatus=0;}
  if(DA_[0].equalsIgnoreCase("store") && ConStatus==1){ Serial.println("STORING,");SystemStatus=DA_[1].toInt();  Svo_NorFunc(1,MyS_.Svo_F_PosLock1[0],1); Svo_NorFunc(2,MyS_.Svo_F_PosLock2[0],1);  
//  Serial.println(" Storing Item");
  }
  }

    
