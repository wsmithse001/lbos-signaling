#include <cstdlib>
#include <iostream>
#include <thread>
#include <semaphore>

namespace app {
typedef struct Context {
    std::counting_semaphore<> countingSemaphore;
    std::counting_semaphore<> coutMutexSemaphore;

    explicit Context() :
    countingSemaphore(std::counting_semaphore<>(0)),
    coutMutexSemaphore(std::counting_semaphore<>(1)) {
    }

} Context;

static void a1(std::string taskId, app::Context* pContext);
static void b1(std::string taskId, app::Context* pContext);
static void enterTask(std::string taskId, app::Context* pContext);
static void exitTask(std::string taskId, app::Context* pContext);
}

int main() {
    app::Context ctx;

    std::thread threads[] = {
        std::thread(app::b1, "1", &ctx),
        std::thread(app::a1, "2", &ctx)
    };

    for (std::thread& t : threads) {
        t.join();
    }

    return (EXIT_SUCCESS);
}

static void app::a1(std::string taskId, app::Context* pContext) {
    app::enterTask(taskId, pContext);
    pContext->countingSemaphore.release();
    app::exitTask(taskId, pContext);
}

static void app::b1(std::string taskId, app::Context* pContext) {
    app::enterTask(taskId, pContext);
    pContext->countingSemaphore.acquire();
    app::exitTask(taskId, pContext);
}

static void app::enterTask(std::string taskId, app::Context* pContext) {
    std::thread::id threadId = std::this_thread::get_id();
    pContext->coutMutexSemaphore.acquire();
    std::cout << "Task(" << taskId << ") Thread(" << threadId << ") entering the task." << std::endl;
    pContext->coutMutexSemaphore.release();
}

static void app::exitTask(std::string taskId, app::Context* pContext) {
    std::thread::id threadId = std::this_thread::get_id();
    pContext->coutMutexSemaphore.acquire();
    std::cout << "Task(" << taskId << ") Thread(" << threadId << ") exiting the task." << std::endl;
    pContext->coutMutexSemaphore.release();
}
