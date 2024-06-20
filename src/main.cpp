#include <filesystem>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <regex>
#include <fstream>
#include <thread>
#include <vector>
#include <syncstream>
#include <semaphore>


class job_manager final {
public:
    explicit job_manager(size_t num_threads);
    job_manager(const job_manager& other) = delete;
    job_manager& operator=(const job_manager& other) = delete;
    job_manager(job_manager&& other) = default;
    job_manager& operator=(job_manager&& other) = default;

    ~job_manager();

    void push_job(std::function<void()> f);
    void wait_workers();

private:

    std::thread create_worker_thread();
    void on_task_completed();

    std::vector<std::thread> thread_pool;

    std::queue<std::function<void()>> task_queue;
    std::condition_variable cv;
    std::mutex mtx;

    uint64_t pending_tasks = 0;
    std::condition_variable cv_wait;
    std::mutex mtx_wait;

    std::atomic<bool> cancelling = false;
};

job_manager::job_manager(size_t num_threads) {
    std::generate_n(std::back_inserter(thread_pool), num_threads, [this]{ return create_worker_thread(); });
}

job_manager::~job_manager() {
    cancelling = true;
    {
        std::queue<std::function<void()>> empty_queue;
        std::lock_guard lock(mtx);
        task_queue.swap(empty_queue);
    }
    cv.notify_all();
    std::for_each(thread_pool.begin(), thread_pool.end(), [](std::thread& worker) {
        if(worker.joinable()) {
            worker.join();
        }
    });
}

void job_manager::push_job(std::function<void()> f) {
    {
        std::lock_guard lock(mtx);
        task_queue.push(std::move(f));
    }
    {
        std::lock_guard lock(mtx_wait);
        ++pending_tasks;
    }
    cv.notify_one();
}

void job_manager::wait_workers() {
    std::unique_lock lock(mtx_wait);
    cv_wait.wait(lock, [this]{ return pending_tasks == 0; });
}

std::thread job_manager::create_worker_thread() {
    auto body = [this]() {
        while(!cancelling) {
            std::unique_lock lock(mtx);
            cv.wait(lock, [this]{ return !task_queue.empty() || cancelling; });
            if(cancelling) {
                return;
            }
            std::function<void()> f = std::move(task_queue.front());
            task_queue.pop();
            lock.unlock();
            f();
            std::lock_guard lock2(mtx_wait);
            if(--pending_tasks == 0)
            {
                cv_wait.notify_all();
            }
        }
    };
    return std::thread(body);
}

static void parse_file(const std::filesystem::path& path, const std::regex& regex) {
    std::ifstream t(path, std::ios::binary);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

    auto it_begin = std::sregex_iterator(str.begin(), str.end(), regex);
    auto it_end = std::sregex_iterator();

    for (std::sregex_iterator it = it_begin; it != it_end; ++it)
    {
        const std::smatch& match = *it;
        auto str_begin = str.rfind('\n', match.position());
        auto str_end = str.find('\n', match.position() + match.length());
        auto actual_str_begin = str_begin == std::string::npos ? 0 : str_begin + 1;
        std::osyncstream(std::cout) << path << ":" << str.substr(actual_str_begin, str_end - actual_str_begin) << '\n';
    }
}

int main(int argc, char **argv) {
    if(argc != 3)
    {
        std::cerr << "Invalid number of arguments\n" << "Usage: tool.exe [regex] [initial_dir]\n";
        return EXIT_FAILURE;
    }

    std::vector<std::string> args(argv, argv + argc);

    std::regex regex{args[1]};
    std::filesystem::path path(args[2]);
    if(!std::filesystem::is_directory(path)) {
        std::cerr << path << " is not a directory or doesn't exist";
        return EXIT_FAILURE;
    }

    job_manager manager(std::thread::hardware_concurrency());

    auto push_job = [&manager, &regex](const std::filesystem::directory_entry& dir_entry){
        if(dir_entry.is_regular_file())
        {
            manager.push_job([path = dir_entry.path(), regex] { parse_file(path, regex); });
        }
    };
    try {
        std::filesystem::recursive_directory_iterator{args[2]};
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }
    std::ranges::for_each(std::filesystem::recursive_directory_iterator{args[2]}, push_job);
    manager.wait_workers();
}
