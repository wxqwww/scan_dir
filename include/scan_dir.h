#ifndef _SCAN_DIR_H
#define _SCAN_DIR_H

#include <sys/types.h>

#include <list>
#include <string>
#include <vector>

class scan_dir
{
struct dirent_action_t
{
	int d_depth;
	mode_t d_mode;
	dentry_action_fun_t d_fun;
	void *d_data;	
};

struct dirent_stack
{
	DIR *d_dir;
	std::string d_name;
}

public:
	typedef int (*dirent_action_fun_t)(const string &dirname, const string &basename, mode_t mode, void *data);

	scan_dir(const string &pathname, int max_depth);
	int add_action(int depth, mode_t mode, dentry_action_fun_t *fun, void *data);
	//int del_action(int depth, mode_t mode);
	int scan();
	
private:
	scan_dir(const scan_dir &);
	scan_dir &operator=(const scan_dir &);
	std::list<std::list<dirent_action_t>>::iterator get_depth_list(int depth, bool create);
	string get_cur_pathname();
	int get_cur_depth();

	int d_max_depth;
	std::string d_pathname;
	std::vector<dirent_stack> d_stack;	
	std::list<std::list<dirent_action_t>> d_actions;
};

inlin
inlinee
int scan_dir::get_cur_depth()
{
	return d_stack.size();
}

#endif //_SCAN_DIR_H
