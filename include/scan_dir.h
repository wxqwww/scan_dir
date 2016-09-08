#ifndef _SCAN_DIR_H
#define _SCAN_DIR_H

#include <sys/types.h>

#include <list>
#include <string>
#include <vector>
#include <dirent.h>

class scan_dir
{
public:
	typedef int (*dirent_action_fun_t)(const std::string &dirname, const std::string &basename, unsigned char mode, void *data);

	scan_dir(const std::string &pathname, int max_depth);
	int add_action(int depth, unsigned char type, dirent_action_fun_t fun, void *data);
	//int del_action(int depth, unsigned char type);
	int scan();
	
private:
	struct dirent_action_t
	{
			int d_depth;
			unsigned char d_type;
			dirent_action_fun_t d_fun;
			void *d_data;	
	};

	struct dirent_stack
	{
			DIR *d_dir;
			std::string d_name;
	};
	
	enum 
	{
		SUCCESS = 0,

		ERR_SYS_START,
		ERR_SYS_OPENDIR,
		ERR_SYS_READDIR,
		ERR_SYS_END,

		ERR_END
	};
	static const char *hints[ERR_END - SUCCESS + 1];

	scan_dir(const scan_dir &);
	scan_dir &operator=(const scan_dir &);
	std::list<std::list<dirent_action_t>>::iterator get_depth_list(int depth, bool create);
	std::string get_cur_pathname();
	int get_cur_depth();
	const char *return_hints(int ret);

	int d_max_depth;
	std::string d_pathname;
	std::vector<dirent_stack> d_stack;	
	std::list<std::list<dirent_action_t>> d_actions;
};

inline
int scan_dir::get_cur_depth()
{
	return d_stack.size();
}
#endif //_SCAN_DIR_H
