#include <sys/wait.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>

#include <cstring>
#include <string>

#define STACK_SIZE (512*512)

namespace docker{
	typedef int proc_statu;
	proc_statu proc_err = -1;
	proc_statu proc_exit = 0;
	proc_statu proc_wait = 1;

	typedef struct container_config{
		std::string host_name;
		std::string root_dir;
	}container_config;

	class container{
	private:
		typedef int process_pid;
		char child_stack[STACK_SIZE];
		container_config config;
		void set_procsys(){
			mount("none", "/proc", "proc", 0, nullptr);
			mount("none", "/sys", "sysfs", 0, nullptr);
		}
		void set_rootdir(){
			chdir(this->config.root_dir.c_str());
			chroot(".");
		}
		void set_hostname(){
			sethostname(this->config.host_name.c_str(),this->config.host_name.length());
		}
		void start_bash(){
			std::string bash = "/bin/bash";
			char* c_bash = new char[bash.length()+1];
			strcpy(c_bash,bash.c_str());
			char* const child_args[] = {c_bash,NULL};
			execv(child_args[0],child_args);
			delete[] c_bash;
		}
	public:
		void start(){
			auto setup = [](void * args) ->int {
				auto _this = reinterpret_cast<container*>(args);
				//config container
				_this->set_hostname();
				_this->set_rootdir();
				_this->set_procsys();
				_this->start_bash();
				
				return proc_wait;
			};
			process_pid child_pid = clone(setup,child_stack+STACK_SIZE,CLONE_NEWPID|CLONE_NEWNS|CLONE_NEWUTS|SIGCHLD,this);
			waitpid(child_pid,nullptr,0);
		}
		container(container_config& config){
			this->config = config;
		}
	};
}