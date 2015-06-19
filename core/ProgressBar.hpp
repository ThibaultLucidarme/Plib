


class ProgressBar
{
	private:
	int* _current;
	int* _max;
	int _size;

	public:
	ProgressBar( int* i, int* m)
	{
		_current = i;
		_max = m;
		_size = 50;
	}
	
	ProgressBar( int* i, int* m, int s)
	{
		_current = i;
		_max = m;
		_size = s;
	}
	
	~ProgressBar()
	{
		_current = nullptr;
		_max = nullptr;
	}
	
	void SetSize(int s)
	{
		_size = s;
	}
	
	void Progress()
	{
		int iter = *_current
		int maxIter = *_max;
		
		int percentage = 100*iter/maxIter;
		int scaledPercentage = _size*iter/maxIter;
		
		cout<<"\r[";
		for(int i=0;i<=scaledPercentage;i++) cout<<"*";
		for(int i=scaledPercentage+1;i<_size;i++) cout<<" ";
		cout<<"]\t"<< iter <<"/"<<maxIter<<"\t("<<percentage<<"%)"<<flush;
		
		if(percentage==100)
		std::cout<<std::endl;
		
	}


};
