#ifndef GATES_H_INCLUDED
#define GATES_H_INCLUDED

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Gate
{
	char * type;
	int id;
	struct Gate * input_a;
	struct Gate * input_b;

	int state;

	Vector2 position;
	int size;

};
typedef struct Gate Gate;

Gate CreateGate(char * type, int id, Vector2 pos)
{
	return (Gate) {
		type,
		id,
		NULL,
		NULL,
		0,
		pos,
		28
	};
}

int AreInputsValid(Gate * g)
{
	return (g->input_a != NULL && g->input_b != NULL);
}

void AndGate(Gate * g)
{
	if(AreInputsValid(g))
	{
		if(g->input_a->state && g->input_b->state)
		{
			g->state = 1;

			return;
		}
	}

	g->state = 0;
}

Gate * GetValidInput(Gate * g)
{
	if(g->input_a)
		return g->input_a;
	else
		return g->input_b;
}

void OrGate(Gate * g)
{
	if(AreInputsValid(g))
	{
		if(g->input_a->state || g->input_b->state)
		{
			g->state = 1;

			return;
		}
	}
	else if(g->input_a != NULL)
	{
		g->state = g->input_a->state;
		return;
	}
	else if(g->input_b != NULL)
	{
		g->state = g->input_b->state;
		return;
	}

	g->state = 0;
}

void NonGate(Gate * g)
{
	Gate * input = GetValidInput(g);
	if(input != NULL)
		g->state = !input->state;
}

void OutputGate(Gate * g)
{
	Gate * input = GetValidInput(g);
	if(input != NULL)
		g->state = input->state;	
}

void XorGate(Gate * g)
{
	if(AreInputsValid(g))
	{
		if(g->input_a->state != g->input_b->state)
		{
			g->state = 1;

			return;
		}
	}
	else if(g->input_a != NULL)
	{
		if(g->input_a->state != 0)
		{
			g->state = 1;

			return;
		}
	}
	else if(g->input_b != NULL)
	{
		if(0 != g->input_b->state)
		{
			g->state = 1;

			return;
		}
	}

	g->state = 0;
}

void UpdateGate(Gate * g, Camera2D camera)
{
	if(strcmp(g->type, "AND") == 0)
	{
		AndGate(g);
	}
	else if(strcmp(g->type, "OR") == 0)
	{
		OrGate(g);
	}
	else if(strcmp(g->type, "NON") == 0)
	{
		NonGate(g);
	}
	else if(strcmp(g->type, "OUTPUT") == 0)
	{
		OutputGate(g);
	}
	else if(strcmp(g->type, "XOR") == 0)
	{
		XorGate(g);
	}
	else if(strcmp(g->type, "SWITCH") == 0)
	{
		Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);

		if(mouse.x >= g->position.x
		&& mouse.y >= g->position.y
		&& mouse.x <= g->position.x + MeasureText(g->type, g->size)
		&& mouse.y <= g->position.y + g->size
		&& IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			g->state = !g->state;	
		}
	}
}

void DrawGate(Gate g, int selected)
{
	Color col = RED;
	if(g.state)
		col = GREEN;

	Gate * a = g.input_a;
	if(a != NULL)
	{
		Color colo = RED;
		if(a->state)
			colo = GREEN;
		DrawLine(a->position.x + MeasureText(a->type, a->size), a->position.y + (a->size + 4) / 2, g.position.x, g.position.y + (g.size + 4) / 2, colo);
	}
	Gate * b = g.input_b;
	if(b != NULL)
	{
		Color colo = RED;
		if(b->state)
			colo = GREEN;
		DrawLine(b->position.x + MeasureText(b->type, b->size), b->position.y + (b->size + 4) / 2, g.position.x, g.position.y + (g.size + 4) / 2, colo);
	}

	if(selected)
		DrawRectangle(g.position.x - 5, g.position.y - 5, MeasureText(g.type, g.size - 4) + 30, g.size + 10, BLUE);
	DrawRectangle(g.position.x, g.position.y, MeasureText(g.type, g.size - 4) + 20, g.size, col);
	DrawText(g.type, g.position.x + 10, g.position.y + 2, g.size - 4, BLACK);

}

int CheckPointGateCollision(Vector2 point, Gate gate)
{
	return CheckCollisionPointRec
	(
		point,
		(Rectangle)
		{
			gate.position.x,
			gate.position.y,
			MeasureText(gate.type, gate.size - 4) + 20,
			gate.size
		}
	);
}

#endif