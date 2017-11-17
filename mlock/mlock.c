#include <sys/mman.h>

int main(int argc, char **argv)
{
	char buf[8192];
	return (mlock(buf, sizeof(buf)) == 0) ? 0 : 1;
}
