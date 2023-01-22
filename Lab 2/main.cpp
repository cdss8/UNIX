#include <iostream>
#include <thread>	
#include <mutex>	
#include <condition_variable>	
#include <chrono>

using namespace std;

condition_variable cv;
mutex cv_m;

bool isProvided = false;
int counter = 0;

void provide()
{
	for(int i=0; i<100; i++){ //to see the first 100 connection
	while (true) {
		unique_lock<mutex> lk(cv_m);
		counter++;
		cout << "provide: " << counter << endl;
		isProvided = true;
		cv.notify_all();
		lk.unlock();		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}
void consume()
{
	for(int i=0; i<100; i++){
	while (true) {
		unique_lock<mutex> lk(cv_m);
		cv.wait(lk, [] {return isProvided; });
		cout << "consume: " << counter << endl;
		isProvided = false;
		lk.unlock();
		}
	}
}

int main()
{
	thread thread1(provide);
	thread thread2(consume);

	thread1.join();
	thread2.join();

	return 0;
}