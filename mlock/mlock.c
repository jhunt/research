#include <sys/mman.h>

int main(int argc, char **argv)
{
	return (mlockall(MCL_CURRENT | MCL_FUTURE) == 0) ? 0 : 1;
}
