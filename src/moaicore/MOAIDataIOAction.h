// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIASYNCACTION_H
#define MOAIASYNCACTION_H

#include <moaicore/MOAIAction.h>

class MOAIDataBuffer;

//================================================================//
// MOAIDataIOAction
//================================================================//
/**	@brief Action for asynchronously loading data.
*/
class MOAIDataIOAction :
	public MOAIAction {
private:

	enum {
		IDLE,
		READY_LOAD,
		READY_SAVE,
		BUSY,
		DONE,
	};

	STLString				mFilename;
	USRef < MOAIDataBuffer >		mData;
	USLuaRef				mOnFinish;

	u32						mState;

	//----------------------------------------------------------------//
	static int	_setCallback		( lua_State* L );

	//----------------------------------------------------------------//
	void		Finished			( USDataIOTask* task );
	void		Load				();
	void		Save				();

public:

	DECL_LUA_DATA ( MOAIDataIOAction )

	//----------------------------------------------------------------//
	void		Init				( cc8* filename, MOAIDataBuffer* data );
	bool		IsBusy				();
				MOAIDataIOAction	();
				~MOAIDataIOAction	();
	void		OnUpdate			( float step );
	void		RegisterLuaClass	( USLuaState& state );
	void		RegisterLuaFuncs	( USLuaState& state );
	void		StartLoad			();
	void		StartSave			();
};

#endif
