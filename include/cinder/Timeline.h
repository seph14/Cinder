/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Based on the sc-Choreograph CinderBlock by David Wicks: http://sansumbrella.com/

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/TimelineItem.h"
#include "cinder/Easing.h"
#include "cinder/Tween.h"

#include <vector>
#include <list>

namespace cinder {

typedef std::shared_ptr<class Cue>			CueRef;
typedef std::shared_ptr<class Timeline>		TimelineRef;
	
class Timeline : public TimelineItem {		
  public:
	Timeline();			

	//! Creates a new timeline, defaulted to infinite
	static TimelineRef	create() { TimelineRef result( new Timeline() ); result->setInfinite( true ); return result; }

	//! Advances time a specified amount and evaluate items
	void	step( float timestep );
	//! Goes to a specific time and evaluate items
	void	stepTo( float absoluteTime );
	
	//! Returns the timeline's most recent current time
	float	getCurrentTime() const { return mCurrentTime; }
	
	//! add a cue to the Timeline at the current time. Use TimelineItem::delay() to offset the start time
	CueRef add( std::function<void ()> action );
	
	//! Create a new tween and adds it to the timeline's current time
	template<typename T, typename Y>
	TweenRef<T> add( T *target, Y endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( endValue ), mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return newTween;
	}
	
	//! Create a new tween and adds it to the timeline's current time
	template<typename T, typename Y, typename Z>
	TweenRef<T> add( T *target, Y startValue, Z endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( startValue ), static_cast<Z>( endValue ), mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return newTween;
	}

	//! Replaces any existing tweens on the \a target with a new tween at the timeline's current time
	template<typename T, typename Y>
	TweenRef<T> apply( T *target, Y endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( endValue ), mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		apply( newTween );
		return newTween;
	}
	
	//! Replaces any existing tweens on the \a target with a new tween at the timeline's current time
	template<typename T, typename Y, typename Z>
	TweenRef<T> apply( T *target, Y startValue, Z endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> ) {
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( startValue ), static_cast<Z>( endValue ), mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		apply( newTween );
		return newTween;
	}

	//! Creates a new tween and adds it to the end of a timeline, setting its start time to the timeline's duration or the current time, whichever is greater
	template<typename T, typename Y>
	TweenRef<T> append( T *target, Y endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> ) {
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( endValue ), std::max( getDuration(), mCurrentTime ), duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return newTween;
	}
	
	//! Creates a new tween and adds it to the end of a timeline, setting its start time to the timeline's duration or the current time, whichever is greater
	template<typename T, typename Y, typename Z>
	TweenRef<T> append( T *target, Y startValue, Z endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> ) {
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( startValue ), static_cast<Z>( endValue ), std::max( getDuration(), mCurrentTime ), duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return newTween;
	}

	//! Creates a new tween and adds it to the end of the last tween whose target matches \a target. The new tween's start time is set to the previous tween's end time or the current time if no existing tween matches the target, whichever is greater
	template<typename T, typename Y>
	TweenRef<T> appendTarget( T *target, Y endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> ) {
		float startTime = mCurrentTime;
		TimelineItemRef last = findLast( target );
		if( last )
			startTime = last->getEndTime();
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( endValue ), std::max( mCurrentTime, startTime ), duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return newTween;
	}
	
	//! Creates a new tween and adds it to the end of the last tween whose target matches \a target. The new tween's start time is set to the previous tween's end time or the current time, whichever is greater
	template<typename T, typename Y, typename Z>
	TweenRef<T> appendTarget( T *target, Y startValue, Z endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> ) {
		float startTime = mCurrentTime;
		TimelineItemRef last = findLast( target );
		if( last )
			startTime = last->getEndTime();
		TweenRef<T> newTween( new Tween<T>( target, static_cast<Y>( startValue ), static_cast<Z>( endValue ), std::max( mCurrentTime, startTime ), duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return newTween;
	}

	//! Appends to the end of the timeline mirror images of all items
	void appendPingPong();

	//! Replaces any existing TimelineItems that match \a item's target and adds \a item to the timeline. Safe to use from callback fn's.
	void apply( TimelineItemRef item );
	
	//! add an item to the timeline at the current time. Safe to use from callback fn's.
	void add( TimelineItemRef item );
	//! adds an item to the timeline. Its start time is not modified. Safe to use from callback fn's.
	void insert( TimelineItemRef item );
	//! adds an item to the timeline, setting its startTime to be at \a atTime. Safe to use from callback fn's.
	void insert( TimelineItemRef item, float atTime ) { item->mStartTime = atTime; insert( item ); }

	//! Returns the number of items in the Timeline
	size_t				getNumItems() const { return mItems.size(); }
	//! Returns the first item in the timeline the target of which matches \a target
	TimelineItemRef		find( void *target );
	//! Returns the latest-starting item in the timeline the target of which matches \a target
	TimelineItemRef		findLast( void *target );
	//! Removes the TimelineItem \a item from the Timeline. Safe to use from callback fn's.
	void				remove( TimelineItemRef item );
	//! Removes all TimelineItems whose target matches \a target
	void				removeTarget( void *target );
	
	//! Remove all tweens from the Timeline. Do not call from callback fn's.
	void clear();
	//! Sets the time to zero, all tweens as not completed, and if \a unsetStarted, marks the tweens as not started. Do not call from callback fn's.
	void reset( bool unsetStarted = false );

	//! Sets the default \a autoRemove value for all future TimelineItems added to the Timeline
	void	setDefaultAutoRemove( bool defaultAutoRemove ) { mDefaultAutoRemove = defaultAutoRemove; }
	//! Returns the default \a autoRemove value for all future TimelineItems added to the Timeline
	bool	getDefaultAutoRemove() const { return mDefaultAutoRemove; }

	//! Call this to notify the Timeline if the \a item's start-time or duration has changed
	void	itemTimeChanged( TimelineItem *item );

	//! Explicitly remove all items which have been marked for deletion. Not generally necessary except in advanced performance usage.
	void		eraseMarked();
  protected:
	virtual void reverse();
	virtual TimelineItemRef cloneReverse() const;
	virtual void start() {}
	virtual void loopStart();
	virtual void update( float absTime );
	virtual void complete() {}

	void						calculateDuration();

	bool						mDefaultAutoRemove;
	float						mCurrentTime;
	std::list<TimelineItemRef>	mItems;
};

class Cue : public TimelineItem {
  public:
	Cue( std::function<void ()> fn, float atTime = 0 );

	void					setFn( std::function<void ()> fn ) { mFunction = fn; }
	std::function<void ()>	getFn() const { return mFunction; }

	// these override their equivalents in TimelineItem so that we can return a CueRef instead of TimelineItemRef
	//! Pushes back the tween's start time by \a amt. Returns a reference to \a this
	CueRef startTime( float newTime ) { setStartTime( newTime ); return getThisRef(); }
	//! Pushes back the tween's start time by \a amt. Returns a reference to \a this
	CueRef delay( float amt ) { setStartTime( mStartTime + amt ); return getThisRef(); }
	//! Sets the tween's duration to \a newDuration. Returns a reference to \a this
	CueRef duration( float newDuration ) { setDuration( newDuration ); return getThisRef(); }			
	//! Sets whether the item will remove itself from the Timeline when it is complete
	CueRef autoRemove( bool autoRmv = true ) { setAutoRemove( autoRmv ); return getThisRef(); }
	//! Sets whether the item starts over when it is complete
	CueRef loop( bool doLoop = true ) { setLoop( doLoop ); return getThisRef(); }

	//! Returns a CueRef to \a this
	CueRef getThisRef() { return CueRef( std::static_pointer_cast<Cue>( shared_from_this() ) ); }
	
  protected:
	virtual void				reverse() { /* no-op */ }
	virtual TimelineItemRef	cloneReverse() const;

	virtual void start() {} // starting is a no-op for Cues
	virtual void loopStart();
	virtual void update( float relativeTime ) {} // update is a no-op for Cues
	virtual void complete() {} // completion is a no-op for Cues
	virtual bool updateAtLoopStart() { return true; }
  
	std::function<void ()>		mFunction;
};

} // namespace cinder