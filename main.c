#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include "gates.h"
#include "contextual.h"
#include "label.h"
#include "save_load.h"

void remove_gate(Gate * gates, unsigned int * gates_count, unsigned int index)
{
	if(index < 0 || index >= *gates_count)
		return;

	for(int i = 0; i < *gates_count; i++)
	{
		if(gates[i].input_a == &gates[index])
			gates[i].input_a = NULL;
		if(gates[i].input_b	 == &gates[index])
			gates[i].input_b = NULL;
	}
	for(int i = index; i < *gates_count; i++)
		gates[i] = gates[i + 1];

	*gates_count = *gates_count - 1;
}

int main(int argc, char ** argv)
{

	InitWindow(800, 800, "Circuit Combinatoire");
	SetTargetFPS(60);

	Gate gates[100] = {0};
	unsigned int gates_count = 0u;
	int selected_gate = -1;

	Label labels[100] = {0};
	unsigned int labels_count = 0u;
	int selected_label = -1;

	if(argc > 1)
	{
		LoadCircuit(argv[1], gates, &gates_count, labels, &labels_count);
	}

	char ** addable = malloc(sizeof(char*) * 7);
	addable[0] = "Label";
	addable[1] = "AND";
	addable[2] = "OR";
	addable[3] = "NON";
	addable[4] = "SWITCH";
	addable[5] = "OUTPUT";
	addable[6] = "XOR";	
	Contextual add_menu = CreateContextual(addable, 7);

	Camera2D camera = {0};
	camera.zoom = 1.f;

	int testing = 0;
	while(!WindowShouldClose())
	{

		camera.zoom *= 1.f + GetMouseWheelMove()/50.f;
		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			camera.target = Vector2Subtract(camera.target, Vector2Scale(GetMouseDelta(), 1.f/camera.zoom));
		}

		if(selected_gate != -1)
		{
			if(IsKeyDown(KEY_UP)) gates[selected_gate].position.y --;
			if(IsKeyDown(KEY_LEFT)) gates[selected_gate].position.x --;
			if(IsKeyDown(KEY_DOWN)) gates[selected_gate].position.y ++;
			if(IsKeyDown(KEY_RIGHT)) gates[selected_gate].position.x ++;
		}

		if(IsKeyReleased(KEY_DELETE) && selected_gate != -1)
		{
			remove_gate(gates, &gates_count, selected_gate);
			selected_gate = -1;
		}

		if(selected_label != -1)
		{
			EditLabel(&labels[selected_label]);
		}

		for(int i = 0; i < labels_count; i++)
		{
			if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckPointLabelCollision(GetScreenToWorld2D(GetMousePosition(), camera), labels[i]))
				selected_label = i;
		}

		int selected = 0;
		int temp = selected_gate;

		for(int i = 0; i < gates_count; i++)
		{
			if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			{
				if(CheckPointGateCollision(GetScreenToWorld2D(GetMousePosition(), camera), gates[i]))
				{
					if(IsKeyDown(KEY_LEFT_CONTROL) && selected_gate != -1)
					{
						if(gates[i].input_a == NULL)
							gates[i].input_a = &gates[selected_gate];
						else if(gates[i].input_b == NULL)
							gates[i].input_b = &gates[selected_gate];
						else
							gates[i].input_a = &gates[selected_gate];
					}
					
					temp = i;
					selected = 1;
				}
				else if(!selected)
					temp = -1;

			}


			UpdateGate(&gates[i], camera);
		}
		if(selected)
			selected_gate = temp;

		UpdateContextual(&add_menu/*, camera*/);
		char * to_add = GetActivatedItem(&add_menu);
		if(to_add != "" && gates_count < 100)
		{
			printf("Adding %s.\n", to_add);
			if(strcmp(to_add, "Label") == 0)
			{
				labels[labels_count] = CreateLabel(GetScreenToWorld2D((Vector2) {add_menu.box.x, add_menu.box.y}, camera));
				selected_label = labels_count;
				labels_count ++;
			}
			else
			{
				gates[gates_count] = CreateGate(to_add, gates_count, GetScreenToWorld2D((Vector2) {add_menu.box.x, add_menu.box.y}, camera));
				gates_count ++;
			}	
		}

		BeginDrawing();
		BeginMode2D(camera);
			ClearBackground(WHITE);

			for(int i = 0; i < gates_count; i++)
			{
				DrawGate(gates[i], i == selected_gate);
			}
			for(int i = 0; i < labels_count; i++)
				DrawLabel(labels[i], selected_label == i);

		EndMode2D();
		DrawContextual(add_menu);
		EndDrawing();
	}

	for(int i = 0; i < labels_count; i++)
		DestroyLabel(labels[i]);
	free(addable);

	CloseWindow();

	return EXIT_SUCCESS;
}