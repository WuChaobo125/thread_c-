//aurthor Chaobo Wu
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
using namespace std;

deque<int>chair;

mutex mtx;
condition_variable cv;
condition_variable c_notempty;

int id=0;//customer ID _global
bool chair_available(int M)
{
    int n=chair.size();
    return (M-n) != 0;
}
void customer(int M) {
    while (true) {
        id++;
        if (chair_available(M)) {
            if(chair.empty()){
     //           cout<<"waking up barber"<<endl;
                c_notempty.notify_all();//waking up the barber
            }
            chair.push_back(id);
            cv.notify_all();
            }else {
                cout << "Customer  " << id << "  leaving" << endl;
                cv.notify_all();
            }
             this_thread::sleep_for(chrono::seconds(3));
    }
}

void barber(){
    int cust;//to get haircut
    while(true) {
        unique_lock <mutex> lck(mtx);
        cv.wait(lck);
        //   cv.wait(lck,chair_available(M));
        if(chair.empty()){
         //   cout<<"No customer, barber sleeping";
            c_notempty.wait(lck);//barber sleeping waiting for signal
            lck.unlock();
        }
        else{
            cust = chair.front();
            chair.pop_front();
            cout << "Barber cutting the hair of customer " << cust << endl;
            cout << "Waiting room:";
            for (int i = 0; i < chair.size(); i++) {
                cout << " " << chair[i];
            }
            cout << endl;
            lck.unlock();
            this_thread::sleep_for(chrono::seconds((rand() % 5) + 1));
        }
    }

}

int main()
{
    int M;
    cout<<"Input number of chairs"<<endl;
    cin>>M;
    thread customer_thread(customer,M);
    thread barber_thread(barber);
    customer_thread.join();
    barber_thread.join();
    return 0;
}