#include "levdist.h"

int main(int argc, char* argv[])
{
	// Create a controller "object"
	levdist_t levdist;
	levdist_init(&levdist);

	// Run the command
	return levdist_run(&levdist, argc, argv);
}
