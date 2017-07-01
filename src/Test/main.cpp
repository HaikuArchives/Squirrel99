 
#include <OS.h> 
#include <iostream.h>

#include <list>
#include <List.h>

void ShowMem()
{
 	area_info info;
 	int32 cookie = 0;
 	size_t sum =0;	
 	
 	while (get_next_area_info(0,&cookie,&info) == B_OK)
 		sum += info.ram_size;

	cout << "MEM USED NOW : " << sum << "\n";
}

void test_list()
{						
	list<int32> *lst;
	bigtime_t t0,t1;
	int32 val;
		
	t0 = system_time();	
	
	lst = new list<int32>;

	/* do something */
	
	for(int32 i=0;i<100;i++)
		lst->push_back(i);
		
	t1 = system_time();
	
	cout << "creation stl list : " << t1 - t0 << " msecs\n";		
		
	t0 = system_time();	
	
	list<int32>::const_iterator j;
	
    j=lst->begin();
    for(int k=0;k<8;k++)
		j++;
    val = *j;
    j=lst->begin();
    for(int k=0;k<2;k++)
		j++;
    val = *j;   
    	
	t1 = system_time();
	
	cout << "access several elem stl list : " << t1 - t0 << " msecs\n";	

	val = 0;
	
	t0 = system_time();	
	
	for(j = lst->begin();j!=lst->end();j++)
		val += *j;		
		
	t1 = system_time();
	
	cout << "sum elem stl list " << val << " : " << t1 - t0 << " msecs\n";	
		
	//ShowMem();

	delete lst;
}

void test_blist()
{						
	BList *lst;
	bigtime_t t0,t1;
	int32 *val,i;
	int32 **elem;	
		
	t0 = system_time();	
	
	lst = new BList();

	/* do something */
	for(i=0;i<100;i++)
	{
		val = new int32;
		*val = i;
		lst->AddItem(val);	
	}	

	t1 = system_time();
	
	cout << "creation be list : " << t1 - t0 << " msecs\n";
		
	//ShowMem();

	t0 = system_time();	
	
	val = (int32 *)(lst->ItemAt(8));
	val = (int32 *)(lst->ItemAt(2));
	
	t1 = system_time();
	
	cout << "access several elem be list " << *val << " : " << t1 - t0 << " msecs\n";		

	val = new int32;

	*val = 0;
	
	elem = (int32 **)(lst->Items());	
		
	for(i=0;i<lst->CountItems();i++)
	{
		*val += *(elem[i]);
		//cout << i << " " << *val << " " << *(elem[i]) << "\n";	
	}	
	
	t1 = system_time();
	
	cout << "sum elem be list " << *val << " : " << t1 - t0 << " msecs\n";		
	
	for(i=0;i<lst->CountItems();i++)
		delete lst->ItemAt(i);
			
	delete lst;
	delete val;
}

int main()
{
	test_list();
	test_blist();	
}
