


#include<stdio.h>
#include<string.h>
#include<stdlib.h>


//function declarations
int haveOrNot(char*);
int inMacroCodes(char*);
char* findMacroCode(char*);
void macrosOperations(char*, char*);
char* changeString(char*, int);
int whichOperation(char*);



//General Global Variables
int ifElseCondition = 0;
int ifElseActiveOrNot = 0;

struct symbol
{
	char name[10];	// symbol name
	char value[10];	// replacement text
};
struct symbol def_symbols[50];
int x = 0; //symbol table index


struct mac
{
	char mname[10];	// macro name
	int nparams;	// number of parameters
	char p[3][10];	// dummy parameters
	char mbody[1000];	// macro body
};
struct mac macros[20];	// maximum 20 macros can be defined
int y = 0; //macro table index
int a = 0; //macro table counter


struct param
{
	char dp[3][10];	// dummy paramaters
	char rp[3][10];	// real paramaters
};
struct param PT[5];	//maximum 5 nested macro calls



//Macro Codes
char* macroCodes[7] = { "@DEFINE","@MACRO","@ENDM","@MCALL","@IF","@ELSE","@ENDIF" };


//File pointers
FILE* file;
FILE* file_w;


void macrosOperations(char* macroCode, char* line)
{
	char* token = (char*)malloc(50);

	if (!strcmp("@DEFINE", macroCode))
	{
		token = strtok(line, " ");

		token = strtok(NULL, " ");
		strcpy(def_symbols[x].name, token);

		token = strtok(NULL, " \n");
		strcpy(def_symbols[x].value, token);


		//printf("%s  %s  %s", macroCode, def_symbols[0].name, def_symbols[0].value); TEST! 

		//SHOW 
		printf("Name:%s\n", def_symbols[x].name);
		printf("Value:%s\n\n\n", def_symbols[x].value);

		++x;
	}
	else if (!strcmp("@MACRO", macroCode))
	{
		if (a == 0)
		{
			token = strtok(line, " ");

			token = strtok(NULL, "'\t'( ");
			strcpy(macros[y].mname, token);

			macros[y].nparams = 0;
			while (macros[y].nparams <= 3)
			{
				token = strtok(NULL, ",)(");
				if (token[0] == '\n')
				{
					++a;
					break;
				}
				strcpy(macros[y].p[macros[y].nparams], token);
				++macros[y].nparams;
			}
		}
		else if (a == 1)
		{
			//while macrocode is @MACRO after operation above
			strcpy(macros[y].mbody + strlen(macros[y].mbody), line);
		}
	}
	else if (!strcmp("@ENDM", macroCode))
	{
		a = 0;

		//SHOW
		printf("Macro Name:%s\n", macros[y].mname);
		printf("Number of Parameters:%d\n", macros[y].nparams);
		printf("Macro Body:\n%s", macros[y].mbody);
		for (int i = 0; i < macros[y].nparams; ++i)
			printf("Dummy[%d]:%s\n", i, macros[y].p[i]);
		printf("\n\n\n");

		++y;
		return;
	}
	else if (!strcmp("@MCALL", macroCode))
	{
		if (!ifElseActiveOrNot)
		{
			char line_temp[100];
			strcpy(line_temp, line);
			token = strtok(line, " ");
			token = strtok(NULL, "'\t'("); //this token must be controlled


			int index = 0;
			//finding true macros[x]
			for (int i = 0; i < y; ++i)
			{
				if (!strcmp(token, macros[i].mname))
				{
					index = i;   //so we know true macros!!
					break;
				}
				else if (i == y - 1)
				{
					printf("Error Message:Macros names have no matches so please control these names\n");
					printf("Your wrong macro code:%s\n\n\n", token);
					return;
				}
			}


			//filling dummy parameters
			for (int i = 0; i < macros[index].nparams; ++i)
			{
				strcpy(PT[0].dp[i], macros[index].p[i]);
			}

			//filling real parameters
			for (int i = 0; i < macros[index].nparams; ++i)
			{
				token = strtok(NULL, ",)(");
				strcpy(PT[0].rp[i], token);
			}


			//macrobody operations
			char* temp = (char*)malloc(100);
			strcpy(temp, line_temp);
			char* macroBodyTemp = (char*)malloc(200);
			strcpy(macroBodyTemp, macros[index].mbody);


			temp = strtok(macroBodyTemp, "\n"); //PROBLEM is here!!



			while (temp != '\0')
			{
				//change operation!
				temp = changeString(temp, index);

				fprintf(file_w, "%s\n", temp);
				temp = strtok(NULL, "\n");
			}
		}

		else if (ifElseActiveOrNot)
		{
			char line_temp[100];
			strcpy(line_temp, line);
			token = strtok(line, " ");
			token = strtok(NULL, "'\t'("); //this token must be controlled


			int index = 0;
			//finding true macros[x]
			for (int i = 0; i < y; ++i)
			{
				if (!strcmp(token, macros[i].mname))
				{
					index = i;   //so we know true macros!!
					break;
				}
				else if (i == y - 1)
				{
					printf("Error Message:Macros names have no matches so please control these names\n");
					printf("Your wrong macro code:%s\n\n\n", token);
					return;
				}
			}


			//filling dummy parameters
			for (int i = 0; i < macros[index].nparams; ++i)
			{
				strcpy(PT[0].dp[i], macros[index].p[i]);
			}

			//filling real parameters
			for (int i = 0; i < macros[index].nparams; ++i)
			{
				token = strtok(NULL, ",)(");
				strcpy(PT[0].rp[i], token);
			}


			//macrobody operations
			char* temp = (char*)malloc(100);
			strcpy(temp, line_temp);
			char* macroBodyTemp = (char*)malloc(200);
			strcpy(macroBodyTemp, macros[index].mbody);


			temp = strtok(macroBodyTemp, "\n"); //PROBLEM is here!!


			if (ifElseCondition)
			{
				while (temp != '\0')
				{
					//change operation!
					temp = changeString(temp, index);

					fprintf(file_w, "%s\n", temp);
					temp = strtok(NULL, "\n");
				}
			}


		}

	}
	else if (!strcmp("@IF", macroCode)) //Burada bir koþul belirle...o koþul gerçekleþmezse tekrar bu metoda girsin ve @ELSE bloguna geçsin
	{
		ifElseActiveOrNot = 1; //@IF condition is active now!
		int trueParameter = 0;
		int operation = 0;
		char* temp;
		char* line_temp = (char*)malloc(50);
		strcpy(line_temp, line);

		operation = whichOperation(line_temp);

		temp = strtok(line_temp, "(");
		temp = strtok(NULL, "=><");     // after this operation temp represents to def_table name at source code!

		for (int i = 0; i < x; ++i)
		{
			if (!strcmp(def_symbols[i].name, temp))
			{
				trueParameter = i;
			}
			else if (i == x)
			{
				printf("Error:Your condition name is incorrect.You must control this value at Source Code!\n");
				printf("Wrong entered name:%s", temp);
			}
		}



		temp = strtok(NULL, ")"); // after this operation temp represents to def_table value at source code!


		//operation checking
		if (operation == 1)  //'='
		{
			if (atoi(temp) == atoi(def_symbols[trueParameter].value))
			{
				ifElseCondition = 1;
			}
		}
		else if (operation == 2) //'>'
		{
			if (atoi(temp) < atoi(def_symbols[trueParameter].value))
			{
				ifElseCondition = 1;
			}
		}
		else if (operation == 3) //'<'
		{
			if (atoi(temp) > atoi(def_symbols[trueParameter].value))
			{
				ifElseCondition = 1;
			}
		}
		else //not found!
			return;
	}
	else if (!strcmp("@ELSE", macroCode))
	{
		if (ifElseCondition == 0)
		{
			ifElseCondition = 1;
			return;
		}

		else if (ifElseCondition == 1)
		{
			ifElseCondition = 0;
			return;
		}
	}
	else if (!strcmp("@ENDIF", macroCode))
	{
		ifElseActiveOrNot = 0; //@IF-@ELSE condition is passive now!
	}
	else
		return;
}


int whichOperation(char* str) //0 ->not   1->'='  2->'>'  3->'<'
{
	for (int i = 0; i < strlen(str); ++i)
	{
		if (str[i] == '=')
		{
			return 1;
			break;
		}
		else if (str[i] == '>')
		{
			return 2;
			break;
		}
		else if (str[i] == '<')
		{
			return 3;
			break;
		}
	}

	printf("Please control your if-else condition.Something is going wrong!\n\n");
	return 0; //non of these are true!
}


char* changeString(char* temp, int index)
{
	//mbody[] içindeki kodlara göre düþün!!!
	char* newTemp = (char*)malloc(200);
	memset(newTemp, 0, 200);

	strcpy(newTemp, temp);

	int address = 0;
	int trueParameter = 0;
	int haveDummyOrNot = 0;
	for (int i = 0; i < macros[index].nparams; ++i) //trying for both dummy parameters because we don't know which parameter has been found in this string
	{
		for (int j = 0; j < strlen(temp); ++j)
		{
			if (temp[j - 1] == '\t' && temp[j] == PT[0].dp[i][0] && temp[j + 1] == ',')
			{
				address = j;
				trueParameter = i;
				haveDummyOrNot = 1;
				break;
			}
			else if (temp[j - 1] == ',' && temp[j] == PT[0].dp[i][0])
			{
				address = j;
				trueParameter = i;
				haveDummyOrNot = 1;
				break;
			}
		}
	}





	//substituting dummy and real parameters
	if (haveDummyOrNot)
	{
		//shifting operation
		if (strlen(PT[0].rp[trueParameter]) > 2)
		{
			for (int i = 0; i < strlen(PT[0].rp[trueParameter]) - 1; ++i)
			{
				newTemp[address + 1 + i + (strlen(PT[0].rp[trueParameter]) - 1)] = newTemp[address + 1 + i];
			}

			for (int i = 0; i < strlen(PT[0].rp[trueParameter]); ++i)
			{
				newTemp[address + i] = PT[0].rp[trueParameter][i];
			}
		}
		else //special case for length=2!
		{
			for (int i = 2; i > 0; --i)
			{
				newTemp[address + i + 1] = newTemp[address + i];
			}

			for (int i = 0; i < strlen(PT[0].rp[trueParameter]); ++i)
			{
				newTemp[address + i] = PT[0].rp[trueParameter][i];
			}
		}
	}


	return newTemp;
}


char* findMacroCode(char* line)
{
	char* token;
	token = strtok(line, " '\t'\n'");

	while (token != '\0')
	{
		if (inMacroCodes(token))
			return token;
		token = strtok(NULL, " ");
	}
}



int haveOrNot(char* line) //return whether any line read has one of the macroCode's element or not
{
	char* token;
	token = strtok(line, " '\t'\n'");

	while (token != '\0')
	{
		if (inMacroCodes(token))
			return 1;
		token = strtok(NULL, " ");
	}
	return 0;
}

int inMacroCodes(char* str)  //return whether any line's token is same one of the macroCode's element or not
{
	for (int i = 0; i < strlen(macroCodes[0]); ++i)
	{
		if (!strcmp(macroCodes[i], str))
			return 1;
		else if (i == strlen(macroCodes[0]) - 1)
			return 0;
	}
}




int main()
{
	char data[100]; //for line by line scanning
	char* token = (char*)malloc(10);
	char* line_temp = (char*)malloc(100); //record line that we are on

	if ((file = fopen("SourceCode2.txt", "r")) == NULL) //If file opening have been failed
	{
		printf("File opening have failed!");
		return 0;
	}

	if ((file_w = fopen("ExpandedCode.txt", "w")) == NULL) //If file opening have been failed
	{
		printf("File_w opening have failed!");
		return 0;
	}

	while (!feof(file))
	{
		fgets(data, 100, file);
		strcpy(line_temp, data);
		if (haveOrNot(data) || !strcmp(token, "@MACRO"))  //whether current line has any macro call or not
		{
			if (!strcmp(token, "@MACRO") && a == 1)
			{
				if (haveOrNot(data) == 0)
					macrosOperations(token, line_temp);
				else if (haveOrNot(data) == 1)
				{
					strcpy(token, findMacroCode(data)); //so token has macro of the line which we are on!
					macrosOperations(token, line_temp);
				}
			}
			else
			{
				strcpy(token, findMacroCode(data)); //so token has macro of the line which we are on!
				macrosOperations(token, line_temp);
			}
		}
		else
		{
			fprintf(file_w, "%s", line_temp);
		}
	}
	fclose(file_w);
	free(token);
	fclose(file);

	return 0;
}

