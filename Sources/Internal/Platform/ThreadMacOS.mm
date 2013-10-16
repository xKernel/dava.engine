/*==================================================================================
    Copyright (c) 2008, DAVA, INC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA, INC nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA, INC AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVA, INC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/
#include "Platform/Thread.h"

#if defined(__DAVAENGINE_IPHONE__) || defined(__DAVAENGINE_MACOS__)
#include <pthread.h>
#import <QuartzCore/QuartzCore.h>

#if defined (__DAVAENGINE_MACOS__)
#import <AppKit/NSOpenGL.h>
	#if defined (__DAVAENGINE_NPAPI__)
	#import "NPAPIOpenGLLayerMacOS.h"
	#endif // #if defined (__DAVAENGINE_NPAPI__)
#endif //#if defined (__DAVAENGINE_MACOS__)

@interface __ThreadMacOSInit__ : NSObject
{
}

-(void)ThreadFunc;

@end

@implementation __ThreadMacOSInit__

-(void)ThreadFunc
{
}

@end

namespace DAVA
{

static bool __isThreadMacOSInited__ = false;

void	Thread::InitMacOS()
{
	if(!__isThreadMacOSInited__)
	{
		__ThreadMacOSInit__ * threadInit = [[__ThreadMacOSInit__ new] autorelease];
		[NSThread detachNewThreadSelector:@selector(ThreadFunc) toTarget:threadInit withObject:nil];
		__isThreadMacOSInited__ = true;
	}
}
	
void * PthreadMain(void * param)
{	
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	Thread * t = (Thread*)param;
	t->SetThreadId(Thread::GetCurrentThreadId());
	if(t->needCopyContext)
	{
#if defined(__DAVAENGINE_IPHONE__)
		
//		EAGLContext *cont = [[EAGLContext alloc] initWithAPI:((EAGLContext*)t->glContext).API sharegroup:((EAGLContext*)t->glContext).sharegroup];
//		if(![EAGLContext setCurrentContext:cont])
//		{
//			Logger::Error("Unable to set thread context");
//		}
//		[cont autorelease];
		if(![EAGLContext setCurrentContext:(EAGLContext*)t->glContext])
		{
			Logger::Error("Unable to set thread context 0x%x", (int)t->glContext);
		}
		Logger::Info("Thread context 0x%x", (int)EAGLContext.currentContext);
#elif defined(__DAVAENGINE_MACOS__)
	#if defined (__DAVAENGINE_NPAPI__)
		CGLError err = CGLSetCurrentContext(t->npAPIGLContext);
		NSLog(@"CGLSetCurrentContext returned %i", err);
	#else
		NSOpenGLContext * context = (NSOpenGLContext*)t->glContext;
		[context makeCurrentContext];
//		if(![NSOpenGLContext setCurrentContext:(NSOpenGLContext*)t->glContext])
//		{
//			Logger::Error("Unable to set thread context 0x%x", (int)t->glContext);
//		}
//		Logger::Info("Thread context 0x%x", (int)EAGLContext.currentContext);
	#endif // #if defined (__DAVAENGINE_NPAPI__)
#endif
	
	}

	
	t->state = Thread::STATE_RUNNING;
	t->msg(t);

	t->state = Thread::STATE_ENDED;
	t->Release();
	
	if(t->needCopyContext)
	{
#if defined(__DAVAENGINE_IPHONE__)
		[EAGLContext setCurrentContext:nil];
#elif defined(__DAVAENGINE_MACOS__)
		// нужно ли что-то здесь освобождать после выхода из потока???
		// зачем контекст обнуляется в iPhone версии? 
		// [NSOpenGLContext setCurrentContext: nil];
	#if defined (__DAVAENGINE_NPAPI__)
		CGLDestroyContext(t->npAPIGLContext);
	#endif // #if defined (__DAVAENGINE_NPAPI__)
#endif
		
	}
	[pool release];
	pthread_exit(0);
}

void	Thread::StartMacOS()
{
	if(needCopyContext)
	{
#if defined(__DAVAENGINE_IPHONE__)
		glContext = [EAGLContext currentContext];
		Logger::Info("Current context 0x%x", (int)glContext);
#elif defined(__DAVAENGINE_MACOS__)
	#if defined (__DAVAENGINE_NPAPI__)
		// NPAPI code
		this->npAPIGLContext = NULL;
		[NPAPIOpenGLLayerMacOS getThreadChildContext:&this->npAPIGLContext];
	#else
		// Pure MacOS code.
		glContext = [NSOpenGLContext currentContext];
	#endif // #if defined (__DAVAENGINE_NPAPI__)
#endif
	}

	pthread_t threadId;
	pthread_create(&threadId, 0, PthreadMain, (void*)this);
}
	
bool Thread::IsMainThread()
{
	return [NSThread isMainThread];
}
    
void Thread::YieldThread()
{
    pthread_yield_np();
}
    
Thread::ThreadId Thread::GetCurrentThreadId()
{
    ThreadId ret;
    ret.internalTid = pthread_self();
    
    return ret;
}
	
};

#endif //__DAVAENGINE_IPHONE__
