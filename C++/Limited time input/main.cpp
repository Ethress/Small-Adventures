#include <iostream>
#include <thread>
#include <ctime>

/*
Thanks to user:2586447 uchar on github for the idea.
Changed so that the user is not required to wait for the end of the timer.

https://stackoverflow.com/questions/22818994/how-to-restrict-input-time-limit-in-cin
*/

int getVal(int defaultValue)
{
    int val=0;
    time_t current_time = time(NULL);

    std::thread t1([&](){
        std::cin>>val;
    });

    while(current_time+3 > time(NULL) && val==0){}
    t1.detach();

    if(val==0)
        val=defaultValue;
    return val;
}

int main()
{
    std::cout<<getVal(123);
}
