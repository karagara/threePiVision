/* main.cpp
 * Authors: Yan Liu, Colten Normore
 *
 * Main method for the 3pi/R.Pi vision system. Sets up the individual modules,
 * and gets things rolling
 */

#include <iostream>
#include "vision.h"
#include "controller.h"
#include "logic.h"

int main() {
	vision* v_module = new vision();
	controller* c_module = new controller();
	logic* l_module = new logic();

}
