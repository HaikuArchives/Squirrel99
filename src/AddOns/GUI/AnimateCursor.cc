/*
    Class: AnimateCursor
    
    Limitations:
    	This code is provided "as is". The author and/or
    	company is not responsible for any problems encountered
    	from using this class.  In using this class, the
    	developer and/or their company accepts the use of
    	this code in current form.
    
	Author: John Tegen
	Dated: 7 July 1997
	Company: OmicronSoft, (c) All Rights Reserved, 1997
	
	Revision History:
	- 26 June 1999: Modified code for 4.5 release when application
					would freeze up wehn thread got killed.
	- 21 July 1997: Correct abnormal crashes when killing
					thread and destructor.
	- 7 July 1997 : Original code
*/

#include <unistd.h>
#include <string.h>
#include <Application.h>
#include <Cursor.h>

#include "AnimateCursor.h"

#define CURSOR_STREAM_SIZE	68

////////////////////////////////////////////////////
//
AnimateCursor::AnimateCursor( const char *a_name,
							   const int32& a_qty,float c = 0.1 )
{
  strcpy( name, a_name );
  cycle = c;
  qty = a_qty;
  run = false;
  
  //
  // Allocate enougph room for the cursors
  //
  cursors = new BCursor *[ qty ];
}

#include <iostream>

////////////////////////////////////////////////////
//
AnimateCursor::~AnimateCursor( void )
{	
  if(run)
  	kill_thread(thrd);	
  	    
  if( qty )
  {
    for( int32 i=0; i < qty; i++ )
    	delete cursors[i];
  
    delete [] cursors;
  }
}

////////////////////////////////////////////////////
//
void
AnimateCursor::SetCursor( const int32& a_index,
						   const unsigned char *a_cursor )
{
	//
  	// Make certain index is within range
  	//
  	if( a_index < qty )
   		cursors[a_index] = new BCursor(a_cursor);   
}

////////////////////////////////////////////////////
//
void
AnimateCursor::Run( const float& a_cycle )
{
  //
  // If no name was given, don't run the thread
  //
  if( !strlen( name ) )return;
  
  //
  // Set the cycle is positive
  //
  if( a_cycle > 0.0 )cycle = a_cycle;
  
  run = true;
  
  //
  // Spawn thread
  //
  thrd = spawn_thread( RunLoop, name,
  						B_LOW_PRIORITY, this );
  resume_thread( thrd );
}

////////////////////////////////////////////////////
//
int32
AnimateCursor::RunLoop( void * a_arg )
{
  AnimateCursor *obj = (AnimateCursor *)a_arg;
  return obj->LoopCursors();
}

////////////////////////////////////////////////////
//
int32
AnimateCursor::LoopCursors( void )
{  
  int32 i = 0;
  // run is set to false in Quit()
  while( run && qty )
  {
    i = 0;
    while( run && i < qty )
    {
      if( cursors && cursors[i] )
      {
        be_app->SetCursor( cursors[i] );
      }
      snooze( (bigtime_t)(cycle * 1000000.0) );		// snooze in miliseconds
      
      i++;
    }
  }
  return 0;
}

////////////////////////////////////////////////////
//
void
AnimateCursor::Quit( void )
{
  run = false;	// tell thread to die
  
  thread_info info;
  while( get_thread_info( thrd, &info ) == B_OK &&
  		( info.state == B_THREAD_RUNNING ||
  		  info.state == B_THREAD_SUSPENDED ||
  		  info.state == B_THREAD_ASLEEP ) )
  {
  	// wait for thread to naturally die
  }
  be_app->SetCursor( B_HAND_CURSOR );
}

//
// eof
//
