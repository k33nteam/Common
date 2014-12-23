#include "State.h"

void
CState::ChangeState(
	__inout CCtx* ctx,
	__inout CState* state
	)
{
	ctx->SetState(state->SwitchCallback(ctx));
}

CState*
CState::SwitchCallback(
	__inout CCtx* ctx
	)
{
	return this;
}

CCtx::CCtx(
	__in CState* state
	)
{
	CState::ChangeState(this, state);
}

void
CCtx::Switch()
{
	m_state->Switch(this);
}

void
CCtx::SetState(
	__in CState* state
	)
{
	m_state = state;
}
