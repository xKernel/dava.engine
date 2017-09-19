#pragma once

#include "Base/BaseMath.h"
#include "Base/BaseTypes.h"
#include "Base/FastNameMap.h"
#include "Reflection/Reflection.h"
#include "Scene3D/SkeletonAnimation/SkeletonPose.h"

#include "Private/MotionState.h"
#include "Private/MotionTransition.h"

namespace DAVA
{
class BlendTree;
class SkeletonComponent;
class YamlNode;
class Motion;
class MotionState;
struct MotionTransitionInfo;

class Motion
{
    Motion() = default;

public:
    enum eMotionBlend
    {
        BLEND_OVERRIDE,
        BLEND_ADD,
        BLEND_DIFF,
        BLEND_LERP,

        BLEND_COUNT
    };

    ~Motion();

    static Motion* LoadFromYaml(const YamlNode* motionNode);

    const FastName& GetName() const;
    eMotionBlend GetBlendMode() const;
    const SkeletonPose& GetCurrentSkeletonPose() const;

    bool RequestState(const FastName& stateID);
    void Update(float32 dTime);

    void BindSkeleton(const SkeletonComponent* skeleton);

    const Vector<FastName>& GetParameterIDs() const;
    bool BindParameter(const FastName& parameterID, const float32* param);
    bool UnbindParameter(const FastName& parameterID);
    void UnbindParameters();

    const Vector<FastName>& GetStateIDs() const;
    const FastName& GetRequestedState() const;

    const Vector<std::pair<FastName, FastName>> GetEndedPhases() const;

protected:
    uint32 GetTransitionIndex(const MotionState* srcState, const MotionState* dstState) const;
    MotionTransitionInfo* GetTransition(const MotionState* srcState, const MotionState* dstState) const;

    FastName name;
    eMotionBlend blendMode = BLEND_COUNT;

    Vector<MotionState> states;
    FastNameMap<MotionState*> statesMap;
    Vector<MotionTransitionInfo*> transitions;

    Vector<FastName> statesIDs;
    Vector<FastName> parameterIDs;

    MotionTransition currentTransition;
    MotionState* currentState = nullptr;
    MotionState* pendingState = nullptr;
    MotionState* afterTransitionState = nullptr; //TODO: *Skinning* state-sequence
    bool transitionIsActive = false;

    SkeletonPose currentPose;
    Vector<std::pair<FastName, FastName>> endedPhases; /*[state-id, phase-id]*/

    //////////////////////////////////////////////////////////////////////////
    //temporary for debug
    void SetStateID(const FastName& id)
    {
        RequestState(id);
    }
    //////////////////////////////////////////////////////////////////////////

    DAVA_REFLECTION(Motion);
};

inline const FastName& Motion::GetName() const
{
    return name;
}

inline Motion::eMotionBlend Motion::GetBlendMode() const
{
    return blendMode;
}

inline const SkeletonPose& Motion::GetCurrentSkeletonPose() const
{
    return currentPose;
}

inline const Vector<FastName>& Motion::GetParameterIDs() const
{
    return parameterIDs;
}

inline const FastName& Motion::GetRequestedState() const
{
    static const FastName invalidID = FastName("#invalid-state");

    if (afterTransitionState != nullptr)
        return afterTransitionState->GetID();
    else if (pendingState != nullptr)
        return pendingState->GetID();
    else if (currentState != nullptr)
        return currentState->GetID();
    else
        return invalidID;
}

inline const Vector<std::pair<FastName, FastName>> Motion::GetEndedPhases() const
{
    return endedPhases;
}

} //ns