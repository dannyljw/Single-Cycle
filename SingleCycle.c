#include<stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#define j 0x2
#define jal 0x3
#define jr 0x08
#define add 0x20
#define addi 0x8
#define addiu 0x9
#define addu 0x21
#define and 0x24
#define andi 0xc
#define beq 0x4
#define bne 0x5
#define lui 0xf
#define lw 0x23
#define nor 0x27
#define or 0x25
#define ori 0xd
#define slt 0x2a
#define slti 0xa
#define sltiu 0xb
#define sltu 0x2b
#define sll 0x00
#define srl 0x02
#define sw 0x2b
#define sub 0x22
#define subu 0x23
int memory[0x80000] = {0, };
int pc;
int rs,rt,rd,shamt,funct,immediate,address;
unsigned inst;
unsigned int opcode;
int r[32]={0 };
int temp;
int word;
int Rtype_count=0;
int Itype_count=0;
int Jtype_count=0;
int memacc=0;
int branch=0;
int branch1=0;
int regdest,alusrc,memtoreg,regwrite,memread,memwrite,pcsrc1,pcsrc2;
int jumpaddr(int a){
	a=((pc+4)& 0xf0000000)|(address<<2);
	return a;
}
int signextimm(int b){
	int a;
//0printf("\n\n immediate : %d\n\n",b);
	if(b>>15==1)
		a=0xffff0000 | b;
	else
		a=0x0000FFFF & b;
//0printf("%d\n",a);
	return a;
}

int branchaddr(int c){
	int a;
	a = signextimm(c)<<2;

	return a;

}

void fetch(){
	inst=memory[pc/4];
	printf("fetch : pc[0x%08x]=0x%08x \n",pc,inst);
}


void decode(){
	opcode=(((inst & 0xfc000000)>> 26) & 0x3f);

	switch (opcode){
		case 0 :
			rs=(inst & 0x03e00000)>>21;
			rt=(inst & 0x001f0000)>>16;
			rd=(inst & 0x0000f800)>>11;
			shamt=(inst & 0x000007c0)>>6;
			funct=(inst & 0x0000003f);
			Rtype_count++;
			printf("rs : %d\n rt : %d\n rd : %d\n shamt : %d\n funct : %d\n",rs,rt,rd,shamt,funct);
			break;
		case j:
			address = (inst & 0x03ffffff);
			Jtype_count++;
			printf("address : %d \n",address);
			break;
		case jal:
			address=(inst & 0x03ffffff);
			Jtype_count++;
			printf("address :%d\n",address);
			break;
		default : 
			rs=(inst & 0x03e00000)>>21;
			rt=(inst & 0x001f0000)>>16;
			immediate=(inst & 0x0000ffff);
			Itype_count++;
			printf("rs : %d\n rt: %d\n immediate : %x\n",rs,rt,immediate);
			break;
	}//switch close
}//decode close



void execution(){//execution start
//0	printf("check execution opcode : %x \n",opcode);
	opcode=(inst & 0xfc000000)>>26;
	int zeroextimm = immediate;
	int funct=(inst & 0x0000003f);

	if(regdest==1){//switch start

		switch(funct){
			case add:
				temp=r[rs]+r[rt];
				printf("instruction : add\n");//checking if excution is working
				break;
			case addu:
				temp=r[rs]+r[rt];
				printf("instruction : addu\n");
				break;
			case and:
				temp=r[rs] & r[rt];
				printf("instruction : and\n");
				break;
			case jr://////////////
				pc=r[rs];
				printf("instruction : jr\n");
				break;
			case nor:
				temp!=(r[rs] | r[rt]);
				printf("instruction : nor\n");
				break;
			case or:
				temp=r[rs]|r[rt];
				printf("instruction : or\n");
				break;
			case slt:///////////////
				temp=((r[rs]<r[rt]) ? 1:0);
				printf("instruction : set less than\n");
				break;
			case sltu:
				temp=((r[rs]<r[rt])? 1:0);
				printf("instruction : set less than unsigned\n");
				break;
			case sll:
				temp=r[rt]<<shamt;
				printf("instruction : set left logical\n");
				break;
		
			case srl:
				temp=r[rt]>>shamt;
				printf("instruction : set right logical\n");
				break;
			case sub:
				temp=r[rs]-r[rt];
				printf("instruction : sub\n");
				break;
			case subu:
				temp=r[rs]-r[rt];
				printf("instruction : subu\n");
				break;
			default:
				break;
		}//switch(funct) close
	}//if(regdest=1) close
	else if (alusrc==1){
		
		switch(opcode){
			case addi:
				temp=(r[rs]+signextimm(immediate));
				printf("instruction : addi\n");
				break;
			case sw:
				word=(r[rs]+signextimm(immediate));
			printf("instruction : STORE WORD EXECUTION\n");
				break;
			case lw:
				word=r[rs]+signextimm(immediate);
				printf("instruction : LOAD WORD EXECUTION\n");
				break;
			case addiu:
				temp=r[rs]+signextimm(immediate);
			printf("instruction : addiu\n");
				break;
			case andi:
				temp=r[rs]&zeroextimm;
			printf("instruction : andi\n");
				break;
			case lui:
				temp=(immediate<<16)&0xffff0000;
				printf("insturction : lui\n");
				break;
			case ori:
				temp=r[rs]|zeroextimm;
				printf("instruction : ori\n");
				break;
			case slti:
				temp=(r[rs]<signextimm(immediate)? 1:0);
		//0		printf("slti %d\n r[%d]=%d\n,%d\n",signextimm(immediate),rs,r[rs],temp);
				printf("instruction : slti\n");
				break;
			case sltiu:
				temp=(r[rs]<signextimm(immediate? 1:0));
				printf("instruction : sltiu\n");
				break;
			default:
				break;
		}//switch(opcode) close
	}//else if(alusrc=1) close
	else if (pcsrc2==1){
			switch (opcode){
			case beq:
			//0	printf("r[%d]=%d\nr[%d]=%d",rs,r[rs],rt,r[rt]);
				if(r[rs]==r[rt])
				{	pc=pc+4+branchaddr(immediate);
					printf("beq pc value :%d\n",pc);
					branch++;
				}
				else{
					pc=pc+4;
					branch1++;
					printf("else pc value : %d\n",pc);
					}
				break;
			case bne:
				printf(" is it working? %d %d\n",r[rs],r[rt]);
				if(r[rs]!=r[rt])
				{	pc=pc+4+branchaddr(immediate);
					branch++;}
				else{
					pc=pc+4;
					branch1++;}
				break;
			default:
				break;
		}//switch(funct) close
	}//else if (pcsrc2=1) close

}//execution close
void memoryacc(){
	if(memread){//lw instuction
		temp=memory[word/4];
		memacc++;
		printf("instruction LOAD WORD is working now\n");
		printf("r[%d]: %d, memory[0x%08x] \n",rt,r[rt],temp);
	}
	else if (memwrite){//sw instruction
		memory[word/4]=r[rt];
		memacc++;
		printf("instruction STORE WORD is working now\n");
		printf("memory[0x%08x],r[%d]: %d\n",word,rt,r[rt]);
	}
}

void control_s(){
	if(opcode==0) regdest=1;
	else regdest=0;
	if((opcode!=0)&&(opcode!=beq)&&(opcode!=bne)) alusrc=1;
	else alusrc=0;
	if(opcode==lw) memtoreg=1;
	else memtoreg=0;
	if((opcode!=sw)&&(opcode!=beq)&&(opcode!=bne)&&(opcode!=j)&&(opcode!=jr))regwrite=1;
	else regwrite=0;
	if(opcode==lw) memread=1;
	else memread=0;
	if(opcode==sw) memwrite=1;
	else memwrite=0;
	if((opcode==j)||(opcode==jal)) pcsrc1=1;
	else pcsrc1=0;
	if((opcode==bne)||(opcode==beq)) pcsrc2=1;
	else pcsrc2=0;
//0	printf("regdest %d regwrite %d alusrc %d memwrite %d memread %d memtoreg %d pcsrc2 %d pcsrc1 %d\n", regdest,regwrite,alusrc,memwrite,memread,memtoreg, pcsrc2,pcsrc1);
}

void writeback(){

	switch(opcode){
		case 0:
			if(rd!=0){
				r[rd]=temp;
				printf("write back to rd r[%d]=0x%08x (temp)\n",rd,temp);
			}
			break;
		case j:
			pc=jumpaddr(address);
				break;
			Jtype_count++;
		case jal:
			printf("0x%08x\n  %d \n",pc, jumpaddr(address));
			r[31]=pc+8;pc=jumpaddr(address);
			Jtype_count++;
			printf("JAL inside SUCCESS\n");
			printf("writeback to pc r[31]=0x%08x \n",pc);
			break;
		default :
			if(regwrite==1){
				r[rt]=temp;
				printf("write back to rt r[%d]=0x%08x (temp )\n", rt,temp);
			}
			break;

	} 
	
}

void main(){
	FILE *fp;
	int cycle=0;
	int data=0;
	int i=0;
	int ret=0;
	unsigned inst_temp;
	unsigned temp;
	r[31]=0xffffffff;

	r[29]=0x100000;

	fp=fopen("test_prog/fib.bin","rb");
	while(1){
		ret=fread(&data,sizeof(int),1,fp);
		if(ret==0){/*printf("not getting into main while\n");*/break;}
		inst=((data & 0xff)<<24)|(((data & 0xff000000)>>24)& 0xff);
		inst_temp=((data& 0xff00)<<8)|(((data & 0xff0000)>>8) & 0xff00);
		inst = (inst | inst_temp);
		printf("memory[0x%08x]: 0x%08x \n", i, inst);
		memory[i++]=inst;
	}
	fclose(fp);

	while(1){
		printf("NNNNNNNNNNNNNNNNNcycle : %d NNNNNNNNNNNNNNNNNN\n",cycle);
		fetch();
		decode();
		control_s();
		execution();	
		memoryacc();
		writeback();



//0		for(i=0;i<32;i++)
//0		{printf("r[%d]=%d   ",i,r[i]);}	
		if(pc==0xffffffff)break;
	
//	if(cycle==1060)break;
		if((funct !=jr) && (opcode!=j) && (opcode!= bne) && (opcode != beq) && (opcode!= jal))
	//	if(regwrite==1)
		{
//0			printf("opcode : %d,pc : 0x%08x\n",opcode,pc);
			pc=pc+4;
//0			printf("after pc value : 0x%08x\n",pc);
		}
		cycle++;
//0		printf("-------------control signals : ----------\n");
//0		printf("funct : %d, opcode : %d \n",funct,opcode);
//0		printf("the fucking result is : r[2]=%d\n\n\n\n \n",r[2]);
			regdest=0,regwrite=0,alusrc=0,memwrite=0,memread=0,memtoreg=0,pcsrc2=0,pcsrc1=0;
		funct=0,opcode=0;
		//	opcode=0,rs=0,rt=0,rd=0,shamt=0,funct=0,immediate=0,address=0;
	}//while close
printf("The final result : r[2] = %d\n",r[2]);
printf("Rtype_count: %d\nItype_count %d\n Jtype_count %d\n memoryacc count : %d\n branch instruction count : %d\n branch collecting count %d\n",Rtype_count,Itype_count,Jtype_count, memacc, branch+branch1,branch);
}//main close


