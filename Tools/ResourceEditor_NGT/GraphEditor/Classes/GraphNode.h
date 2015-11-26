/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.
 
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
 
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.
 
    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/

#ifndef __GRAPHEDITOR_GRAPHNODE_H__
#define __GRAPHEDITOR_GRAPHNODE_H__

#include <core_common/signal.hpp>
#include <core_data_model/i_list_model.hpp>
#include <core_reflection/reflected_object.hpp>
#include <core_reflection/object_handle.hpp>

#include <string>
#include <QPointF>

class ConnectionSlot;
class GraphNode
{
    DECLARE_REFLECTED
public:
    GraphNode();
    ~GraphNode();

    struct Params
    {
        using TSlotPtr = ObjectHandleT<ConnectionSlot>;
        using TSlotCollection = std::vector<TSlotPtr>;

        TSlotCollection inputSlots;
        TSlotCollection outputSlots;
        std::string typeId;
    };

    void Init(Params&& params);
    void SetPosition(float x, float y);
    QPointF GetPosition() const;

    std::string const& GetTitle() const;
    void SetTitle(std::string const& title);

    size_t GetUID() const;
    std::string const& GetType()
    {
        return typeId;
    }

    void Shift(float pixelShiftX, float pixelShiftY);
    void ApplyTransform();

    Signal<void()> NodeMoved;

private:
    IListModel* GetInputSlots() const;
    IListModel* GetOutputSlots() const;

    float GetPosX() const;
    void SetPosX(const float& x);

    float GetPosY() const;
    void SetPosY(const float& y);

    float GetScale() const;
    void SetScale(const float& scale);

private:
    std::string title;
    float modelX = 0.0f, modelY = 0.0f;
    float pixelX = 0.0f, pixelY = 0.0f;
    float scale = 1.0f;

    std::unique_ptr<IListModel> inputSlots;
    std::unique_ptr<IListModel> outputSlots;
    std::string typeId;

    size_t transformConnectionID = 0;
};

#endif // __GRAPHEDITOR_GRAPHNODE_H__