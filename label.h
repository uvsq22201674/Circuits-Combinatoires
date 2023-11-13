#ifndef LABEL_H_INCLUDED
#define LABEL_H_INCLUDED

#include <string.h>
#include <stdio.h>

typedef struct
{
	char * text;
	Vector2 position;
	unsigned int cursor;

} Label;

Label CreateLabel(Vector2 pos)
{
	Label new_label = {malloc(sizeof(char) * 100), pos, 4};
	strcpy(new_label.text, "Label");

	return new_label;
}
void DestroyLabel(Label l)
{
	free(l.text);
}

void EditLabel(Label * l)
{

	char new_text[100];
	strcpy(new_text, l->text);

	int key = GetKeyPressed();

	if(IsKeyPressed(KEY_BACKSPACE))
	{
		int i = 0;
		do
		{
			if(new_text[i + 1] == '\0')
			{
				new_text[i] = '\0';
				break;
			}
			i ++;
		}while(i < 100);
		l->cursor --;
	}
	else if(key >= KEY_A && key <= KEY_Z)
	{
		
		int i = l->cursor + 2;
		do
		{
			if(new_text[i] == '\0')
			{
				new_text[i + 1] == '\0';
				new_text[i] = new_text[i - 1];
				break;
			}
			new_text[i] = new_text[i - 1]; 
			i ++;
		}while(i < 99);
		new_text[l->cursor + 1] = (char) (key);
		l->cursor ++;
	}

	strcpy(l->text, new_text);
}

void DrawLabel(Label l, int selected)
{
	char b4[100];
	strcpy(b4, l.text);
	b4[l.cursor + 1] = '\0';
	int cursor_x = l.position.x + MeasureText(b4, 28);

	DrawText(l.text, l.position.x, l.position.y, 28, BLACK);
	if(selected)
		DrawRectangle(cursor_x, l.position.y, 3, 28, BLACK);
}

int CheckPointLabelCollision(Vector2 point, Label l)
{
	return (point.x >= l.position.x
	&& point.y >= l.position.y
	&& point.x <= l.position.x + MeasureText(l.text, 28)
	&& point.y <= l.position.y + 28);
}

#endif