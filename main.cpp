#include <iostream>
#include <thread>
#include <ctime>
#include <vector>
#include <mutex>

enum ORDER
{
    PIZZA,
    SOUP,
    STEAK,
    SALAD,
    SUSHI
};

std::string orderNames[] = {"pizza", "soup", "steak", "salad", "sushi"};

std::mutex orderArrayLock;
std::mutex deliveryLock;
std::mutex coutLock;
std::mutex succes_deliveries_lock;

int succes_deliveries=0;

std::vector<ORDER> ordersToCook;
std::vector<ORDER> ordersToDeliver;

void orderGeneration()
{
    std::srand(std::time(nullptr));
    int temp;

    while(true)
    {
        succes_deliveries_lock.lock();
        if(succes_deliveries>=10)
        {
            succes_deliveries_lock.unlock();
            break;
        }
        succes_deliveries_lock.unlock();
        temp=std::rand()%6+5;
        std::this_thread::sleep_for(std::chrono::seconds(temp));
        temp=std::rand()%5;
        coutLock.lock();
        std::cout << orderNames[temp] << " was ordered\n";
        coutLock.unlock();
        orderArrayLock.lock();
        ordersToCook.push_back((ORDER)(temp));
        orderArrayLock.unlock();
    }
}

void kitchen()
{
    std::srand(std::time(nullptr));
    int order, timeout;

    while(true)
    {
        succes_deliveries_lock.lock();
        if(succes_deliveries>=10)
        {
            succes_deliveries_lock.unlock();
            break;
        }
        succes_deliveries_lock.unlock();
        if(ordersToCook.size()!=0)
        {
            orderArrayLock.lock();
            order=ordersToCook.back();
            ordersToCook.pop_back();
            orderArrayLock.unlock();
            coutLock.lock();
            std::cout << orderNames[order] << " was started to cook\n";
            coutLock.unlock();
            timeout=std::rand()%11+5;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            coutLock.lock();
            std::cout << orderNames[order] << " was cooked\n";
            coutLock.unlock();
            deliveryLock.lock();
            ordersToDeliver.push_back((ORDER)order);
            deliveryLock.unlock();
        }
    }
}

void delivery()
{
    while(true)
    {
        deliveryLock.lock();
        if(ordersToDeliver.size()!=0)
        {
            deliveryLock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(30));
            deliveryLock.lock();
            for (auto i = ordersToDeliver.begin(); i != ordersToDeliver.end(); i++) {
                coutLock.lock();
                std::cout << orderNames[*i] << " was delivered\n";
                coutLock.unlock();
                succes_deliveries_lock.lock();
                succes_deliveries++;
                succes_deliveries_lock.unlock();
            }
            succes_deliveries_lock.lock();
            coutLock.lock();
            std::cout << "Total deliveries count: " << succes_deliveries << std::endl;
            coutLock.unlock();
            succes_deliveries_lock.unlock();
            ordersToDeliver.erase(ordersToDeliver.begin(), ordersToDeliver.end());
        }
        deliveryLock.unlock();
    }
}

int main()
{
    std::thread orderIncome(orderGeneration);
    std::thread cooking(kitchen);
    std::thread (delivery).detach();

    orderIncome.join();
    cooking.join();
    std::cout << "More or equal ten deliveries were done";
    return 0;
}