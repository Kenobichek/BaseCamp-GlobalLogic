#pragma once
#include <iostream>
#include <filesystem>
#include <thread>
#include <semaphore>
#include <condition_variable>
#include <chrono>
#include <queue>

std::queue<std::filesystem::path> freeFolders;

std::counting_semaphore ready{ 0 };
std::mutex m;
bool success = false;

void searchingThread(std::string name, std::filesystem::path& path) {
    while (success == false || !freeFolders.empty()) {
        std::filesystem::path tmpPath;
        ready.acquire();
        {
            std::lock_guard<std::mutex> lk(m);
            tmpPath = freeFolders.front();
            freeFolders.pop();
        }
        try {
            for (const std::filesystem::path& path_tmp : std::filesystem::directory_iterator(tmpPath)) {
                if (std::filesystem::is_directory(path_tmp)) {
                    {
                        std::lock_guard<std::mutex> lk(m);
                        freeFolders.push(path_tmp);
                        ready.release();
                    }
                }
                if (path_tmp.filename() == name) {
                    success = true;
                    path = path_tmp;
                }
            }
        }
        catch (std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

std::filesystem::path searchPath(std::filesystem::path path, std::string name) {

    std::thread t1(searchingThread, name, std::ref(path));
    std::thread t2(searchingThread, name, std::ref(path));
    std::thread t3(searchingThread, name, std::ref(path));
    std::thread t4(searchingThread, name, std::ref(path));
    std::thread t5(searchingThread, name, std::ref(path));
    std::thread t6(searchingThread, name, std::ref(path));
    std::thread t7(searchingThread, name, std::ref(path));
    std::thread t8(searchingThread, name, std::ref(path));

    try {
        for (const std::filesystem::path& path_tmp : std::filesystem::directory_iterator(path)) {

            if (std::filesystem::is_directory(path_tmp)) {
                {
                    std::lock_guard<std::mutex> lk(m);
                    freeFolders.push(path_tmp);
                    ready.release();
                }
            }

            if (path_tmp.filename() == name) {
                path = path_tmp;
                return path;
            }
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
    }

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    return path;
}