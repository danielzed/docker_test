#include "docker.hpp"
#include <iostream>

int main(int argc,char** argv){
	std::cout << "...start container" << std::endl;
	docker::container_config config;

	//config container
	config.host_name = "shiyanlou";
	config.root_dir = "./shiyanlou";

	docker::container container(config);
	container.start();
	std::cout << "stop container..." <<std::endl;
	return 0;
}