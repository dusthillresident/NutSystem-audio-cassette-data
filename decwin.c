#include <stdio.h>

static FILE* infp;
static FILE* outfp;

static int sample=128,oldsample=128;
static int invert=-1;
static int powerlimit=100;
static int av_a=110,av_b=40;
static int chk_a=0,chk_b=0,chk_c=0,chk_d=0;
static int bits[8];
static int bls[2];
static int rubbish_unused=0; /* if this isn't here, bls[2] gets set to 1 when started gets set to 1 at line 28. I don't know if this is a compiler bug or if I'm doing something wrong.*/
static int started=0;

void main() {
 printf("Start \n");
 int filelength=0,bytecount=0,databyte=0;

 infp=fopen("/tmp/readdata","rb");
 outfp=fopen("miscfiles/recovered_file","wb");

 setpolarity();
 if(readstartsignal()==1){
  started=1;
  readchecksum();
  readfiledescription();
  filelength=readfilelength();

  while(bytecount<filelength){
   databyte=readbyte(); bytecount++;
   fputc(databyte,outfp);
   checksumprocess(databyte);
  }

  checksumresult();
 }

 fclose(infp);
 fclose(outfp);

 printf("powerlimit %d \n",powerlimit);
 printf("initial nutpowers %d %d \n",bls[1],bls[2]);
 printf("average nutpowers %d %d \n",av_a,av_b);
 printf("End \n");
}

int readsample() {
 oldsample=sample;
 sample=fgetc(infp);
 if(invert==1){
  sample=255-sample;
 }
 return sample;
}

int setpolarity() {
 int i=1;
 int null=0;
 int samp=0;
 int noise_hi=128;
 int noise_lo=128;

 while(i<4){
  null=readsample();
  i++;
 }
 i=0;
 while(i<77661){
  samp=readsample();
  if(samp>noise_hi){
   noise_hi=samp;
  }
  if(samp<noise_lo){
   noise_lo=samp;
  }
  i++;
 }
 noise_hi+=8; noise_lo-=8;

 while(invert==-1){
  samp=readsample();
  if(samp>noise_hi){
   invert=0;
  }
  if(samp<noise_lo){
   invert=1;
  }
 }
 printf("invert: %d \n",invert);
 return invert;
}

int readbeep() {
 int sxc=0,hi=0,lo=255;
 /*printf("readbeep \n");*/

 do{
  readsample();
  if(sample>oldsample){
   if(sxc>0){ sxc--; }   
  }
  else{
   sxc++;
  }
  if(sample>hi){ hi=sample; }
 }while(sxc<3);

 sxc=0; 
 do{
  readsample();
  if(sample<oldsample){
   if(sxc>0){ sxc--; }   
  }
  else{
   sxc++;
  }
  if(sample<lo){ lo=sample; }
 }while(sxc<3);

 if(started==1){
  powerlimit=av_a-av_b; powerlimit=powerlimit/3.4; powerlimit+=av_b;
 } 

 return hi-lo;
}

int readstartsignal() {
 int i=0,chek=0,ret=0;
 printf("readstartsignal \n");
 
 do{
  convertlengthtotype(readbeep());
  i++;
 }while(i<1399);
 bls[1]=readbeep();
 do{
  bls[1]+=readbeep(); bls[1]=bls[1]/2;
  convertlengthtotype(bls[1]);
  i++;
 }while(i<2601);

 bls[2]=bls[1]/2; 
 powerlimit=bls[1]/2+10;

 do{
  chek=convertlengthtotype(readbeep());
 }while(chek!=2);
 i=0;

 bls[2]=readbeep();
 do{  
  bls[2]+=readbeep(); bls[2]=bls[2]/2;
  i++;
 }while(i<5);

 powerlimit=bls[1]-bls[2]; powerlimit=powerlimit/3.4; powerlimit+=bls[2];

 chek=readbyte();
 if(chek!=54){ 
  printf("start signal error, got %d \n",chek); ret=0;
 }
 else{
  printf("start signal OK \n"); ret=1;
 }

 return ret;
}

int convertlengthtotype(int len) {
 int type=0;
 if(len>=powerlimit){
  type=1;
  av_a+=len; av_a=av_a/2;
 }
 else{
  type=2;
  av_b+=len; av_b=av_b/2;
 }
 return type; 
}

int convertfrombin() {
 int bval=0,xval=128,i=0;
 while(i<8){
  if(bits[i]>0){ bval+=xval; }
  xval=xval/2;
  i++;
 }
 return bval; 
}

int readbyte() {
 int i=0;
 while(i<8){
  bits[i]=convertlengthtotype(readbeep())-1;
  i++;
 }
 return convertfrombin();
}

int readfilelength() {
 int fs=0,bv=0;
 int a,b,c;

 a=readbyte()*256*256;
 b=readbyte()*256;
 c=readbyte();
 fs=a+b+c;

 printf("%d bytes \n",fs);
 return fs;
}

int readfiledescription() {
 int i=0;
 char letter=0;

 int fnl=readbyte();
 printf("File description: ");
 do{
  letter=readbyte();
  printf("%c",letter);
  i++;
 }while(i!=fnl);
 printf("\n");

 return 0; 
}

int readchecksum() {
 chk_a=readbyte();
 chk_b=readbyte();
 return 0;
}

int checksumprocess(int data) {
 
 chk_c+=data;
 chk_d+=(data+chk_c);
 if(chk_c>100000){ chk_c-=90000; }
 if(chk_d>100000){ chk_d-=90000; }

 return 0;
}

int checksumresult() {

 while(!( chk_c<256 )){
  if(chk_c>255){ chk_c-=116; }
 }
 while(!( chk_c>-1 )){
  if(chk_c<0){ chk_c+=116; }
 }

 while(!( chk_d<256 )){
  if(chk_d>255){ chk_d-=116; }
 }
 while(!( chk_d>-1 )){
  if(chk_d<0){ chk_d+=116; }
 }

 if( chk_a!=chk_c || chk_b!=chk_d ){
  printf("Checksum failed!!!! \n"); 
 }
 else{
  printf("Checksum OK \n");
 }
 /*printf(" checksum rubbish %d %d %d %d \n",chk_a,chk_b,chk_c,chk_d);*/
 return 0;
}
