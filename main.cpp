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

int succes_deliveries=0;
std::vector<ORDER> ordersToCook;
std::vector<ORDER> ordersToDeliver;

void orderGeneration()
{
    std::srand(std::time(nullptr));
    int temp;

    while(succes_deliveries<10)
    {
        temp=std::rand()%6+5;
        std::this_thread::sleep_for(std::chrono::seconds(temp));
        orderArrayLock.lock();
        temp=std::rand()%5;
        std::cout << orderNames[temp] << " was ordered\n";
        ordersToCook.push_back((ORDER)(temp));
        orderArrayLock.unlock();
    }
}

void kitchen()
{
    std::srand(std::time(nullptr));
    int order, timeout;

    while(succes_deliveries<10)
    {
        if(ordersToCook.size()!=0)
        {
            orderArrayLock.lock();
            order=ordersToCook.back();
            ordersToCook.pop_back();
            orderArrayLock.unlock();
            std::cout << orderNames[order] << " was started to cook\n";
            timeout=std::rand()%11+5;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << orderNames[order] << " was cooked\n";
            deliveryLock.lock();
            ordersToDeliver.push_back((ORDER)order);
            deliveryLock.unlock();
        }
    }
}

void delivery()
{
    while(succes_deliveries<10)
    {
        if(ordersToDeliver.size()!=0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            deliveryLock.lock();
            for (auto i = ordersToDeliver.begin(); i != ordersToDeliver.end(); i++) {
                std::cout << orderNames[*i] << " was delivered\n";
                succes_deliveries++;
            }
            ordersToDeliver.erase(ordersToDeliver.begin(), ordersToDeliver.end());

            deliveryLock.unlock();
        }
    }

    std::cout << "10 deliveries done";
}

int main()
{
    std::thread orderIncome(orderGeneration);
    std::thread cooking(kitchen);
    std::thread (delivery).detach();

    orderIncome.join();
    cooking.join();
    return 0;
}
