#ifndef CONTEXTUAL_H_INCLUDED
#define CONTEXTUAL_H_INCLUDED

#include <raylib.h>

typedef struct 
{
	char ** items;
	unsigned int item_count;
	int selected;

	Rectangle box;
	int hidden;

	int activated;

} Contextual;

Contextual CreateContextual(char ** items, unsigned int item_count)
{
	return (Contextual)
	{
		items,
		item_count,
		-1,
		(Rectangle) {0, 0, 150, item_count * 30},
		1,
		-1
	};
}

void OpenContextual(Contextual * c, Vector2 pos)
{
	c->box.x = pos.x;
	c->box.y = pos.y;
	c->hidden = 0;
}

void CloseContextual(Contextual * c)
{
	c->hidden = 1;
}

void UpdateContextual(Contextual  * c/*, Camera2D camera*/)
{
	Vector2 mouse = /*GetScreenToWorld2D*/(GetMousePosition()/*, camera*/);

	if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
	{	
		OpenContextual(c, mouse);
	}
	else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		if(!c->hidden)
		{
			if(CheckCollisionPointRec(mouse, c->box))
			{
				c->activated = (int) ((mouse.y - c->box.y)/(c->box.height/c->item_count));
			}
			else
				c->activated = -1;
		}
		CloseContextual(c);
	}
	else if(!c->hidden)
	{
		if(CheckCollisionPointRec(mouse, c->box))
		{
			c->selected = (int) ((mouse.y - c->box.y)/(c->box.height/c->item_count));
		}
		else
			c->selected = -1;
	}
}

void DrawContextual(Contextual c)
{
	if(c.hidden)return;

	const int font_size = (c.box.height / c.item_count) - 2;

	DrawRectangleRec(c.box, (Color) {200, 200, 200, 255});

	for(int i = 0; i < c.item_count; i++)
	{
		DrawText(c.items[i], c.box.x, c.box.y + i * (font_size + 2), font_size, BLACK);
		if(i == c.selected)
		{
			DrawRectangleLines(c.box.x, c.box.y + i * (font_size + 2) - 2, c.box.width, font_size + 4, RED);
		}
	}

}

char * GetActivatedItem(Contextual * c)
{
	char * r = "";

	if(c->activated != -1)
		r = c->items[c->activated];
	c->activated = -1;

	return r;
}

#endif