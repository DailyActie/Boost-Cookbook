// Amost all the code for this example is in this header
#include "tasks_processor_base.hpp"
using namespace tp_base;

int func_test() {
    static int counter = 0;
    ++ counter;
    boost::this_thread::interruption_point();

    switch (counter) {
    case 3:
        throw std::logic_error("Just checking");

    case 10:
        // Emulation of thread interruption.
        // Caught inside task_wrapped and does not stop execution.
        throw boost::thread_interrupted();

    case 90:
        // Stopping the tasks_processor.
        tasks_processor::stop();
    }

    return counter;
}

int main () {
    static const std::size_t tasks_count = 100;
    // stop() is called at 90
    BOOST_STATIC_ASSERT(tasks_count > 90);

    for (std::size_t i =0; i < tasks_count; ++i) {
        tasks_processor::push_task(&func_test);
    }
    // Processing was not started.
    assert(func_test() == 1);

    // We can also use lambda as a task.
#ifndef BOOST_NO_CXX11_LAMBDAS
    // Counting 2 + 2 asynchronously.
    int sum = 0;
    tasks_processor::push_task(
        [&sum]() { sum = 2 + 2; }
    );
    // Processing was not started.
    assert(sum == 0);
#endif

    // Does not throw, but blocks till
    // one of the tasks it is owning
    // calls tasks_processor::stop().
    tasks_processor::start();
    assert(func_test() == 91);
}
