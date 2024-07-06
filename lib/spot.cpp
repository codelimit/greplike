#include "spot.h"

#include <algorithm>
#include <fstream>
#include <future>
#include <queue>
#include <thread>

#include <search_engine.h>

namespace spot
{
    class task_wrapper
    {
    public:
        template<typename Fn>
        task_wrapper(Fn&& f) : ptr(std::make_unique<task<Fn>>(std::forward<Fn>(f)))
        {}

        task_wrapper() = default;
        task_wrapper(const task_wrapper& other) = delete;
        task_wrapper& operator=(const task_wrapper& other) = delete;
        task_wrapper(task_wrapper&& other) noexcept : ptr(std::move(other.ptr)) {}
        task_wrapper& operator=(task_wrapper&& other) noexcept
        {
            if (this != &other)
            {
                ptr = std::move(other.ptr);
            }
            return *this;
        }

        void operator()() const { ptr->execute(); }

    private:
        struct task_base
        {
            virtual void execute() = 0;
            virtual ~task_base() = default;
        };

        template<typename Fn>
        struct task final : task_base
        {
            Fn f;
            task(Fn&& f) : f(std::move(f)) {}
            void execute() override { f(); }
        };

        std::unique_ptr<task_base> ptr;
    };

    class job_manager final
    {
    public:
        explicit job_manager(size_t num_threads);
        job_manager(const job_manager& other) = delete;
        job_manager& operator=(const job_manager& other) = delete;
        job_manager(job_manager&& other) = delete;
        job_manager& operator=(job_manager&& other) = delete;

        ~job_manager();

        template<typename Fn>
        std::future<std::invoke_result_t<Fn>> push_job(Fn f)
        {
            using result_type = std::invoke_result_t<Fn>;
            std::packaged_task<result_type()> task(std::move(f));
            std::future<result_type> future(task.get_future());
            {
                std::lock_guard lk(mtx);
                tasks.push(std::move(task));
            }
            cv.notify_one();
            return future;
        }

    private:
        std::queue<task_wrapper> tasks;
        std::condition_variable_any cv;
        std::mutex mtx;

        std::vector<std::jthread> thread_pool;
    };

    job_manager::job_manager(size_t num_threads)
    {
        auto job_thread = [this](std::stop_token st)
        {
            while (!st.stop_requested())
            {
                std::unique_lock lock(mtx);
                if (!cv.wait(lock, st, [this] { return !tasks.empty(); }))
                {
                    return;
                }
                task_wrapper task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                task();
            }
        };

        thread_pool.reserve(num_threads);
        std::ranges::generate_n(std::back_inserter(thread_pool), static_cast<int>(num_threads),
                                [&] { return std::jthread(job_thread); });
    }

    job_manager::~job_manager()
    {
        std::ranges::for_each(thread_pool, [](std::jthread& job_thread) { job_thread.request_stop(); });
    }

    void search_file(const std::filesystem::path& path, const search_engine& engine, const callback& on_match,
                     const search_options& options)
    {
        std::ifstream file(path);

        size_t lines_counter = 0;
        for (std::string line; std::getline(file, line);)
        {
            ++lines_counter;

            std::string_view sv(line);

            for (auto it = engine.search(sv); it != sv.end();
                 it = engine.search(sv.substr(std::distance(sv.begin(), it))))
            {
                match_info info;
                info.file_path = path.string();
                info.line = line;
                info.line_number = lines_counter;
                info.pos = it;
                if (on_match(info) == false)
                {
                    return;
                }
                if (!options.all_occurrances)
                {
                    break;
                }
            }
        }
    }

    void search_file(const std::filesystem::path& path, const std::string& regex, const callback& on_match,
                     const search_options& options)
    {
        const std::unique_ptr<search_engine> engine = make_search_engine(regex, options.case_sensetive);
        search_file(path, *engine, on_match, options);
    }

    void search_directory(const std::filesystem::path& directory, const std::string& regex, const callback& on_match,
                          const search_options& options)
    {
        job_manager manager(std::thread::hardware_concurrency());
        std::vector<std::future<void>> futures;

        const std::unique_ptr<search_engine> engine = make_search_engine(regex, options.case_sensetive);

        auto push_job = [&](const std::filesystem::directory_entry& dir_entry)
        {
            if (dir_entry.is_regular_file())
            {
                auto future = manager.push_job([entry = dir_entry, &engine, &on_match, &options]
                                               { search_file(entry.path(), *engine, on_match, options); });
                futures.push_back(std::move(future));
            }
        };

        std::ranges::for_each(std::filesystem::recursive_directory_iterator{directory}, push_job);
        std::ranges::for_each(futures, [](const std::future<void>& future) { future.wait(); });
    }
} // namespace spot
