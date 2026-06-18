#pragma once

#include "config.h"

enum Colors{
	ROJO,
	VERDE,
	AZUL,
	AMARILLO,
	NARANJA,
	LIMA,
	CYAN,
	MORADO,
	MAGENTA,
	NEGRO,
	CAFE,
	BLANCO,
	PERU, // TERRACOTA
	RUST, // TERRACOTA OSCURA
	DORADO,
	OLIVA, // VERDE OLIVA
	ARENA,
	GRIS
};

class RGB{
public:
	float r;
	float g;
	float b;
};

extern RGB ColorTable[];

