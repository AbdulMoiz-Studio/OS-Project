// command to execute the program : g++ process_manager.cpp -o process_manager -pthread -std=c++14

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <numeric>
#include <memory>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iomanip>

// --- Process Definition ---

enum ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

std::string getStateString(ProcessState state) {
    switch (state) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case WAITING: return "WAITING";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

struct Process {
    int pid;
    ProcessState state;
    int priority;
    int burstTime;
    int arrivalTime;
    int remainingTime;
    int waitingTime;
    int turnaroundTime;
    std::vector<int> resources;
    bool isIdle; // Keep the isIdle flag

    // Constructor (isIdle will be set later)
    Process(int id, int arrival, int burst, int priority) :
        pid(id), state(NEW), priority(priority), burstTime(burst), arrivalTime(arrival),
        remainingTime(burst), waitingTime(0), turnaroundTime(0), isIdle(false) {} // Initialize isIdle to false

    void printProcessInfo() const {
        std::cout << std::left << std::setw(5) << pid
                  << std::setw(12) << getStateString(state)
                  << std::setw(9) << arrivalTime
                  << std::setw(7) << burstTime
                  << std::setw(10) << priority
                  << std::setw(11) << remainingTime
                  << std::setw(9) << waitingTime
                  << std::setw(12) << turnaroundTime
                  << std::setw(6) << (isIdle ? "Yes" : "No") << std::endl;
    }

    bool operator<(const Process& other) const {
        return remainingTime > other.remainingTime;
    }
};

// --- Scheduler Definition ---

class Scheduler {
public:
    virtual void addProcess(Process process) = 0;
    virtual Process getNextProcess() = 0;
    virtual bool hasProcesses() const = 0;
    virtual ~Scheduler() {}
    virtual std::string getName() const = 0;
    virtual void updateWaitingTimes(int currentTime) = 0;
    virtual bool isPreemptive() const = 0;
    virtual bool hasNonIdleProcesses() const = 0;
};

// --- FCFS Scheduler (Non-Preemptive) ---

class FCFSScheduler : public Scheduler {
private:
    std::queue<Process> readyQueue;

public:
    void addProcess(Process process) override {
        readyQueue.push(process);
    }

    Process getNextProcess() override {
        if (!readyQueue.empty()) {
            Process next = readyQueue.front();
            readyQueue.pop();
            return next;
        }
        return Process(-1, -1, -1, -1);
    }

    bool hasProcesses() const override {
        return !readyQueue.empty();
    }

    std::string getName() const override {
        return "FCFS (Non-Preemptive)";
    }

    void updateWaitingTimes(int currentTime) override {
        std::queue<Process> tempQueue;
        while (!readyQueue.empty()) {
            Process p = readyQueue.front();
            readyQueue.pop();
            if (p.state == READY) {
                p.waitingTime++;
            }
            tempQueue.push(p);
        }
        readyQueue = tempQueue;
    }

    bool isPreemptive() const override {
        return false;
    }

    bool hasNonIdleProcesses() const override {
        std::queue<Process> tempQueue = readyQueue;
        while (!tempQueue.empty()) {
            if (!tempQueue.front().isIdle) {
                return true;
            }
            tempQueue.pop();
        }
        return false;
    }
};

// --- Preemptive FCFS ---
class PreemptiveFCFSScheduler : public Scheduler {
private:
    std::queue<Process> readyQueue;

public:
    void addProcess(Process process) override {
        readyQueue.push(process);
    }

    Process getNextProcess() override {
        if (!readyQueue.empty()) {
            Process next = readyQueue.front();
            readyQueue.pop();
            return next;
        }
        return Process(-1, -1, -1, -1);
    }

    bool hasProcesses() const override {
        return !readyQueue.empty();
    }
    std::string getName() const override{
        return "FCFS (Preemptive)";
    }
    void updateWaitingTimes(int currentTime) override {
        std::queue<Process> tempQueue;
        while (!readyQueue.empty()) {
            Process p = readyQueue.front();
            readyQueue.pop();
            if (p.state == READY) {
                p.waitingTime++;
            }
            tempQueue.push(p);
        }
        readyQueue = tempQueue;
    }
    bool isPreemptive() const override {
        return true;
    }
    bool hasNonIdleProcesses() const override {
        std::queue<Process> tempQueue = readyQueue;
        while (!tempQueue.empty()) {
            if (!tempQueue.front().isIdle) {
                return true;
            }
            tempQueue.pop();
        }
        return false;
    }

};

// --- SJF Scheduler (Non-Preemptive) ---
class SJFScheduler : public Scheduler {
private:
    std::priority_queue<Process> readyQueue;

public:
    void addProcess(Process process) override {
        readyQueue.push(process);
    }

    Process getNextProcess() override {
        if (!readyQueue.empty()) {
            Process next = readyQueue.top();
            readyQueue.pop();
            return next;
        }
        return Process(-1, -1, -1, -1);
    }

    bool hasProcesses() const override {
        return !readyQueue.empty();
    }

    std::string getName() const override {
        return "SJF (Non-Preemptive)";
    }

    void updateWaitingTimes(int currentTime) override {
        std::priority_queue<Process> tempQueue;
        while (!readyQueue.empty()) {
            Process p = readyQueue.top();
            readyQueue.pop();
            if (p.state == READY) {
                p.waitingTime++;
            }
            tempQueue.push(p);
        }
        readyQueue = tempQueue;
    }

    bool isPreemptive() const override {
        return false;
    }
    bool hasNonIdleProcesses() const override {
        std::priority_queue<Process> tempQueue = readyQueue;
        while (!tempQueue.empty()) {
            if (!tempQueue.top().isIdle) {
                return true;
            }
            tempQueue.pop();
        }
        return false;
    }
};
// --- Preemptive SJF Scheduler ---
class PreemptiveSJFScheduler : public Scheduler {
private:
    std::priority_queue<Process> readyQueue;

public:
    void addProcess(Process process) override {
        readyQueue.push(process);
    }

    Process getNextProcess() override {
        if (!readyQueue.empty()) {
            Process next = readyQueue.top();
            readyQueue.pop();
            return next;
        }
        return Process(-1,-1,-1, -1);
    }
    bool hasProcesses() const override {
        return !readyQueue.empty();
    }
    std::string getName() const override {
        return "SJF (Preemptive)";
    }

    void updateWaitingTimes(int currentTime) override {
        std::priority_queue<Process> tempQueue;
        while (!readyQueue.empty()) {
            Process p = readyQueue.top();
            readyQueue.pop();
            if (p.state == READY) {
                p.waitingTime++;
            }
            tempQueue.push(p);
        }
        readyQueue = tempQueue;
    }

    bool isPreemptive() const override {
        return true;
    }
    bool hasNonIdleProcesses() const override {
        std::priority_queue<Process> tempQueue = readyQueue;
        while (!tempQueue.empty()) {
            if (!tempQueue.top().isIdle) {
                return true;
            }
            tempQueue.pop();
        }
        return false;
    }

};

// --- Round Robin Scheduler ---

class RRScheduler : public Scheduler {
private:
    std::queue<Process> readyQueue;
    int timeQuantum;

public:
    RRScheduler(int quantum) : timeQuantum(quantum) {}

    void addProcess(Process process) override {
        readyQueue.push(process);
    }

    Process getNextProcess() override {
        if (!readyQueue.empty()) {
            Process next = readyQueue.front();
            readyQueue.pop();

            if (next.remainingTime > timeQuantum) {
                next.remainingTime -= timeQuantum;
                readyQueue.push(next);
            }
            return next;
        }
        return Process(-1, -1, -1, -1);
    }

    bool hasProcesses() const override {
        return !readyQueue.empty();
    }

    std::string getName() const override {
        return "RR";
    }

    void updateWaitingTimes(int currentTime) override {
        std::queue<Process> tempQueue;
        while (!readyQueue.empty()) {
            Process p = readyQueue.front();
            readyQueue.pop();
            if (p.state == READY) {
                p.waitingTime++;
            }
            tempQueue.push(p);
        }
        readyQueue = tempQueue;
    }
    bool isPreemptive() const override {
        return true;
    }
    bool hasNonIdleProcesses() const override {
        std::queue<Process> tempQueue = readyQueue;
        while (!tempQueue.empty()) {
            if (!tempQueue.front().isIdle) {
                return true;
            }
            tempQueue.pop();
        }
        return false;
    }
};

// --- Simplified Shared Memory (for IPC) ---
class SharedMemory {
private:
    std::vector<int> data;
    int size;
    static constexpr int DEFAULT_SIZE = 10;
public:
    SharedMemory(int size_ = DEFAULT_SIZE) : size(size_) {
        data.resize(size);
        for(int i = 0; i < size; i++)
            data[i] = 0;
    }

    int read(int index) {
        if(index < 0 || index >= size)
            throw std::out_of_range("SharedMemory::read: Index out of range");
        return data[index];
    }

    void write(int index, int value) {
        if(index < 0 || index >= size)
            throw std::out_of_range("SharedMemory::write: Index out of range");
        data[index] = value;
    }
};

// --- Semaphore ---
class Semaphore {
    private:
        int count;
        std::mutex mtx;
        std::condition_variable cv;

    public:
        Semaphore(int initialCount) : count(initialCount) {}

        void wait() {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return count > 0; });
            count--;
        }

        void signal() {
            std::unique_lock<std::mutex> lock(mtx);
            count++;
            cv.notify_one();
        }
};
// --- Simple Deadlock Detection (Circular Wait) ---

bool detectDeadlock(const std::vector<Process>& processes) {
    std::vector<std::vector<int>> allocationGraph(processes.size());
    for (size_t i = 0; i < processes.size(); ++i) {
        for (int resource : processes[i].resources) {
            for (size_t j = 0; j < processes.size(); ++j) {
                if (i != j) {
                    if (std::find(processes[j].resources.begin(), processes[j].resources.end(), resource) != processes[j].resources.end())
                        allocationGraph[i].push_back(j);
                }
            }
        }
    }
    std::vector<bool> visited(processes.size(), false);
    std::vector<bool> recursionStack(processes.size(), false);

    std::function<bool(int)> isCyclicUtil = [&](int v) {
        if (!visited[v]) {
            visited[v] = true;
            recursionStack[v] = true;

            for (int neighbor : allocationGraph[v]) {
                if (!visited[neighbor] && isCyclicUtil(neighbor))
                    return true;
                else if (recursionStack[neighbor])
                    return true;
            }
        }
        recursionStack[v] = false;
        return false;
    };

    for (size_t i = 0; i < processes.size(); ++i) {
        if (isCyclicUtil(i))
            return true;
    }

    return false;
}
// --- Main Simulation Function ---

void simulate(std::unique_ptr<Scheduler>& scheduler, const std::vector<Process>& processes) {
    std::vector<Process> readyProcesses = processes;
    std::vector<Process> completedProcesses;
    SharedMemory sharedMem(20);
    Semaphore semaphore(1);

    int currentTime = 0;
    while (scheduler->hasProcesses() || !readyProcesses.empty()) {
        for (auto it = readyProcesses.begin(); it != readyProcesses.end(); ) {
            if (it->arrivalTime <= currentTime) {
                it->state = READY;
                scheduler->addProcess(*it);
                it = readyProcesses.erase(it);
            } else {
                ++it;
            }
        }
        if (scheduler->hasProcesses()) {
            scheduler->updateWaitingTimes(currentTime);
            if(scheduler->hasNonIdleProcesses()){
                Process currentProcess = scheduler->getNextProcess();
                while(scheduler->hasProcesses() && currentProcess.isIdle){
                    currentProcess = scheduler->getNextProcess();
                }

                if(!currentProcess.isIdle){
                    currentProcess.state = RUNNING;
                    semaphore.wait();
                    sharedMem.write(currentProcess.pid % 20, currentProcess.pid * 10);
                    int readValue = sharedMem.read(currentProcess.pid % 20);
                    semaphore.signal();

                    currentProcess.remainingTime--;

                    if(currentTime % 10 == 0){
                        if(detectDeadlock(processes))
                            std::cerr << "Deadlock detected at time: " << currentTime << "!\n";
                    }

                    if (currentProcess.remainingTime <= 0) {
                        currentProcess.state = TERMINATED;
                        currentProcess.turnaroundTime = currentTime + 1 - currentProcess.arrivalTime;
                        currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
                        completedProcesses.push_back(currentProcess);
                    } else if (scheduler->isPreemptive()) {
                        currentProcess.state = READY;
                        scheduler->addProcess(currentProcess);
                    }
                   else{
                        currentProcess.state = READY;
                        scheduler->addProcess(currentProcess);
                    }
                }

            }
        }
        currentTime++;
    }

    std::cout << "\n--- Simulation Results (" << scheduler->getName() << ") ---\n";

    std::cout << std::left << std::setw(5) << "PID"
              << std::setw(12) << "State"
              << std::setw(9) << "Arrival"
              << std::setw(7) << "Burst"
              << std::setw(10) << "Priority"
              << std::setw(11) << "Remaining"
              << std::setw(9) << "Waiting"
              << std::setw(12) << "Turnaround"
              << std::setw(6) << "Idle" << std::endl;

    for (const auto& process : completedProcesses) {
        process.printProcessInfo();
    }
    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    for (const auto& process : completedProcesses) {
        totalWaitingTime += process.waitingTime;
        totalTurnaroundTime += process.turnaroundTime;
    }

    double avgWaitingTime = totalWaitingTime / completedProcesses.size();
    double avgTurnaroundTime = totalTurnaroundTime / completedProcesses.size();

    std::cout << "Average Waiting Time: " << avgWaitingTime << std::endl;
    std::cout << "Average Turnaround Time: " << avgTurnaroundTime << std::endl;
}
int main() {
    srand(time(0));

    int numProcesses, schedulerChoice, timeQuantum, preemptiveChoice;

    std::cout << "Enter the number of processes: ";
    std::cin >> numProcesses;

    std::vector<Process> processes;
    for (int i = 0; i < numProcesses; ++i) {
        int arrivalTime, burstTime, priority;

        std::cout << "Enter arrival time for process " << i << ": ";
        std::cin >> arrivalTime;
        while (arrivalTime < 0) {
            std::cout << "Arrival time cannot be negative.  Enter again: ";
            std::cin >> arrivalTime;
        }

        std::cout << "Enter burst time for process " << i << ": ";
        std::cin >> burstTime;
        while(burstTime <= 0){
            std::cout << "Burst time must be greater than 0. Enter again: ";
            std::cin >> burstTime;
        }
        std::cout << "Enter priority for process " << i << " (lower number = higher priority): ";
        std::cin >> priority;
        while (priority < 0){
            std::cout << "Priority cannot be negative. Enter again: ";
            std::cin >> priority;
        }

        // Create the process (isIdle will be set later)
        processes.emplace_back(i, arrivalTime, burstTime, priority);
    }

    // Calculate average burst time
    double totalBurstTime = 0;
    for (const auto& p : processes) {
        totalBurstTime += p.burstTime;
    }
    double avgBurstTime = totalBurstTime / numProcesses;

    // Determine and set the isIdle flag for each process
    const double idleThreshold = 0.2; // 20% of average burst time
    for (auto& p : processes) {
        if (p.burstTime < idleThreshold * avgBurstTime) {
            p.isIdle = true; // Mark as idle
        }
    }

    std::cout << "\n--- Input Data ---\n";
    std::cout << std::left << std::setw(5) << "PID"
            << std::setw(9) << "Arrival"
            << std::setw(7) << "Burst"
            << std::setw(10) << "Priority"
            << std::setw(6) << "Idle" << std::endl;
    for (const auto& process : processes) {
        std::cout << std::left << std::setw(5) << process.pid
                << std::setw(9) << process.arrivalTime
                << std::setw(7) << process.burstTime
                << std::setw(10) << process.priority
                << std::setw(6) << (process.isIdle ? "Yes" : "No") << std::endl;
    }

    std::cout << "\nChoose a scheduling algorithm:\n";
    std::cout << "1. FCFS\n2. SJF\n3. Round Robin\n";
    std::cout << "Enter your choice (1-3): ";
    std::cin >> schedulerChoice;

    std::unique_ptr<Scheduler> scheduler;

    switch (schedulerChoice) {
        case 1:
            std::cout << "Do you want preemptive FCFS (1) or non-preemptive FCFS (0)? ";
            std::cin >> preemptiveChoice;
            while (preemptiveChoice != 0 && preemptiveChoice != 1) {
                std::cout << "Invalid choice. Enter 0 for non-preemptive or 1 for preemptive: ";
                std::cin >> preemptiveChoice;
            }
            if (preemptiveChoice == 1) {
                scheduler = std::make_unique<PreemptiveFCFSScheduler>();
            } else {
                scheduler = std::make_unique<FCFSScheduler>();
            }
            break;
        case 2:
            std::cout << "Do you want preemptive SJF (1) or non-preemptive SJF (0)? ";
            std::cin >> preemptiveChoice;
            while (preemptiveChoice != 0 && preemptiveChoice != 1) {
                std::cout << "Invalid choice.  Enter 0 for non-preemptive or 1 for preemptive: ";
                std::cin >> preemptiveChoice;
            }
            if (preemptiveChoice == 1) {
                scheduler = std::make_unique<PreemptiveSJFScheduler>();
            } else {
                scheduler = std::make_unique<SJFScheduler>();
            }
            break;
        case 3:
            std::cout << "Enter the time quantum for Round Robin: ";
            std::cin >> timeQuantum;
            scheduler = std::make_unique<RRScheduler>(timeQuantum);
            break;
        default:
            std::cerr << "Invalid scheduler choice.  Exiting.\n";
            return 1;
    }

    try {
        simulate(scheduler, processes);
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
