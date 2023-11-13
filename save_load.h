#ifndef SAVE_LOAD_H_INCLUDED
#define SAVE_LOAD_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gates.h"
#include "label.h"

int AreStrEqual(char * a, char * b, unsigned int s)
{
	int to_return = 1;
	for(int i = 0; i < s; i++)
	{
		if(b[i] != a[i])
		{
			to_return = 0;
			break;
		}
	}
	return to_return;
}

void PrintLine(char * s)
{
	int i = 0;
	while(s[i] != '\n' && s[i] != '\0')
	{
		printf("%c", s[i]);
		i++;
	}
	printf("\n");
}

void SetGateHeader(Gate * g, char * data)
{
	int offset = 0;

	if(AreStrEqual("SWITCH", data + 5, 6))
	{
		g->type = "SWITCH";
		offset = 11;
	}
	else if(AreStrEqual("AND", data + 5, 3))
	{
		g->type = "AND";
		offset = 8;
	}
	else if(AreStrEqual("OR", data + 5, 2))
	{
		g->type = "OR";
		offset = 7;
	}
	else if(AreStrEqual("XOR", data + 5, 3))
	{
		g->type = "XOR";
		offset = 8;
	}
	else if(AreStrEqual("OUTPUT", data + 5, 6))
	{
		g->type = "OUTPUT";
		offset = 11;
	}

	char id[16] = {0};
	int i = offset;
	while(data[i] != '\n' && data[i] != '\0' && i - offset < 16)
	{
		id[i - offset] = data[i];
		i ++;
	}
	g->id = atoi(id);
}

void ParseTuple(char * line, float * x, float * y)
{
	int state = 0;
	int i = 0;
	char buffer[12] = {0};
	unsigned int buffer_index = 0u;

	while(state != 3 && line[i] != '\0')
	{
		if(state == 1)
		{
			if(line[i] == ',')
			{
				*x = atoi(buffer);
				buffer[0] = '\0';
				buffer_index = 0;
				state = 2;
			}
			else if(line[i] != ' ')
			{
				buffer[buffer_index] = line[i];
				buffer_index ++;
			}
		}
		else if(state == 2)
		{
			if(line[i] == ')')
			{
				*y = atoi(buffer);
				state = 3;
			}
			else if(line[i] != ' ')
			{
				buffer[buffer_index] = line[i];
				buffer_index ++;
			}
		}

		if(line[i] == '(') state = 1;

		i++;
	}
}

void ConnectInput(Gate ** input_x, char * data, Gate * gates, unsigned int gates_count)
{
	int state = 0;
	int i = 0;
	char buffer[12] = {0};
	unsigned int buffer_index = 0u;

	while(state != 3 && data[i] != '\0')
	{
		if(data[i] == '=')
			state = 1;
		else if(state == 1)
		{
			if(data[i] == '\n')
			{
				state = 3;
			}
			else if(data[i] != ' ' && buffer_index < 12)
			{
				buffer[buffer_index] = data[i];
				buffer_index ++;
			}
		}
		i ++;
	}

	*input_x = NULL;
	for(int i = 0; i < gates_count; i++)
	{
		if(gates[i].id == atoi(buffer))
			*input_x = gates + i;
	}

}

void SetGateBody(Gate * g, char * code, Gate * gates, unsigned int gates_count)
{
	int state = 0;

	int i = 0;
	while(state != 3)
	{
		if(state == 1)
		{
			if(AreStrEqual("position", code + i, 8))
				ParseTuple(code + i, &g->position.x, &g->position.y);
			if(AreStrEqual("input_a", code + i, 7))
				ConnectInput(&g->input_a, code + i, gates, gates_count);
			if(AreStrEqual("input_b", code + i, 7))
				ConnectInput(&g->input_b, code + i, gates, gates_count);
		}
		if(code[i] == '}')state = 3;
		if(code[i] == '{')state = 1;

		i ++;
	}
}

int LoadCircuit(char * path, Gate * gates, unsigned int * gates_count, Label * labels, unsigned int * label_count)
{
	FILE * filin = fopen(path, "r");

	if(!filin)
	{
		fclose(filin);
		return 0;
	}

	fseek(filin, 0, SEEK_END);
	const long int size = ftell(filin);
	fseek(filin, 0, SEEK_SET);

	char * code = malloc(sizeof(char) * size);
	fread(code, size, size, filin);

	fclose(filin);

	Gate gate_work = {0};
	gate_work.size = 28;

	for(int i = 0; i < size; i++)
	{
		if(AreStrEqual("Gate", code + i, 4))
		{
			SetGateHeader(&gate_work, code + i);
			SetGateBody(&gate_work, code + i, gates, *gates_count);
			gates[*gates_count] = gate_work;
			*gates_count = *gates_count + 1;
			printf("Added gate %s named %d\n", gate_work.type, gate_work.id);
			
			gate_work.input_a = NULL;
			gate_work.input_b = NULL;
		}
	}

	free(code);

	return 1;
}

#endif