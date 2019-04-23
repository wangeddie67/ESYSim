/*
 * regular.c
 *
 *  Created on: 2016年12月12日
 *      Author: root
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regular.h"

void exit_err(const char* msg){
	printf("it's a error, the msg is %s\n", msg);
	exit(1);
}

/*为objdump后的指令对应C行数的若干个处理函数*/
//\t->blank
char* t2blank(const char* str_init){
	int i,j = 0;
	int str_rt_len = strlen(str_init);
	char* str_rt = (char*)malloc(str_rt_len+1);
	int isblank = 0;
	for(i=0;i<str_rt_len;i++){
		if(str_init[i]=='\t')
			continue;
		if(str_init[i]==' '){
			if(isblank){
				continue;
			}
			isblank = 1;
		}
		else
			isblank = 0;
		memcpy(&(str_rt[j]),&(str_init[i]),sizeof(char));
		j++;
	}
	str_rt[j] = '\0';
	return str_rt;
}


int analysis_regex(const char* ana_obj, FILE* fto, int cline){
	regex_t reg,reg_block;
	regmatch_t match,match_block;
	int status;
	char* rcdstr;
//	char* errorbuf = (char*)malloc(20);
	const char* inst_line_pattern = "^[ ]*[0-9a-z]*:[ ]*\t[0-9a-z][0-9a-z][ ][0-9a-z][0-9a-z][ ][0-9a-z][0-9a-z][ ][0-9a-z][0-9a-z][ ]*\t";
	if((status = regcomp(&reg, inst_line_pattern, REG_ICASE))!=0){return status;}
	if((status = regexec(&reg, ana_obj, 1, &match, 0))!=0){return status;}
	char* matched_string = (char*)malloc(match.rm_eo+1);
//	memset(matched_string,'\0',sizeof(matched_string));
	memset(matched_string,'\0',match.rm_eo+1);
	strncpy(matched_string, ana_obj, match.rm_eo);
//匹配ana_obj开头的空格以及编号和冒号,并删除
	const char* block_pattern = "^[ ]*[a-z0-9]*:";
	status = regcomp(&reg_block, block_pattern, REG_ICASE);
	status = regexec(&reg_block, matched_string, 1, &match_block, 0);

	char* result_string = (char*)malloc(match.rm_eo-match_block.rm_eo);
//	memset(result_string,'\0',sizeof(result_string));
	memset(result_string,'\0',match.rm_eo-match_block.rm_eo);
//删除空格，记录到新的字符串result_string
	strncpy(result_string, ana_obj+match_block.rm_eo, match.rm_eo-match_block.rm_eo);
//	printf("the result matched string is %s\n",result_string);
//没有错误，则进行写入
	if(status==0){
//		fwrite(result_string, sizeof(result_string)+4, 1, fto);
		rcdstr = t2blank(result_string);//将\t替换为空格
		fprintf(fto,"%s%d\n",rcdstr, cline);

	}
	return 0;
}

int analysis_cline(char* ana_obj, FILE* fto, int* cline){
	regex_t reg,reg_num; regmatch_t match,match_num;
	int status;
//	char* errorbuf = (char*)malloc(20);
	const char* c_line_pattern = "^[a-zA-Z0-9_]*.c:[0-9]*";
	const char* c_getline_pattern = "^[a-zA-Z0-9_]*.c:";
	if(cline<=0){exit_err("it's a error: cline<=0");}
	if((status = regcomp(&reg, c_line_pattern, REG_ICASE))!=0){return status;}
	if((status = regexec(&reg, ana_obj, 1, &match, 0))!=0){return status;}
	if((status = regcomp(&reg_num, c_getline_pattern, REG_ICASE))!=0){return status;}
	if((status = regexec(&reg_num, ana_obj, 1, &match_num, 0))!=0){return status;}
	char* matched_string = (char*)malloc(match.rm_eo+1);
	memset(matched_string,'\0',match.rm_eo+1);
	strncpy(matched_string, ana_obj, match.rm_eo);
	char line[4] = {};
	memcpy(line,&(matched_string[match_num.rm_eo]),match.rm_eo - match_num.rm_eo);
	*cline = atoi(line);
//	if(status==0){
//			fprintf(fto, "%d", *cline);
//	//		rcdstr = t2blank(result_string);//将\t替换为空格
//			fputs("\n",fto);
//	}
	return 0;
}

int analysis_file(const char* ffrom, const char* __restrict__ fto, int fbuffer){
	int status;
	FILE* filefrom = fopen(ffrom,"r");
	FILE* fileto = fopen(fto,"w+");
	if(!filefrom){exit_err("it's a error, ffrom is filed to open\n");}
	if(!fileto){exit_err("it's a error, fto is filed to open\n");}
	char* filebuf = (char*)malloc(fbuffer*BUFSIZ);
	setbuf(fileto,filebuf);
	char file_line[512];//每一行最多512个字符
	memset(file_line,'\0',sizeof(file_line));
	int cline = -1;
	while(fgets(file_line,sizeof(file_line),filefrom)!=NULL){
		analysis_cline(file_line,fileto,&cline);
		analysis_regex(file_line,fileto,cline);
		memset(file_line,'\0',sizeof(file_line));
	}

	fclose(fileto);
	return 0;
}
/*以上为objdump后的指令对应C行数*/

/*处理两个文件之间的关系：
 * 文件1：objdump后，指令对应C行数的文件
 * 文件2：读入text段后，指令对应PC值的文件*/

int getstartline(FILE* asm2cline, FILE* pc2asm, int col_asmf1, int col_cline, int col_asmf2, int col_pc){
//	map<int,char*>f2asmstream;
	if((col_asmf1||col_asmf2) != 0){
		exit_err("it's a error, asm column of files <sm2line> and <pc2asm> must be 0");
	}
	char* f2asmstream = (char*)malloc(5000000*sizeof(char));
	char* f1asmstream = (char*)malloc(5000000*sizeof(char));
	char f2line[80];
	char* ctemp = NULL;
	int size_f2line = sizeof(f2line);
	int f1linenum = 0, f2linenum = 0;
	int charindex = 0, comp = 0;
	memset(f2asmstream,'\0',sizeof(f2asmstream));
	memset(f2line,'\0',size_f2line);
	while(fgets(f2line,size_f2line,pc2asm)!=NULL){
		ctemp = strtok(f2line," ");
//		f2asmstream[f2linenum] = *ctemp;
		strcpy(&f2asmstream[f2linenum*2],ctemp);
		f2linenum++;
	}
	while(fgets(f2line,size_f2line,asm2cline)!=NULL){
			ctemp = strtok(f2line," ");
			strcpy(&f1asmstream[f1linenum*2],ctemp);
//			f1asmstream[f2linenum] = *ctemp;
			f1linenum++;
	}
	f1linenum--, f2linenum--;
	char* location = strstr(f2asmstream,f1asmstream);
	if(!location) {exit_err("it's a error, inst(pc->cline) match error");}
	//利用字符串之间的地址差，来计算出指令完全匹配时首次匹配到pc->asm的多少行，行数从0开始，除以2是因为每个指令占据2个char
	return (((long int)location - (long int)f2asmstream)/2);
}


//从asm_cline文件某行析出cline
inline int getcline(char* s){
	char* index = s;
	int i, numofblank=0, rt;
	char* result_s = calloc(100,sizeof(char));
	memset(result_s,'\0',100);
	for(i=0;i<strlen(s);i++){
		if(index[i] != ' ')
			continue;
		numofblank++;
		if(numofblank==4)
			break;
	}
	i++;
	memcpy(result_s,&(index[i]),strlen(index)-i-1);
	rt = atoi(result_s);
	free(result_s);
	return rt;
}

void pc2cline(FILE* fout, FILE* pc_asm, FILE* cline_asm, long int startline){
	int index=0,sign = 0, cline;
	char pc_asm_theline[60];
	char cline_asm_theline[60];
	memset(pc_asm_theline,'\0',sizeof(pc_asm_theline));
	memset(cline_asm_theline,'\0',sizeof(cline_asm_theline));
	while(fgets(pc_asm_theline,sizeof(pc_asm_theline),pc_asm) != NULL){
		index++;
		if(index==startline)	sign=1;
		if(sign){//进入匹配段，二者开始同时读取
			if(fgets(cline_asm_theline,sizeof(cline_asm_theline),cline_asm)){
				cline = getcline(cline_asm_theline);//获得当前pc文件行对应的cline
//				printf("cline is %d\n",cline);
				//处理pc_asm字符串中的\n然后记录文件
				pc_asm_theline[strlen(pc_asm_theline)-1] = '\0';
				fprintf(fout,"%s %d\n",pc_asm_theline, cline);
			}
			else
				break;
		}
		memset(pc_asm_theline,'\0',sizeof(pc_asm_theline));
		memset(cline_asm_theline,'\0',sizeof(cline_asm_theline));
	}
}



int main(){
//	int status;
//	status = analysis_file("../testmpi/hasgdb-i","result",1);
//	printf("status = %d\n",status);

	FILE* f1 = fopen("result","r");
	FILE* f2 = fopen("../records/inststream/node_5","r");
	FILE* f = fopen("../records/pc2clinefile","w+");
	int line = getstartline(f1,f2,0,4,0,1);
	fseek(f1,0,0);
	fseek(f2,0,0);
	pc2cline(f,f2,f1,line+1);
	return 0;
}
