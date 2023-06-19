#include <stdio.h>
#include <string.h>
#include "outputs.h"

int main (int argc, char ** argv)
{
	if (argc == 1)
	{
		return 1;
	}
	for (int idx = 0; outputs[idx].args != NULL; idx++)
	{
		bool match = true;
		int arg;
		for (arg = 0; arg < argc - 1; arg++)
		{
			if (outputs[idx].args[arg] == NULL)
			{
				match = false;
				break;
			}
			if (strcmp(argv[arg + 1], outputs[idx].args[arg]) != 0)
			{
				match = false;
				break;
			}
		}
		if (match == true && arg == argc - 1 && outputs[idx].args[arg] == NULL)
		{
			fwrite(outputs[idx].info, 1, outputs[idx].size, stdout);
			fflush(stdout);
			return 0;
		}
	}
	return 1;
}