// For uWebSockets
#define _CRT_SECURE_NO_WARNINGS

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>

#include "gpu.h"

using json = nlohmann::json;

std::atomic<bool> keep_running(true);
GPUInfo latest_info;

static void signal_handler(int signal) {
	if (signal == SIGINT) {
		std::cout << "Exiting..." << std::endl;
		keep_running = false;
	}
}

void runMonitoring() {
	GPUWatcher watcher;
	std::cout << "Monitoring: " << watcher.getDeviceName() << std::endl;

	while (keep_running) {
		GPUInfo info = watcher.snapshot();
		/*std::cout << "Mem: " << info.bytesFree << "B of " << info.bytesTotal << "B free" << std::endl;
		std::cout << "GPU Utilization: " << info.utilization << "%" << std::endl;
		std::cout << "Temp: " << info.temperature << "C" << std::endl;*/
		latest_info = info;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void runServer() {
	uWS::App().get("/stats", [](auto* res, auto* req) {
		const json output = {
			{"mem_free", latest_info.bytesFree},
			{"mem_total", latest_info.bytesTotal},
			{"util", latest_info.utilization},
			{"temp", latest_info.temperature},
		};
		res->writeHeader("Content-Type", "application/json");
		res->end(output.dump());
	}).listen(61208, [](auto* listenSocket) {
		if (listenSocket) {
			std::cout << "Listening for connections..." << std::endl;
		}
	}).run();
}

int main() {
	std::cout << "WinEye - GPU Monitor\n";
	std::signal(SIGINT, signal_handler);
	std::thread t(runMonitoring);
	runServer();
	t.join();
	return 0;
}
