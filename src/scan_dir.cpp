#include "scan_dir.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <sstream>

using namespace std;

const char *scan_dir::hints[scan_dir::ERR_END - scan_dir::SUCCESS + 1] = 
{
	"success.",

	"err_sys_start.(If you see it, you may use ERR_SYS_START badly)",
	"opendir",
	"readdir()",
	"err_sys_end.(If you see it, you may use ERR_SYS_END badly)",

	"end.(If you see it, you may use ERR_END badly)"
};

scan_dir::scan_dir(const string &pathname, int max_depth):
	d_max_depth(max_depth), d_pathname(pathname)
{
}

int scan_dir::add_action(int depth, unsigned char type, scan_dir::dirent_action_fun_t fun, void *data)
{	
	assert(depth <= d_max_depth);
	
	list<list<dirent_action_t>>::iterator iter = get_depth_list(depth, 1);
	assert(iter != d_actions.end());
	
	list<dirent_action_t> &l_actions = *iter;
	for(list<dirent_action_t>::iterator iter1 = l_actions.begin(); iter1 != l_actions.end(); ++iter1)
	{
		if(iter1->d_type & type)
		{
			iter1->d_fun = fun;
			iter1->d_data = data;			
			return scan_dir::SUCCESS;
		}
	}

	dirent_action_t action;
	action.d_depth = depth;
	action.d_type = type;
	action.d_fun = fun;
	action.d_data = data;
	l_actions.push_front(action);
	return scan_dir::SUCCESS;
}

int scan_dir::scan()
{
	dirent_stack ds;
	if((ds.d_dir = opendir(d_pathname.c_str())) == NULL)
	{
		return scan_dir::ERR_SYS_OPENDIR;
	}
	ds.d_name = d_pathname;
	
	d_stack.clear();
	d_stack.push_back(ds);

	while(d_stack.size() > 0)
	{
		dirent_stack &ds = d_stack.back();
		DIR *dir = ds.d_dir;
		const string &cur_pathname = get_cur_pathname(); 
	
		struct dirent *entry;
		while((entry = readdir(dir)) != NULL)
		{
			if((strncmp(entry->d_name, ".", 1) == 0 && entry->d_name[1] == '\0') || strncmp(entry->d_name, "..", 2) == 0)
			{
				continue;
			}

			list<list<dirent_action_t>>::iterator iter;
			if((iter = get_depth_list(get_cur_depth(), 0)) != d_actions.end())
			{
				list<dirent_action_t> &l_actions = *iter;
				for(list<dirent_action_t>::iterator iter1 = l_actions.begin(); iter1 != l_actions.end(); ++iter1)
				{
					if(iter1->d_type & entry->d_type)
					{
						iter1->d_fun(cur_pathname, entry->d_name, entry->d_type, iter1->d_data);
						break;
					}
				}
			}
			if(get_cur_depth() < d_max_depth && entry->d_type & DT_DIR)
			{
				DIR *tmp_dir;
				if((tmp_dir = opendir((cur_pathname + entry->d_name).c_str())) == NULL)
				{
					if(errno == ENOENT)
					{
						continue;
					}
					return scan_dir::ERR_SYS_OPENDIR;
				}
				dir = tmp_dir;
				dirent_stack ds;
				ds.d_dir = dir;
				ds.d_name = entry->d_name;
				d_stack.push_back(ds);
				break;
			}
		}
		if(entry == NULL)
		{
			dirent_stack &ds = d_stack.back();
			closedir(ds.d_dir);
			d_stack.pop_back();
		}
	}
	return scan_dir::SUCCESS;
}

list<list<scan_dir::dirent_action_t>>::iterator scan_dir::get_depth_list(int depth, bool create)
{
	for(std::list<std::list<dirent_action_t>>::iterator iter = d_actions.begin(); iter != d_actions.end(); ++iter)
	{
		if(iter->begin()->d_depth == depth)
		{
			return iter;
		}
		else if(iter->begin()->d_depth > depth) 
		{
			if(create)
			{
				list<dirent_action_t> actions;
				return d_actions.insert(iter, actions);
			}
			else
			{
				return d_actions.end();
			}
		}
	}
	if(create)
	{
		list<dirent_action_t> actions;
		d_actions.push_back(actions);
		return --d_actions.end();
	}
	return d_actions.end();
}

inline
string scan_dir::get_cur_pathname()
{
	ostringstream oss;
	for(vector<dirent_stack>::iterator iter = d_stack.begin(); iter != d_stack.end(); ++iter)
	{
		oss<<iter->d_name<<"/";	
	}
	return oss.str();
}

const char *scan_dir::return_hints(int ret)
{
	if(ret < scan_dir::SUCCESS || ret > scan_dir::ERR_END)
	{   
		return NULL;
	}   

	if(ret > scan_dir::ERR_SYS_START && ret < scan_dir::ERR_SYS_END)
	{   
		static char buf[128];
		snprintf(buf, sizeof(buf),  "%s: %s\n", hints[ret], strerror(errno));
		return buf;
	}   
	return hints[ret];
}

