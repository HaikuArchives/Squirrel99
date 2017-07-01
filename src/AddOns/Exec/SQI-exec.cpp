/*
 * Squirrel project
 *
 * file ?	SQI-exec.cpp
 * what	?	Shell Execute builtin predicates
 * who	?	jlv
 * when	?	02/06/00
 * last	?	10/05/00
 *
 *
 * (c) electron Technology 1999
 */
 
 // First the needed header

#include "SQI-AddOn.h"
#include <Application.h>
#include <Entry.h>
#include <Roster.h>
#include <Node.h>
#include <NodeInfo.h>
#include <posix/unistd.h>

#include <image.h>
#include <OS.h>

// now some infos on the package

char *Exec_Pkg_name = "Exec";
char *Exec_Pkg_purpose = "Exec builtins";
char *Exec_Pkg_author = "Kirilla";
char *Exec_Pkg_version = "0.5";
char *Exec_Pkg_date = "10/05/00";


// type used by the monitoring thread
typedef struct TMonitor {
	
	thread_id	waited; // id of the thread to wait 
	status_t	status; // return status of the exec
	
} TMonitor;

// Monitor function for the primitive exec.wait
int32 Monitor(void *data)
{
	status_t ret;
	TMonitor *info = static_cast<TMonitor *>(data);
	while ((ret = wait_for_thread(info->waited, &(info->status)))  < B_NO_ERROR 
                         && ret == B_INTERRUPTED);
                         	
	return 0;
}

// type used by the monitoring thread
typedef struct TSqiMonitor {

	thread_id monitor;			// thread id of the monitor
	thread_id waited; 			// id of the thread to monitor
	int pipe[2]; 				// output pipes of the thread	
	SQI_Node *func;				// function to execute 
	list<SQI_Object *> *args;	// the inputs of the function to execute
	SQI_Squirrel *parent; 		// squirrel launching the exec.bg
	
} TSqiMonitor;

// Monitor function for the primitive exec.bg
int32 MonitorBg(void *data)
{
	status_t ret;
	TSqiMonitor *info = static_cast<TSqiMonitor *>(data);
	SQI_Squirrel *squirrel;
	bool running = true;
	char *line;
	char buffer[1024];
	SQI_String *str;
	
	// first we need to create a squirrel			
	squirrel =  new SQI_Squirrel();	
	squirrel->SameGCConfig(info->parent);
	squirrel->SamePrecision(info->parent);
	// We set the interpreter
	squirrel->interpreter = info->parent->interpreter;	
    squirrel->thread = info->monitor;                     	
    // we import the function           
    info->func->Export(squirrel->LocalHeap);
    
    info->func->AddRef();
      		
    info->func->SetArgs(info->args);
     	
   	// now we open the file and wait for input 
  	FILE *fp = fdopen(info->pipe[0], "r");
  	
  	resume_thread(info->waited);
  							
  	while(running && !feof(fp))
  	{
  		line = fgets(buffer,1024,fp);
  		if(line)
  		{
  			// we need to remove a \n if there's one
  			if(line[strlen(line)-1] == '\n')
  			{
  				char *l;
  				l = (char *)malloc(sizeof(char)*strlen(line));
  				strncpy(l,line,strlen(line)-1);
  				l[strlen(line)-1] = '\0';
  				
  				str = new SQI_String(squirrel->LocalHeap,l);
  				str->AddRef();
  				info->args->push_front(str);	
  					
  				try
				{
					squirrel->HopOnLeave(squirrel->interpreter,info->func);
				} catch (SQI_Exception *ex) {
					string *str = ex->Print();
					*(squirrel->interpreter->Stream->err) << *str << "\n" << '\0';
					squirrel->interpreter->Stream->Flush();
					delete str;
					delete ex;		
				}	
  			
  				info->args->pop_front();
  				str->RemRef();	
  				free(l);	
  			}
  			else
  			{
  				str = new SQI_String(squirrel->LocalHeap,line);
  				str->AddRef();
  				info->args->push_front(str);	
  					
  				try
				{
					squirrel->HopOnLeave(squirrel->interpreter,info->func);
				} catch (SQI_Exception *ex) {
					string *str = ex->Print();
					*(squirrel->interpreter->Stream->err) << *str << "\n" << '\0';
					squirrel->interpreter->Stream->Flush();
					delete str;
					delete ex;		
				}	
  				  				
  				info->args->pop_front();
  				str->RemRef();  					
  			}	
  		}
  		else
  			running = false;	
  	}				
   	
   	info->func->SetArgs(NULL);	
 		   					
  	fclose(fp);            
    delete squirrel;                     	
    delete info;                  	
	return 0;
}


/*
 * function    : SQI_Exec_Wait
 * purpose     : Builtin predicate "exec.wait"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Exec_Wait(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(args->Length()<2)
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"exec.wait","need 2 inputs at least"));
    
    SQI_Keyword *var = IsKeyword((*args)[0]);
    if(!var)
   		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exec.wait","first input must be a word"));
    	
 	SQI_String *bin = IsString((*args)[1]);
  	if(!bin)
  		bin = IsKeyword((*args)[1]);
  		  
	if(bin)
  	{
  		const char *path = bin->Data()->c_str();
  		BNode node(path);
  		
  		if(node.InitCheck()!=B_NO_ERROR)
  			throw(new SQI_Exception(SQI_EX_ERROR,"exec.wait","unknow program ",path));
  			   
  		char **arg_v;
  		int32 arg_c = args->Length() - 1;
  		thread_id exec_thread;
  		int32 return_value;
  		string *str;
  		int pd[2];	
  		
  		arg_v = (char **)malloc(sizeof(char *)*(args->Length()));	
  		
  		arg_v[0] = strdup(path);	
  		
  		for(int i=2;i<args->Length();i++)
  		{	
  			str = (*args)[i]->Print(squirrel->precision);
  			arg_v[i-1] = strdup(str->c_str()); 
  			delete str;
  		} 
  		
  		arg_v[args->Length()-1] = NULL; 
  			  	  		
  		pipe(pd);
  		
  		if(be_app->Lock())
  		{
  	  		// 1. Save the current file descriptors. 
            int old_stdin = dup(0); 
           	int old_stdout = dup(1); 
            int old_stderr = dup(2); 			
  		
  			// 2. Install the new standard output/error
  			dup2(pd[1], 1); 
            dup2(pd[1], 2); 
            close(pd[1]); 	
  		
  			exec_thread = load_image(arg_c, const_cast<const char **>(arg_v), NULL); 

           	// 4. Restore original file descriptors. 
            dup2(old_stdin, 0); 
            close(old_stdin);               
           	dup2(old_stdout, 1); 
           	close(old_stdout);              
            dup2(old_stderr, 2); 
            close(old_stderr);              

            // Leave the critical section. 
            be_app->Unlock();		
            
            // we delete the arg_v
            
            for(int i=0;i<args->Length()-1;i++)
  				free(arg_v[i]); 
  			free(arg_v);
  			
  			if(exec_thread > B_ERROR) 
  			{
  				thread_id monitor;
  				TMonitor info;
  				char buffer[1024];
  				char *line;
  				bool running = true;
  				SQI_List *output = new SQI_List(squirrel->LocalHeap);
  					
  				// reading input from the child	
  				FILE *fp = fdopen(pd[0], "r");
  				// we start a monitor thread that wait for the exec thread to
  				// be finish, this way we get the return status of the command
  				
  				info.waited = exec_thread;	
  				
  				monitor = spawn_thread(Monitor,"mexec",B_LOW_PRIORITY,&info);
  					
  				// we resume both of the thread now	
  				resume_thread(monitor);	
  				resume_thread(exec_thread);
  							
  				while(running && !feof(fp))
  				{
  					line = fgets(buffer,1024,fp);
  					if(line)
  					{
  						// we need to remove a \n if there's one
  						if(line[strlen(line)-1] == '\n')
  						{
  							char *l;
  							l = (char *)malloc(sizeof(char)*strlen(line));
  							strncpy(l,line,strlen(line)-1);
  							l[strlen(line)-1] = '\0';
  							output->Add2End(new SQI_String(squirrel->LocalHeap,l));
  							free(l);	
  						}
  						else
  							output->Add2End(new SQI_String(squirrel->LocalHeap,line));
  					}
  					else
  						running = false;	
  				}
  				
  				int32 ret;
  				wait_for_thread(monitor,&ret);				
  					
  				fclose(fp);
  				
  				// we set the variable with the list and we output the return status
  				// of the exec
  				
  				squirrel->SetVarValue(interp,var->Data(),output);	
  					
  				return new SQI_Number(squirrel->LocalHeap,info.status);							
  			}
  			else
  			{
  				close(pd[0]);
				throw(new SQI_Exception(SQI_EX_ERROR,"exec.wait","program couldn't be started"));			 		
  			}	
  		}
  		return NULL;	  		
  	}  		  
	else
		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exec.wait","second input must be a string"));			 
}

/*
 * function    : SQI_Exec_Bg
 * purpose     : Builtin predicate "exec.bg"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Exec_Bg(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(args->Length()<2)
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"exec.bg","need 2 inputs at least"));
    
    SQI_Keyword *func = IsKeyword((*args)[0]);
    SQI_List *lst;
   
    if(func)
    {
	    SQI_String *bin = IsString((*args)[1]);
	  	if(!bin)
	  		bin = IsKeyword((*args)[1]);
	  		  
		if(bin)
	  	{
	  		const char *path = bin->Data()->c_str();
	  		BNode node(path);
	  		
	  		if(node.InitCheck()!=B_NO_ERROR)
	  			throw(new SQI_Exception(SQI_EX_ERROR,"exec.bg","unknow program ",path));
	  			  
	  		char **arg_v;
	  		int32 arg_c = args->Length() - 1;
	  		thread_id exec_thread;
	  		int32 return_value;
	  		string *str;
	  		int pd[2];	
	  		
	  		arg_v = (char **)malloc(sizeof(char *)*(args->Length()));	
	  		
	  		arg_v[0] = strdup(path);	
	  		
	  		for(int i=2;i<args->Length();i++)
	  		{	
	  			str = (*args)[i]->Print(squirrel->precision);
	  			arg_v[i-1] = strdup(str->c_str()); 
	  			delete str;
	  		} 
	  		
	  		arg_v[args->Length()-1] = NULL; 
	  			  	  		
	  		pipe(pd);
	  			  		
	  		if(be_app->Lock())
	  		{
	  	  		// 1. Save the current file descriptors. 
	            int old_stdin = dup(0); 
	           	int old_stdout = dup(1); 
	            int old_stderr = dup(2); 			
	  		
	  			// 2. Install the new standard output/error
	  			dup2(pd[1], 1); 
	            dup2(pd[1], 2); 
	            close(pd[1]); 	
	  		
	  			exec_thread = load_image(arg_c, const_cast<const char **>(arg_v), NULL); 
	
	           	// 4. Restore original file descriptors. 
	            dup2(old_stdin, 0); 
	            close(old_stdin);               
	           	dup2(old_stdout, 1); 
	           	close(old_stdout);              
	            dup2(old_stderr, 2); 
	            close(old_stderr);              
	
	            // Leave the critical section. 
	            be_app->Unlock();		
	            
	            // we delete the arg_v
	            
	            for(int i=0;i<args->Length()-1;i++)
	  				free(arg_v[i]); 
	  			free(arg_v);
	  			
		  		if(exec_thread > B_ERROR) 
	  			{
	  				thread_id monitor;
	  				TSqiMonitor *info;
	  				
	  				info = new TSqiMonitor;
	  				
	  				info->waited = exec_thread;
	  				info->pipe[0] = pd[0];
	  				info->pipe[1] = pd[1];
      				info->func = dynamic_cast<SQI_Node *>(interp->FindCall(func->Data()->c_str()));
      				info->args = new list<SQI_Object *>;
      				info->parent = squirrel;	
      		  		
					monitor = spawn_thread(MonitorBg,"bgexec",B_NORMAL_PRIORITY,info);      				
	  				
	  				resume_thread(monitor);
	  				
	  				return new SQI_Number(squirrel->LocalHeap,monitor);					
	  			}
	  			else
	  			{
	  				close(pd[0]);
					throw(new SQI_Exception(SQI_EX_ERROR,"exec.bg","program couldn't be started"));			 		
	  			}	
	  		}
	  		
	  		// we delete the arg_v     
	        for(int i=0;i<args->Length()-1;i++)
	  			free(arg_v[i]); 
	  		free(arg_v);
	  		
	  		throw(new SQI_Exception(SQI_EX_ERROR,"exec.bg","error creating thread!"));
	  	}
	  	else
	  		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exec.bg","second input must be a word or a string"));			
    }
    else
    {
		lst = IsList((*args)[0]);
		SQI_String *bin = IsString((*args)[1]);
	  	if(!bin)
	  		bin = IsKeyword((*args)[1]);
	  				
		if(lst && bin)
		{
			if(lst->Length()>=1)
			{
				SQI_Keyword *func = IsKeyword((*lst)[0]);
		  										
			  	if(func)
			  	{
			  		const char *path = bin->Data()->c_str();
			  		BNode node(path);
			  		
			  		if(node.InitCheck()!=B_NO_ERROR)
			  			throw(new SQI_Exception(SQI_EX_ERROR,"exec.bg","unknow program ",path));
			  			  
			  		char **arg_v;
			  		int32 arg_c = args->Length() - 1;
			  		thread_id exec_thread;
			  		int32 return_value;
			  		string *str;
			  		int pd[2];	
			  		
			  		arg_v = (char **)malloc(sizeof(char *)*(args->Length()));	
			  		
			  		arg_v[0] = strdup(path);	
			  		
			  		for(int i=2;i<args->Length();i++)
			  		{	
			  			str = (*args)[i]->Print(squirrel->precision);
			  			arg_v[i-1] = strdup(str->c_str()); 
			  			delete str;
			  		} 
			  		
			  		arg_v[args->Length()-1] = NULL; 
			  			  	  		
			  		pipe(pd);
			  			  		
			  		if(be_app->Lock())
			  		{
			  	  		// 1. Save the current file descriptors. 
			            int old_stdin = dup(0); 
			           	int old_stdout = dup(1); 
			            int old_stderr = dup(2); 			
			  		
			  			// 2. Install the new standard output/error
			  			dup2(pd[1], 1); 
			            dup2(pd[1], 2); 
			            close(pd[1]); 	
			  		
			  			exec_thread = load_image(arg_c, const_cast<const char **>(arg_v), NULL); 
			
			           	// 4. Restore original file descriptors. 
			            dup2(old_stdin, 0); 
			            close(old_stdin);               
			           	dup2(old_stdout, 1); 
			           	close(old_stdout);              
			            dup2(old_stderr, 2); 
			            close(old_stderr);              
			
			            // Leave the critical section. 
			            be_app->Unlock();		
			            
			            // we delete the arg_v
			            
			            for(int i=0;i<args->Length()-1;i++)
			  				free(arg_v[i]); 
			  			free(arg_v);
			  			
				  		if(exec_thread > B_ERROR) 
			  			{
			  				thread_id monitor;
			  				TSqiMonitor *info;
			  				
			  				info = new TSqiMonitor;
			  				
			  				info->waited = exec_thread;
			  				info->pipe[0] = pd[0];
			  				info->pipe[1] = pd[1];
		      				info->func = dynamic_cast<SQI_Node *>(interp->FindCall(func->Data()->c_str()));
		      				info->args = new list<SQI_Object *>;
		      				info->parent = squirrel;	
		      		  		
		      		  		// we need to copy now the inputs of the callback function
		      		  		
		      		  		for(int m=1;m<lst->Length();m++)
		      		  		{
		      		  			(*lst)[m]->Export(squirrel->interpreter->GlobalHeap);
		      		  			(*lst)[m]->AddRef();
		      		  			info->args->push_back((*lst)[m]);
		      		  		}	
		      		  		
							monitor = spawn_thread(MonitorBg,"bgexec",B_NORMAL_PRIORITY,info);      				
			  				
			  				resume_thread(monitor);
			  				
			  				return new SQI_Number(squirrel->LocalHeap,monitor);					
			  			}
			  			else
			  			{
			  				close(pd[0]);
							throw(new SQI_Exception(SQI_EX_ERROR,"exec.bg","program couldn't be started"));			 		
			  			}	
			  		}
			  		
			  		// we delete the arg_v     
			        for(int i=0;i<args->Length()-1;i++)
			  			free(arg_v[i]); 
			  		free(arg_v);
			  		
			  		throw(new SQI_Exception(SQI_EX_ERROR,"exec.bg","error creating thread!"));
			  	}
			  	else
			  		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exec.bg","second input must be a word or a string"));	  									
			}
			else
				throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exec.bg","the list must have one element at least"));				
		}
		else
			throw(new SQI_Exception(SQI_EX_BADARGTYPE,"exec.bg","first input must be a word or a list"));
	}
}

/*
 * function    : SQI_Exec_Launch
 * purpose     : Builtin predicate "launch"
 * input       : 
 *
 * list<SQI_Object *> *args, List of the argument
 * SQI_Squirrel *squirrel, Squirrel executing the code
 * SQI_Interp *interp, Interpreter
 *
 * output      : SQI_Object *, an object returned by the function
 * side effect : none
 */

SQI_Object *SQI_Exec_Launch(SQI_Args *args,SQI_Squirrel *squirrel,SQI_Interp *interp)
{
  	if(args->Length()!=1)
    	throw(new SQI_Exception(SQI_EX_BADARGSNUM,"launch","need 1 input"));
    
  
    SQI_String *file = IsString((*args)[0]);
  	
	if(file)
  	{
  		const char *path = file->Data()->c_str();
  		BEntry entry(path);
  		entry_ref ref;	
  		
  		if(entry.InitCheck()!=B_NO_ERROR)
  		{
  			if(!entry.Exists())
  				throw(new SQI_Exception(SQI_EX_ERROR,"launch","can't find the file ",path));
  		}	
  			
  		entry.GetRef(&ref);	
  			
  		be_roster->Launch(&ref,NULL,NULL);	
  			
  		return NULL;	
  	}
  	else
  		throw(new SQI_Exception(SQI_EX_BADARGTYPE,"launch","need a string as input"));
}

/*
 * function    : SQI_Exec_Remove
 * purpose     : Remove all the function of the addon from the builtinmap 
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map where the functions are
 *
 * output      : none
 * side effect : none
 */
void SQI_Exec_Remove(SQI_BuiltInMap *Map)
{
  Map->RemoveFunc("exec.wait");
  Map->RemoveFunc("exec.bg");
  Map->RemoveFunc("launch");
}

/*
 * function    : SQI_AddOn_Init
 * purpose     : Add in the function Map the function definied in this package
 * input       : 
 *
 * SQI_BuiltInMap *Map, Map to add the functions in
 * image_id addon_id, AddOn ID
 *
 * output      : none
 * side effect : none
 */
void SQI_AddOn_Init(SQI_BuiltInMap *Map,image_id addon_id)
{
  	Map->AddAddOn(addon_id,Exec_Pkg_name,Exec_Pkg_purpose,Exec_Pkg_author,Exec_Pkg_version,Exec_Pkg_date,SQI_Exec_Remove);
  
  	Map->AddFunc("exec.wait",
	       Exec_Pkg_name,
	       "Execute a binary file and wait for it end",
	       "exec.wait word string things...",
	       SQI_Exec_Wait
	       );
	       
	Map->AddFunc("exec.bg",
	       Exec_Pkg_name,
	       "Execute a binary file in the background and output the thread id",
	       "exec.bg word | [word things...] string (thing)*",
	       SQI_Exec_Bg
	       );
	       
	Map->AddFunc("launch",
	       Exec_Pkg_name,
	       "launch path",
	       "Launch the application associated with the mime type of the file",
	       SQI_Exec_Launch
	       );           
}

