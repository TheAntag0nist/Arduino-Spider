#define CURL_STATICLIB
#include <iostream>
#include <conio.h>
#include <string>

#include <Windows.h>
//============================================================================
			//////////////////////////////////////////////////
			//					INFORMATION
			//////////////////////////////////////////////////
			/// 
			///		It's Robot-Spider DLL for remote control.
			/// 
			//////////////////////////////////////////////////
#include <curl.h>
//============================================================================
struct memory {
	char* response;
	size_t size;
};

char error_buffer[1024];
int currentMode;
//============================================================================
#define IDLE -1

#define BACKWARD 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3
//============================================================================
//								PROTOTYPES
//============================================================================
int connect_robot();
void send_dt(int command);
int close_connect();
size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userdata);
//============================================================================
//								MAIN_PROG
//============================================================================
int main() {
	if (!connect_robot()) {
		currentMode = IDLE;
		int comm = 0;

		while (true) {
			comm = _getch();

			if (comm == 224)
				comm = _getch();

			if (comm == 75)
				send_dt(LEFT);
			if (comm == 77)
				send_dt(RIGHT);
			if (comm == 72)
				send_dt(FORWARD);
			if (comm == 80)
				send_dt(BACKWARD);

			if (comm == 27)
				break;

			comm = NULL;
		}
	}

	std::cout << "[+]:> EXIT;" << std::endl;
	close_connect();

	system("pause");
	return 0;
}
//============================================================================
//								OTHER_PROG
//============================================================================
int connect_robot() {
	std::cout << "[+]curl_Auto:> program that works as remote control;" << std::endl;
	curl_global_init(CURL_GLOBAL_ALL);

	memory chunk_info;
 
	error_buffer[0] = '\0';
	chunk_info.response = nullptr;
	chunk_info.size = 0;

	CURL* curl_process = curl_easy_init();

	if (curl_process) {
		// 1. set URL option
		// 2. set error buffer
		curl_easy_setopt(curl_process, CURLOPT_URL, "192.168.0.5");
		curl_easy_setopt(curl_process, CURLOPT_ERRORBUFFER, error_buffer);

		curl_easy_setopt(curl_process, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl_process, CURLOPT_WRITEDATA, &chunk_info);

		// connect
		CURLcode success = curl_easy_perform(curl_process);
		if (success != CURLE_OK) {
			std::cout << "[!!]:> ERROR::CURL_PERFORM --> info\n" << error_buffer << std::endl;

			return 1;
		}
	}

	std::cout << "[+]:> SUCCESS_CONNECTION;" << std::endl;
	curl_easy_cleanup(curl_process);
	return 0;
}
//============================================================================
//============================================================================
size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
	memory* tmp_pnt = (memory*)userdata;
	size_t realsize = size * nmemb;

	if (userdata != NULL)
		delete[] tmp_pnt->response;

	tmp_pnt->response = new char[size + realsize + 1];

	for (int i = 0; i < realsize; ++i)
		tmp_pnt->response[i] = ((char*)ptr)[i];

	tmp_pnt->size = realsize;
	tmp_pnt->response[tmp_pnt->size] = '\0';

#ifdef _DEBUG
	std::cout << tmp_pnt->response << std::endl;
#endif

	return realsize;
}
//============================================================================
//============================================================================
void send_dt(int command) {
	CURL* curl_process = curl_easy_init();
	memory chunk_info;

	chunk_info.response = nullptr;
	chunk_info.size = 0;

	curl_easy_setopt(curl_process, CURLOPT_ERRORBUFFER, error_buffer);

	curl_easy_setopt(curl_process, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl_process, CURLOPT_WRITEDATA, &chunk_info);

	if (command == BACKWARD)
		curl_easy_setopt(curl_process, CURLOPT_URL, "192.168.0.5/down");
	if (command == FORWARD)
		curl_easy_setopt(curl_process, CURLOPT_URL, "192.168.0.5/up");
	if (command == LEFT)
		curl_easy_setopt(curl_process, CURLOPT_URL, "192.168.0.5/left");
	if (command == RIGHT) 
		curl_easy_setopt(curl_process, CURLOPT_URL, "192.168.0.5/right");

	if (currentMode != command) {
		std::cout << "[+]:> Command: " << command << std::endl;
		currentMode = command;
	}
	// connect
	CURLcode success = curl_easy_perform(curl_process);
	if (success != CURLE_OK) {
		std::cout << "[!!]:> ERROR::CURL_PERFORM --> info\n" << error_buffer << std::endl;

		return;
	}

	curl_easy_cleanup(curl_process);
}
//============================================================================
//============================================================================
int close_connect() {
	curl_global_cleanup();
	return 0;
}