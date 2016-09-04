#include "scan_dir"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <sstream>

scan_dir::scan_dir(const string &pathname, int max_depth):
	d_max_depth(max_depth), d_pathname(pathname)
{
}

int scan_dir::add_action(int depth, mode_t mode, dentry_action_fun_t *fun, void *data)
{	
	assert(depth <= d_max_depth);
	
	list<list<dirent_action_t>>::iterator iter = get_depth_list(depth, 1);
	assert(iter != d_actions.end());
	
	list<dirent_action_t> &l_actions = *iter;
	for(list<dirent_action_t>::iterator iter1 = l_actions.begin(); iter1 != l_actions.end(); ++iter1)
	{
		if(iter1->d_mode & mode)
		{
			iter1->d_fun = fun;
			iter1->data = data;			
			return 0;
		}
	}

	dirent_action_t action;
	action.d_depth = depth;
	action.d_mode = mode;
	action.d_fun = fun;
	action.d_data = data;
	l_actions.push_front(action);
	return 0;
}

int scan_dir::scan()
{
	dirent_stack ds;
	if((ds.d_dir = opendir(d_pathname.c_str())) == NULL)
	{
		perror("scan_dir::scan, opendir");
		return -1;
	}
	ds.d_name = d_pathname;

	d_stack.push_back(ds);

	while(d_stack.size() > 0)
	{
		dirent_stack &ds = *(ds.back());
		Dir *dir = ds.d_dir;
		const string &cur_pathname = get_cur_pathname(); 
	
		struct dirent *entry;
		while((entry = readdir(dir)) != NULL)
		{
			list<list<dirent_action_t>>::iterator iter;
			if((iter = get_depth_list(get_cur_depth(), 0)) != d_actions.end())
			{
				dirent_action_t &l_actions = *iter;
				for(list<dirent_action_t>::iterator iter1 = l_actions.begin(); iter1 != l_actions.end(); ++iter1)
				{
					if(iter1->d_mode & entry->d_mode)
					{
						iter1->fun(cur_pathname, entry->d_name, entry->d_mode, iter1->data);
						break;
					}
				}
			}
			if(get_cur_depth() < d_max_depth && entry->mode & S_IFDIR)
			{
				if((dir = opendir(pathname.c_str())) == NULL)
				{
					perror("scan_dir::scan, operdir");
					return -1;
				}
				dirent_stack ds;
				ds.d_dir = dir;
				ds.d_name = entry->d_name;
				d_stack.push_back(ds);
				break;
			}
		}
		if(entry == NULL)
		{
			dirent_stack &ds = *(d_stack.back());
			closedir(ds.d_dir);
			d_stack.pop_back();
		}
	}
}
list<list<dirent_action_t>>::iterator scan_dir::get_depth_list(int depth, bool create)
{
	for(std::list<std::list<dirent_action_t>>::itrerator iter = d_actions.begin(); iter != d_actions.end(); ++iter)
	{
		if(iter->begin()->d_depth == depth)
		{
			return iter;
		}
		else if(iter->begin()->depth > depth) 
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
		return d_actions.back();
	}
	return d_actions.end();
}

string scan_dir::get_cur_pathname()
{
	osringstream oss;
	for(vector<dirent_stack>::iterator iter = d_stack.begin(); iter != d_stack.end(); ++iter)
	{
		oss<<iter->d_name<<"/";	
	}
	return oss.str();
}
