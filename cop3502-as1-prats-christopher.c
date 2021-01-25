#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sp21_cop3502_as1.h"
#include "leak_detector_c.h"
//Function Constructors

//The Main Function
int main() {
	//Report Memory Leaks When the Program Exits
	atexit(report_mem_leak);
	return 0;
}


