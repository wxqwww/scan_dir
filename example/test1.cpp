#include "scan_dir.h"
#include <iostream>
#include <dirent.h>

using namespace std;

int fun(const string &dirname, const string &basename, unsigned char mode, void *data)
{
	cout<<dirname<<basename<<endl;
}
int main()
{
	scan_dir scan("/etc", 5);
	scan.add_action(1, DT_DIR, fun, NULL);
	scan.add_action(2, DT_DIR, fun, NULL);
	scan.add_action(3, DT_DIR, fun, NULL);
	scan.add_action(4, DT_DIR, fun, NULL);
	scan.add_action(5, DT_DIR, fun, NULL);
	scan.scan();
}
