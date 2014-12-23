#pragma once

#include <sal.h>
#include "Singleton.hpp"

class CState
{
	friend class CCtx;
protected:
	static
	void
	ChangeState(
		__inout CCtx* ctx,
		__inout CState* state
		);

public:
 	virtual 
	void 
	Switch(
		__inout CCtx* ctx
		) = 0;

	virtual
	CState*
	SwitchCallback(
		__inout CCtx* ctx
		);
};

class CCtx
{
	CState* m_state;
public:
	CCtx(
		__in CState* state
		);

	void
	Switch();

	void
	SetState(
		__in CState* state
		);
};
