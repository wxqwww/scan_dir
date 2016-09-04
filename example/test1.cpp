#include "scan_dir.h"
#include <iostream>
using namespace std;

int fun(const string &dirname, const string &basename, int mode, void *data)
{
	cout<<dirname<<"/"<<basename<<endl;
}
int main()
{
	scan_dir scan("/tmp", 1);
	scan.add_action(1, S_IFDIR, fun, NULL);
	scan.run();
}
