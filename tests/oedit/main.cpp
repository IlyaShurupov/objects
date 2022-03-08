

#include "oedit.h"

oeditor* oedit;

void imGuiWindowInitialize() {
	oedit = new oeditor();
}

void imGuiWindowFinalize() {
	delete oedit;
}

void imGuiWindowDraw() {
	oedit->draw();
}