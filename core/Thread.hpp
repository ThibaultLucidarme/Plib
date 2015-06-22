#include <iostream>
#include <thread>
#include <chrono>

/************************************* Optimal Plib threads ******************************************************
p::Thread updateDensityThread([&] {diffuse(0, _density0, _density, _diff); });
p::Thread advectVelocityXThread([&] {advect(1, _Vx, _Vx0, _Vx0, _Vy0); });
advectVelocityXThread.join();
updateDensityThread.join();
***************************************************************************************************************/

namespace p
{

	class Thread
	{
	private:

		static const unsigned short _optimumMaxThreads = std::thread::hardware_concurrency();
		static unsigned short _currentThreadsInUse = 0;
		static bool _strict = true;

		std::unique_ptr<std::thread> _thread;

	public:
		Thread(std::Function fn)
		{
			if (!strict || !isMaxedOut())
			{
				//spawn
				_thread = new std::thread(fn);
				_currentThreadsInUse++;
			}
			else std::cerr << "Trying to use more threads than optimum allowed in strict mode. Thread not launched." << std::endl;
		}

		void Join(void){

			_thread->join();
			_currentThreadsInUse--;

		}

		void Detach(void){

			_thread->detach();

		}

		void Pause(int time, string unit = "ms"){

			if (unit == "s") std::this_thread::sleep_for(std::chrono::seconds(time));
			else if (unit == "ms") std::this_thread::sleep_for(std::chrono::milliseconds(time));
			else if (unit == "ns") std::this_thread::sleep_for(std::chrono::nanoseconds(time));
			else if (unit == "min") std::this_thread::sleep_for(std::chrono::minutes(time));
			else if (unit == "hr") std::this_thread::sleep_for(std::chrono::hours(time));

		}

		static void SetStrict(bool s)
		{
			if (!s || !isMaxedOut()) _strict = s;
			else std::cerr << "There are already more than the optimum number of threads spawned. Cannot enforce strict mode" << std::endl;
		}

		static bool isMaxedOut(void)
		{
			return !(_currentThreadsInUse < _optimumMaxThreads - 1);
		}

	};

}
